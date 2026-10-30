#pragma once

#include "core/sim_clock.hpp"
#include "core/window.hpp"
#include "grid/grid.hpp"
#include "grid/grid_renderer.hpp"

#include <raylib.h>

#include <cstdint>

struct AppConfig {
    WindowConfig window_config;
    SimClockConfig clock_config;

    int grid_width = 200;
    int grid_height = 112;
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

    AppConfig m_config;
    Window m_window;
    SimClock m_clock;
    Grid<uint8_t> m_front_grid;
    Grid<uint8_t> m_back_grid;
    GridRenderer m_renderer;

    bool m_show_ui = true;
};
