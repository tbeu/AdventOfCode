// Advent of Code 2023
// Day 17: Clumsy Crucible
// https://adventofcode.com/2023/day/17

#include <array>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
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
using HeatMap = std::vector<std::string>;

struct State
{
    State(const Pos& pos, Dir dir, Dir step = 0, uint16_t heat = 0) : pos{pos}, dir{dir}, step{step}, heat{heat}
    {
    }
    Pos pos{};
    uint16_t heat{};
    Dir dir{};
    Dir step{};

    uint32_t hash() const
    {
        uint32_t hash{};
        hash |= static_cast<uint32_t>(pos[0]);
        hash |= static_cast<uint32_t>(pos[1]) << 8;
        hash |= static_cast<uint32_t>(step) << 16;
        hash |= static_cast<uint32_t>(dir) << 24;
        return hash;
    }
};

static uint16_t dijkstra(const HeatMap& heatmap, const Pos& start, const Pos& end, Dir minStep = 1, Dir maxStep = 3)
{
    std::unordered_map<uint32_t, uint16_t> visited{};
    constexpr std::array<std::array<int8_t, 2>, 4> adjs{{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};
    auto cmp = [](const auto& a, const auto& b) { return a.heat > b.heat; };
    std::priority_queue<State, std::vector<State>, decltype(cmp)> q(cmp);
    for (Dir dir = 0; dir < adjs.size(); ++dir) {
        q.push(std::move(State{start, dir}));
    }
    while (!q.empty()) {
        const auto state = q.top();
        if (state.pos == end && state.step >= minStep) {
            return state.heat;
        }
        q.pop();
        const auto [r, c] = state.pos;
        for (Dir dir = 0; dir < adjs.size(); ++dir) {
            if (dir == (state.dir + 2) % adjs.size()) {  // must not reverse
                continue;
            }
            if (0 == dir && heatmap[0].size() - 1 == c) {  // E
                continue;
            }
            if (1 == dir && heatmap.size() - 1 == r) {  // S
                continue;
            }
            if (2 == dir && 0 == c) {  // W
                continue;
            }
            if (3 == dir && 0 == r) {  // N
                continue;
            }
            if (dir != state.dir && state.step < minStep) {  // must move at least min steps in same direction
                continue;
            }
            const Dir step = dir == state.dir ? state.step + 1 : 1;
            if (step > maxStep) {  // must not move more than max steps in same direction
                continue;
            }
            const auto& [rr, cc] = adjs[dir];
            const auto pos = Pos{static_cast<uint8_t>(r + rr), static_cast<uint8_t>(c + cc)};
            const auto heat = static_cast<uint16_t>(state.heat + heatmap[pos[0]][pos[1]] - '0');
            const auto next = State{pos, dir, step, heat};
            const auto hash = next.hash();
            if (auto it = visited.find(hash); it != visited.end()) {
                if (heat < it->second) {
                    it->second = heat;
                    q.push(std::move(next));
                }
            } else {
                visited[hash] = heat;
                q.push(std::move(next));
            }
        }
    }
    return UINT16_MAX;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> heatmap{};
    if (2 != argc || !readFile(argv[1], heatmap)) {
        return EXIT_FAILURE;
    }

    const auto start = Pos{0, 0};
    const auto end = Pos{static_cast<uint8_t>(heatmap.size() - 1), static_cast<uint8_t>(heatmap[0].size() - 1)};

    {
        // Part 1
        const auto amt = dijkstra(heatmap, start, end, 1, 3);
        std::cout << amt << std::endl;
    }
    {  // Part 2
        const auto amt = dijkstra(heatmap, start, end, 4, 10);
        std::cout << amt << std::endl;
    }

    return EXIT_SUCCESS;
}
