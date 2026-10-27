#pragma once

#include <raylib.h>

namespace theme {
    // Background
    inline const Color bg0_hard = {29, 32, 33, 255};
    inline const Color bg0 = {40, 40, 40, 255};
    inline const Color bg1 = {60, 56, 54, 255};
    inline const Color bg2 = {80, 73, 69, 255};
    inline const Color bg3 = {102, 92, 84, 255};
    inline const Color bg4 = {124, 111, 100, 255};

    // Foreground
    inline const Color fg4 = {168, 153, 132, 255};
    inline const Color fg3 = {189, 174, 147, 255};
    inline const Color fg2 = {213, 196, 161, 255};
    inline const Color fg1 = {235, 219, 178, 255};
    inline const Color fg0 = {251, 241, 199, 255};

    // Normal colors
    inline const Color red = {204, 36, 29, 255};
    inline const Color green = {152, 151, 26, 255};
    inline const Color yellow = {215, 153, 33, 255};
    inline const Color blue = {69, 133, 136, 255};
    inline const Color purple = {177, 98, 134, 255};
    inline const Color aqua = {104, 157, 106, 255};
    inline const Color orange = {214, 93, 14, 255};
    inline const Color gray = {146, 131, 116, 255};

    // Bright colors
    inline const Color bright_red = {251, 73, 52, 255};
    inline const Color bright_green = {184, 187, 38, 255};
    inline const Color bright_yellow = {250, 189, 47, 255};
    inline const Color bright_blue = {131, 165, 152, 255};
    inline const Color bright_purple = {211, 134, 155, 255};
    inline const Color bright_aqua = {142, 192, 124, 255};
    inline const Color bright_orange = {254, 128, 25, 255};
    inline const Color bright_gray = {168, 153, 132, 255};

    void apply_to_imgui();
}
