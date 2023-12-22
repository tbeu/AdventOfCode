// Advent of Code 2023
// Day 21: Step Counter
// https://adventofcode.com/2023/day/21

#include <assert.h>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
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

using Pos = std::array<uint8_t, 2>;
using Dir = uint8_t;
using Grid = std::vector<std::string>;

struct State
{
    State(const Pos& pos, uint32_t step = 0, std::array<int16_t, 2> tile = std::array<int16_t, 2>{})
        : pos{pos}, step{step}, tile{tile}
    {
    }

    uint64_t hash() const
    {
        uint64_t hash{};
        hash |= static_cast<uint64_t>(pos[0]);
        hash |= static_cast<uint64_t>(pos[1]) << 8;
        hash |= static_cast<uint64_t>(tile[0] + INT16_MAX) << 16;
        hash |= static_cast<uint64_t>(tile[1] + INT16_MAX) << 32;
        return hash;
    }

    Pos pos{};
    uint32_t step{};
    std::array<int16_t, 2> tile{};
};

static size_t bfs(const Grid& grid, uint32_t maxSteps)
{
    std::unordered_map<uint64_t, uint32_t> visited{};
    constexpr std::array<std::array<int8_t, 2>, 4> adjs{{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};
    std::queue<State> q;
    const Pos dim{static_cast<uint8_t>(grid.size()), static_cast<uint8_t>(grid[0].size())};
    for (uint8_t r = dim[0] / 2; r < dim[0]; ++r) {
        for (uint8_t c = dim[1] / 2; c < dim[1]; ++c) {
            if ('S' == grid[r][c]) {
                q.push(Pos{r, c});
                break;
            }
        }
    }
    size_t count{};
    while (!q.empty()) {
        const auto state = q.front();
        q.pop();
        const auto step = static_cast<uint32_t>(state.step + 1);
        if (step > maxSteps) {
            continue;
        }
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
            const auto pos = Pos{static_cast<uint8_t>(r + rr), static_cast<uint8_t>(c + cc)};
            if ('#' == grid[pos[0]][pos[1]]) {
                continue;
            }
            const auto next = State{pos, step};
            const auto hash = next.hash();
            if (visited.find(hash) != visited.end()) {
                continue;
            }
            const auto rem = maxSteps - next.step;
            if (0 == rem % 2) {
                count++;
            }
            visited[hash] = next.step;
            q.push(std::move(next));
        }
    }
    return count;
}

static size_t bfsInf(const Grid& grid, uint32_t maxSteps)
{
    std::unordered_map<uint64_t, uint32_t> visited{};
    constexpr std::array<std::array<int8_t, 2>, 4> adjs{{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};
    std::queue<State> q;
    const Pos dim{static_cast<uint8_t>(grid.size()), static_cast<uint8_t>(grid[0].size())};
    for (uint8_t r = dim[0] / 2; r < dim[0]; ++r) {
        for (uint8_t c = dim[1] / 2; c < dim[1]; ++c) {
            if ('S' == grid[r][c]) {
                q.push(Pos{r, c});
                break;
            }
        }
    }
    size_t count{};
    while (!q.empty()) {
        const auto state = q.front();
        q.pop();
        const auto step = static_cast<uint32_t>(state.step + 1);
        if (step > maxSteps) {
            continue;
        }
        const auto [r, c] = state.pos;
        for (Dir dir = 0; dir < adjs.size(); ++dir) {
            Pos pos;
            auto tile = state.tile;
            if (0 == dir && grid[0].size() - 1 == c) {  // E
                pos = {r, 0};
                tile[0]++;
            } else if (1 == dir && grid.size() - 1 == r) {  // S
                pos = {0, c};
                tile[1]++;
            } else if (2 == dir && 0 == c) {  // W
                pos = {r, static_cast<uint8_t>(grid[0].size() - 1)};
                tile[0]--;
            } else if (3 == dir && 0 == r) {  // N
                pos = {static_cast<uint8_t>(grid.size() - 1), c};
                tile[1]--;
            } else {
                const auto& [rr, cc] = adjs[dir];
                pos = Pos{static_cast<uint8_t>(r + rr), static_cast<uint8_t>(c + cc)};
            }
            if ('#' == grid[pos[0]][pos[1]]) {
                continue;
            }
            const auto next = State{pos, step, tile};
            const auto hash = next.hash();
            if (visited.find(hash) != visited.end()) {
                continue;
            }
            const auto rem = maxSteps - next.step;
            if (0 == rem % 2) {
                count++;
            }
            visited[hash] = next.step;
            q.push(std::move(next));
        }
    }
    return count;
}

int64_t evalQuadratic(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t x3, int64_t y3, int64_t x)
{
    return ((x - x2) * (x - x3) / ((x1 - x2) * (x1 - x3))) * y1 + ((x - x1) * (x - x3) / ((x2 - x1) * (x2 - x3))) * y2 +
           ((x - x1) * (x - x2) / ((x3 - x1) * (x3 - x2))) * y3;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> grid{};
    if (2 != argc || !readFile(argv[1], grid)) {
        return EXIT_FAILURE;
    }

    {
        // Part 1
        const auto amt = bfs(grid, 64);
        std::cout << amt << std::endl;
    }
    {  // Part 2
        constexpr uint64_t dim = 131;
        assert(dim == grid.size());
        assert(dim == grid[0].size());
        assert(0 == (26501365 - 65) % dim);
        // extrapolate quadratic polynomial from 3 samples (n, bfsInf(grid, 65 + n * dim)) for n = (26501365 - 65) / dim
        const auto amt = evalQuadratic(0, bfsInf(grid, 65 + 0 * dim), 1, bfsInf(grid, 65 + 1 * dim), 2,
                                       bfsInf(grid, 65 + 2 * dim), (26501365 - 65) / dim);

        std::cout << amt << std::endl;
    }

    return EXIT_SUCCESS;
}
