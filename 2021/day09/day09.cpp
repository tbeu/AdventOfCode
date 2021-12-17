// Advent of Code 2021
// Day 9: Smoke Basin
// https://adventofcode.com/2021/day/9

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <gsl/util>

static bool readFile(std::string fileName, std::vector<std::string>& map)
{
    std::ifstream in{fileName.c_str()};
    if (!in) {
        std::cerr << "Cannot open file " << fileName << std::endl;
        return false;
    }
    auto closeStream = gsl::finally([&in] { in.close(); });
    std::string str;
    while (std::getline(in, str)) {
        map.push_back(str);
    }
    return true;
}

constexpr std::string::value_type zero = '0';
constexpr std::string::value_type nine = '9';
using Point = std::pair<size_t, size_t>;
using Points = std::vector<Point>;
using Lines = std::vector<std::string>;

static void findBasin(const Lines& map, Points& points)
{
    const auto& point{points.back()};
    size_t i{point.first};
    size_t j{point.second};
    const auto& val{map[i][j]};
    const std::array<Point, 4> adjs{{{i + 1, j}, {i - 1, j}, {i, j + 1}, {i, j - 1}}};
    for (const auto& adj : adjs) {
        if (std::find(points.cbegin(), points.cend(), adj) != points.cend()) {
            continue;
        }
        if (val < map[adj.first][adj.second] && map[adj.first][adj.second] < nine) {
            points.push_back(adj);
            findBasin(map, points);
        }
    }
}

int main(int argc, char* argv[])
{
    std::vector<std::string> map{};
    if (argc == 2) {
        if (!readFile(argv[1], map)) {
            return EXIT_FAILURE;
        }
    }
    if (map.size() < 1) {
        return EXIT_FAILURE;
    }

    size_t len{map[0].size()};
    {
        std::string border(len, nine);
        map.insert(map.begin(), border);
        map.emplace_back(border);
    }
    std::for_each(map.begin(), map.end(), [](auto& row) { row = nine + row + nine; });
    Points mins{};
    for (size_t i = 1; i < map.size() - 1; ++i) {
        for (size_t j = 1; j < len + 1; ++j) {
            const auto& val{map[i][j]};
            if (val < map[i + 1][j] && val < map[i - 1][j] && val < map[i][j + 1] && val < map[i][j - 1]) {
                mins.push_back({i, j});
            }
        }
    }

    {  //Part 1
        size_t risk{mins.size()};
        for (const auto& point : mins) {
            const auto& val{map[point.first][point.second]};
            risk += val - zero;
        }
        std::cout << risk << std::endl;
    }
    {  //Part 2
        std::vector<size_t> sizes(mins.size());
        std::transform(mins.begin(), mins.end(), sizes.begin(), [&](const auto& point) {
            Points points{point};
            findBasin(map, points);
            return points.size();
        });
        std::nth_element(sizes.begin(), sizes.begin() + 2, sizes.end(), std::greater{});
        size_t prod{sizes[0] * sizes[1] * sizes[2]};
        std::cout << prod << std::endl;
    }

    return EXIT_SUCCESS;
}
