#pragma once

class TestSim {
public:
    void tick();

    float angle() const;
    float prev_angle() const;

private:
    static constexpr float ANGLE_PER_TICK = 0.12f;

    float m_angle = 0.0f;
    float m_prev_angle = 0.0f;
};
