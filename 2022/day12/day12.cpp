// Advent of Code 2022
// Day 12: Hill Climbing Algorithm
// https://adventofcode.com/2022/day/12

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
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

using Pos = std::array<uint32_t, 2>;
using Map = std::vector<std::vector<uint32_t> >;

static uint32_t bfs(const Map& map, const Pos& start, const Pos& end)
{
    Pos dim{static_cast<uint32_t>(map.size()), static_cast<uint32_t>(map[0].size())};
    Map dist{dim[0], std::vector<uint32_t>(dim[1])};
    for (uint32_t i = 0; i < dim[0]; ++i) {
        for (uint32_t j = 0; j < dim[1]; ++j) {
            dist[i][j] = UINT32_MAX;
        }
    }
    dist[start[0]][start[1]] = 0;
    std::queue<Pos> q{};
    q.push(start);
    while (!q.empty()) {
        const auto [i, j] = q.front();
        const auto d = dist[i][j];
        q.pop();
        if (i < dim[0] - 1 && d + 1 < dist[i + 1][j] && map[i + 1][j] <= map[i][j] + 1) {
            dist[i + 1][j] = d + 1;
            q.push({i + 1, j});
        }
        if (j < dim[1] - 1 && d + 1 < dist[i][j + 1] && map[i][j + 1] <= map[i][j] + 1) {
            dist[i][j + 1] = d + 1;
            q.push({i, j + 1});
        }
        if (i > 0 && d + 1 < dist[i - 1][j] && map[i - 1][j] <= map[i][j] + 1) {
            dist[i - 1][j] = d + 1;
            q.push({i - 1, j});
        }
        if (j > 0 && d + 1 < dist[i][j - 1] && map[i][j - 1] <= map[i][j] + 1) {
            dist[i][j - 1] = d + 1;
            q.push({i, j - 1});
        }
    }
    return dist[end[0]][end[1]];
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    Pos S{};
    Pos E{};
    Pos dim{static_cast<uint32_t>(lines.size()), static_cast<uint32_t>(lines[0].size())};
    Map map{dim[0], std::vector<uint32_t>(dim[1])};

    for (uint32_t i = 0; i < dim[0]; ++i) {
        for (uint32_t j = 0; j < dim[1]; ++j) {
            if (lines[i][j] == 'S') {
                S = {i, j};
                map[i][j] = 0;
            } else if (lines[i][j] == 'E') {
                E = {i, j};
                map[i][j] = 'z' - 'a';
            } else {
                map[i][j] = lines[i][j] - 'a';
            }
        }
    }

    uint32_t dist{UINT32_MAX};
    {
        // Part 1
        dist = bfs(map, S, E);
        std::cout << dist << std::endl;
    }
    {  // Part 2
        for (uint32_t i = 0; i < dim[0]; ++i) {
            for (uint32_t j = 0; j < dim[1]; ++j) {
                if (map[i][j] == 0) {
                    dist = std::min<uint32_t>(bfs(map, Pos{i, j}, E), dist);
                }
            }
        }
        std::cout << dist << std::endl;
    }

    return EXIT_SUCCESS;
}
