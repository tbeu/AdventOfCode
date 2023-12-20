// Advent of Code 2023
// Day 18: Lavaduct Lagoon
// https://adventofcode.com/2023/day/18

#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
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

using Pos = std::array<int64_t, 2>;
constexpr auto adjs = std::array<Pos, 4>{{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};

inline int64_t manhattanDistance(const Pos& a, const Pos& b)
{
    return std::abs(a[0] - b[0]) + std::abs(a[1] - b[1]);
}

static int64_t count(const std::vector<Pos>& polygon)
{
    int64_t len{}, count{};
    for (size_t i = 0; i < polygon.size() - 1; ++i) {
        const auto& [r1, c1] = polygon[i];
        const auto& [r2, c2] = polygon[i + 1];
        count += (r1 + r2) * (c1 - c2);
        len += std::abs(r1 - r2) + std::abs(c1 - c2);
    }
    {
        const auto& [r1, c1] = polygon[polygon.size() - 1];
        const auto& [r2, c2] = polygon[0];
        count += (r1 + r2) * (c1 - c2);
        len += std::abs(r1 - r2) + std::abs(c1 - c2);
    }
    return count / 2 + len / 2 + 1;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (2 != argc || !readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    {  // Part 1
        int64_t lastr{};
        int64_t lastc{};
        std::vector<Pos> polygon{Pos{0, 0}};
        for (const auto& line : lines) {
            char d;
            int64_t amt;
            std::istringstream iss{line};
            iss >> d >> amt;
            if ('R' == d) {
                lastc += amt;
            } else if ('D' == d) {
                lastr += amt;
            } else if ('L' == d) {
                lastc -= amt;
            } else if ('U' == d) {
                lastr -= amt;
            }
            polygon.push_back(Pos{lastr, lastc});
        }
        std::cout << count(polygon) << std::endl;
    }
    {  // Part 2
        int64_t lastr{};
        int64_t lastc{};
        std::vector<Pos> polygon{Pos{0, 0}};
        for (const auto& line : lines) {
            char d;
            int64_t amt;
            std::string color;
            std::istringstream iss{line};
            iss >> d >> amt >> color;
            d = color[color.size() - 2];
            color = color.substr(2, 5);
            char* endptr;
            amt = std::strtol(color.c_str(), &endptr, 16);
            if ('0' == d) {
                lastc += amt;
            } else if ('1' == d) {
                lastr += amt;
            } else if ('2' == d) {
                lastc -= amt;
            } else if ('3' == d) {
                lastr -= amt;
            }
            polygon.push_back(Pos{lastr, lastc});
        }
        std::cout << count(polygon) << std::endl;
    }

    return EXIT_SUCCESS;
}
