#pragma once

#include "grid/grid.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

struct Pattern {
    std::string name;        // Name from #N line, or empty
    std::string description; // Comments from #C lines
    std::string rule;        // Rule specified by header
    Grid<uint8_t> grid;
    int count = 0;

    int width() const { return static_cast<int>(grid.width()); }
    int height() const { return static_cast<int>(grid.height()); }
    bool alive(int x, int y) const { return grid(x, y); }
};


namespace rle {
    // Parse RLE text, returning nullopt on failure and, if `error` is given, an error message
    // Supports two-state patterns, run counts, line ends, #N and #C lines, and wrapped lines
    std::optional<Pattern> parse(std::string_view text, std::string* error = nullptr);
}
