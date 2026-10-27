#include "ui/theme.hpp"

#include <imgui.h>

namespace {
    // Convert Color (0-255) to ImVec4 (0.0f-1.0f).
    ImVec4 to_imvec(const Color& c) {
        return ImVec4(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f);
    }
}

void theme::apply_to_imgui() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Backgrounds
    colors[ImGuiCol_WindowBg] = to_imvec(bg0);
    colors[ImGuiCol_ChildBg] = to_imvec(bg0_hard);
    colors[ImGuiCol_PopupBg] = to_imvec(bg1);

    // Text
    colors[ImGuiCol_Text] = to_imvec(fg1);
    colors[ImGuiCol_TextDisabled] = to_imvec(bg4);
    colors[ImGuiCol_TextSelectedBg] = to_imvec(bg4);

    // Borders
    colors[ImGuiCol_Border] = to_imvec(bg2);
    colors[ImGuiCol_BorderShadow] = to_imvec(bg0_hard);

    // Frames
    colors[ImGuiCol_FrameBg] = to_imvec(bg1);
    colors[ImGuiCol_FrameBgHovered] = to_imvec(bg2);
    colors[ImGuiCol_FrameBgActive] = to_imvec(bg3);

    // Titles
    colors[ImGuiCol_TitleBg] = to_imvec(bg1);
    colors[ImGuiCol_TitleBgActive] = to_imvec(bg2);
    colors[ImGuiCol_TitleBgCollapsed] = to_imvec(bg1);

    // Buttons
    colors[ImGuiCol_Button] = to_imvec(bg2);
    colors[ImGuiCol_ButtonHovered] = to_imvec(bg3);
    colors[ImGuiCol_ButtonActive] = to_imvec(bg4);

    // Headers
    colors[ImGuiCol_Header] = to_imvec(bg2);
    colors[ImGuiCol_HeaderHovered] = to_imvec(bg3);
    colors[ImGuiCol_HeaderActive] = to_imvec(bg4);

    // Tabs
    colors[ImGuiCol_Tab] = to_imvec(bg1);
    colors[ImGuiCol_TabHovered] = to_imvec(bg3);
    colors[ImGuiCol_TabActive] = to_imvec(bg2);
    colors[ImGuiCol_TabUnfocused] = to_imvec(bg1);
    colors[ImGuiCol_TabUnfocusedActive] = to_imvec(bg2);

    // Scrollbars
    colors[ImGuiCol_ScrollbarBg] = to_imvec(bg0_hard);
    colors[ImGuiCol_ScrollbarGrab] = to_imvec(bg2);
    colors[ImGuiCol_ScrollbarGrabHovered] = to_imvec(bg3);
    colors[ImGuiCol_ScrollbarGrabActive] = to_imvec(bg4);

    // Accents
    colors[ImGuiCol_CheckMark] = to_imvec(bright_green);
    colors[ImGuiCol_SliderGrab] = to_imvec(bright_blue);
    colors[ImGuiCol_SliderGrabActive] = to_imvec(blue);
    colors[ImGuiCol_PlotLines] = to_imvec(bright_aqua);
    colors[ImGuiCol_PlotLinesHovered] = to_imvec(bright_orange);
    colors[ImGuiCol_PlotHistogram] = to_imvec(bright_yellow);
    colors[ImGuiCol_PlotHistogramHovered] = to_imvec(bright_orange);

    // Rounding
    style.WindowRounding = 4.0f;
    style.FrameRounding = 2.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 2.0f;
    style.GrabRounding = 2.0f;
    style.TabRounding = 2.0f;
}
