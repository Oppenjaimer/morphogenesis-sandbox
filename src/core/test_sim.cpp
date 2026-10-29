#include "core/test_sim.hpp"

void TestSim::tick() {
    m_prev_angle = m_angle;
    m_angle += ANGLE_PER_TICK;
}

float TestSim::angle() const { return m_angle; }
float TestSim::prev_angle() const { return m_prev_angle; }
