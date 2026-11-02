#pragma once

#include <cstdint>
#include <string_view>

struct Rule {
    uint16_t birth = 0;
    uint16_t survival = 0;

    constexpr bool operator==(const Rule&) const = default;

    constexpr bool can_birth(int neighbors) const { return birth & (1 << neighbors); }
    constexpr bool can_survive(int neighbors) const { return survival & (1 << neighbors); }

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

namespace presets {
    inline constexpr Rule game_of_life       = Rule::from_golly("B3/S23");
    inline constexpr Rule replicator         = Rule::from_golly("B1357/S1357");
    inline constexpr Rule seeds              = Rule::from_golly("B2/S");
    inline constexpr Rule life_without_death = Rule::from_golly("B3/S012345678");
    inline constexpr Rule high_life          = Rule::from_golly("B36/S23");
    inline constexpr Rule day_and_night      = Rule::from_golly("B3678/S34678");
    inline constexpr Rule maze               = Rule::from_golly("B3/S12345");
    inline constexpr Rule anneal             = Rule::from_golly("B4678/S35678");

    struct Preset {
        const char* name;
        Rule rule;
    };

    inline constexpr Preset presets_arr[] = {
        {"Game of Life", game_of_life}, {"Replicator", replicator},
        {"Seeds", seeds}, {"Life without Death", life_without_death},
        {"HighLife", high_life}, {"Day and Night", day_and_night},
        {"Maze", maze}, {"Anneal", anneal}
    };
}
