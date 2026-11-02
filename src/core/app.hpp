#pragma once

#include "core/scenario.hpp"
#include "core/sim_clock.hpp"
#include "core/window.hpp"

#include <raylib.h>

#include <memory>

struct AppConfig {
    WindowConfig window_config;
    SimClockConfig clock_config;
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

    AppConfig m_config;
    Window m_window;
    SimClock m_clock;

    std::unique_ptr<Scenario> m_scenario;
    bool m_show_ui = true;
};
