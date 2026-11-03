#include "scenarios/life_like/rle.hpp"
#include "grid/grid.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>

namespace {
    constexpr int max_dimension = 4096;    // Limit on header's x and y
    constexpr int max_run = max_dimension; // Longest run count

    std::string_view trim(std::string_view str) {
        while (!str.empty() && std::isspace(static_cast<unsigned char>(str.front()))) str.remove_prefix(1);
        while (!str.empty() && std::isspace(static_cast<unsigned char>(str.back()))) str.remove_suffix(1);

        return str;
    }

    bool parse_int(std::string_view str, int& out) {
        const auto [end, ec] = std::from_chars(str.data(), str.data() + str.size(), out);
        return ec == std::errc{} && end == str.data() + str.size();
    }

    // Returns an empty string on success, or an error message on failure
    std::string parse_header(std::string_view line, int& width, int& height, std::string& rule) {
        std::string compact;
        for (const char c : line) {
            if (!std::isspace(static_cast<unsigned char>(c))) compact += c;
        }

        bool has_x = false;
        bool has_y = false;
        std::string_view rest = compact;

        while (!rest.empty()) {
            const size_t comma = rest.find(',');
            const std::string_view field = rest.substr(0, comma);
            rest = (comma == std::string_view::npos) ? std::string_view{} : rest.substr(comma + 1);

            const size_t equals = field.find('=');
            if (equals == std::string_view::npos)
                return "Invalid header field '" + std::string(field) + "'";

            std::string key(field.substr(0, equals));
            std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c){ return std::tolower(c); });
            const std::string_view value = field.substr(equals + 1);

            if (key == "x") {
                if (!parse_int(value, width)) return "Invalid value for '" + key + "' in header";
                has_x = true;
            } else if (key == "y") {
                if (!parse_int(value, height)) return "Invalid value for '" + key + "' in header";
                has_y = true;
            } else if (key == "rule") {
                rule = std::string(value);
            }
        }

        if (!has_x || !has_y) return "Header must contain both x and y";
        if (width < 1 || height < 1 || width > max_dimension || height > max_dimension)
            return "Pattern size of " + std::to_string(width) + "x" + std::to_string(height) +
                   " not between 1x1 and " + std::to_string(max_dimension) + "x" + std::to_string(max_dimension);

        return {};
    }

    struct BodyState {
        int x = 0;
        int y = 0;
        int run = 0;
        bool has_run = false;
        bool done = false;
    };

    // Returns an empty string on success, or an error message on failure
    std::string decode_line(std::string_view line, Pattern& pattern, BodyState& state) {
        for (const char c : line) {
            if (state.done) break; // '!' reached
            if (std::isspace(static_cast<unsigned char>(c))) continue;

            if (c >= '0' && c <= '9') {
                state.run = state.run * 10 + (c - '0');
                state.has_run = true;

                if (state.run > max_run) return "Run length in pattern data too large";
                continue;
            }

            const int count = state.has_run ? state.run : 1;
            state.run = 0;
            state.has_run = false;

            switch (c) {
                case 'b':
                    state.x += count;
                    break;
                case 'o': {
                    if (state.y >= pattern.height() || state.x + count > pattern.width())
                        return "Live cells fall outside declared pattern size";

                    for (int i = 0; i < count; ++i) {
                        pattern.grid(state.x+ i, state.y) = 1;
                    }

                    state.x += count;
                    pattern.count += count;
                    break;
                }

                case '$':
                    state.y += count;
                    state.x = 0;
                    break;

                case '!':
                    state.done = true;
                    break;

                default:
                    return std::string("Unexpected character '") + c + "' in pattern data";
            }
        }

        return {};
    }
}

std::optional<Pattern> rle::parse(std::string_view text, std::string* error) {
    const auto fail = [error](std::string message) -> std::optional<Pattern> {
        if (error) *error = std::move(message);
        return std::nullopt;
    };

    Pattern pattern;
    BodyState state;
    bool has_header = false;

    while (!text.empty() && !state.done) {
        const size_t newline = text.find('\n');
        const std::string_view line = trim(text.substr(0, newline));
        text = (newline == std::string_view::npos) ? std::string_view{} : text.substr(newline + 1);

        if (line.empty()) continue;

        if (line.front() == '#') {
            if (line.size() < 2) continue;

            const std::string_view content = trim(line.substr(2));
            if (line[1] == 'N') {
                pattern.name = std::string(content);
            } else if (line[1] == 'C' || line[1] == 'c') {
                if (!pattern.description.empty()) pattern.description += '\n';
                pattern.description += content;
            }

            continue;
        }

        if (!has_header) {
            int width = 0;
            int height = 0;

            std::string message = parse_header(line, width, height, pattern.rule);
            if (!message.empty()) return fail(std::move(message));

            pattern.grid = Grid<uint8_t>(width, height);
            has_header = true;
            continue;
        }

        std::string message = decode_line(line, pattern, state);
        if (!message.empty()) return fail(std::move(message));
    }

    if (!has_header) return fail("Missing header line");
    if (!state.done) return fail("Missing '!' at the end of pattern data");
    if (pattern.count == 0) return fail("Pattern has no live cells");

    return pattern;
}
