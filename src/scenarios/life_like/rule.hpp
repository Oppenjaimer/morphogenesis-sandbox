#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

struct Rule {
    uint16_t birth = 0;
    uint16_t survival = 0;

    constexpr bool operator==(const Rule&) const = default;
    constexpr bool operator!=(const Rule&) const = default;

    constexpr bool can_birth(int neighbors) const { return birth & (1 << neighbors); }
    constexpr bool can_survive(int neighbors) const { return survival & (1 << neighbors); }

    // Express rule in Golly (B/S) notation
    std::string to_golly() const {
        std::string out = "B";

        for (int i = 0; i <= 8; ++i) if (can_birth(i)) out += std::to_string(i);
        out += "/S";
        for (int i = 0; i <= 8; ++i) if (can_survive(i)) out += std::to_string(i);

        return out;
    }

    // Parse rule string using Golly (B/S) notation, like "B3/S23"
    static constexpr Rule from_golly(std::string_view str) {
        Rule rule;
        bool parsing_birth = true; // B comes first

        for (char c : str) {
            if (c == 'b' || c == 'B') parsing_birth = true;
            else if (c == 's' || c == 'S') parsing_birth = false;
            else if (c == '/') parsing_birth = false;
            else if (c >= '0' && c <= '8') {
                int neighbors = c - '0';
                if (parsing_birth) rule.birth |= (1 << neighbors);
                else rule.survival |= (1 << neighbors);
            }
        }

        return rule;
    }
};

struct Preset {
    std::string name;
    Rule rule;
};

namespace presets {
    // Barebones preset loader, the current format is just temporary
    std::vector<Preset> load(const std::filesystem::path& file);
}
