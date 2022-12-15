// Advent of Code 2022
// Day 15: Beacon Exclusion Zone
// https://adventofcode.com/2022/day/15

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
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

using Pos = std::array<int64_t, 2>;

int64_t manhattanDistance(const Pos& a, const Pos& b)
{
    return std::abs(a[0] - b[0]) + std::abs(a[1] - b[1]);
}

inline Pos rotateAndScale(const Pos& pos)
{
    return Pos{pos[0] - pos[1], pos[0] + pos[1]};
}

inline Pos rotateAndScaleInv(const Pos& pos)
{
    return Pos{(pos[0] + pos[1]) / 2, (-pos[0] + pos[1]) / 2};
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    {  // Part 1
        const int64_t y{2000000};
        std::vector<Pos> intervals{};
        std::set<int64_t> xbs{};
        for (const auto& line : lines) {
            std::istringstream iss{line.substr(12, line.size() - 12)};
            int64_t xs, ys, xb, yb;
            iss >> xs;
            iss.ignore(4);
            iss >> ys;
            iss.ignore(25);
            iss >> xb;
            iss.ignore(4);
            iss >> yb;
            if (yb == y) {
                xbs.insert(xb);
            }
            if (int64_t d{manhattanDistance({xs, ys}, {xb, yb}) - std::abs(ys - y)}; d >= 0) {
                intervals.emplace_back(Pos{xs - d, xs + d});
            }
        }

        std::set<int64_t> xs{};
        for (const auto& [xmin, xmax] : intervals) {
            for (int64_t x = xmin; x <= xmax; ++x) {
                xs.insert(x);
            }
        }
        std::cout << xs.size() - xbs.size() << std::endl;
    }
    {  // Part 2
        // Idea: Make use of AABB in rotated and scaled CS
        std::vector<std::pair<Pos, Pos> > aabbs{};
        for (const auto& line : lines) {
            std::istringstream iss{line.substr(12, line.size() - 12)};
            int64_t xs, ys, xb, yb;
            iss >> xs;
            iss.ignore(4);
            iss >> ys;
            iss.ignore(25);
            iss >> xb;
            iss.ignore(4);
            iss >> yb;
            auto d{manhattanDistance({xs, ys}, {xb, yb})};
            Pos s{rotateAndScale({xs, ys})};
            aabbs.push_back(std::make_pair(Pos{s[0] - d, s[1] - d}, Pos{s[0] + d, s[1] + d}));
        }
        // The *one* non-intersected pos must be found on the border of an AABB.
        for (size_t i = 0; i < aabbs.size(); ++i) {
            const auto& [pi1, pi2] = aabbs[i];
            std::array<std::pair<Pos, Pos>, 4> borders{};
            borders[0] = std::make_pair(Pos{pi1[0] - 1, pi1[0] - 1}, Pos{pi1[1], pi2[1]});
            borders[1] = std::make_pair(Pos{pi2[0] + 1, pi2[0] + 1}, Pos{pi1[1], pi2[1]});
            borders[2] = std::make_pair(Pos{pi1[0], pi2[0]}, Pos{pi1[1] - 1, pi1[1] - 1});
            borders[3] = std::make_pair(Pos{pi1[0], pi2[0]}, Pos{pi2[1] + 1, pi2[1] + 1});
            for (const auto& [px, py] : borders) {
                for (int64_t x = px[0]; x <= px[1]; ++x) {
                    for (int64_t y = py[0]; y <= py[1]; ++y) {
                        const Pos p{rotateAndScaleInv({x, y})};
                        if (p[0] < 0 || p[1] < 0 || p[0] > 4000000 || p[1] > 4000000) {
                            continue;
                        }
                        bool overlap{false};
                        for (size_t j = 0; j < aabbs.size(); ++j) {
                            if (i == j) {
                                continue;
                            }
                            const auto& [pj1, pj2] = aabbs[j];
                            if (pj1[0] <= x && x <= pj2[0] && pj1[1] <= y && y <= pj2[1]) {
                                overlap = true;
                                break;
                            }
                        }
                        if (!overlap) {
                            std::cout << p[0] * 4000000 + p[1] << std::endl;
                            return EXIT_SUCCESS;
                        }
                    }
                }
            }
        }
    }

    return EXIT_SUCCESS;
}
