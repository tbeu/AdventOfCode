// Advent of Code 2022
// Day 14: Regolith Reservoir
// https://adventofcode.com/2022/day/14

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <sstream>
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

constexpr char air{'.'};
constexpr char sand{'o'};
constexpr char rock{'#'};
constexpr size_t dim{1000};
using Map = std::array<std::array<char, dim>, dim>;

size_t toMap(const std::vector<std::string>& lines, std::unique_ptr<Map>& pmap)
{
    auto& map = *pmap;
    size_t ymax{0};
    for (auto& row : map) {
        std::fill_n(row.begin(), row.size(), air);
    }

    for (const auto& line : lines) {
        std::istringstream iss{line};
        size_t x, y;
        char c;
        std::vector<std::array<size_t, 2> > pos{};
        while (iss >> x >> c >> y) {
            pos.push_back({x, y});
            iss.ignore(4);
            ymax = std::max<size_t>(y, ymax);
        }
        for (size_t i = 0; i < pos.size() - 1; ++i) {
            auto& [x1, y1] = pos[i];
            auto& [x2, y2] = pos[i + 1];
            if (y1 < y2) {
                for (size_t y = y1; y <= y2; ++y) {
                    map[x1][y] = rock;
                }
            } else if (y2 < y1) {
                for (size_t y = y2; y <= y1; ++y) {
                    map[x1][y] = rock;
                }
            } else if (x1 < x2) {
                for (size_t x = x1; x <= x2; ++x) {
                    map[x][y1] = rock;
                }
            } else if (x2 < x1) {
                for (size_t x = x2; x <= x1; ++x) {
                    map[x][y1] = rock;
                }
            }
        }
    }
    return ymax;
}

size_t simulate(std::unique_ptr<Map>& pmap, size_t ymax)
{
    auto& map = *pmap;
    size_t round{};
    bool done{false};
    while (!done) {
        size_t x{500};
        size_t y{0};
        do {
            y++;
            if (map[x][y] == air) {
                continue;
            }
            if (map[x - 1][y] == air) {
                x--;
                continue;
            }
            if (map[x + 1][y] == air) {
                x++;
                continue;
            }
            done = --y == 0;
            break;
        } while (y < ymax);
        if (y >= ymax) {
            done = true;
            break;
        }
        map[x][y] = sand;
        round++;
    }
    return round;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    auto pmap = std::make_unique<Map>();
    auto ymax = toMap(lines, pmap);
    size_t round{};

    {  // Part 1
        round += simulate(pmap, ymax);
        std::cout << round << std::endl;
    }
    {  // Part 2
        ymax += 2;
        auto& map = *pmap;
        for (size_t x = 0; x < dim; ++x) {
            map[x][ymax] = rock;
        }
        round += simulate(pmap, ymax);
        std::cout << round << std::endl;
    }
    return EXIT_SUCCESS;
}
