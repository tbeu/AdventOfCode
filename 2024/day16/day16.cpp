// Advent of Code 2024
// Day 16: Reindeer Maze
// https://adventofcode.com/2024/day/16

#include <array>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <set>
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

using Coord = uint16_t;
using Pos = std::array<Coord, 2>;
using Dir = uint8_t;

struct State
{
    std::vector<Pos> path;
    Dir dir;
    uint16_t step;
    uint16_t turns;
    uint32_t score() const
    {
        return step + 1000 * turns;
    }
};

static std::pair<State, std::set<Pos> > dijkstra(const std::vector<std::string>& grid, const Pos& start, const Pos& end)
{
    std::map<std::pair<Pos, Dir>, uint32_t> visited{};
    constexpr std::array<std::array<int8_t, 2>, 4> adjs{{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};
    const auto cmp = [](const auto& a, const auto& b) { return a.score() > b.score(); };
    std::priority_queue<State, std::vector<State>, decltype(cmp)> q(cmp);
    q.push(std::move(State{std::vector<Pos>{start}, 0, 0, 0}));
    visited[{start, 0}] = 0;
    auto minState = std::optional<State>();
    std::set<Pos> bestPos;

    const auto pushQ = [&](State&& next) {
        const auto& pos = next.path.back();
        if (auto it = visited.find({pos, next.dir}); it != visited.end()) {
            if (const auto score = next.score(); score <= it->second) {
                it->second = score;
                q.push(std::move(next));
            }
        } else {
            visited[{pos, next.dir}] = next.score();
            q.push(std::move(next));
        }
    };

    while (!q.empty()) {
        const auto state = q.top();
        if (state.path.back() == end) {
            if (!minState.has_value()) {
                minState = state;
            }
            if (state.score() > minState->score()) {
                break;
            }
            for (const auto& p : state.path) {
                bestPos.insert(p);
            }
            q.pop();
            continue;
        }
        q.pop();
        {
            // turn clockwise or anti-clockwise
            const auto turns = static_cast<uint16_t>(state.turns + 1);
            pushQ(State{state.path, static_cast<Dir>((state.dir + 1) % 4), state.step, turns});
            pushQ(State{state.path, static_cast<Dir>((state.dir + 3) % 4), state.step, turns});
        }
        {
            // step ahead
            const auto [rr, cc] = adjs[state.dir];
            const auto [r, c] = state.path.back();
            if (grid[r + rr][c + cc] == '#') {
                continue;
            }
            const auto pos = Pos{static_cast<Coord>(r + rr), static_cast<Coord>(c + cc)};
            auto path = state.path;
            path.push_back(pos);
            pushQ(State{path, state.dir, static_cast<uint16_t>(state.step + 1), state.turns});
        }
    }
    return {minState.value(), bestPos};
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

    assert(grid[grid.size() - 2][1] == 'S');
    assert(grid[1][grid[1].size() - 2] == 'E');
    const auto& [state, pathPos] =
        dijkstra(grid, Pos{static_cast<Coord>(grid.size() - 2), 1}, Pos{1, static_cast<Coord>(grid[1].size() - 2)});

    {  // Part 1
        std::cout << state.score() << '\n';
    }
    {  // Part 2
        std::cout << pathPos.size() << '\n';
    }

    return EXIT_SUCCESS;
}
