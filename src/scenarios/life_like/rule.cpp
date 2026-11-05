#include "scenarios/life_like/rule.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

std::vector<Preset> presets::load(const std::filesystem::path& path) {
    std::vector<Preset> presets;
    std::ifstream file(path);
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line.front() == '#') continue;

        size_t delim = line.find('=');
        if (delim == std::string::npos) continue;

        std::string name = line.substr(0, delim);
        std::string rule = line.substr(delim + 1);

        // Trim
        while (!name.empty() && std::isspace(name.back())) name.pop_back();
        while (!name.empty() && std::isspace(name.front())) name.erase(0, 1);

        presets.push_back({name, Rule::from_golly(rule)});
    }

    return presets;
}
