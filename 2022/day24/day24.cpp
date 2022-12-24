// Advent of Code 2022
// Day 24: Blizzard Basin
// https://adventofcode.com/2022/day/24

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <string>
#include <vector>

#include <gsl/util>

static bool readFile(std::string fileName, std::vector<std::string>& lines)
{
    std::ifstream in{fileName.c_str()};
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

constexpr int8_t Ground{0};
constexpr int8_t E{1};
constexpr int8_t W{2};
constexpr int8_t S{4};
constexpr int16_t N{8};
constexpr int16_t Wall{16};
using Pos = std::array<uint16_t, 2>;
using Map = std::vector<std::vector<int16_t> >;
using Maps = std::vector<Map>;

struct World
{
    World(const Pos& pos, uint16_t step = 0) : pos{pos}, step{step}
    {
    }
    Pos pos{};
    uint16_t step{};

    uint32_t hash() const
    {
        uint32_t hash{};
        hash |= static_cast<uint32_t>(pos[0]);
        hash |= static_cast<uint32_t>(pos[1]) << 8;
        hash |= static_cast<uint32_t>(step) << 16;
        return hash;
    }
};

void move(Map& map)
{
    const Pos dim{static_cast<uint16_t>(map.size()), static_cast<uint16_t>(map[0].size())};
    Map next{static_cast<size_t>(dim[0] - 2), std::vector<int16_t>(dim[1], Ground)};
    {
        const std::vector<int16_t> border(dim[1], Wall);
        next.insert(next.begin(), border);
        next.emplace_back(border);
    }
    for (int16_t i = 1; i < dim[0] - 1; ++i) {
        next[i][0] = Wall;
        next[i][dim[1] - 1] = Wall;
        for (int16_t j = 1; j < dim[1] - 1; ++j) {
            if (map[i][j] == Wall) {
                next[i][j] = Wall;
                continue;
            }
            if (i == dim[0] - 3 && map[2][j] & N || map[i + 1][j] & N) {
                next[i][j] |= N;
            }
            if (i == 2 && map[dim[0] - 3][j] & S || map[i - 1][j] & S) {
                next[i][j] |= S;
            }
            if (j == dim[1] - 2 && map[i][1] & W || map[i][j + 1] & W) {
                next[i][j] |= W;
            }
            if (j == 1 && map[i][dim[1] - 2] & E || map[i][j - 1] & E) {
                next[i][j] |= E;
            }
        }
    }
    map.swap(next);
}

inline int32_t manhattanDistance(const Pos& a, const Pos& b)
{
    return std::abs(a[0] - b[0]) + std::abs(a[1] - b[1]);
}

uint16_t dijkstra(const Maps& maps, const Pos& start, const Pos& end, uint16_t startStep = 0)
{
    std::map<uint32_t, size_t> visited{};
    uint16_t minSteps{UINT16_MAX};
    const auto period = static_cast<uint16_t>(maps.size());
    constexpr std::array<std::array<int32_t, 2>, 4> adjs{{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};
    auto cmp = [&end](const auto& a, const auto& b) {
        return a.step + manhattanDistance(a.pos, end) > b.step + manhattanDistance(b.pos, end);
    };
    std::priority_queue<World, std::vector<World>, decltype(cmp)> q(cmp);
    q.push(std::move(World(start, startStep)));
    auto pushQ = [&](const World& next) {
        const auto hash = next.hash();
        if (auto it = visited.find(hash); it == visited.end()) {
            visited[hash] = next.step;
            if (next.pos[0] == end[0] && next.pos[1] == end[1] && next.step < minSteps) {
                minSteps = next.step;
            }
            q.push(std::move(next));
        }
    };
    while (!q.empty()) {
        auto world = q.top();
        q.pop();
        if (world.pos == end) {
            continue;
        }
        world.step++;
        if (world.step >= minSteps) {
            continue;
        }
        const auto [i, j] = world.pos;
        const auto& map = maps[world.step % period];
        for (const auto& adj : adjs) {
            Pos nextPos{static_cast<uint16_t>(i + adj[0]), static_cast<uint16_t>(j + adj[1])};
            if (map[nextPos[0]][nextPos[1]] != Ground) {
                continue;
            }
            pushQ(World(nextPos, world.step));
        }
        if (map[i][j] != Ground) {
            continue;
        }
        pushQ(world);
    }
    return minSteps - startStep;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    const Pos dim{static_cast<uint16_t>(lines.size()), static_cast<uint16_t>(lines[0].size())};
    Map map{static_cast<size_t>(dim[0]), std::vector<int16_t>(dim[1], Ground)};

    for (int16_t i = 0; i < dim[0]; ++i) {
        for (int16_t j = 0; j < dim[1]; ++j) {
            if (lines[i][j] == '>') {
                map[i][j] = E;
            } else if (lines[i][j] == '<') {
                map[i][j] = W;
            } else if (lines[i][j] == '^') {
                map[i][j] = N;
            } else if (lines[i][j] == 'v') {
                map[i][j] = S;
            } else if (lines[i][j] == '#') {
                map[i][j] = Wall;
            }
        }
    }
    {
        const std::vector<int16_t> border(dim[1], Wall);
        map.insert(map.begin(), border);
        map.emplace_back(border);
    }

    // Precalculate all possible map states based on the lcm of the grid dimensions
    Maps maps{};
    maps.push_back(map);
    for (int i = 1; i < std::lcm(dim[0] - 2, dim[1] - 2); ++i) {
        move(map);
        maps.push_back(map);
    }

    constexpr Pos start{1, 1};
    const Pos end{static_cast<uint16_t>(dim[0]), static_cast<uint16_t>(dim[1] - 2)};
    uint16_t count{};
    {  // Part 1
        count = dijkstra(maps, start, end);
        std::cout << count << std::endl;
    }
    {  // Part 2
        count += dijkstra(maps, end, start, count);
        count += dijkstra(maps, start, end, count);
        std::cout << count << std::endl;
    }

    return EXIT_SUCCESS;
}
