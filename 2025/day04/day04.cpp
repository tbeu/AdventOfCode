// Advent of Code 2025
// Day 4: Printing Department
// https://adventofcode.com/2025/day/4

#include <array>
#include <fstream>
#include <iostream>
#include <queue>
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
constexpr const auto roll = '@';

size_t getAdjCount(const std::vector<std::string>& grid, const Pos& pos)
{
    const auto [r, c] = pos;
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
    return count;
}

std::vector<Pos> getMovable(const std::vector<std::string>& grid)
{
    std::vector<Pos> movables;
    for (size_t r = 0; r < grid.size(); ++r) {
        for (size_t c = 0; c < grid[r].size(); ++c) {
            if (grid[r][c] != roll) {
                continue;
            }
            if (auto pos = Pos{static_cast<uint8_t>(r), static_cast<uint8_t>(c)}; getAdjCount(grid, pos) < 4) {
                movables.push_back(pos);
            }
        }
    }
    return movables;
}

size_t bfs(const std::vector<std::string>& grid, const std::vector<Pos>& init)
{
    size_t sum = 0;
    auto visited = grid;
    std::queue<Pos> q;
    for (const auto& pos : init) {
        const auto [r, c] = pos;
        visited[r][c] = '+';
        q.push(pos);
    }
    const auto mayPushQ = [&](Pos next) {
        const auto [r, c] = next;
        if (visited[r][c] == roll && getAdjCount(visited, next) < 4) {
            visited[r][c] = '+';
            q.push(next);
        }
    };
    while (!q.empty()) {
        ++sum;
        const auto pos = q.front();
        q.pop();
        const auto [r, c] = pos;
        if (r < grid.size() - 1) {
            mayPushQ({static_cast<uint8_t>(r + 1), static_cast<uint8_t>(c)});
        }
        if (r > 0) {
            mayPushQ({static_cast<uint8_t>(r - 1), static_cast<uint8_t>(c)});
        }
        if (c < grid[r].size() - 1) {
            mayPushQ({static_cast<uint8_t>(r), static_cast<uint8_t>(c + 1)});
        }
        if (c > 0) {
            mayPushQ({static_cast<uint8_t>(r), static_cast<uint8_t>(c - 1)});
        }
        if (r < grid.size() - 1 && c < grid[r].size() - 1) {
            mayPushQ({static_cast<uint8_t>(r + 1), static_cast<uint8_t>(c + 1)});
        }
        if (r < grid.size() - 1 && c > 0) {
            mayPushQ({static_cast<uint8_t>(r + 1), static_cast<uint8_t>(c - 1)});
        }
        if (r > 0 && c < grid[r].size() - 1) {
            mayPushQ({static_cast<uint8_t>(r - 1), static_cast<uint8_t>(c + 1)});
        }
        if (r > 0 && c > 0) {
            mayPushQ({static_cast<uint8_t>(r - 1), static_cast<uint8_t>(c - 1)});
        }
    }
    return sum;
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

    {  // Part 1
        std::cout << movables.size() << '\n';
    }
    {  // Part 2
        std::cout << bfs(grid, movables) << '\n';
    }

    return EXIT_SUCCESS;
}
