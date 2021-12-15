#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <map>
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
using Map = std::map<Point, size_t>;

static size_t dijkstra(Map& map, const Point& src, const Point& dst, const Point& dim)
{
    std::map<Point, size_t> dist{};
    auto cmp = [&](const auto& a, const auto& b) { return dist[a] > dist[b]; };
    std::priority_queue<Point, std::vector<Point>, decltype(cmp)> q(cmp);
    for (auto& [node, _] : map) {
        dist[node] = UINT32_MAX;
    }
    dist[src] = 0;
    q.push(src);
    auto [dim1, dim2] = dim;
    while (!q.empty()) {
        auto [i, j] = q.top();
        auto d = dist[q.top()];
        q.pop();
        if (i < dim1 && dist[{i + 1, j}] == UINT32_MAX) {
            if (auto alt{d + map[{i + 1, j}]}; alt < dist[{i + 1, j}]) {
                dist[{i + 1, j}] = alt;
                q.push({i + 1, j});
            }
        }
        if (j < dim2 && dist[{i, j + 1}] == UINT32_MAX) {
            if (auto alt{d + map[{i, j + 1}]}; alt < dist[{i, j + 1}]) {
                dist[{i, j + 1}] = alt;
                q.push({i, j + 1});
            }
        }
        if (i > 0 && dist[{i - 1, j}] == UINT32_MAX) {
            if (auto alt{d + map[{i - 1, j}]}; alt < dist[{i - 1, j}]) {
                dist[{i - 1, j}] = alt;
                q.push({i - 1, j});
            }
        }
        if (j > 0 && dist[{i, j - 1}] == UINT32_MAX) {
            if (auto alt{d + map[{i, j - 1}]}; alt < dist[{i, j - 1}]) {
                dist[{i, j - 1}] = alt;
                q.push({i, j - 1});
            }
        }
    }
    return dist[dst];
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
        Map map{};
        for (size_t i = 0; i < dim1; i++) {
            for (size_t j = 0; j < dim2; j++) {
                map.insert_or_assign({i, j}, lines[i][j] - '0');
            }
        }
        auto dist = dijkstra(map, {0, 0}, {dim1 - 1, dim2 - 1}, {dim1, dim2});
        std::cout << dist << std::endl;
    }
    {  // Part 2
        Map map{};
        for (size_t k = 0; k < 5; ++k) {
            for (size_t l = 0; l < 5; ++l) {
                for (size_t i = 0; i < lines.size(); i++) {
                    for (size_t j = 0; j < lines[i].size(); j++) {
                        size_t v{lines[i][j] - '0' + k + l};
                        v = 1 + (v - 1) % 9;
                        map.insert_or_assign({k * dim1 + i, l * dim2 + j}, v);
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
