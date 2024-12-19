// Advent of Code 2024
// Day 19: Linen Layout
// https://adventofcode.com/2024/day/19

#include <fstream>
#include <iostream>
#include <map>
#include <set>
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

using Patterns = std::set<std::string>;

bool canBuild(const Patterns& patterns, const std::string& word, std::map<std::string, bool>& cache)
{
    if (auto it = cache.find(word); it != cache.end()) {
        return it->second;
    }
    if (patterns.find(word) != patterns.end()) {
        cache[word] = true;
        return true;
    }
    bool res = false;
    for (size_t i = 1; i < word.size(); ++i) {
        const auto left = word.substr(0, i);
        if (!canBuild(patterns, left, cache)) {
            continue;
        }
        const auto right = word.substr(i, word.size() - i);
        if (canBuild(patterns, right, cache)) {
            res = true;
            break;
        }
    }
    cache[word] = res;
    return res;
}

uint64_t countBuilds(const Patterns& patterns, const std::string& word, std::map<std::string, uint64_t>& cache)
{
    if (auto it = cache.find(word); it != cache.end()) {
        return it->second;
    }
    uint64_t count = 0;
    if (patterns.find(word) != patterns.end()) {
        ++count;
    }
    for (size_t i = 1; i < word.size(); ++i) {
        const auto left = word.substr(0, i);
        if (patterns.find(left) == patterns.end()) {
            continue;
        }
        if (countBuilds(patterns, left, cache) == 0) {
            continue;
        }
        const auto right = word.substr(i, word.size() - i);
        count += countBuilds(patterns, right, cache);
    }
    cache[word] = count;
    return count;
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

    Patterns patterns;
    {
        std::istringstream iss{lines[0]};
        std::string p;
        while (iss >> p) {
            if (p[p.size() - 1] == ',') {
                p = p.substr(0, p.size() - 1);
            }
            if (p.empty()) {
                continue;
            }
            patterns.insert(p);
        }
    }

    std::vector<size_t> goods;

    {  // Part 1
        uint16_t count = 0;
        std::map<std::string, bool> cache;
        for (size_t i = 2; i < lines.size(); ++i) {
            if (canBuild(patterns, lines[i], cache)) {
                goods.push_back(i);
                ++count;
            }
        }
        std::cout << count << '\n';
    }
    {  // Part 2
        std::map<std::string, uint64_t> cache;
        uint64_t count = 0;
        for (const auto i : goods) {
            count += countBuilds(patterns, lines[i], cache);
        }
        std::cout << count << '\n';
    }

    return EXIT_SUCCESS;
}
