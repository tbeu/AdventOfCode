// Advent of Code 2023
// Day 22: Sand Slabs
// https://adventofcode.com/2023/day/22

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
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

using Index = uint16_t;
using Cube = std::array<Index, 3>;
using Brick = std::array<Cube, 2>;
using Bricks = std::vector<Brick>;
using IndexSet = std::unordered_set<Index>;

static bool overlap(const Brick& a, const Brick& b)
{
    return (std::max(a[0][0], b[0][0]) < std::min(a[1][0], b[1][0])) &&
           (std::max(a[0][1], b[0][1]) < std::min(a[1][1], b[1][1]));
}

struct Jenga
{
    Jenga(const Bricks& _bricks, Index minz) : bricks{_bricks}
    {
        // Move all to ground
        for (auto& b : bricks) {
            b[0][2] -= minz;
            b[1][2] -= minz;
        }

        // Determine the bricks which are above or below each brick
        for (Index i = 0; i < bricks.size(); ++i) {
            for (Index j = i + 1; j < bricks.size(); ++j) {
                const auto& a = bricks[i];
                const auto& b = bricks[j];
                if (!overlap(a, b)) {
                    continue;
                }
                if (a[1][2] <= b[0][2]) {
                    above[i].insert(j);
                    below[j].insert(i);
                } else if (b[1][2] <= a[0][2]) {
                    above[j].insert(i);
                    below[i].insert(j);
                } else {
                    assert(false);
                }
            }
        }

        settle();
    }

    void settle()
    {
        bool moved;
        do {
            moved = false;
            for (Index i = 0; i < bricks.size(); ++i) {
                auto& b = bricks[i];
                if (auto it = below.find(i); it != below.end()) {
                    Index minz{UINT16_MAX};
                    for (auto j : it->second) {
                        const auto& a = bricks[j];
                        assert(a[1][2] <= b[0][2]);
                        minz = std::min<Index>(minz, b[0][2] - a[1][2]);
                        if (0 == minz) {
                            break;
                        }
                    }
                    if (minz > 0) {
                        b[1][2] -= minz;
                        b[0][2] -= minz;
                        moved = true;
                    }
                } else if (0 != b[0][2]) {
                    b[1][2] -= b[0][2];
                    b[0][2] = 0;
                    moved = true;
                }
            }
        } while (moved);
    }

    size_t settle(Index k) const
    {
        // Idea: Set z dimension to zero and simulate settle (on copy of bricks)
        auto cricks = bricks;
        cricks[k][1][2] = cricks[k][0][2];
        IndexSet movers;
        bool moved;
        do {
            moved = false;
            for (Index i = 0; i < bricks.size(); ++i) {
                auto& b = cricks[i];
                if (b[0][2] < cricks[k][1][2]) {
                    continue;
                }
                if (auto it = below.find(i); it != below.end()) {
                    Index minz{UINT16_MAX};
                    for (auto j : it->second) {
                        const auto& a = cricks[j];
                        assert(a[1][2] <= b[0][2]);
                        minz = std::min<Index>(minz, b[0][2] - a[1][2]);
                        if (0 == minz) {
                            break;
                        }
                    }
                    if (minz > 0) {
                        b[1][2] -= minz;
                        b[0][2] -= minz;
                        moved = true;
                        movers.insert(i);
                    }
                } else if (0 != b[0][2]) {
                    b[1][2] -= b[0][2];
                    b[0][2] = 0;
                    moved = true;
                    movers.insert(i);
                }
            }
        } while (moved);
        return movers.size();
    }

    bool willMove(Index k) const
    {
        assert(k < bricks.size());
        const auto& c = bricks[k];
        if (auto jt = above.find(k); jt != above.end()) {
            for (auto j : jt->second) {
                const auto& b = bricks[j];
                assert(c[1][2] <= b[0][2]);
                if (c[1][2] < b[0][2]) {
                    continue;
                }
                if (auto it = below.find(j); it != below.end()) {
                    Index minz{UINT16_MAX};
                    for (auto i : it->second) {
                        if (i == k) {
                            continue;
                        }
                        const auto& a = bricks[i];
                        assert(a[1][2] <= b[0][2]);
                        minz = std::min<Index>(minz, b[0][2] - a[1][2]);
                    }
                    if (minz > 0) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    IndexSet disintegratable() const
    {
        IndexSet removable;
        for (Index i = 0; i < bricks.size(); ++i) {
            if (!willMove(i)) {
                removable.insert(i);
            }
        }
        return removable;
    }

    size_t countFalling() const
    {
        size_t count{};
        for (Index i = 0; i < bricks.size(); ++i) {
            count += settle(i);
        }
        return count;
    }

private:
    Bricks bricks;
    std::unordered_map<Index, IndexSet> above;
    std::unordered_map<Index, IndexSet> below;
};

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (2 != argc || !readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    const auto toBricks = [](const std::vector<std::string>& lines) -> std::pair<Bricks, Index> {
        Bricks bricks{};
        Index minz{UINT16_MAX};
        for (const auto& line : lines) {
            std::istringstream iss{line};
            Brick b;
            for (Index i = 0; i < 2; ++i) {
                for (Index j = 0; j < 3; ++j) {
                    Index v;
                    iss >> v;
                    iss.ignore(1);
                    b[i][j] = v + i;
                }
                minz = std::min(minz, b[i][2]);
            }
            bricks.emplace_back(b);
        }
        return {bricks, minz};
    };

    const auto& [bricks, minz] = toBricks(lines);
    const auto jenga = Jenga(bricks, minz);

    {  // Part 1
        std::cout << jenga.disintegratable().size() << std::endl;
    }
    {  // Part 2
        std::cout << jenga.countFalling() << std::endl;
    }

    return EXIT_SUCCESS;
}
