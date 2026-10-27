#pragma once

class TestSim {
public:
    void tick();

    float get_angle() const;
    float get_prev_angle() const;

private:
    static constexpr float ANGLE_PER_TICK = 0.12f;

    float angle = 0.0f;
    float prev_angle = 0.0f;
};
