// Advent of Code 2024
// Day 10: Hoof It
// https://adventofcode.com/2024/day/10

#include <array>
#include <fstream>
#include <iostream>
#include <set>
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

using Coord = uint8_t;
using Height = uint8_t;
using Pos = std::array<Coord, 2>;

void dfs(const Pos& pos, const std::vector<std::string>& grid, std::set<Pos>& tops, size_t& topsCount,
         Height height = 0)
{
    constexpr const Height topHeight = 9;
    if (height == topHeight) {
        tops.insert(pos);
        ++topsCount;
        return;
    }
    const auto& [r, c] = pos;
    if (c < grid[r].size() - 1 && height + 1 == static_cast<Height>(grid[r][c + 1] - '0')) {  // E
        dfs(Pos{r, static_cast<Coord>(c + 1)}, grid, tops, topsCount, height + 1);
    }
    if (r < grid.size() - 1 && height + 1 == static_cast<Height>(grid[r + 1][c] - '0')) {  // S
        dfs(Pos{static_cast<Coord>(r + 1), c}, grid, tops, topsCount, height + 1);
    }
    if (c > 0 && height + 1 == static_cast<Height>(grid[r][c - 1] - '0')) {  // W
        dfs(Pos{r, static_cast<Coord>(c - 1)}, grid, tops, topsCount, height + 1);
    }
    if (r > 0 && height + 1 == static_cast<Height>(grid[r - 1][c] - '0')) {  // N
        dfs(Pos{static_cast<Coord>(r - 1), c}, grid, tops, topsCount, height + 1);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    std::vector<std::string> grid{};
    if (!readFile(argv[1], grid)) {
        return EXIT_FAILURE;
    }

    std::vector<Pos> starts;
    for (size_t r = 0; r < grid.size(); ++r) {
        for (size_t c = 0; c < grid[r].size(); ++c) {
            if (grid[r][c] == '0') {
                starts.push_back(Pos{static_cast<Coord>(r), static_cast<Coord>(c)});
            }
        }
    }

    size_t topsCountUnique{0};
    size_t topsCount{0};
    for (const auto& start : starts) {
        std::set<Pos> tops;
        dfs(start, grid, tops, topsCount);
        topsCountUnique += tops.size();
    }

    {  // Part 1
        std::cout << topsCountUnique << '\n';
    }
    {  // Part 2
        std::cout << topsCount << '\n';
    }

    return EXIT_SUCCESS;
}
