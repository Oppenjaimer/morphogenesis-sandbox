#pragma once

#include "core/sim_clock.hpp"
#include "core/window.hpp"
#include "grid/grid.hpp"
#include "grid/grid_renderer.hpp"
#include "scenarios/life_like/rule.hpp"

#include <raylib.h>

#include <cstdint>

struct AppConfig {
    WindowConfig window_config;
    SimClockConfig clock_config;

    int grid_width = 320;
    int grid_height = 180;
};

class App {
public:
    explicit App(const AppConfig& config = {});

    void run();

private:
    void handle_input();
    void update(double frame_time);
    void render();
    void draw_ui();

    void randomize_grid();
    void clear_grid();

    // Draw B/S rule editor, returns whether a button was clicked
    bool draw_toggle_row(const char* label, uint16_t& mask);

    AppConfig m_config;
    Window m_window;
    SimClock m_clock;
    Grid<uint8_t> m_front_grid;
    Grid<uint8_t> m_back_grid;
    GridRenderer m_renderer;
    Rule m_rule = presets::game_of_life;

    bool m_show_ui = true;
    int m_preset_idx = 0;
};
