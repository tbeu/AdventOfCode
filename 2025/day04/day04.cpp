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

using Pos = std::array<uint16_t, 2>;
using Grid = std::vector<std::string>;
constexpr const auto roll = '@';
constexpr std::array<std::array<int16_t, 2>, 8> adjs{
    {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};

size_t getAdjCount(const Grid& grid, const Pos& pos)
{
    const auto [r, c] = pos;
    size_t count = 0;
    for (const auto [rr, cc] : adjs) {
        count += grid[r + rr][c + cc] == roll ? 1 : 0;
    }
    return count;
}

std::vector<Pos> getMovable(const Grid& grid)
{
    std::vector<Pos> movables;
    for (size_t r = 1; r < grid.size() - 1; ++r) {
        for (size_t c = 1; c < grid[r].size() - 1; ++c) {
            if (grid[r][c] != roll) {
                continue;
            }
            if (auto pos = Pos{static_cast<uint16_t>(r), static_cast<uint16_t>(c)}; getAdjCount(grid, pos) < 4) {
                movables.push_back(pos);
            }
        }
    }
    return movables;
}

size_t bfs(Grid visited, const std::vector<Pos>& init)
{
    size_t sum = 0;
    std::queue<Pos> q;
    for (const auto& pos : init) {
        const auto [r, c] = pos;
        visited[r][c] = '+';
        q.push(pos);
    }
    while (!q.empty()) {
        ++sum;
        const auto [r, c] = q.front();
        q.pop();
        for (const auto [rr, cc] : adjs) {
            if (visited[r + rr][c + cc] != roll) {
                continue;
            }
            if (auto pos = Pos{static_cast<uint16_t>(r + rr), static_cast<uint16_t>(c + cc)};
                getAdjCount(visited, pos) < 4) {
                visited[r + rr][c + cc] = '+';
                q.push(pos);
            }
        }
    }
    return sum;
}

void enlarge(Grid& grid, std::string::value_type c = '.')
{
    const std::string border(grid[0].size(), c);
    grid.insert(grid.begin(), border);
    grid.emplace_back(border);
    std::transform(grid.begin(), grid.end(), grid.begin(), [c](auto& row) { return c + std::move(row) + c; });
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

    enlarge(grid);
    auto movables = getMovable(grid);

    {  // Part 1
        std::cout << movables.size() << '\n';
    }
    {  // Part 2
        std::cout << bfs(grid, movables) << '\n';
    }

    return EXIT_SUCCESS;
}
