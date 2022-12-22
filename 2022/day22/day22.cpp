// Advent of Code 2022
// Day 22: Monkey Map
// https://adventofcode.com/2022/day/22

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
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

constexpr char walk{'.'};
constexpr char wall{'#'};
using Face = int32_t;
constexpr Face r{0};
constexpr Face d{1};
constexpr Face l{2};
constexpr Face u{3};
using Pos = std::array<int32_t, 2>;
using Map = std::map<Pos, bool>;
std::array<Pos, 4> Update{{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};

enum class WrapEdge : int32_t
{
    Flat = 1,
    Cube = 2,
};

bool wrapFlatWithLoop(const Map& map, Pos& pos, Face face)
{
    const auto [yLast, xLast] = pos;
    auto& [y, x] = pos;
    const auto [dy, dx] = Update[face];
    while (true) {
        y -= dy;
        x -= dx;
        if (const auto it = map.find(pos); it == map.cend()) {
            y += dy;
            x += dx;
            break;
        }
    }
    if (const auto it = map.find(pos); !it->second) {
        // Reset if wall
        y = yLast;
        x = xLast;
        return false;
    }
    return true;
}

/*
 Wrapping on edges is based on the following tile layout with edges labeled 01 ... 14.
 Each of the six tiles is of dimension 50 x 50 (which is hard-coded).

            |--13--|--14--|
            |      |      |
           12      |      01
            |      |      |
            |------|--02--|
            |      |
           11      03
            |      |
     |--10--|------|
     |      |      |
    09      |      04
     |      |      |
     |------|--05--|
     |      |
    08      06
     |      |
     |--07--|

*/

bool wrapFlat(const Map& map, Pos& pos, Face face)
{
    const auto [yLast, xLast] = pos;
    auto& [y, x] = pos;
    const auto [dy, dx] = Update[face];
    y += dy;
    x += dx;
    if (face == r && x >= 150 && 0 <= y && y < 50) {  // 1 -> 12
        x = 50;
    } else if (face == d && y >= 50 && 100 <= x && x < 150) {  // 2 -> 14
        y = 0;
    } else if (face == r && x >= 100 && 50 <= y && y < 100) {  // 3 -> 11
        x = 50;
    } else if (face == r && x >= 100 && 100 <= y && y < 150) {  // 4 -> 9
        x = 0;
    } else if (face == d && y >= 150 && 50 <= x && x < 100) {  // 5 -> 13
        y = 0;
    } else if (face == r && x >= 50 && 150 <= y && y < 200) {  // 6 -> 8
        x = 0;
    } else if (face == d && y >= 200 && 0 <= x && x < 50) {  // 7 -> 10
        y = 100;
    } else if (face == l && x <= -1 && 150 <= y && y < 200) {  // 8 -> 6
        x = 49;
    } else if (face == l && x <= -1 && 100 <= y && y < 150) {  // 9 -> 4
        x = 99;
    } else if (face == u && y <= 99 && 0 <= x && x < 50) {  // 10 -> 7
        y = 199;
    } else if (face == l && x <= 49 && 50 <= y && y < 100) {  // 11 -> 3
        x = 99;
    } else if (face == l && x <= 49 && 0 <= y && y < 50) {  // 12 -> 1
        x = 149;
    } else if (face == u && y <= -1 && 50 <= x && x < 100) {  // 13 -> 5
        y = 149;
    } else if (face == u && y <= -1 && 100 <= x && x < 150) {  // 14 -> 2
        y = 49;
    }
    if (const auto it = map.find(pos); !it->second) {
        // Reset if wall
        x = xLast;
        y = yLast;
        return false;
    }
    return true;
}

bool wrapCube(const Map& map, Pos& pos, Face& face)
{
    const auto [yLast, xLast] = pos;
    const auto faceLast = face;
    auto& [y, x] = pos;
    const auto [dy, dx] = Update[face];
    y += dy;
    x += dx;
    if (face == r && x >= 150 && 0 <= y && y < 50) {  // 1 -> 4
        x = 99;
        y = 149 - yLast;
        face = l;
    } else if (face == d && y >= 50 && 100 <= x && x < 150) {  // 2 -> 3
        y = xLast - 50;
        x = 99;
        face = l;
    } else if (face == r && x >= 100 && 50 <= y && y < 100) {  // 3 -> 2
        x = yLast + 50;
        y = 49;
        face = u;
    } else if (face == r && x >= 100 && 100 <= y && y < 150) {  // 4 -> 1
        x = 149;
        y = 149 - yLast;
        face = l;
    } else if (face == d && y >= 150 && 50 <= x && x < 100) {  // 5 -> 6
        y = 100 + xLast;
        x = 49;
        face = l;
    } else if (face == r && x >= 50 && 150 <= y && y < 200) {  // 6 -> 5
        x = yLast - 100;
        y = 149;
        face = u;
    } else if (face == d && y >= 200 && 0 <= x && x < 50) {  // 7 -> 14
        x = 100 + xLast;
        y = 0;
        face = d;
    } else if (face == l && x <= -1 && 150 <= y && y < 200) {  // 8 -> 13
        x = yLast - 100;
        y = 0;
        face = d;
    } else if (face == l && x <= -1 && 100 <= y && y < 150) {  // 9 -> 12
        x = 50;
        y = 149 - yLast;
        face = r;
    } else if (face == u && y <= 99 && 0 <= x && x < 50) {  // 10 -> 11
        y = 50 + xLast;
        x = 50;
        face = r;
    } else if (face == l && x <= 49 && 50 <= y && y < 100) {  // 11 -> 10
        x = yLast - 50;
        y = 100;
        face = d;
    } else if (face == l && x <= 49 && 0 <= y && y < 50) {  // 12 -> 9
        y = 149 - yLast;
        x = 0;
        face = r;
    } else if (face == u && y <= -1 && 50 <= x && x < 100) {  // 13 -> 8
        y = 100 + xLast;
        x = 0;
        face = r;
    } else if (face == u && y <= -1 && 100 <= x && x < 150) {  // 14 -> 7
        x = xLast - 100;
        y = 199;
        face = u;
    }
    if (const auto it = map.find(pos); !it->second) {
        // Reset if wall
        x = xLast;
        y = yLast;
        face = faceLast;
        return false;
    }
    return true;
}

void walkMap(const Map& map, Pos& pos, Face& face, uint32_t steps, WrapEdge wrapEdge)
{
    auto& [y, x] = pos;
    const auto [yLast, xLast] = pos;
    auto [dy, dx] = Update[face];
    uint32_t step{};
    while (step < steps) {
        step++;
        y += dy;
        x += dx;
        if (const auto it = map.find(pos); it != map.cend()) {
            if (!it->second) {
                y -= dy;
                x -= dx;
                break;
            }
        } else {
            y -= dy;
            x -= dx;
            if (wrapEdge == WrapEdge::Cube) {
                if (wrapCube(map, pos, face)) {
                    // Update walking direction based on updated face
                    dy = Update[face][0];
                    dx = Update[face][1];
                } else {
                    break;
                }
            } else {
                if (!wrapFlat(map, pos, face)) {
                    break;
                }
            }
        }
    }
}

std::pair<Pos, Face> walkPath(const Map& map, int32_t yStart, const std::string& path, WrapEdge wrapEdge)
{
    Pos pos{0, yStart};
    Face face{r};
    std::istringstream iss{path};
    int32_t steps;
    while (iss >> steps) {
        walkMap(map, pos, face, steps, wrapEdge);
        char turn;
        if (iss >> turn) {
            if (turn == 'R') {
                face = (face + 1) % 4;
            } else if (turn == 'L') {
                face = (face + 3) % 4;
            }
        }
    }
    return {pos, face};
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    Map map{};
    int32_t yStart{};
    for (int32_t i = 0; i < lines.size(); ++i) {
        if (lines[i].empty()) {
            break;
        }
        for (int32_t j = 0; j < lines[i].size(); ++j) {
            if (lines[i][j] == walk) {
                map[{i, j}] = true;
            } else if (lines[i][j] == wall) {
                map[{i, j}] = false;
            } else if (i == 0) {
                yStart++;
            }
        }
    }
    std::string& path = lines[lines.size() - 1];

    {  // Part 1
        const auto& [pos, face] = walkPath(map, yStart, path, WrapEdge::Flat);
        std::cout << 1000 * (pos[0] + 1) + 4 * (pos[1] + 1) + face << std::endl;
    }
    {  // Part 2
        const auto& [pos, face] = walkPath(map, yStart, path, WrapEdge::Cube);
        std::cout << 1000 * (pos[0] + 1) + 4 * (pos[1] + 1) + face << std::endl;
    }

    return EXIT_SUCCESS;
}
