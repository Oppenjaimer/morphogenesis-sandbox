#pragma once

#include "scenarios/life_like/rle.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

struct PatternEntry {
    std::string filename;
    std::optional<Pattern> pattern;
    std::string error;
};

struct PatternFolder {
    std::string name; // Empty for root
    std::vector<PatternFolder> folders;
    std::vector<PatternEntry> entries;
};

class PatternLibrary {
public:
    // Scans the directory recursively, leaving out folders without any RLE files in them
    void fetch(const std::filesystem::path& directory);

    const PatternFolder& root() const;
    const std::string& error() const;
    bool empty() const;

private:
    PatternFolder m_root;
    std::string m_error;
};
