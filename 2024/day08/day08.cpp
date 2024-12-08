// Advent of Code 2024
// Day 8: Resonant Collinearity
// https://adventofcode.com/2024/day/8

#include <array>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
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
using Map = std::map<char, std::vector<Pos> >;

int main(int argc, char* argv[])
{
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    std::vector<std::string> grid{};
    if (!readFile(argv[1], grid)) {
        return EXIT_FAILURE;
    }

    Map ants;
    for (Coord r = 0; r < static_cast<Coord>(grid.size()); ++r) {
        for (Coord c = 0; c < static_cast<Coord>(grid[r].size()); ++c) {
            if (std::isdigit(grid[r][c]) || std::isalpha(grid[r][c])) {
                if (const auto it = ants.find(grid[r][c]); it != ants.end()) {
                    it->second.emplace_back(Pos{r, c});
                } else {
                    ants.emplace(grid[r][c], std::vector<Pos>{{r, c}});
                }
            }
        }
    }

    {  // Part 1
        uint16_t sum{};
        auto ans = std::vector<std::vector<bool> >(grid.size(), std::vector<bool>(grid[0].size()));
        for (const auto& [_, poss] : ants) {
            for (size_t i = 0; i < poss.size() - 1; ++i) {
                for (size_t j = i + 1; j < poss.size(); ++j) {
                    const auto rr = poss[i][0] - poss[j][0];
                    const auto cc = poss[i][1] - poss[j][1];
                    for (const auto& [dr, dc] : {std::make_pair(rr, cc), std::make_pair(-2 * rr, -2 * cc)}) {
                        const auto r = poss[i][0] + dr;
                        const auto c = poss[i][1] + dc;
                        if (r >= 0 && c >= 0 && r < grid.size() && c < grid[r].size()) {
                            sum += ans[r][c] ? 0 : 1;
                            ans[r][c] = true;
                        }
                    }
                }
            }
        }
        std::cout << sum << '\n';
    }
    {  // Part 2
        uint16_t sum{};
        auto ans = std::vector<std::vector<bool> >(grid.size(), std::vector<bool>(grid[0].size()));
        for (const auto& [_, poss] : ants) {
            for (size_t i = 0; i < poss.size() - 1; ++i) {
                for (size_t j = i + 1; j < poss.size(); ++j) {
                    const auto rr = poss[i][0] - poss[j][0];
                    const auto cc = poss[i][1] - poss[j][1];
                    for (const auto& [dr, dc] : {std::make_pair(rr, cc), std::make_pair(-rr, -cc)}) {
                        auto r = poss[i][0];
                        auto c = poss[i][1];
                        do {
                            sum += ans[r][c] ? 0 : 1;
                            ans[r][c] = true;
                            r += dr;
                            c += dc;
                        } while (r >= 0 && c >= 0 && r < grid.size() && c < grid[r].size());
                    }
                }
            }
        }
        std::cout << sum << '\n';
    }

    return EXIT_SUCCESS;
}
