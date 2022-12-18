// Advent of Code 2022
// Day 17: Pyroclastic Flow
// https://adventofcode.com/2022/day/17

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
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

constexpr char p1{'-'};
constexpr char p2{'+'};
constexpr char p3{'L'};
constexpr char p4{'|'};
constexpr char p5{'#'};
constexpr std::array<char, 5> parts{p1, p2, p3, p4, p5};
constexpr char left{'<'};
constexpr char right{'>'};
constexpr char down{'v'};
constexpr uint64_t width{7};

using Pos = std::array<uint64_t, 2>;
using Map = std::vector<Pos>;
using Cache = std::map<std::pair<size_t, std::set<Pos> >, std::pair<uint64_t, uint64_t> >;
using Caches = std::array<Cache, 5>;

Caches caches{};
Map map{};

inline Pos create(uint64_t y)
{
    if (!map.empty()) {
        return {2, y + 4};
    }
    return {2, 3};
}

inline bool isFree(const Pos& pos)
{
    return std::find(map.cbegin(), map.cend(), pos) == map.cend();
}

bool tryMove(char dir, char part, Pos& pos)
{
    auto& [x, y] = pos;
    switch (part) {
        case p1:
            switch (dir) {
                case right:
                    if (x < width - 4 && isFree({x + 4, y})) {
                        x++;
                        return true;
                    }
                    break;
                case left:
                    if (x > 0 && isFree({x - 1, y})) {
                        x--;
                        return true;
                    }
                    break;
                default:
                    if (y > 0 && isFree({x, y - 1}) && isFree({x + 1, y - 1}) && isFree({x + 2, y - 1}) &&
                        isFree({x + 3, y - 1})) {
                        y--;
                        return true;
                    }
                    break;
            }
            break;
        case p2:
            switch (dir) {
                case right:
                    if (x < width - 3 && isFree({x + 2, y}) && isFree({x + 3, y + 1}) && isFree({x + 2, y + 2})) {
                        x++;
                        return true;
                    }
                    break;
                case left:
                    if (x > 0 && isFree({x, y}) && isFree({x - 1, y + 1}) && isFree({x, y + 2})) {
                        x--;
                        return true;
                    }
                    break;
                default:
                    if (y > 0 && isFree({x, y}) && isFree({x + 1, y - 1}) && isFree({x + 2, y})) {
                        y--;
                        return true;
                    }
                    break;
            }
            break;
        case p3:
            switch (dir) {
                case right:
                    if (x < width - 3 && isFree({x + 3, y}) && isFree({x + 3, y + 1}) && isFree({x + 3, y + 2})) {
                        x++;
                        return true;
                    }
                    break;
                case left:
                    if (x > 0 && isFree({x - 1, y}) && isFree({x + 1, y + 1}) && isFree({x + 1, y + 2})) {
                        x--;
                        return true;
                    }
                    break;
                default:
                    if (y > 0 && isFree({x, y - 1}) && isFree({x + 1, y - 1}) && isFree({x + 2, y - 1})) {
                        y--;
                        return true;
                    }
                    break;
            }
            break;
        case p4:
            switch (dir) {
                case right:
                    if (x < width - 1 && isFree({x + 1, y}) && isFree({x + 1, y + 1}) && isFree({x + 1, y + 2}) &&
                        isFree({x + 1, y + 3})) {
                        x++;
                        return true;
                    }
                    break;
                case left:
                    if (x > 0 && isFree({x - 1, y}) && isFree({x - 1, y + 1}) && isFree({x - 1, y + 2}) &&
                        isFree({x - 1, y + 3})) {
                        x--;
                        return true;
                    }
                    break;
                default:
                    if (y > 0 && isFree({x, y - 1})) {
                        y--;
                        return true;
                    }
                    break;
            }
            break;
        case p5:
            switch (dir) {
                case right:
                    if (x < width - 2 && isFree({x + 2, y}) && isFree({x + 2, y + 1})) {
                        x++;
                        return true;
                    }
                    break;
                case left:
                    if (x > 0 && isFree({x - 1, y}) && isFree({x - 1, y + 1})) {
                        x--;
                        return true;
                    }
                    break;
                default:
                    if (y > 0 && isFree({x, y - 1}) && isFree({x + 1, y - 1})) {
                        y--;
                        return true;
                    }
                    break;
            }
            break;
        default:
            break;
    }
    return false;
}

