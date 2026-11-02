#pragma once

#include "core/scenario.hpp"
#include "grid/grid.hpp"
#include "grid/grid_renderer.hpp"
#include "scenarios/life_like/rule.hpp"

#include <cstdint>
#include <random>

struct LifeLikeConfig {
    int grid_width = 320;
    int grid_height = 180;
    float random_alive_rate = 0.2f; // 20% chance for a cell to be alive when randomizing grid
};

class LifeLikeScenario final : public Scenario {
public:
    explicit LifeLikeScenario(const LifeLikeConfig& config = {});

    void handle_input() override;
    void tick() override;
    void render() override;
    void draw_ui() override;

private:
    void randomize_grid();
    void clear_grid();
    void draw_toggle_row(const char* label, uint16_t& mask);

    LifeLikeConfig m_config;
    Grid<uint8_t> m_front_grid;
    Grid<uint8_t> m_back_grid;
    GridRenderer m_renderer;
    Rule m_rule = presets::game_of_life;

    std::mt19937 m_rng;

    int m_preset_idx = 0;
    bool m_dirty = true; // Flag for grid GPU upload when modified only
};
