// Advent of Code 2024
// Day 12: Garden Groups
// https://adventofcode.com/2024/day/12

#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
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

using Coord = int16_t;
using Pos = std::array<Coord, 2>;

struct PosSet
{
    void insert(const Pos& pos)
    {
        poss.insert(pos);
    }

    bool contains(const Pos& pos) const
    {
        return poss.find(pos) != poss.end();
    }

protected:
    std::set<Pos> poss;
};

struct Region : public PosSet
{
    Region(const std::vector<std::string>& grid) : grid{grid}
    {
    }
    static Region create(const std::vector<std::string>& grid, const Pos& start);

    uint32_t area() const
    {
        return static_cast<uint32_t>(poss.size());
    }

    uint32_t perimeter() const
    {
        uint32_t sum{};
        for (const auto [r, c] : poss) {
            if (auto dim = grid[r].size() - 1; c == dim || (c < dim && !contains(Pos{r, c + 1})))  // E
            {
                ++sum;
            }
            if (auto dim = grid.size() - 1; r == dim || (r < dim && !contains(Pos{r + 1, c})))  // S
            {
                ++sum;
            }
            if (c == 0 || (c > 0 && !contains(Pos{r, c - 1})))  // W
            {
                ++sum;
            }
            if (r == 0 || (r > 0 && !contains(Pos{r - 1, c})))  // N
            {
                ++sum;
            }
        }
        return sum;
    }

    uint32_t sides() const
    {
        uint32_t sum{};
        PosSet visitedE, visitedS, visitedW, visitedN;
        for (const auto [r, c] : poss) {
            if (!visitedE.contains(Pos{r, c}) &&
                (c == grid[r].size() - 1 || (c < grid[r].size() - 1 && !contains(Pos{r, c + 1}))))  // E
            {
                ++sum;
                visitedE.insert(Pos{r, c});
                auto rr = r;
                while (rr < grid.size() - 1 && contains(Pos{rr + 1, c})) {
                    if (c < grid[rr].size() - 1 && contains(Pos{rr + 1, c + 1})) {
                        break;
                    }
                    visitedE.insert(Pos{++rr, c});
                }
            }
            if (!visitedS.contains(Pos{r, c}) &&
                (r == grid.size() - 1 || (r < grid.size() - 1 && !contains(Pos{r + 1, c}))))  // S
            {
                ++sum;
                visitedS.insert(Pos{r, c});
                auto cc = c;
                while (cc < grid[r].size() - 1 && contains(Pos{r, cc + 1})) {
                    if (r < grid.size() - 1 && contains(Pos{r + 1, cc + 1})) {
                        break;
                    }
                    visitedS.insert(Pos{r, ++cc});
                }
            }
            if (!visitedW.contains(Pos{r, c}) && (c == 0 || (c > 0 && !contains(Pos{r, c - 1}))))  // W
            {
                ++sum;
                visitedW.insert(Pos{r, c});
                auto rr = r;
                while (rr < grid.size() - 1 && contains(Pos{rr + 1, c})) {
                    if (c > 0 && contains(Pos{rr + 1, c - 1})) {
                        break;
                    }
                    visitedW.insert(Pos{++rr, c});
                }
            }
            if (!visitedN.contains(Pos{r, c}) && (r == 0 || (r > 0 && !contains(Pos{r - 1, c}))))  // N
            {
                ++sum;
                visitedN.insert(Pos{r, c});
                auto cc = c;
                while (cc < grid[r].size() - 1 && contains(Pos{r, cc + 1})) {
                    if (r > 0 && contains(Pos{r - 1, cc + 1})) {
                        break;
                    }
                    visitedN.insert(Pos{r, ++cc});
                }
            }
        }
        return sum;
    }

    const std::vector<std::string>& grid;
};

void dfs(Region& region, const Pos& pos, char id)
{
    const auto [r, c] = pos;
    const auto& grid = region.grid;
    if (grid[r][c] != id) {
        return;
    }
    region.insert(pos);
    if (auto dim = grid[r].size() - 1; c < dim && !region.contains(Pos{r, c + 1}))  // E
    {
        dfs(region, Pos{r, c + 1}, id);
    }
    if (auto dim = grid.size() - 1; r < dim && !region.contains(Pos{r + 1, c}))  // S
    {
        dfs(region, Pos{r + 1, c}, id);
    }
    if (c > 0 && !region.contains(Pos{r, c - 1}))  // W
    {
        dfs(region, Pos{r, c - 1}, id);
    }
    if (r > 0 && !region.contains(Pos{r - 1, c}))  // N
    {
        dfs(region, Pos{r - 1, c}, id);
    }
}

Region Region::create(const std::vector<std::string>& grid, const Pos& start)
{
    auto region = Region(grid);
    dfs(region, start, grid[start[0]][start[1]]);
    return region;
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    std::vector<std::string> grid{};
    if (!readFile(argv[1], grid)) {
        return EXIT_FAILURE;
    }

    std::multimap<char, Region> map;
    for (Coord r = 0; r < grid.size(); ++r) {
        for (Coord c = 0; c < grid[r].size(); ++c) {
            const auto& regions = map.equal_range(grid[r][c]);
            bool found = false;
            for (auto it = regions.first; it != regions.second; ++it) {
                if (const auto& region = it->second; region.contains(Pos{r, c})) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                map.emplace(grid[r][c], Region::create(grid, Pos{r, c}));
            }
        }
    }

    {  // Part 1
        uint32_t sum{};
        for (const auto& [_, region] : map) {
            sum += region.area() * region.perimeter();
        }
        std::cout << sum << '\n';
    }
    {  // Part 2
        uint32_t sum{};
        for (const auto& [_, region] : map) {
            sum += region.area() * region.sides();
        }
        std::cout << sum << '\n';
    }

    return EXIT_SUCCESS;
}
