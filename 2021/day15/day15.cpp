// Advent of Code 2021
// Day 15: Chiton
// https://adventofcode.com/2021/day/15

#include <algorithm>
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

using Point = std::pair<size_t, size_t>;
using Map = std::vector<std::vector<size_t> >;

static size_t dijkstra(Map& map, const Point& src, const Point& dst, const Point& dim)
{
    auto [dim1, dim2] = dim;
    Map dist{dim1, std::vector<size_t>(dim2)};
    auto cmp = [&](const auto& a, const auto& b) { return dist[a.first][a.second] > dist[b.first][b.second]; };
    std::priority_queue<Point, std::vector<Point>, decltype(cmp)> q(cmp);
    for (size_t i = 0; i < dim1; i++) {
        for (size_t j = 0; j < dim2; j++) {
            dist[i][j] = SIZE_MAX;
        }
    }
    dist[src.first][src.second] = 0;
    q.push(src);
    while (!q.empty()) {
        auto [i, j] = q.top();
        auto d = dist[i][j];
        q.pop();
        if (i < dim1 - 1 && dist[i + 1][j] == SIZE_MAX) {
            if (auto alt{d + map[i + 1][j]}; alt < dist[i + 1][j]) {
                dist[i + 1][j] = alt;
                q.push({i + 1, j});
            }
        }
        if (j < dim2 - 1 && dist[i][j + 1] == SIZE_MAX) {
            if (auto alt{d + map[i][j + 1]}; alt < dist[i][j + 1]) {
                dist[i][j + 1] = alt;
                q.push({i, j + 1});
            }
        }
        if (i > 0 && dist[i - 1][j] == SIZE_MAX) {
            if (auto alt{d + map[i - 1][j]}; alt < dist[i - 1][j]) {
                dist[i - 1][j] = alt;
                q.push({i - 1, j});
            }
        }
        if (j > 0 && dist[i][j - 1] == SIZE_MAX) {
            if (auto alt{d + map[i][j - 1]}; alt < dist[i][j - 1]) {
                dist[i][j - 1] = alt;
                q.push({i, j - 1});
            }
        }
    }
    return dist[dst.first][dst.second];
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    auto dim1{lines.size()};
    auto dim2{lines[0].size()};
    {  // Part 1
        Map map{dim1, std::vector<size_t>(dim2)};
        for (size_t i = 0; i < dim1; i++) {
            for (size_t j = 0; j < dim2; j++) {
                map[i][j] = lines[i][j] - '0';
            }
        }
        auto dist = dijkstra(map, {0, 0}, {dim1 - 1, dim2 - 1}, {dim1, dim2});
        std::cout << dist << std::endl;
    }
    {  // Part 2
        Map map{5 * dim1, std::vector<size_t>(5 * dim2)};
        for (size_t k = 0; k < 5; ++k) {
            for (size_t l = 0; l < 5; ++l) {
                for (size_t i = 0; i < lines.size(); i++) {
                    for (size_t j = 0; j < lines[i].size(); j++) {
                        size_t v{lines[i][j] - '0' + k + l};
                        v = 1 + (v - 1) % 9;
                        map[k * dim1 + i][l * dim2 + j] = v;
                    }
                }
            }
        }
        dim1 *= 5;
        dim2 *= 5;
        auto dist = dijkstra(map, {0, 0}, {dim1 - 1, dim2 - 1}, {dim1, dim2});
        std::cout << dist << std::endl;
    }

    return EXIT_SUCCESS;
}
