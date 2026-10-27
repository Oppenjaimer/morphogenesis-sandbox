#pragma once

#include <cstdint>

struct SimClockConfig {
    double max_frame_time = 0.25;
    int max_steps_per_frame = 16;
    int initial_hz = 30;
    int min_hz = 1;
    int max_hz = 120;
};

class SimClock {
public:
    explicit SimClock(const SimClockConfig& config = {});

    int advance(double frame_time); // Call once per rendered frame, returns how many sim ticks to run
    float alpha() const;            // Fraction of the way to the next tick for render interpolation
    void toggle_pause();
    void request_step();

    bool is_paused() const;
    int get_hz() const;
    std::uint64_t get_total_ticks() const;
    const SimClockConfig& get_config() const;

    void set_hz(int new_hz);

private:
    SimClockConfig config;

    std::uint64_t total_ticks = 0;
    double accumulator = 0.0;
    int hz;

    bool paused = false;
    bool step_requested = false;
};
