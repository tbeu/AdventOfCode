// Advent of Code 2022
// Day 18: Boiling Boulders
// https://adventofcode.com/2022/day/18

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
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

using Cube = std::array<int16_t, 3>;
using Cubes = std::vector<Cube>;

size_t bfs(const Cubes& cubes, const Cube& mins, const Cube& maxs)
{
    size_t count{};
    Cubes visited{};
    std::queue<Cube> q{};
    q.push(mins);
    auto isVisited = [&visited](const Cube& cube) {
        return std::find(visited.crbegin(), visited.crend(), cube) != visited.crend();
    };
    auto isLava = [&cubes](const Cube& cube) { return std::find(cubes.cbegin(), cubes.cend(), cube) != cubes.cend(); };
    auto evalCube = [&](const Cube&& cube) {
        if (isLava(cube)) {
            count++;
        } else if (!isVisited(cube)) {
            visited.push_back(cube);
            q.push(cube);
        }
    };

    while (!q.empty()) {
        auto cube = q.front();
        q.pop();
        auto [x, y, z] = cube;
        if (x > mins[0]) {
            evalCube({x - 1, y, z});
        }
        if (x < maxs[0]) {
            evalCube({x + 1, y, z});
        }
        if (y > mins[1]) {
            evalCube({x, y - 1, z});
        }
        if (y < maxs[1]) {
            evalCube({x, y + 1, z});
        }
        if (z > mins[2]) {
            evalCube({x, y, z - 1});
        }
        if (z < maxs[2]) {
            evalCube({x, y, z + 1});
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

    Cubes cubes{};
    for (const auto& line : lines) {
        std::istringstream iss{line};
        int16_t x, y, z;
        iss >> x;
        iss.ignore(1);
        iss >> y;
        iss.ignore(1);
        iss >> z;
        cubes.emplace_back(Cube{x, y, z});
    }

    int16_t xmin{INT16_MAX}, xmax{INT16_MIN}, ymin{INT16_MAX}, ymax{INT16_MIN}, zmin{INT16_MAX}, zmax{INT16_MIN};
    {  // Part 1
        size_t count{6 * cubes.size()};
        for (size_t i = 0; i < cubes.size(); ++i) {
            const auto [xi, yi, zi] = cubes[i];
            xmin = std::min(xmin, xi);
            ymin = std::min(ymin, yi);
            zmin = std::min(zmin, zi);
            xmax = std::max(xmax, xi);
            ymax = std::max(ymax, yi);
            zmax = std::max(zmax, zi);
            for (size_t j = i + 1; j < cubes.size(); ++j) {
                const auto [xj, yj, zj] = cubes[j];
                if ((std::abs(xi - xj) + std::abs(yi - yj) + std::abs(zi - zj)) == 1) {
                    count -= 2;
                }
            }
        }
        std::cout << count << std::endl;
    }
    {  // Part 2
        auto count = bfs(cubes, {xmin - 1, ymin - 1, zmin - 1}, {xmax + 1, ymax + 1, zmax + 1});
        std::cout << count << std::endl;
    }

    return EXIT_SUCCESS;
}
