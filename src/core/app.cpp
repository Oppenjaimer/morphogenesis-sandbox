#include "core/app.hpp"
#include "core/sim_clock.hpp"
#include "ui/theme.hpp"

#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>

#include <algorithm>
#include <cmath>

App::App(const AppConfig& config) : m_config(config), m_clock(config.clock_config) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(config.window_width, config.window_height, config.window_title);

    rlImGuiSetup(true);
    theme::apply_to_imgui();
    ImGui::GetIO().IniFilename = nullptr; // Disable imgui.ini
}

App::~App() {
    rlImGuiShutdown();
    CloseWindow();
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
    if (IsKeyPressed(KEY_SPACE)) m_clock.toggle_pause();
    if (IsKeyPressed(KEY_ENTER)) m_clock.request_step();
}

void App::update(double frame_time) {
    const int steps = m_clock.advance(frame_time);
    for (int i = 0; i < steps; ++i) m_sim.tick();
}

void App::render() {
    const float cx = GetScreenWidth() * 0.5f;
    const float cy = GetScreenHeight() * 0.5f;
    const float radius = std::min(cx, cy) * 0.5f;

    const float angle = m_interpolate
        ? m_sim.prev_angle() + (m_sim.angle() - m_sim.prev_angle()) * m_clock.alpha()
        : m_sim.angle();

    DrawCircleLines(static_cast<int>(cx), static_cast<int>(cy), radius, theme::bg4);
    DrawCircle(
        static_cast<int>(cx + std::cos(angle) * radius),
        static_cast<int>(cy + std::sin(angle) * radius),
        12.0f, theme::fg0
    );
}

void App::draw_ui() {
    rlImGuiBegin();
    ImGui::Begin("Sandbox");

    ImGui::Text("FPS: %d (%.2f ms)", GetFPS(), GetFrameTime() * 1000.0f);
    ImGui::Separator();
    ImGui::Text("Sim ticks: %llu", static_cast<unsigned long long>(m_clock.total_ticks()));

    int hz = m_clock.hz();
    const SimClockConfig& clock_config = m_clock.config();

    if (ImGui::SliderInt("Sim Hz", &hz, clock_config.min_hz, clock_config.max_hz)) m_clock.set_hz(hz);

    ImGui::Checkbox("Interpolate", &m_interpolate);

    if (ImGui::Button(m_clock.paused() ? "Resume [SPACE]" : "Pause [SPACE]")) m_clock.toggle_pause();
    ImGui::SameLine();
    if (ImGui::Button("Step [ENTER]")) m_clock.request_step();

    ImGui::End();
    rlImGuiEnd();
}
