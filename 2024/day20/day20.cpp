// Advent of Code 2024
// Day 20: Race Condition
// https://adventofcode.com/2024/day/20

#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <tuple>

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

using Coord = uint16_t;
using Pos = std::array<Coord, 2>;
using Dir = uint8_t;
using Grid = std::vector<std::vector<bool> >;

struct StatePos
{
    Pos pos{};
    uint16_t step{};
};

static uint32_t bfs(const Grid& grid, const Pos& start, const Pos& end)
{
    auto visited = grid;
    constexpr std::array<std::array<int8_t, 2>, 4> adjs{{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};
    std::queue<StatePos> q;
    q.push(StatePos{start, 0});
    visited[start[0]][start[1]] = true;
    while (!q.empty()) {
        const auto state = q.front();
        if (state.pos == end) {
            return state.step;
        }
        q.pop();
        const auto step = static_cast<uint16_t>(state.step + 1);
        const auto [r, c] = state.pos;
        for (Dir dir = 0; dir < adjs.size(); ++dir) {
            if (0 == dir && grid[0].size() - 1 == c) {  // E
                continue;
            }
            if (1 == dir && grid.size() - 1 == r) {  // S
                continue;
            }
            if (2 == dir && 0 == c) {  // W
                continue;
            }
            if (3 == dir && 0 == r) {  // N
                continue;
            }
            const auto& [rr, cc] = adjs[dir];
            const auto pos = Pos{static_cast<Coord>(r + rr), static_cast<Coord>(c + cc)};
            if (visited[pos[0]][pos[1]]) {
                continue;
            }
            visited[pos[0]][pos[1]] = true;
            q.push(StatePos{pos, step});
        }
    }
    return UINT32_MAX;
}

struct StatePath
{
    std::vector<Pos> path{};
    uint16_t step{};
};

static StatePath bfsWithPath(const Grid& grid, const Pos& start, const Pos& end)
{
    auto visited = grid;
    constexpr std::array<std::array<int8_t, 2>, 4> adjs{{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};
    std::queue<StatePath> q;
    q.push(StatePath{std::vector<Pos>{start}, 0});
    visited[start[0]][start[1]] = true;
    while (!q.empty()) {
        const auto state = q.front();
        if (state.path.back() == end) {
            return state;
        }
        q.pop();
        const auto step = static_cast<uint16_t>(state.step + 1);
        const auto [r, c] = state.path.back();
        for (Dir dir = 0; dir < adjs.size(); ++dir) {
            if (0 == dir && grid[0].size() - 1 == c) {  // E
                continue;
            }
            if (1 == dir && grid.size() - 1 == r) {  // S
                continue;
            }
            if (2 == dir && 0 == c) {  // W
                continue;
            }
            if (3 == dir && 0 == r) {  // N
                continue;
            }
            const auto& [rr, cc] = adjs[dir];
            const auto pos = Pos{static_cast<Coord>(r + rr), static_cast<Coord>(c + cc)};
            if (visited[pos[0]][pos[1]]) {
                continue;
            }
            visited[pos[0]][pos[1]] = true;
            auto path = state.path;
            path.push_back(pos);
            q.push(StatePath{path, step});
        }
    }
    return StatePath{std::vector<Pos>{}, UINT16_MAX};
}

inline int16_t manhattanDistance(const Pos& a, const Pos& b)
{
    return std::abs(a[0] - b[0]) + std::abs(a[1] - b[1]);
}

uint32_t countCheats(const std::vector<Pos>& path, uint16_t maxDist)
{
    uint32_t count = 0;
    for (size_t i = 0; i < path.size(); ++i) {
        for (size_t j = i + 1; j < path.size(); ++j) {
            const auto& start = path[i];
            const auto& end = path[j];
            const auto dist = manhattanDistance(start, end);
            if (dist == 1 || dist > maxDist) {
                continue;
            }
            if (j - i >= dist + 100) {
                ++count;
            }
        }
    }
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

    const auto& [grid, start, end] = [&lines] {
        auto grid = Grid(lines.size(), std::vector<bool>(lines[0].size(), false));
        Pos start, end;
        for (size_t r = 0; r < grid.size(); ++r) {
            for (size_t c = 0; c < grid[r].size(); ++c) {
                if (lines[r][c] == 'S') {
                    start = Pos{static_cast<Coord>(r), static_cast<Coord>(c)};
                } else if (lines[r][c] == 'E') {
                    end = Pos{static_cast<Coord>(r), static_cast<Coord>(c)};
                } else if (lines[r][c] == '#') {
                    grid[r][c] = true;
                }
            }
        }
        return std::make_tuple(grid, start, end);
    }();

    const auto original = bfsWithPath(grid, start, end);

    {  // Part 1
        if constexpr (false) {
            uint32_t count = 0;
            for (size_t r = 1; r < grid.size() - 1; ++r) {
                for (size_t c = 1; c < grid[r].size() - 1; ++c) {
                    if (!grid[r][c]) {
                        continue;
                    }
                    auto gridCheat = grid;
                    gridCheat[r][c] = false;
                    if (bfs(gridCheat, start, end) + 100 <= original.step) {
                        ++count;
                    }
                }
            }
            std::cout << count << '\n';
        } else {
            std::cout << countCheats(original.path, 2) << '\n';
        }
    }
    {  // Part 2
        std::cout << countCheats(original.path, 20) << '\n';
    }

    return EXIT_SUCCESS;
}
