// Advent of Code 2025
// Day 7: Laboratories
// https://adventofcode.com/2025/day/7

#include <array>
#include <fstream>
#include <iostream>
#include <map>
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

using Pos = std::array<uint16_t, 2>;
using Beams = std::map<Pos, uint64_t>;
using Grid = std::vector<std::string>;

uint32_t simulate(const Grid& grid, Beams& beams)
{
    Beams next{};
    uint32_t numSplits = 0;
    for (const auto [pos, count] : beams) {
        auto [r, c] = pos;
        if ('^' == grid[r + 1][c]) {
            ++numSplits;
            next[{static_cast<uint16_t>(r + 1), static_cast<uint16_t>(c - 1)}] += count;
            next[{static_cast<uint16_t>(r + 1), static_cast<uint16_t>(c + 1)}] += count;
        } else {
            next[{static_cast<uint16_t>(r + 1), c}] += count;
        }
    }
    beams.swap(next);
    return numSplits;
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    Grid grid{};
    if (!readFile(argv[1], grid)) {
        return EXIT_FAILURE;
    }

    Beams beams{};
    for (size_t c = 0; c < grid[0].size(); ++c) {
        if ('S' == grid[0][c]) {
            beams[{1, static_cast<uint16_t>(c)}] = 1;
        }
    }

    uint32_t numSplits = 0;
    uint16_t r = 1;
    while (r++ < grid.size() - 1) {
        numSplits += simulate(grid, beams);
    }

    {  // Part 1
        std::cout << numSplits << '\n';
    }
    {  // Part 2
        uint64_t numBeams = 0;
        for (const auto& kvp : beams) {
            numBeams += kvp.second;
        }
        std::cout << numBeams << '\n';
    }

    return EXIT_SUCCESS;
}
