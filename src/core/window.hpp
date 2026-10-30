#pragma once

#include "ui/theme.hpp"

#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>

struct WindowConfig {
    int width = 1280;
    int height = 720;
    const char* title = "Morphogenesis Sandbox";
};

// RAII wrapper around the window context
class Window {
public:
    explicit Window(const WindowConfig& config = {}) : m_config(config) {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
        InitWindow(config.width, config.height, config.title);

        rlImGuiSetup(true);
        theme::apply_to_imgui();
        ImGui::GetIO().IniFilename = nullptr; // Disable imgui.ini
    }

    ~Window() {
        rlImGuiShutdown();
        CloseWindow();
    }

    // Remove copy semantics to avoid double-closing the window
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

private:
    WindowConfig m_config;
};
