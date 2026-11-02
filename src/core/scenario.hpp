#pragma once

class Scenario {
public:
    virtual ~Scenario() = default;

    virtual void handle_input() = 0;
    virtual void tick() = 0;
    virtual void render() = 0;
    virtual void draw_ui() = 0;
};
