// Advent of Code 2024
// Day 5: Print Queue
// https://adventofcode.com/2024/day/5

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <gsl/util>

static bool readFile(const std::string& fileName, std::vector<std::string>& lines)
{
    std::ifstream in{fileName};
    if (!in) {
        std::cerr << "Cannot open file " << fileName << '\n';
        return false;
    }
    auto closeStream = gsl::finally([&in] { in.close(); });
    std::string str;
    while (std::getline(in, str)) {
        lines.push_back(str);
    }
    return true;
}

using Rules = std::multimap<uint32_t, uint32_t>;
using Update = std::vector<uint32_t>;
using Updates = std::vector<Update>;

std::pair<Rules, Updates> toRulesAndUpdates(const std::vector<std::string>& lines)
{
    Rules rules;
    Updates updates;
    bool isUpdates{false};
    for (const auto& line : lines) {
        if (line.empty()) {
            isUpdates = true;
            continue;
        }
        std::istringstream iss{line};
        if (isUpdates) {
            Update update;
            uint32_t p{};
            while (iss >> p) {
                update.push_back(p);
                iss.ignore(1);
            }
            updates.push_back(update);

        } else {
            uint32_t i{};
            uint32_t j{};
            iss >> i;
            iss.ignore(1);
            iss >> j;
            rules.emplace(j, i);
        }
    }
    return {rules, updates};
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    std::vector<std::string> lines{};
    if (!readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    auto [rules, updates] = toRulesAndUpdates(lines);

    const auto sorted = [&rules](const Update& update) {
        for (size_t i = 0; i < update.size() - 1; ++i) {
            const auto& rule = rules.equal_range(update[i]);
            for (size_t j = i + 1; j < update.size(); ++j) {
                for (auto it = rule.first; it != rule.second; ++it) {
                    if (update[j] == it->second) {
                        return false;
                    }
                }
            }
        }
        return true;
    };

    {  // Part 1
        uint64_t sum{};
        for (const auto& update : updates) {
            if (sorted(update)) {
                sum += update[update.size() / 2];
            }
        }
        std::cout << sum << '\n';
    }
    {  // Part 2
        const auto compare = [&rules](const uint32_t a, const uint32_t b) {
            const auto& rule = rules.equal_range(b);
            for (auto it = rule.first; it != rule.second; ++it) {
                if (a == it->second) {
                    return true;
                }
            }
            return false;
        };
        uint64_t sum{};
        for (auto& update : updates) {
            if (sorted(update)) {
                continue;
            }
            std::nth_element(update.begin(), update.begin() + update.size() / 2, update.end(), compare);
            sum += update[update.size() / 2];
        }
        std::cout << sum << '\n';
    }

    return EXIT_SUCCESS;
}
