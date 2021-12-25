// Advent of Code 2021
// Day 25: Sea Cucumber
// https://adventofcode.com/2021/day/25

#include <algorithm>
#include <fstream>
#include <iostream>
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

constexpr int8_t E{1};
constexpr int8_t S{-1};
constexpr int8_t FREE{0};
using Map = std::vector<std::vector<int8_t> >;

static bool moveE(Map& map)
{
    const auto m{map.size()};
    const auto n{map[0].size()};
    bool hasMoved{false};
    for (size_t i = 0; i < m; ++i) {
        auto first{map[i][0]};
        auto last{map[i][n - 1]};
        auto temp{first};
        for (size_t j = 0; j < n - 1; ++j) {
            if (temp == E && map[i][j + 1] == FREE) {
                map[i][j] = FREE;
                temp = FREE;
                map[i][j + 1] = E;
                hasMoved = true;
            } else {
                temp = map[i][j + 1];
            }
        }
        if (last == E && first == FREE) {
            map[i][n - 1] = FREE;
            map[i][0] = E;
            hasMoved = true;
        }
    }
    return hasMoved;
}

static bool moveS(Map& map)
{
    const auto m{map.size()};
    const auto n{map[0].size()};
    bool hasMoved{false};
    for (size_t j = 0; j < n; ++j) {
        auto first{map[0][j]};
        auto last{map[m - 1][j]};
        auto temp{first};
        for (size_t i = 0; i < m - 1; ++i) {
            if (temp == S && map[i + 1][j] == FREE) {
                map[i][j] = FREE;
                temp = FREE;
                map[i + 1][j] = S;
                hasMoved = true;
            } else {
                temp = map[i + 1][j];
            }
        }
        if (last == S && first == FREE) {
            map[m - 1][j] = FREE;
            map[0][j] = S;
            hasMoved = true;
        }
    }
    return hasMoved;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    const auto m{lines.size()};
    const auto n{lines[0].size()};

    if (std::any_of(lines.cbegin(), lines.cend(), [&](const auto& line) { return line.size() != n; })) {
        return EXIT_FAILURE;
    }

    Map map{m, std::vector<int8_t>(n)};
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (lines[i][j] == '>') {
                map[i][j] = E;
            } else if (lines[i][j] == 'v') {
                map[i][j] = S;
            }
        }
    }

    bool hasMovedE{false};
    bool hasMovedS{false};
    size_t count{0};
    do {
        ++count;
        hasMovedE = moveE(map);
        hasMovedS = moveS(map);
    } while (hasMovedE || hasMovedS);

    std::cout << count << std::endl;

    return EXIT_SUCCESS;
}
