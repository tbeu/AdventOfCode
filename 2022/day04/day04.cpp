// Advent of Code 2022
// Day 4: Camp Cleanup
// https://adventofcode.com/2022/day/4

#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include <gsl/util>

using Range = std::pair<int32_t, int32_t>;
using RangePair = std::pair<Range, Range>;
using RangePairs = std::vector<RangePair>;

static bool readFile(const std::string& fileName, std::vector<std::string>& lines)
{
    std::ifstream in{fileName};
    if (!in) {
        std::cerr << "Cannot open file " << fileName << std::endl;
        return false;
    }
    auto closeStream = gsl::finally([&in] { in.close(); });
    std::string str;
    while (std::getline(in, str)) {
        lines.push_back(str);
    }
    return true;
}

RangePairs toRangePairs(const std::vector<std::string>& lines)
{
    RangePairs rangePairs{};
    for (const auto& line : lines) {
        RangePair rangePair{};
        std::sscanf(line.c_str(), "%d-%d,%d-%d", &rangePair.first.first, &rangePair.first.second,
                    &rangePair.second.first, &rangePair.second.second);
        rangePairs.emplace_back(rangePair);
    }
    return rangePairs;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    RangePairs rangePairs{toRangePairs(lines)};
    {  // Part 1
        auto contain = [](const auto& rangePair) {
            const auto [x1, y1] = rangePair.first;
            const auto [x2, y2] = rangePair.second;
            return (x1 <= x2 && y2 <= y1) || (x2 <= x1 && y1 <= y2);
        };
        auto count = std::count_if(rangePairs.cbegin(), rangePairs.cend(), contain);
        std::cout << count << std::endl;
    }
    {  // Part 2
        auto overlap = [](const auto& rangePair) {
            const auto [x1, y1] = rangePair.first;
            const auto [x2, y2] = rangePair.second;
            return x1 <= y2 && x2 <= y1;
        };
        auto count = std::count_if(rangePairs.cbegin(), rangePairs.cend(), overlap);
        std::cout << count << std::endl;
    }

    return EXIT_SUCCESS;
}
