// Advent of Code 2023
// Day 16: The Floor Will Be Lava
// https://adventofcode.com/2023/day/16

#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <queue>
#include <unordered_set>
#include <vector>

#include <gsl/util>

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

using Coord = uint8_t;
using Pos = std::array<Coord, 2>;
using Dir = uint8_t;
using Beam = std::pair<Pos, Dir>;

struct BeamHash
{
    size_t operator()(const Beam& beam) const
    {
        return (beam.first[0] << 8) + beam.first[1];
    }
};

static bool reflect(Beam& beam, const std::vector<std::string>& grid)
{
    auto& [pos, dir] = beam;
    const auto& [r, c] = pos;
    if (0 == dir) {  // E
        if ('\\' == grid[r][c]) {
            dir = 1;
        } else if ('/' == grid[r][c]) {
            dir = 3;
        } else if ('|' == grid[r][c]) {
            dir = 1;
            return true;
        }
    } else if (1 == dir) {  // S
        if ('\\' == grid[r][c]) {
            dir = 0;
        } else if ('/' == grid[r][c]) {
            dir = 2;
        } else if ('-' == grid[r][c]) {
            dir = 0;
            return true;
        }
    } else if (2 == dir) {  // W
        if ('\\' == grid[r][c]) {
            dir = 3;
        } else if ('/' == grid[r][c]) {
            dir = 1;
        } else if ('|' == grid[r][c]) {
            dir = 1;
            return true;
        }
    } else if (3 == dir) {  // N
        if ('\\' == grid[r][c]) {
            dir = 2;
        } else if ('/' == grid[r][c]) {
            dir = 0;
        } else if ('-' == grid[r][c]) {
            dir = 0;
            return true;
        }
    }
    return false;
}

static bool move(Beam& beam, const std::vector<std::string>& grid)
{
    auto& [pos, dir] = beam;
    auto& [r, c] = pos;
    if (0 == dir) {  // E
        if (grid[0].size() - 1 == c) {
            return false;
        }
        c++;
    } else if (1 == dir) {  // S
        if (grid.size() - 1 == r) {
            return false;
        }
        r++;
    } else if (2 == dir) {  // W
        if (0 == c) {
            return false;
        }
        c--;
    } else if (3 == dir) {  // N
        if (0 == r) {
            return false;
        }
        r--;
    }
    return true;
}

static size_t simulate(const Beam& start, const std::vector<std::string>& grid)
{
    std::queue<Beam> beams;
    beams.push(start);
    std::unordered_set<Beam, BeamHash> visited;
    while (!beams.empty()) {
        auto beam = beams.front();
        beams.pop();
        while (visited.find(beam) == visited.end()) {
            visited.insert(beam);
            if (reflect(beam, grid)) {
                const auto& [pos, dir] = beam;
                beams.push({pos, (dir + 2) % 4});
            }
            if (!move(beam, grid)) {
                break;
            }
        }
    }

    // count the different positions
    std::unordered_set<size_t> energized;
    energized.reserve(visited.size());
    std::transform(visited.cbegin(), visited.cend(), std::inserter(energized, energized.end()),
                   [](const Beam& beam) { return BeamHash{}(beam); });
    return energized.size();
}

int main(int argc, char* argv[])
{
    std::vector<std::string> grid{};
    if (2 != argc || !readFile(argv[1], grid)) {
        return EXIT_FAILURE;
    }

    size_t max{};

    {  // Part 1
        max = simulate({{0, 0}, 0}, grid);
        std::cout << max << std::endl;
    }
    {  // Part 2
        for (Coord r = 1; r < grid.size(); ++r) {
            max = std::max(max, simulate({{r, 0}, 0}, grid));                                       // E
            max = std::max(max, simulate({{r, static_cast<Coord>(grid[0].size() - 1)}, 2}, grid));  // W
        }
        max = std::max(max, simulate({{0, static_cast<Coord>(grid[0].size() - 1)}, 2}, grid));  // W
        for (Coord c = 0; c < grid[0].size(); ++c) {
            max = std::max(max, simulate({{0, c}, 1}, grid));                                    // S
            max = std::max(max, simulate({{static_cast<Coord>(grid.size() - 1), c}, 3}, grid));  // N
        }
        std::cout << max << std::endl;
    }

    return EXIT_SUCCESS;
}
