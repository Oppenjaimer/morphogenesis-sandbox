#include <cmath>
#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>

#include <algorithm>
#include <cstdint>

namespace {
    // Runs the simulation at a fixed rate regardless of the frame rate.
    class SimClock {
        public:
            // Call once per rendered frame. Returns how many sim ticks to run.
            int advance(double frame_time) {
                if (paused) {
                    if (step_requested) {
                        step_requested = false;
                        ++total_ticks;
                        return 1;
                    }

                    return 0;
                }

                // Clamp large frame rates to avoid spiral of death
                accumulator += std::min(frame_time, MAX_FRAME_TIME);

                const double dt = 1.0 / hz;
                const int max_steps = static_cast<int>(hz * MAX_FRAME_TIME);
                int steps = 0;

                while (accumulator >= dt && steps < max_steps) {
                    accumulator -= dt;
                    ++steps;
                }

                // Skip catching up
                if (steps == max_steps) accumulator = 0.0;

                total_ticks += static_cast<std::uint64_t>(steps);

                return steps;
            }

            // Fraction of the way to the next tick for render interpolation: accumulator / dt
            float alpha() const { return paused ? 1.0f : static_cast<float>(accumulator * hz); }

            void toggle_pause() { paused = !paused; }
            void request_step() { step_requested = true; }

            bool is_paused() const { return paused; }
            int get_hz() const { return hz; }
            std::uint64_t get_total_clicks() const { return total_ticks; }

            void set_hz(int new_hz) { hz = std::clamp(new_hz, 1, 240); }

        private:
            static constexpr double MAX_FRAME_TIME = 0.25;

            std::uint64_t total_ticks = 0;
            double accumulator = 0.0;
            int hz = 30;

            bool paused = false;
            bool step_requested = false;
    };

    // Test simulation state with an orbiting dot
    struct TestSim {
        static constexpr float ANGLE_PER_TICK = 0.12f;

        float angle = 0.0f;
        float prev_angle = 0.0f;

        void tick() {
            prev_angle = angle;
            angle += ANGLE_PER_TICK;
        }
    };
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(1280, 720, "Morphogenesis Sandbox");
    rlImGuiSetup(true);

    // Disable imgui.ini
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    SimClock clock;
    TestSim sim;
    bool interpolate = true;

    while (!WindowShouldClose()) {
        // Input
        if (IsKeyPressed(KEY_SPACE)) clock.toggle_pause();
        if (IsKeyPressed(KEY_ENTER)) clock.request_step();

        // Update
        const int steps = clock.advance(GetFrameTime());
        for (int i = 0; i < steps; i++) sim.tick();

        // Render
        BeginDrawing();
        ClearBackground(BLACK);

        const float cx = GetScreenWidth() * 0.5f;
        const float cy = GetScreenHeight() * 0.5f;
        const float radius = std::min(cx, cy) * 0.5f;

        const float angle = interpolate
            ? sim.prev_angle + (sim.angle - sim.prev_angle) * clock.alpha()
            : sim.angle;

        DrawCircleLines(static_cast<int>(cx), static_cast<int>(cy), radius, DARKGRAY);
        DrawCircle(cx + std::cos(angle) * radius, cy + std::sin(angle) * radius, 12.0f, WHITE);

        // UI
        rlImGuiBegin();
        ImGui::Begin("Sandbox");
        ImGui::Text("FPS: %d (%.2f ms)", GetFPS(), GetFrameTime() * 1000.0f);
        ImGui::Separator();
        ImGui::Text("Sim ticks: %llu", static_cast<unsigned long long>(clock.get_total_clicks()));

        int hz = clock.get_hz();
        if (ImGui::SliderInt("Sim Hz", &hz, 1, 240)) clock.set_hz(hz);
        ImGui::Checkbox("Interpolate", &interpolate);

        if (ImGui::Button(clock.is_paused() ? "Resume [SPACE]" : "Pause [SPACE]")) clock.toggle_pause();
        ImGui::SameLine();
        if (ImGui::Button("Step [ENTER]")) clock.request_step();
        ImGui::End();
        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
