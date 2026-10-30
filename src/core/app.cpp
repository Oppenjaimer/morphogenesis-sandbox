#include "core/app.hpp"
#include "core/sim_clock.hpp"
#include "ui/theme.hpp"

#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>

#include <algorithm>

App::App(const AppConfig& config)
    : m_config(config),
      m_clock(config.clock_config),
      m_grid(config.grid_width, config.grid_height),
      m_pixel_buffer(config.grid_width * config.grid_height, theme::bg0)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(config.window_width, config.window_height, config.window_title);

    rlImGuiSetup(true);
    theme::apply_to_imgui();
    ImGui::GetIO().IniFilename = nullptr; // Disable imgui.ini

    Image image = GenImageColor(m_config.grid_width, m_config.grid_height, BLANK);
    m_texture = LoadTextureFromImage(image);
    UnloadImage(image);
    SetTextureFilter(m_texture, TEXTURE_FILTER_POINT);

    m_grid(50, 30) = 1;
    m_grid(51, 30) = 1;
    m_grid(52, 30) = 1;
    m_grid(52, 29) = 1;
    m_grid(51, 28) = 1;
}

App::~App() {
    UnloadTexture(m_texture);
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

    for (int i = 0; i < steps; ++i) {
        // TODO: Game of Life logic
    }

    // Map grid state to pixel buffer
    for (size_t y = 0; y < m_grid.height(); ++y) {
        for (size_t x = 0; x < m_grid.width(); ++x) {
            m_pixel_buffer[m_grid.index(x, y)] = m_grid(x, y) ? theme::fg1 : theme::bg0;
        }
    }

    UpdateTexture(m_texture, m_pixel_buffer.data());
}

void App::render() {
    // Calculate scale to fit window maintaining aspect ratio
    const float scale_x = static_cast<float>(GetScreenWidth()) / m_texture.width;
    const float scale_y = static_cast<float>(GetScreenHeight()) / m_texture.height;
    const float scale = std::min(scale_x, scale_y);

    // Center texture
    const float dest_width = m_texture.width * scale;
    const float dest_height = m_texture.height * scale;
    const float offset_x = (GetScreenWidth() - dest_width) * 0.5f;
    const float offset_y = (GetScreenHeight() - dest_height) * 0.5f;

    const Rectangle source = {0.0f, 0.0f, static_cast<float>(m_texture.width), static_cast<float>(m_texture.height)};
    const Rectangle dest = {offset_x, offset_y, dest_width, dest_height};
    const Vector2 origin = {0.0f, 0.0f};

    DrawTexturePro(m_texture, source, dest, origin, 0.0f, WHITE);
}

void App::draw_ui() {
    // TODO: create UI config struct
    rlImGuiBegin();
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::Begin("Sandbox", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    // TODO: use S to toggle UI collapse

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
