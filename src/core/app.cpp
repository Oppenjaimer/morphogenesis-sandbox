#include "core/app.hpp"
#include "core/sim_clock.hpp"
#include "ui/theme.hpp"

#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>

#include <cstdint>
#include <random>
#include <utility>

App::App(const AppConfig& config)
    : m_config(config),
      m_window(config.window_config),
      m_clock(config.clock_config),
      m_front_grid(config.grid_width, config.grid_height),
      m_back_grid(config.grid_width, config.grid_height),
      m_renderer(config.grid_width, config.grid_height)
{
    randomize_grid();
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
    if (IsKeyPressed(KEY_R))     randomize_grid();
}

void App::update(double frame_time) {
    const int steps = m_clock.advance(frame_time);

    for (int i = 0; i < steps; ++i) {
        for (size_t y = 0; y < m_front_grid.height(); ++y) {
            for (size_t x = 0; x < m_front_grid.width(); ++x) {
                int alive_neighbors = 0;

                for (int j = -1; j <= 1; ++j) {
                    for (int i = -1; i <= 1; ++i) {
                        if (i == 0 && j == 0) continue;
                        alive_neighbors += m_front_grid.at(static_cast<int>(x) + i, static_cast<int>(y) + j);
                    }
                }

                if (m_front_grid(x, y) && (alive_neighbors == 2 || alive_neighbors == 3))
                    m_back_grid(x, y) = 1;
                else if (!m_front_grid(x, y) && alive_neighbors == 3)
                    m_back_grid(x, y) = 1;
                else
                    m_back_grid(x, y) = 0;
            }
        }

        std::swap(m_front_grid, m_back_grid);
    }

    m_renderer.update(m_front_grid, [](uint8_t cell) { return cell ? theme::fg0 : theme::bg0; });
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
    if (ImGui::Button("Randomize [R]")) randomize_grid();

    ImGui::End();
    rlImGuiEnd();
}

void App::randomize_grid() {
    std::mt19937 gen(std::random_device{}());
    std::bernoulli_distribution dist(0.2); // 20% chance for a cell to be alive

    for (size_t y = 0; y < m_front_grid.height(); ++y) {
        for (size_t x = 0; x < m_front_grid.width(); ++x) {
            m_front_grid(x, y) = dist(gen) ? 1 : 0;
        }
    }
}
