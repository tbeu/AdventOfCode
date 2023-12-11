// Advent of Code 2023
// Day 11: Cosmic Expansion
// https://adventofcode.com/2023/day/11

#include <fstream>
#include <iostream>
#include <numeric>
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

using Pos = std::array<uint16_t, 2>;

int64_t manhattanDistance(const Pos& a, const Pos& b)
{
    return std::abs(a[0] - b[0]) + std::abs(a[1] - b[1]);
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (2 != argc || !readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    std::vector<size_t> rs;
    for (size_t r = 0; r < lines.size(); ++r) {
        bool shouldExpand = true;
        for (size_t c = 0; c < lines[r].size(); ++c) {
            if (lines[r][c] == '#') {
                shouldExpand = false;
                break;
            }
        }
        if (shouldExpand) {
            rs.push_back(r);
        }
    }
    std::vector<size_t> cs;
    for (size_t c = 0; c < lines[0].size(); ++c) {
        bool shouldExpand = true;
        for (size_t r = 0; r < lines.size(); ++r) {
            if (lines[r][c] == '#') {
                shouldExpand = false;
                break;
            }
        }
        if (shouldExpand) {
            cs.push_back(c);
        }
    }

    std::vector<Pos> poss;
    for (uint16_t r = 0; r < lines.size(); ++r) {
        for (uint16_t c = 0; c < lines[r].size(); ++c) {
            if (lines[r][c] == '#') {
                poss.push_back({r, c});
            }
        }
    }

    {  // Part 1

        /* // done the naive way first
        for (size_t i = rs.size(); i > 0; --i) {
            const auto r = rs[i - 1];
            lines.insert(lines.begin() + r, std::string(lines[r].size(), '.'));
        }
        for (size_t i = cs.size(); i > 0; --i) {
            const auto c = cs[i - 1];
            std::transform(lines.begin(), lines.end(), lines.begin(),
                           [&c](std::string& row) { return row.substr(0, c) + '.' + row.substr(c, row.size() - c); });
        }
		*/

        uint64_t sum{};
        for (size_t i = 0; i < poss.size(); ++i) {
            for (size_t j = i + 1; j < poss.size(); ++j) {
                sum += manhattanDistance(poss[i], poss[j]);
                for (const auto r : rs) {
                    if ((poss[i][0] < r && r < poss[j][0]) || (poss[j][0] < r && r < poss[i][0])) {
                        sum += 2 - 1;
                    }
                }
                for (const auto c : cs) {
                    if ((poss[i][1] < c && c < poss[j][1]) || (poss[j][1] < c && c < poss[i][1])) {
                        sum += 2 - 1;
                    }
                }
            }
        }
        std::cout << sum << std::endl;
    }
    {  // Part 2
        uint64_t sum{};
        for (size_t i = 0; i < poss.size(); ++i) {
            for (size_t j = i + 1; j < poss.size(); ++j) {
                sum += manhattanDistance(poss[i], poss[j]);
                for (const auto r : rs) {
                    if ((poss[i][0] < r && r < poss[j][0]) || (poss[j][0] < r && r < poss[i][0])) {
                        sum += 1000000 - 1;
                    }
                }
                for (const auto c : cs) {
                    if ((poss[i][1] < c && c < poss[j][1]) || (poss[j][1] < c && c < poss[i][1])) {
                        sum += 1000000 - 1;
                    }
                }
            }
        }
        std::cout << sum << std::endl;
    }

    return EXIT_SUCCESS;
}
