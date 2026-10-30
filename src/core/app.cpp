#include "core/app.hpp"
#include "core/sim_clock.hpp"
#include "ui/theme.hpp"

#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>

#include <cstdint>

App::App(const AppConfig& config)
    : m_config(config),
      m_window(config.window_config),
      m_clock(config.clock_config),
      m_grid(config.grid_width, config.grid_height),
      m_renderer(config.grid_width, config.grid_height)
{
    m_grid(50, 30) = 1;
    m_grid(51, 30) = 1;
    m_grid(52, 30) = 1;
    m_grid(52, 29) = 1;
    m_grid(51, 28) = 1;
}

void App::run() {
    while (!WindowShouldClose()) {
        handle_input();
        update(GetFrameTime());

        BeginDrawing();
        ClearBackground(theme::bg0);

        render();
        draw_ui();

        EndDrawing();
    }
}

void App::handle_input() {
    if (IsKeyPressed(KEY_S))     m_show_ui = !m_show_ui;
    if (IsKeyPressed(KEY_SPACE)) m_clock.toggle_pause();
    if (IsKeyPressed(KEY_ENTER)) m_clock.request_step();
}

void App::update(double frame_time) {
    const int steps = m_clock.advance(frame_time);

    for (int i = 0; i < steps; ++i) {
        // TODO: Game of Life logic
    }

    m_renderer.update(m_grid, [](uint8_t cell) { return cell ? theme::fg0 : theme::bg0; });
}

void App::render() {
    m_renderer.render();
}

void App::draw_ui() {
    rlImGuiBegin();
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowCollapsed(!m_show_ui, ImGuiCond_Always);
    ImGui::Begin("Sandbox [S]", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("FPS: %d", GetFPS());
    ImGui::SeparatorText("Simulation");

    ImGui::Text("Ticks: %llu", static_cast<unsigned long long>(m_clock.total_ticks()));

    int hz = m_clock.hz();
    const SimClockConfig& clock_config = m_clock.config();

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Hz"); ImGui::SameLine();
    if (ImGui::SliderInt("##hz", &hz, clock_config.min_hz, clock_config.max_hz)) m_clock.set_hz(hz);

    ImGui::SeparatorText("Controls");

    if (ImGui::Button(m_clock.paused() ? "Resume [SPACE]" : "Pause [SPACE]")) m_clock.toggle_pause();
    ImGui::SameLine();
    ImGui::BeginDisabled(!m_clock.paused());
    if (ImGui::Button("Step [ENTER]")) m_clock.request_step();
    ImGui::EndDisabled();

    ImGui::End();
    rlImGuiEnd();
}
