// Advent of Code 2025
// Day 4: Printing Department
// https://adventofcode.com/2025/day/4

#include <array>
#include <fstream>
#include <iostream>
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

using Pos = std::array<uint8_t, 2>;

std::vector<Pos> getMovable(const std::vector<std::string>& grid)
{
    std::vector<Pos> movables;
    constexpr const auto roll = '@';
    for (size_t r = 0; r < grid.size(); ++r) {
        for (size_t c = 0; c < grid[r].size(); ++c) {
            if (grid[r][c] != roll) {
                continue;
            }
            size_t count = 0;
            if (r < grid.size() - 1 && grid[r + 1][c] == roll) {
                ++count;
            }
            if (r > 0 && grid[r - 1][c] == roll) {
                ++count;
            }
            if (c < grid[r].size() - 1 && grid[r][c + 1] == roll) {
                ++count;
            }
            if (c > 0 && grid[r][c - 1] == roll) {
                ++count;
            }
            if (r < grid.size() - 1 && c < grid[r].size() - 1 && grid[r + 1][c + 1] == roll) {
                ++count;
            }
            if (r < grid.size() - 1 && c > 0 && grid[r + 1][c - 1] == roll) {
                ++count;
            }
            if (r > 0 && c < grid[r].size() - 1 && grid[r - 1][c + 1] == roll) {
                ++count;
            }
            if (r > 0 && c > 0 && grid[r - 1][c - 1] == roll) {
                ++count;
            }
            if (count < 4) {
                movables.push_back({static_cast<uint8_t>(r), static_cast<uint8_t>(c)});
            }
        }
    }
    return movables;
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

    auto movables = getMovable(grid);
    auto sum = movables.size();

    {  // Part 1
        std::cout << sum << '\n';
    }
    {  // Part 2
        while (!movables.empty()) {
            for (const auto [r, c] : movables) {
                grid[r][c] = '+';
            }
            movables = getMovable(grid);
            sum += movables.size();
        }
        std::cout << sum << '\n';
    }

    return EXIT_SUCCESS;
}
