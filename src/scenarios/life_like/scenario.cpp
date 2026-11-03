#include "scenarios/life_like/scenario.hpp"
#include "scenarios/life_like/rule.hpp"
#include "ui/theme.hpp"

#include <imgui.h>
#include <raylib.h>

#include <cstdint>
#include <filesystem>
#include <random>
#include <string>

namespace {
    const std::filesystem::path patterns_dir = std::filesystem::path(ASSETS_DIR) / "patterns";
}

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

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) || IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
        m_pattern_lock = false;

    if (!io.WantCaptureMouse) {
        size_t gx = 0;
        size_t gy = 0;

        if (m_renderer.screen_to_grid(GetMouseX(), GetMouseY(), gx, gy)) {
            if (m_active_pattern.has_value()) placement_mode(gx, gy);
            else if (!m_pattern_lock) painting_mode(gx, gy);
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
    draw_pattern_overlay();
}

void LifeLikeScenario::draw_ui() {
    // Controls
    if (ImGui::Button("Randomize [R]")) randomize_grid();
    ImGui::SameLine();
    if (ImGui::Button("Clear [C]")) clear_grid();

    ImGui::SeparatorText("Rules & Patterns");

    // Popup triggers
    if (ImGui::Button("Load Preset")) ImGui::OpenPopup("preset_popup");
    ImGui::SameLine();
    if (ImGui::Button("Place Pattern")) {
        m_pattern_library.fetch(patterns_dir);
        ImGui::OpenPopup("pattern_popup");
    }

    // Cancel placement button
    ImGui::SameLine();
    ImGui::BeginDisabled(!m_active_pattern.has_value());
    if (ImGui::Button("Cancel Placement")) m_active_pattern.reset();
    ImGui::EndDisabled();

    // Preset switching popup
    if (ImGui::BeginPopup("preset_popup")) {
        for (const auto& preset : presets::presets_arr) {
            if (ImGui::Selectable(preset.name)) {
                m_rule = preset.rule;
                m_dirty = true;
            }

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("%s", preset.rule.to_golly().c_str());
        }

        ImGui::EndPopup();
    }

    // Pattern placement popup
    if (ImGui::BeginPopup("pattern_popup")) {
        if (!m_pattern_library.error().empty()) {
            ImGui::TextDisabled("%s", m_pattern_library.error().c_str());
        } else if (m_pattern_library.entries().empty()) {
            ImGui::TextDisabled("No RLE files found in %s", patterns_dir.c_str());
        } else {
            for (const auto& entry : m_pattern_library.entries()) {
                if (!entry.error.empty()) {
                    ImGui::TextDisabled("%s (error)", entry.filename.c_str());
                    if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", entry.error.c_str());
                } else {
                    // Check rules mismatch
                    bool rule_mismatch = false;
                    if (entry.pattern.has_value() && !entry.pattern->rule.empty()) {
                        Rule pattern_rule = Rule::from_golly(entry.pattern->rule);
                        if (pattern_rule != m_rule) rule_mismatch = true;
                    }

                    if (rule_mismatch)
                        ImGui::PushStyleColor(ImGuiCol_Text, theme::to_imvec(theme::bright_yellow));

                    if (ImGui::Selectable(entry.filename.c_str()))
                        m_active_pattern = entry.pattern;

                    if (rule_mismatch) ImGui::PopStyleColor();

                    if (ImGui::IsItemHovered() && entry.pattern.has_value()) {
                        ImGui::BeginTooltip();

                        if (!entry.pattern->name.empty())
                            ImGui::Text("%s", entry.pattern->name.c_str());

                        if (rule_mismatch) {
                            if (!entry.pattern->name.empty()) ImGui::Separator();
                            ImGui::TextColored(
                                theme::to_imvec(theme::bright_yellow),
                                "Pattern uses rule %s", entry.pattern->rule.c_str()
                            );
                        }

                        if (!entry.pattern->description.empty()) {
                            if (!entry.pattern->name.empty()) ImGui::Separator();
                            ImGui::TextDisabled("%s", entry.pattern->description.c_str());
                        }

                        ImGui::EndTooltip();
                    }
                }
            }
        }

        ImGui::EndPopup();
    }

    ImGui::Spacing();

    // Rule toggles
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

void LifeLikeScenario::draw_pattern_overlay() {
    if (!m_active_pattern.has_value() || ImGui::GetIO().WantCaptureMouse) return;

    size_t gx = 0;
    size_t gy = 0;

    if (!m_renderer.screen_to_grid(GetMouseX(), GetMouseY(), gx, gy)) return;

    Color color = theme::fg0;
    color.a = 128;

    for (int py = 0; py < m_active_pattern->height(); ++py) {
        for (int px = 0; px < m_active_pattern->width(); ++px) {
            if (m_active_pattern->alive(px, py)) {
                if (gx + px < m_front_grid.width() && gy + py < m_front_grid.height()) {
                    DrawRectangleRec(m_renderer.cell_rect(gx + px, gy + py), color);
                }
            }
        }
    }
}

void LifeLikeScenario::placement_mode(size_t gx, size_t gy) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        // Place pattern
        for (int py = 0; py < m_active_pattern->height(); ++py) {
            for (int px = 0; px < m_active_pattern->width(); ++px) {
                if (!m_active_pattern->alive(px, py)) continue;

                size_t tx = gx + px;
                size_t ty = gy + py;

                if (tx >= m_front_grid.width() || ty >= m_front_grid.height()) continue;

                m_front_grid(tx, ty) = 1;
                m_back_grid(tx, ty) = 1;
            }
        }

        m_dirty = true;
    } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        // Exit placement mode
        m_active_pattern.reset();
        m_pattern_lock = true; // Prevent fallthrough erase
    }
}

void LifeLikeScenario::painting_mode(size_t gx, size_t gy) {
    if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) return;

    uint8_t state = IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? 1 : 0;
    m_front_grid(gx, gy) = state;
    m_back_grid(gx, gy) = state;
    m_dirty = true;
}
