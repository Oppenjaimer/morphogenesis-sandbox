#include "core/sim_clock.hpp"
#include <algorithm>

SimClock::SimClock(const SimClockConfig& config) : m_config(config), m_hz(config.initial_hz) {
    // Ensure value within config bounds
    set_hz(m_hz);
}

int SimClock::advance(double frame_time) {
    if (m_paused) {
        if (m_step_requested) {
            m_step_requested = false;
            ++m_total_ticks;

            return 1;
        }

        return 0;
    }

    // Clamp large frame rates to avoid spiral of death
    m_accumulator += std::min(frame_time, m_config.max_frame_time);

    const double dt = 1.0 / m_hz;
    int steps = 0;

    while (m_accumulator >= dt && steps < m_config.max_steps_per_frame) {
        m_accumulator -= dt;
        ++steps;
    }

    // Skip catching up if max number of steps per frame reached
    if (steps == m_config.max_steps_per_frame) m_accumulator = 0.0;

    m_total_ticks += static_cast<std::uint64_t>(steps);

    return steps;
}

float SimClock::alpha() const {
    // alpha = accumulator / dt
    return m_paused ? 1.0f : static_cast<float>(m_accumulator * m_hz);
}

void SimClock::toggle_pause() { m_paused = !m_paused; }
void SimClock::request_step() { m_step_requested = true; }

bool SimClock::paused() const { return m_paused; }
int SimClock::hz() const { return m_hz; }
std::uint64_t SimClock::total_ticks() const { return m_total_ticks; }
const SimClockConfig& SimClock::config() const { return m_config; }

void SimClock::set_hz(int new_hz) { m_hz = std::clamp(new_hz, m_config.min_hz, m_config.max_hz); }
