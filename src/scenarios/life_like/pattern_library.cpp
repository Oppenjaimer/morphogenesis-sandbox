#include "scenarios/life_like/pattern_library.hpp"
#include "scenarios/life_like/rle.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <system_error>

namespace {
    constexpr std::uintmax_t max_file_bytes = 1u << 20; // 1 MiB

    std::string lowercase(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::tolower(c); });
        return str;
    }

    PatternEntry load_entry(const std::filesystem::path& path) {
        PatternEntry entry;
        entry.filename = path.filename().string();

        std::error_code ec;
        const uintmax_t size = std::filesystem::file_size(path, ec);

        if (ec) {
            entry.error = "Unable to read file: " + ec.message();
            return entry;
        }

        if (size > max_file_bytes) {
            entry.error = "File is larger than 1 MiB";
            return entry;
        }

        std::ifstream file(path, std::ios::binary);
        if (!file) {
            entry.error = "Unable to open file";
            return entry;
        }

        std::ostringstream contents;
        contents << file.rdbuf();

        entry.pattern = rle::parse(contents.str(), &entry.error);
        if (entry.pattern && entry.pattern->name.empty())
            entry.pattern->name = path.stem().string();

        return entry;
    }
}

void PatternLibrary::fetch(const std::filesystem::path& directory) {
    m_entries.clear();
    m_error.clear();

    std::error_code ec;
    std::filesystem::directory_iterator it(directory, ec);
    if (ec) {
        m_error = "Unable to read " + directory.string() + ": " + ec.message();
        return;
    }

    const std::filesystem::directory_iterator end;
    for (; it != end; it.increment(ec)) {
        std::error_code file_ec;
        if (!it->is_regular_file(file_ec)) continue;
        if (lowercase(it->path().extension().string()) != ".rle") continue;

        m_entries.push_back(load_entry(it->path()));
    }

    if (ec) m_error = "Error while listing " + directory.string() + ": " + ec.message();

    std::ranges::sort(m_entries, {}, &PatternEntry::filename);
}

std::span<const PatternEntry> PatternLibrary::entries() const { return m_entries; }
const std::string& PatternLibrary::error() const { return m_error; }
