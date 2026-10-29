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

    // Call once per rendered frame, returns how many sim ticks to run
    int advance(double frame_time);

    // Fraction of the way to the next tick for render interpolation
    float alpha() const;

    void toggle_pause();
    void request_step();

    bool paused() const;
    int hz() const;
    std::uint64_t total_ticks() const;
    const SimClockConfig& config() const;

    void set_hz(int hz);

private:
    SimClockConfig m_config;

    std::uint64_t m_total_ticks = 0;
    double m_accumulator = 0.0;
    int m_hz;

    bool m_paused = false;
    bool m_step_requested = false;
};
