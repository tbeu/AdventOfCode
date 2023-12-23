// Advent of Code 2023
// Day 23: A Long Walk
// https://adventofcode.com/2023/day/23

#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <string>
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
using Grid = std::vector<std::string>;
constexpr std::array<std::array<int8_t, 2>, 4> adjs{{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};

struct State
{
    explicit State(const Pos& pos) : pos{pos}
    {
    }
    State(const Pos& pos, const std::set<Pos>& _path) : pos{pos}, path{_path}
    {
        path.insert(pos);
    }

    Pos pos{0, 0};
    std::set<Pos> path{};
    size_t dist{};
    Pos prev{0, 0};
    bool branched{false};
};

struct Graph
{
    Pos root;
    std::set<Pos> nodes;
    std::map<Pos, std::set<std::pair<Pos, size_t> > > edges;
};

static size_t longest_path(const Grid& grid, const Pos& start, const Pos& end)
{
    const auto isNode = [&](const Pos& pos) {
        if ((pos[0] == start[0] && pos[1] == start[1]) || (pos[0] == end[0] && pos[1] == end[1])) {
            return true;
        }
        const auto [r, c] = pos;
        size_t count{};
        for (size_t dir = 0; dir < adjs.size(); ++dir) {
            const auto [rr, cc] = adjs[dir];
            const auto adj = Pos{static_cast<uint8_t>(r + rr), static_cast<uint8_t>(c + cc)};
            if ('#' != grid[adj[0]][adj[1]]) {
                count++;
            }
        }
        return count > 2 && '#' != grid[r][c];
    };

    const auto dim = Pos{static_cast<uint8_t>(grid.size()), static_cast<uint8_t>(grid[0].size())};
    std::map<std::pair<Pos, uint8_t>, size_t> visited;
    visited[{start, 1}] = 0;
    std::queue<State> q{};
    auto first = State{start};
    first.prev = start;
    q.push(first);
    Graph g;
    g.root = start;
    const auto updateQ = [&](const State& next, uint8_t dir) {
        if (auto it = visited.find({next.pos, dir}); it != visited.end()) {
            if (next.branched || isNode(next.pos)) {
                q.push(next);
            }
        } else {
            visited[{next.pos, dir}] = next.path.size();
            q.push(next);
        }
    };

    while (!q.empty()) {
        auto state = q.front();
        q.pop();
        size_t dist{};
        const auto branched = isNode(state.pos);
        if (branched) {
            g.nodes.insert(state.pos);
            dist = state.path.size();
            if (dist > state.dist) {
                g.edges[state.prev].insert({state.pos, dist - state.dist});
                g.edges[state.pos].insert({state.prev, dist - state.dist});
            }
        }
        const auto [r, c] = state.pos;
        for (uint8_t dir = 0; dir < adjs.size(); ++dir) {
            if (1 == dir && r == end[0] && c == end[1]) {  // S
                continue;
            }
            if (3 == dir && r == start[0] && c == start[1]) {  // N
                continue;
            }
            const auto [rr, cc] = adjs[dir];
            const auto pos = Pos{static_cast<uint8_t>(r + rr), static_cast<uint8_t>(c + cc)};
            if ('#' == grid[pos[0]][pos[1]]) {
                continue;
            }
            if (state.path.find(pos) != state.path.end()) {
                continue;
            }
            auto next = State{pos};
            next.path = state.path;
            next.path.insert(state.pos);
            next.branched = branched;
            if (dist > 0) {
                next.prev = state.pos;
                next.dist = dist;
            } else {
                next.prev = state.prev;
                next.dist = state.dist;
            }
            updateQ(next, dir);
        }
    }

    auto cmp = [](const auto& a, const auto& b) { return a.dist < b.dist; };
    std::priority_queue<State, std::vector<State>, decltype(cmp)> q2(cmp);
    visited.clear();
    q2.push(State{g.root});
    visited[{g.root, 0}] = 0;
    const auto updateQ2 = [&](const State& next) {
        if (auto it = visited.find({next.pos, 0}); it != visited.end()) {
            if (next.dist > it->second) {
                it->second = next.dist;
            }
            q2.push(next);
        } else {
            visited[{next.pos, 0}] = next.dist;
            q2.push(next);
        }
    };
    while (!q2.empty()) {
        const auto state = q2.top();
        q2.pop();
        const auto& dsts = g.edges.at(state.pos);
        for (const auto& [pos, w] : dsts) {
            if (state.path.find(pos) != state.path.end()) {
                continue;
            }
            auto next = State{pos};
            next.path = state.path;
            next.path.insert(state.pos);
            next.dist = state.dist + w;
            updateQ2(next);
        }
    }
    return visited.at({end, 0});
}

static size_t dijkstra_dag(const Grid& grid, const Pos& start, const Pos& end)
{
    const auto dim = Pos{static_cast<uint8_t>(grid.size()), static_cast<uint8_t>(grid[0].size())};
    std::map<Pos, size_t> visited;
    visited[start] = 0;
    auto cmp = [](const auto& a, const auto& b) { return a.path.size() < b.path.size(); };
    std::priority_queue<State, std::vector<State>, decltype(cmp)> q(cmp);
    auto first = State{start};
    q.push(first);
    const auto updateQ = [&](const State& next) {
        if (auto it = visited.find(next.pos); it != visited.end()) {
            if (next.path.size() > it->second) {
                it->second = next.path.size();
                q.push(next);
            }
        } else {
            visited[next.pos] = next.path.size();
            q.push(next);
        }
    };

    while (!q.empty()) {
        const auto state = q.top();
        q.pop();
        const auto [r, c] = state.pos;
        for (size_t dir = 0; dir < adjs.size(); ++dir) {
            if (dir != 0 && '>' == grid[r][c]) {  // E
                continue;
            }
            if (dir != 1 && 'v' == grid[r][c]) {  // S
                continue;
            }
            if (dir != 2 && '<' == grid[r][c]) {  // W
                continue;
            }
            if (dir != 3 && '^' == grid[r][c]) {  // N
                continue;
            }
            if (1 == dir && r == end[0] && c == end[1]) {  // S
                continue;
            }
            if (3 == dir && r == start[0] && c == start[1]) {  // N
                continue;
            }
            const auto [rr, cc] = adjs[dir];
            const auto pos = Pos{static_cast<uint8_t>(r + rr), static_cast<uint8_t>(c + cc)};
            if ('#' == grid[pos[0]][pos[1]]) {
                continue;
            }
            if (state.path.find(pos) != state.path.end()) {
                continue;
            }
            updateQ(State{pos, state.path});
        }
    }

    return visited.at(end);
}

//static size_t bfs2(const Grid& grid, const Pos& start, const Pos& end)
//{
//    const auto dim = Pos{static_cast<uint8_t>(grid.size()), static_cast<uint8_t>(grid[0].size())};
//    std::map<Pos, std::set<Pos> > visited;
//    visited[start] = std::set<Pos>{};
//    std::queue<State> q{};
//    auto first = State{start};
//    first.path.insert(start);
//    q.push(first);
//    const auto updateQ = [&](State& next) {
//        if (auto it = visited.find(next.pos); it != visited.end()) {
//            if (next.step > it->second && next.path.find(next.pos) == next.path.end()) {
//                next.path.insert(next.pos);
//                it->second = next.step;
//                q.push(next);
//            }
//        } else {
//            visited[next.pos] = next.path;
//            next.path.insert(next.pos);
//            q.push(next);
//        }
//    };
//
//    while (!q.empty()) {
//        auto state = q.front();
//        q.pop();
//        const auto [r, c] = state.pos;
//        for (size_t dir = 0; dir < adjs.size(); ++dir) {
//            //if (dir != 0 && '>' == grid[r][c]) {  // E
//            //    continue;
//            //}
//            //if (dir != 1 && 'v' == grid[r][c]) {  // S
//            //    continue;
//            //}
//            //if (dir != 2 && '<' == grid[r][c]) {  // W
//            //    continue;
//            //}
//            //if (dir != 3 && '^' == grid[r][c]) {  // N
//            //    continue;
//            //}
//            if (1 == dir && r == end[0] && c == end[1]) {  // S
//                continue;
//            }
//            if (3 == dir && r == start[0] && c == start[1]) {  // N
//                continue;
//            }
//            const auto [rr, cc] = adjs[dir];
//            const auto pos = Pos{static_cast<uint8_t>(r + rr), static_cast<uint8_t>(c + cc)};
//            if ('#' == grid[pos[0]][pos[1]]) {
//                continue;
//            }
//            const auto step = state.step + 1;
//            auto next = State{pos, step};
//            next.path = state.path;
//            next.prev = state.pos;
//            updateQ(next);
//        }
//    }
//
//    return visited.at(end).size();
//}

int main(int argc, char* argv[])
{
    std::vector<std::string> grid{};
    if (2 != argc || !readFile(argv[1], grid)) {
        return EXIT_FAILURE;
    }

    const auto start = Pos{0, 1};
    const auto end = Pos{static_cast<uint8_t>(grid.size() - 1), static_cast<uint8_t>(grid[0].size() - 2)};

    {
        // Part 1
        const auto steps = dijkstra_dag(grid, start, end);
        std::cout << steps << std::endl;
    }
    {  // Part 2
        const auto steps = longest_path(grid, start, end);
        std::cout << steps << std::endl;
    }

    return EXIT_SUCCESS;
}
