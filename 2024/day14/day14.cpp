// Advent of Code 2024
// Day 14: Restroom Redoubt
// https://adventofcode.com/2024/day/14

#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <gsl/util>

static bool readFile(const std::string& fileName, std::vector<std::string>& lines)
{
    std::ifstream in{fileName};
    if (!in) {
        std::cerr << "Cannot open file " << fileName << '\n';
        return false;
    }
    auto closeStream = gsl::finally([&in] { in.close(); });
    std::string str;
    while (std::getline(in, str)) {
        lines.push_back(str);
    }
    return true;
}

constexpr const auto dim = std::array<int16_t, 2>{101, 103};
using Pos = std::array<int16_t, 2>;
using V = std::array<int16_t, 2>;
using Robot = std::pair<Pos, V>;
using Robots = std::vector<Robot>;

void simulate(Robots& robots)
{
    for (auto& robot : robots) {
        auto& [pos, v] = robot;
        pos[0] += v[0];
        pos[1] += v[1];
        for (auto i : {0, 1}) {
            while (pos[i] < 0) {
                pos[i] += dim[i];
            }
            while (pos[i] >= dim[i]) {
                pos[i] -= dim[i];
            }
        }
    }
}

bool find(const Pos& pos, const Robots& robots)
{
    for (const auto& [posr, _] : robots) {
        if (posr[0] == pos[0] && posr[1] == pos[1]) {
            return true;
        }
    }
    return false;
}

size_t dfs(const Pos& pos, const Robots& robots, std::vector<std::vector<bool> >& visited)
{
    auto [c, r] = pos;
    if (visited[r][c]) {
        return 0;
    }
    visited[r][c] = true;
    if (!find(pos, robots)) {
        return 0;
    }
    size_t count = 1;
    if (c < dim[0] - 1) {
        count += dfs(Pos{c + 1, r}, robots, visited);
    }
    if (r < dim[1] - 1) {
        count += dfs(Pos{c, r + 1}, robots, visited);
    }
    if (c > 0) {
        count += dfs(Pos{c - 1, r}, robots, visited);
    }
    if (r > 0) {
        count += dfs(Pos{c, r - 1}, robots, visited);
    }
    return count;
}

std::map<size_t, size_t> checkGroups(const Robots& robots)
{
    std::map<size_t, size_t> map{{1, 0}};
    auto visited = std::vector<std::vector<bool> >(dim[1], std::vector<bool>(dim[0], false));
    for (const auto [pos, v] : robots) {
        if (auto size = dfs(pos, robots, visited); size > 0) {
            map[size] += 1;
        }
        // Heuristic to exit early:
        // If more than 10 % of the robots are solitary, it is no longer likely to have a large group
        if (map[1] > robots.size() / 10) {
            break;
        }
    }
    return map;
}

size_t maxGroupSize(const Robots& robots)
{
    const auto map = checkGroups(robots);
    size_t max = 0;
    for (const auto& group : map) {
        max = std::max(max, group.first);
    }
    return max;
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    std::vector<std::string> lines{};
    if (!readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    Robots robots;
    for (const auto& line : lines) {
        Pos pos;
        V v;
        std::istringstream iss{line};
        iss.ignore(2);
        iss >> pos[0];
        iss.ignore(1);
        iss >> pos[1];
        iss.ignore(3);
        iss >> v[0];
        iss.ignore(1);
        iss >> v[1];
        robots.push_back(Robot{pos, v});
    }

    size_t step = 0;
    {  // Part 1
        while (step++ < 100) {
            simulate(robots);
        }
        uint64_t s1 = 0, s2 = 0, s3 = 0, s4 = 0;
        for (const auto& [pos, _] : robots) {
            s1 += (pos[0] < dim[0] / 2 && pos[1] < dim[1] / 2) ? 1 : 0;
            s2 += (pos[0] > dim[0] / 2 && pos[1] < dim[1] / 2) ? 1 : 0;
            s3 += (pos[0] < dim[0] / 2 && pos[1] > dim[1] / 2) ? 1 : 0;
            s4 += (pos[0] > dim[0] / 2 && pos[1] > dim[1] / 2) ? 1 : 0;
        }
        std::cout << s1 * s2 * s3 * s4 << '\n';
    }
    {  // Part 2
        const auto estimatedGroupSize = robots.size() / 10;
        while (true) {
            simulate(robots);
            if (maxGroupSize(robots) > estimatedGroupSize) {
                for (int16_t r = 0; r < dim[1]; ++r) {
                    for (int16_t c = 0; c < dim[0]; ++c) {
                        std::cout << (find(Pos{c, r}, robots) ? '#' : '.');
                    }
                    std::cout << '\n';
                }
                break;
            }
            ++step;
        }
        std::cout << step << '\n';
    }
    return EXIT_SUCCESS;
}
