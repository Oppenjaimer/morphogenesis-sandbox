#include "core/test_sim.hpp"

void TestSim::tick() {
    prev_angle = angle;
    angle += ANGLE_PER_TICK;
}

float TestSim::get_angle() const { return angle; }
float TestSim::get_prev_angle() const { return prev_angle; }
