#include "scenarios/life_like/scenario.hpp"
#include "scenarios/life_like/rule.hpp"
#include "ui/theme.hpp"

#include <imgui.h>

#include <random>

LifeLikeScenario::LifeLikeScenario(const LifeLikeConfig& config)
    : m_config(config),
      m_front_grid(config.grid_width, config.grid_height),
      m_back_grid(config.grid_width, config.grid_height),
      m_renderer(config.grid_width, config.grid_height),
      m_rng(std::random_device{}())
{
    randomize_grid();
}

void LifeLikeScenario::handle_input() {
    ImGuiIO& io = ImGui::GetIO();

    if (!io.WantTextInput) {
        if (IsKeyPressed(KEY_R))  randomize_grid();
        if (IsKeyPressed(KEY_C)) clear_grid();
    }

    // Mouse painting
    if (!io.WantCaptureMouse && (IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_RIGHT_BUTTON))) {
        size_t grid_x = 0;
        size_t grid_y = 0;

        if (m_renderer.screen_to_grid(GetMouseX(), GetMouseY(), grid_x, grid_y)) {
            uint8_t state = IsMouseButtonDown(MOUSE_LEFT_BUTTON) ? 1 : 0;

            m_front_grid(grid_x, grid_y) = state;
            m_back_grid(grid_x, grid_y) = state;
            m_dirty = true;
        }
    }
}

void LifeLikeScenario::tick() {
    for (size_t y = 0; y < m_front_grid.height(); ++y) {
        for (size_t x = 0; x < m_front_grid.width(); ++x) {
            int alive_neighbors = 0;

            for (int j = -1; j <= 1; ++j) {
                for (int i = -1; i <= 1; ++i) {
                    if (i == 0 && j == 0) continue;
                    alive_neighbors += m_front_grid.at(static_cast<int>(x) + i, static_cast<int>(y) + j);
                }
            }

            m_back_grid(x, y) = m_front_grid(x, y)
                ? m_rule.can_survive(alive_neighbors)
                : m_rule.can_birth(alive_neighbors);
        }
    }

    std::swap(m_front_grid, m_back_grid);
    m_dirty = true;
}

void LifeLikeScenario::render() {
    if (m_dirty) {
        m_renderer.update(m_front_grid, [](uint8_t cell) { return cell ? theme::fg0 : theme::bg0; });
        m_dirty = false;
    }

    m_renderer.render();
}

void LifeLikeScenario::draw_ui() {
    if (ImGui::Button("Randomize [R]")) randomize_grid();
    ImGui::SameLine();
    if (ImGui::Button("Clear [C]")) clear_grid();

    ImGui::SeparatorText("Rules");

    if (ImGui::Button("Load Preset")) {
        ImGui::OpenPopup("preset_popup");
    }

    if (ImGui::BeginPopup("preset_popup")) {
        for (const auto& preset : presets::presets_arr) {
            if (ImGui::Selectable(preset.name)) {
                m_rule = preset.rule;
                m_dirty = true;
            }
        }

        ImGui::EndPopup();
    }

    ImGui::Spacing();

    draw_toggle_row("B:", m_rule.birth);
    draw_toggle_row("S:", m_rule.survival);
}

void LifeLikeScenario::randomize_grid() {
    std::bernoulli_distribution dist(m_config.random_alive_rate);

    for (size_t y = 0; y < m_front_grid.height(); ++y) {
        for (size_t x = 0; x < m_front_grid.width(); ++x) {
            m_front_grid(x, y) = dist(m_rng) ? 1 : 0;
        }
    }

    m_dirty = true;
}

void LifeLikeScenario::clear_grid() {
    m_front_grid.fill(0);
    m_back_grid.fill(0);
    m_dirty = true;
}

void LifeLikeScenario::draw_toggle_row(const char* label, uint16_t& mask) {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", label);

    for (int i = 0; i <= 8; ++i) {
        ImGui::SameLine();
        bool is_set = mask & (1 << i);

        if (is_set) {
            ImGui::PushStyleColor(ImGuiCol_Button, theme::to_imvec(theme::bright_blue));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, theme::to_imvec(theme::blue));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, theme::to_imvec(theme::blue));
            ImGui::PushStyleColor(ImGuiCol_Text, theme::to_imvec(theme::bg0_hard));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, theme::to_imvec(theme::fg1, 0.1f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, theme::to_imvec(theme::fg1, 0.2f));
        }

        char id[16];
        snprintf(id, sizeof(id), "%d##%s", i, label);

        if (ImGui::Button(id, ImVec2(22, 22))) mask ^= (1 << i);

        if (is_set) ImGui::PopStyleColor(4);
        else ImGui::PopStyleColor(3);
    }
}
