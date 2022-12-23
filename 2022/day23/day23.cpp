// Advent of Code 2022
// Day 23: Unstable Diffusion
// https://adventofcode.com/2022/day/23

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
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

enum class Move : int32_t
{
    None = 0,
    North = 1,
    South = 2,
    West = 3,
    East = 4
};

constexpr char elve{'#'};
using Pos = std::array<int32_t, 2>;
using Map = std::map<Pos, Move>;

bool isFree(const Map& map, const Pos& pos)
{
    if (const auto it = map.find(pos); it == map.cend()) {
        return true;
    }
    return false;
}

bool move(const Map& in, Map& out, size_t round)
{
    std::vector<Map::iterator> blocked{};
    std::vector<Pos> none{};
    auto tryInsert = [&](const Pos& pos, Move move) {
        if (auto [it, inserted] = out.try_emplace(pos, move); !inserted) {
            blocked.push_back(it);
            const auto [i, j] = pos;
            if (move == Move::North) {
                none.push_back({i + 1, j});
            } else if (move == Move::South) {
                none.push_back({i - 1, j});
            } else if (move == Move::West) {
                none.push_back({i, j + 1});
            } else if (move == Move::East) {
                none.push_back({i, j - 1});
            }
        }
    };
    for (const auto& [pos, val] : in) {
        const auto [i, j] = pos;
        auto nw = isFree(in, {i - 1, j - 1});
        auto n = isFree(in, {i - 1, j});
        auto ne = isFree(in, {i - 1, j + 1});
        auto e = isFree(in, {i, j + 1});
        auto se = isFree(in, {i + 1, j + 1});
        auto s = isFree(in, {i + 1, j});
        auto sw = isFree(in, {i + 1, j - 1});
        auto w = isFree(in, {i, j - 1});
        if (nw && n && ne && e && se && s && sw && w) {
            none.push_back(pos);
            continue;
        }
        bool moved{false};
        for (size_t count = 0; count < 4; ++count) {
            const auto check = (count + round) % 4;
            if (check == 0 && nw && n && ne) {  // N
                tryInsert({i - 1, j}, Move::North);
                moved = true;
                break;
            }
            if (check == 1 && se && s && sw) {  // S
                tryInsert({i + 1, j}, Move::South);
                moved = true;
                break;
            }
            if (check == 2 && nw && w && sw) {  // W
                tryInsert({i, j - 1}, Move::West);
                moved = true;
                break;
            }
            if (check == 3 && ne && e && se) {  // E
                tryInsert({i, j + 1}, Move::East);
                moved = true;
                break;
            }
        }
        if (!moved) {
            none.push_back(pos);
        }
    }

    for (auto it : blocked) {
        auto [i, j] = it->first;
        auto move = it->second;
        out.erase(it);
        if (move == Move::North) {
            out[{i + 1, j}] = Move::None;
        } else if (move == Move::South) {
            out[{i - 1, j}] = Move::None;
        } else if (move == Move::West) {
            out[{i, j + 1}] = Move::None;
        } else if (move == Move::East) {
            out[{i, j - 1}] = Move::None;
        }
    }
    for (const auto& pos : none) {
        out[pos] = Move::None;
    }
    return std::any_of(out.cbegin(), out.cend(), [](const auto it) { return it.second != Move::None; });
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
    for (int32_t i = 0; i < lines.size(); ++i) {
        if (lines[i].empty()) {
            break;
        }
        for (int32_t j = 0; j < lines[i].size(); ++j) {
            if (lines[i][j] == elve) {
                map[{i, j}] = Move::None;
            }
        }
    }

    size_t round{};
    {  // Part 1
        do {
            Map next{};
            if (auto hasMoved = move(map, next, round++); hasMoved) {
                map.swap(next);
            } else {
                break;
            }
        } while (round < 10);
        int32_t minI{INT32_MAX}, maxI{INT32_MIN}, minJ{INT32_MAX}, maxJ{INT32_MIN};
        for (const auto it : map) {
            auto [i, j] = it.first;
            minI = std::min<int32_t>(minI, i);
            maxI = std::max<int32_t>(maxI, i);
            minJ = std::min<int32_t>(minJ, j);
            maxJ = std::max<int32_t>(maxJ, j);
        }
        std::cout << (maxI - minI + 1) * (maxJ - minJ + 1) - map.size() << std::endl;
    }
    {  // Part 2
        while (true) {
            Map next{};
            if (auto hasMoved = move(map, next, round++); hasMoved) {
                map.swap(next);
            } else {
                break;
            }
        }
        std::cout << round << std::endl;
    }

    return EXIT_SUCCESS;
}
