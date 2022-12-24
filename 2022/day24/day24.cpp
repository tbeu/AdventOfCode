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
using Pos = std::array<int16_t, 2>;
using Map = std::vector<std::vector<int16_t> >;
using Maps = std::vector<Map>;

struct World
{
    World(const Pos& pos, size_t step = 0, uint16_t wait = 0) : pos{pos}, step{step}, wait{wait}
    {
    }
    Pos pos{};
    size_t step{};
    uint16_t wait{};

    uint64_t hash() const
    {
        uint64_t hash{};
        hash |= static_cast<uint64_t>(pos[0]);
        hash |= static_cast<uint64_t>(pos[1]) << 16;
        hash |= static_cast<uint64_t>(step) << 32;
        hash |= static_cast<uint64_t>(wait) << 48;
        return hash;
    }
};

void move(Map& map)
{
    Pos dim{static_cast<int16_t>(map.size()), static_cast<int16_t>(map[0].size())};
    Map next{static_cast<size_t>(dim[0] - 2), std::vector<int16_t>(dim[1])};
    {
        std::vector<int16_t> border(dim[1], Wall);
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

size_t bfs(const Maps& maps, const Pos& start, const Pos& end, size_t startStep = 0)
{
    std::map<uint64_t, size_t> visited{};
    size_t minSteps{UINT32_MAX};
    const auto period = maps.size();
    constexpr std::array<Pos, 4> adjs{{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};
    std::queue<World> q{};
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
        auto world = q.front();
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
            Pos nextPos{i + adj[0], j + adj[1]};
            if (map[nextPos[0]][nextPos[1]] != Ground) {
                continue;
            }
            pushQ(World(nextPos, world.step));
        }
        if (map[i][j] != Ground) {
            continue;
        }
        world.wait++;
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

    const Pos dim{static_cast<int16_t>(lines.size()), static_cast<int16_t>(lines[0].size())};
    Map map{static_cast<size_t>(dim[0]), std::vector<int16_t>(dim[1])};

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
        std::vector<int16_t> border(dim[1], Wall);
        map.insert(map.begin(), border);
        map.emplace_back(border);
    }

    // Precalculate all possible map states based on the lcm of the grid dimensions
    Maps maps{};
    maps.push_back(map);
    for (int16_t i = 1; i < std::lcm(dim[0] - 2, dim[1] - 2); ++i) {
        move(map);
        maps.push_back(map);
    }

    Pos start{1, 1};
    Pos end{static_cast<int16_t>(dim[0]), static_cast<int16_t>(dim[1] - 2)};
    size_t count{};
    {  // Part 1
        count = bfs(maps, start, end);
        std::cout << count << std::endl;
    }
    {  // Part 2
        count += bfs(maps, end, start, count);
        count += bfs(maps, start, end, count);
        std::cout << count << std::endl;
    }

    return EXIT_SUCCESS;
}
