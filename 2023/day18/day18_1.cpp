// Advent of Code 2023
// Day 18: Lavaduct Lagoon
// https://adventofcode.com/2023/day/18

#include <assert.h>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
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

using Pos = std::array<int32_t, 2>;
using Edge = std::pair<Pos, Pos>;
constexpr std::array<std::array<int32_t, 2>, 4> adjs{{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};

static size_t bfs(const std::set<Pos>& plan, const Pos& dimMin, const Pos& dimMax, const Pos& start)
{
    std::set<Pos> visited{start};
    std::queue<Pos> q{};
    q.push(start);
    const auto isVisited = [&](const Pos& pos) {
        assert(pos[0] >= dimMin[0]);
        assert(pos[0] < dimMax[0]);
        assert(pos[1] >= dimMin[1]);
        assert(pos[1] < dimMax[1]);
        return visited.find(pos) != visited.end();
    };
    const auto isEdge = [&plan](const Pos& pos) { return plan.find(pos) != plan.end(); };
    const auto updateQ = [&](const Pos& next) {
        if (!isVisited(next)) {
            visited.insert(next);
            q.push(next);
        }
    };

    while (!q.empty()) {
        auto pos = q.front();
        q.pop();
        const auto [r, c] = pos;
        for (size_t dir = 0; dir < adjs.size(); ++dir) {
            if (0 == dir && c == dimMax[1] - 1) {  // E
                continue;
            }
            if (1 == dir && r == dimMax[0] - 1) {  // S
                continue;
            }
            if (2 == dir && c == dimMin[1]) {  // W
                continue;
            }
            if (3 == dir && r == dimMin[0]) {  // N
                continue;
            }
            const auto [rr, cc] = adjs[dir];
            const auto next = Pos{r + rr, c + cc};
            if (isEdge(next)) {
                continue;
            }
            updateQ(next);
        }
    }

    return (dimMax[0] - dimMin[0]) * (dimMax[1] - dimMin[1]) - visited.size();
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (2 != argc || !readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    {  // Part 1
        uint64_t sum{};
        std::set<Pos> map{Pos{0, 0}};
        int32_t minr{0}, minc{0}, lastr{};
        int32_t maxr{0}, maxc{0}, lastc{};
        for (const auto& line : lines) {
            uint32_t amt{};
            char d;
            std::istringstream iss{line};
            iss >> d >> amt;
            size_t dir;
            if ('R' == d) {
                dir = 0;
            } else if ('D' == d) {
                dir = 1;
            } else if ('L' == d) {
                dir = 2;
            } else if ('U' == d) {
                dir = 3;
            }
            int32_t r = lastr;
            int32_t c = lastc;
            for (uint32_t step = 0; step < amt; ++step) {
                r += adjs[dir][0];
                c += adjs[dir][1];
                minr = std::min(minr, r);
                minc = std::min(minc, c);
                maxr = std::max(maxr, r);
                maxc = std::max(maxc, c);
                map.insert(Pos{r, c});
            }
            lastr = r;
            lastc = c;
        }
        const auto count = bfs(map, Pos{minr - 1, minc - 1}, Pos{maxr + 2, maxc + 2}, Pos{minr - 1, minc - 1});
        std::cout << count << std::endl;
    }

    return EXIT_SUCCESS;
}
