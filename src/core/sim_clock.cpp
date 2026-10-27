#include "core/sim_clock.hpp"
#include <algorithm>

SimClock::SimClock(const SimClockConfig& config) : config(config), hz(config.initial_hz) {
    set_hz(hz); // Ensurevalue within config bounds
}

int SimClock::advance(double frame_time) {
    if (paused) {
        if (step_requested) {
            step_requested = false;
            ++total_ticks;

            return 1;
        }

        return 0;
    }

    // Clamp large frame rates to avoid spiral of death
    accumulator += std::min(frame_time, config.max_frame_time);

    const double dt = 1.0 / hz;
    int steps = 0;

    while (accumulator >= dt && steps < config.max_steps_per_frame) {
        accumulator -= dt;
        ++steps;
    }

    // Skip catching up if max number of steps per frame reached
    if (steps == config.max_steps_per_frame) accumulator = 0.0;

    total_ticks += static_cast<std::uint64_t>(steps);

    return steps;
}

float SimClock::alpha() const {
    // alpha = accumulator / dt
    return paused ? 1.0f : static_cast<float>(accumulator * hz);
}

void SimClock::toggle_pause() { paused = !paused; }
void SimClock::request_step() { step_requested = true; }

bool SimClock::is_paused() const { return paused; }
int SimClock::get_hz() const { return hz; }
std::uint64_t SimClock::get_total_ticks() const { return total_ticks; }
const SimClockConfig& SimClock::get_config() const { return config; }

void SimClock::set_hz(int new_hz) { hz = std::clamp(new_hz, config.min_hz, config.max_hz); }
