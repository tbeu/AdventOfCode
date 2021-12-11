#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <queue>
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

constexpr size_t dim{10};
using Point = std::pair<size_t, size_t>;
using Row = std::array<uint16_t, dim + 2>;
using Map = std::array<Row, dim + 2>;

static uint64_t simulate(Map& map)
{
    uint64_t count{0};
    std::queue<Point> q{};
    for (size_t i = 1; i < dim + 1; ++i) {
        for (size_t j = 1; j < dim + 1; ++j) {
            if (++map[i][j] > 9) {
                q.emplace(i, j);
                map[i][j] = 0;
            }
        }
    }

    while (!q.empty()) {
        ++count;
        const auto& f{q.front()};
        q.pop();
        size_t i{f.first};
        size_t j{f.second};
        const std::array<Point, 8> adjs{{{i + 1, j},
                                         {i + 1, j + 1},
                                         {i + 1, j - 1},
                                         {i, j + 1},
                                         {i, j - 1},
                                         {i - 1, j},
                                         {i - 1, j + 1},
                                         {i - 1, j - 1}}};
        for (const auto& adj : adjs) {
            if (map[adj.first][adj.second] == 0) {
                continue;
            }
            if (++map[adj.first][adj.second] > 9) {
                q.emplace(adj.first, adj.second);
                map[adj.first][adj.second] = 0;
            }
        }
    }
    return count;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }
    if (lines.size() != dim ||
        std::any_of(lines.cbegin(), lines.cend(), [](const auto& line) { return line.size() != dim; })) {
        return EXIT_FAILURE;
    }

    Map map{};
    for (size_t i = 0; i < dim; ++i) {
        for (size_t j = 0; j < dim; ++j) {
            map[i + 1][j + 1] = lines[i][j] - '0';
        }
    }

    size_t days{0};
    {  // Part 1
        uint64_t count100{0};
        for (size_t day = 0; day < 100; ++day) {
            const auto count{simulate(map)};
			count100 += count;
            if (days == 0 && count == dim * dim) {
                // Early case for part 2
                days = day + 1;
            }
        }
        std::cout << count100 << std::endl;
    }
    {  // Part 2
        if (days == 0) {
            days = 100;
            uint64_t count{0};
            do {
                ++days;
                count = simulate(map);
            } while (count < dim * dim);
        }
        std::cout << days << std::endl;
    }

    return EXIT_SUCCESS;
}
