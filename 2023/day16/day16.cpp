// Advent of Code 2023
// Day 16: The Floor Will Be Lava
// https://adventofcode.com/2023/day/16

#include <array>
#include <fstream>
#include <iostream>
#include <string>
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

using Pos = std::array<size_t, 2>;
using Dir = uint8_t;
using Beam = std::pair<Pos, Dir>;
using Beams = std::vector<Beam>;

struct BeamHash
{
    size_t operator()(const Beam& beam) const
    {
        return beam.first[0] * 1000 + beam.first[1];
    }
};

using BeamSet = std::unordered_set<Beam, BeamHash>;

static void simulate(Beams& beams, const std::vector<std::string>& grid, const BeamSet& visited)
{
    BeamSet spawn;
    for (auto it = beams.begin(); it != beams.end();) {
        auto& [pos, dir] = *it;
        auto& [r, c] = pos;
        if (0 == dir) {  // E
            if (grid[r].size() - 1 == c) {
                it = beams.erase(it);
                continue;
            }
            c++;
            if ('\\' == grid[r][c]) {
                dir = 1;
            } else if ('/' == grid[r][c]) {
                dir = 3;
            } else if ('|' == grid[r][c]) {
                dir = 1;
                auto snd = Beam({{r, c}, 3});
                if (visited.find(snd) == visited.end()) {
                    spawn.insert(snd);
                }
            }
        } else if (1 == dir) {  // S
            if (grid.size() - 1 == r) {
                it = beams.erase(it);
                continue;
            }
            r++;
            if ('\\' == grid[r][c]) {
                dir = 0;
            } else if ('/' == grid[r][c]) {
                dir = 2;
            } else if ('-' == grid[r][c]) {
                dir = 0;
                auto snd = Beam({{r, c}, 2});
                if (visited.find(snd) == visited.end()) {
                    spawn.insert(snd);
                }
            }
        } else if (2 == dir) {  // W
            if (0 == c) {
                it = beams.erase(it);
                continue;
            }
            c--;
            if ('\\' == grid[r][c]) {
                dir = 3;
            } else if ('/' == grid[r][c]) {
                dir = 1;
            } else if ('|' == grid[r][c]) {
                dir = 1;
                auto snd = Beam({{r, c}, 3});
                if (visited.find(snd) == visited.end()) {
                    spawn.insert(snd);
                }
            }
        } else if (3 == dir) {  // N
            if (0 == r) {
                it = beams.erase(it);
                continue;
            }
            r--;
            if ('\\' == grid[r][c]) {
                dir = 2;
            } else if ('/' == grid[r][c]) {
                dir = 0;
            } else if ('-' == grid[r][c]) {
                dir = 0;
                auto snd = Beam({{r, c}, 2});
                if (visited.find(snd) == visited.end()) {
                    spawn.insert(snd);
                }
            }
        }
        if (visited.find(*it) != visited.end()) {
            it = beams.erase(it);
            continue;
        }
        ++it;
    }
    beams.insert(beams.end(), spawn.begin(), spawn.end());
}

static size_t simulate(const Beam& start, const std::vector<std::string>& grid)
{
    Beams beams{start};
    BeamSet visited{start};
    size_t s;
    do {
        s = visited.size();
        simulate(beams, grid, visited);
        visited.insert(beams.begin(), beams.end());
    } while (s < visited.size());

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

    {  // Part 1
        Dir startDir = '\\' == grid[0][0] ? 1 : 0;
        const auto count = simulate({{0, 0}, startDir}, grid);
        std::cout << count << std::endl;
    }
    {  // Part 2
        size_t max{};
        for (size_t r = 0; r < grid.size(); ++r) {
            max = std::max(max, simulate({{r, 0}, 0}, grid));                   // E
            max = std::max(max, simulate({{r, grid[r].size() - 1}, 2}, grid));  // W
        }
        for (size_t c = 0; c < grid[0].size(); ++c) {
            max = std::max(max, simulate({{0, c}, 1}, grid));                // S
            max = std::max(max, simulate({{grid.size() - 1, c}, 3}, grid));  // N
        }
        std::cout << max << std::endl;
    }

    return EXIT_SUCCESS;
}
