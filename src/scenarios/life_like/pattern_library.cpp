#include "scenarios/life_like/pattern_library.hpp"
#include "scenarios/life_like/rle.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <system_error>

namespace {
    constexpr std::uintmax_t max_file_bytes = 1u << 20; // 1 MiB
    constexpr int max_depth = 8; // Folder levels below patterns directory

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

    PatternFolder& folder_at(PatternFolder& root, const std::filesystem::path& relative) {
        PatternFolder* folder = &root;

        for (const auto& part : relative) {
            const std::string name = part.string();
            if (name.empty() || name == ".") continue;

            const auto it = std::ranges::find(folder->folders, name, &PatternFolder::name);
            if (it != folder->folders.end()) {
                folder = &*it;
            } else {
                folder = &folder->folders.emplace_back();
                folder->name = name;
            }
        }

        return *folder;
    }

    void sort_folder(PatternFolder& folder) {
        std::ranges::sort(folder.folders, {}, &PatternFolder::name);
        std::ranges::sort(folder.entries, {}, &PatternEntry::filename);

        for (auto& sub : folder.folders) sort_folder(sub);
    }
}

void PatternLibrary::fetch(const std::filesystem::path& directory) {
    m_root = {};
    m_error.clear();

    std::error_code ec;
    std::filesystem::recursive_directory_iterator it(directory, ec);
    if (ec) {
        m_error = "Unable to read " + directory.string() + ": " + ec.message();
        return;
    }

    const std::filesystem::recursive_directory_iterator end;
    for (; it != end; it.increment(ec)) {
        if (it.depth() >= max_depth) it.disable_recursion_pending();

        std::error_code file_ec;
        if (!it->is_regular_file(file_ec)) continue;
        if (lowercase(it->path().extension().string()) != ".rle") continue;

        const auto relative = it->path().parent_path().lexically_relative(directory);
        folder_at(m_root, relative).entries.push_back(load_entry(it->path()));
    }

    if (ec) m_error = "Error while listing " + directory.string() + ": " + ec.message();

    sort_folder(m_root);
}

const PatternFolder& PatternLibrary::root() const { return m_root; }
const std::string& PatternLibrary::error() const { return m_error; }
bool PatternLibrary::empty() const { return m_root.folders.empty() && m_root.entries.empty(); }
