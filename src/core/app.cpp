#include "core/app.hpp"
#include "core/scenario.hpp"
#include "core/sim_clock.hpp"
#include "scenarios/life_like/scenario.hpp"
#include "ui/theme.hpp"

#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>

App::App(const AppConfig& config)
    : m_config(config),
      m_window(config.window_config),
      m_clock(config.clock_config)
{
    m_scenario = std::make_unique<LifeLikeScenario>();
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
    if (!ImGui::GetIO().WantTextInput) {
        if (IsKeyPressed(KEY_S)) m_show_ui = !m_show_ui;
        if (IsKeyPressed(KEY_SPACE)) m_clock.toggle_pause();
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressedRepeat(KEY_ENTER)) m_clock.request_step();
    }

    m_scenario->handle_input();
}

void App::update(double frame_time) {
    const int steps = m_clock.advance(frame_time);

    for (int step = 0; step < steps; ++step) {
        m_scenario->tick();
    }
}

void App::render() {
    m_scenario->render();
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

    m_scenario->draw_ui();

    ImGui::End();
    rlImGuiEnd();
}
