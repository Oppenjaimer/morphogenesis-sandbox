#pragma once

#include "core/sim_clock.hpp"
#include "core/test_sim.hpp"

struct AppConfig {
    int window_width = 1280;
    int window_height = 720;
    const char* window_title = "Morphogenesis Sandbox";

    SimClockConfig clock_config;
};

class App {
public:
    explicit App(const AppConfig& config = {});
    ~App();

    // Remove copy semantics to avoid double-closing the window
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    void run();

private:
    void handle_input();
    void update(double frame_time);
    void render();
    void draw_ui();

    AppConfig m_config;
    SimClock m_clock;
    TestSim m_sim;

    bool m_interpolate = true;
};