uint64_t rest(char part, const Pos& pos, uint64_t maxY)
{
    const auto& [x, y] = pos;
    switch (part) {
        case p1:
            map.push_back(pos);
            map.push_back({x + 1, y});
            map.push_back({x + 2, y});
            map.push_back({x + 3, y});
            return std::max<uint64_t>(maxY, y);
        case p2:
            map.push_back({x + 1, y});
            map.push_back({x, y + 1});
            map.push_back({x + 1, y + 1});
            map.push_back({x + 2, y + 1});
            map.push_back({x + 1, y + 2});
            return std::max<uint64_t>(maxY, y + 2);
        case p3:
            map.push_back(pos);
            map.push_back({x + 1, y});
            map.push_back({x + 2, y});
            map.push_back({x + 2, y + 1});
            map.push_back({x + 2, y + 2});
            return std::max<uint64_t>(maxY, y + 2);
        case p4:
            map.push_back(pos);
            map.push_back({x, y + 1});
            map.push_back({x, y + 2});
            map.push_back({x, y + 3});
            return std::max<uint64_t>(maxY, y + 3);
        case p5:
            map.push_back(pos);
            map.push_back({x + 1, y});
            map.push_back({x, y + 1});
            map.push_back({x + 1, y + 1});
            return std::max<uint64_t>(maxY, y + 1);
        default:
            return maxY;
    }
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    const auto& dirs = lines[0];
    size_t dirPos{0};
    uint64_t maxY{};
    {  // Part 1
        for (size_t step = 0; step < 2022; ++step) {
            const auto part = parts[step % 5];
            auto pos = create(maxY);
            while (true) {
                tryMove(dirs[dirPos], part, pos);
                dirPos = (dirPos + 1) % dirs.size();
                if (!tryMove(down, part, pos)) {
                    maxY = rest(part, pos, maxY);
                    break;
                }
            }
        }
        std::cout << maxY + 1 << std::endl;
    }
    {  // Part 2
        uint64_t step{2022};
        const uint64_t stop{1000000000000};
        uint64_t skippedY{};
        while (step < stop) {
            const auto part = parts[step % 5];
            auto pos = create(maxY);
            while (true) {
                tryMove(dirs[dirPos], part, pos);
                dirPos = (dirPos + 1) % dirs.size();
                if (!tryMove(down, part, pos)) {
                    maxY = rest(part, pos, maxY);
                    std::set<Pos> set{};
                    for (const auto& [x, y] : map) {
                        if (maxY - y < 100) {
                            // Heuristic: Only check for relative y positions within certain relative threshold
                            set.insert({x, maxY - y});
                        }
                    }
                    auto& cache = caches[step % 5];
                    auto [it, isInserted] = cache.try_emplace(std::pair(dirPos, set), std::pair(step, maxY));
                    if (!isInserted) {
                        // Idea: Check for a repeating pattern given the current dirPos and ordered set of existing parts (with relative y coordinates)
                        // If found: Determine stepPeriod and number of cycles to skip while still being less than stop
                        const auto [lastStep, lastMaxY] = it->second;
                        const auto stepPeriod = step - lastStep;
                        const auto cyclesToSkip = (stop - step) / stepPeriod;
                        skippedY += cyclesToSkip * (maxY - lastMaxY);
                        step = stop - (stop - step) % stepPeriod;
                    }
                    break;
                }
            }
            step++;
        }
        std::cout << maxY + 1 + skippedY << std::endl;
    }

    return EXIT_SUCCESS;
}
