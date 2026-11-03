#pragma once

#include "scenarios/life_like/rle.hpp"

#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <vector>

struct PatternEntry {
    std::string filename;
    std::optional<Pattern> pattern;
    std::string error;
};

class PatternLibrary {
public:
    void fetch(const std::filesystem::path& directory);

    std::span<const PatternEntry> entries() const;
    const std::string& error() const;

private:
    std::vector<PatternEntry> m_entries;
    std::string m_error;
};
