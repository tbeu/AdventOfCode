// Advent of Code 2021
// Day 11: Dumbo Octopus
// https://adventofcode.com/2021/day/11

#if defined(_MSC_VER)
#include "../../cppconlib/include/conmanip.h"
#endif

#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <queue>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <gsl/util>

static bool readFile(std::string fileName, std::vector<std::string>& map)
{
    std::ifstream in{fileName.c_str()};
    if (!in) {
        std::cerr << "Cannot open file " << fileName << std::endl;
        return false;
    }
    auto closeStream = gsl::finally([&in] { in.close(); });
    std::string str;
    while (std::getline(in, str)) {
        map.push_back(str);
    }
    return true;
}

constexpr size_t dim{10};
using Point = std::pair<size_t, size_t>;
using Row = std::array<uint16_t, dim + 2>;
using Map = std::array<Row, dim + 2>;

#if defined(_MSC_VER)
static bool print(Map& map, size_t day, uint64_t count, const COORD& pos)
{
    std::cout << conmanip::setpos(pos.X, pos.Y) << conmanip::settextcolor(conmanip::console_text_colors::light_white)
              << conmanip::setbgcolor(conmanip::console_bg_colors::black) << "Day: " << day << '\n'
              << std::endl;
    for (size_t i = 1; i < dim + 1; ++i) {
        for (size_t j = 1; j < dim + 1; ++j) {
            if (map[i][j] != 0) {
                std::cout << conmanip::settextcolor(conmanip::console_text_colors::black) << map[i][j];
            } else if (count < dim * dim) {
                std::cout << conmanip::settextcolor(conmanip::console_text_colors::yellow) << map[i][j];
            } else {
                std::cout << conmanip::settextcolor(conmanip::console_text_colors::light_yellow) << map[i][j];
            }
        }
        std::cout << '\n';
    }

    std::chrono::duration<int, std::milli> timespan(200);
    std::this_thread::sleep_for(timespan);

    return static_cast<bool>(GetAsyncKeyState(VK_ESCAPE));
}
#endif

static uint64_t simulate(Map& map)
{
    uint64_t count{0};
    std::queue<Point> q{};
    for (size_t i = 1; i < dim + 1; ++i) {
        for (size_t j = 1; j < dim + 1; ++j) {
            if (++map[i][j] > 9) {
                q.emplace(i, j);
                map[i][j] = 0;
            }
        }
    }

    while (!q.empty()) {
        ++count;
        const auto& f{q.front()};
        q.pop();
        size_t i{f.first};
        size_t j{f.second};
        const std::array<Point, 8> adjs{{{i + 1, j},
                                         {i + 1, j + 1},
                                         {i + 1, j - 1},
                                         {i, j + 1},
                                         {i, j - 1},
                                         {i - 1, j},
                                         {i - 1, j + 1},
                                         {i - 1, j - 1}}};
        for (const auto& adj : adjs) {
            if (map[adj.first][adj.second] == 0) {
                continue;
            }
            if (++map[adj.first][adj.second] > 9) {
                q.emplace(adj.first, adj.second);
                map[adj.first][adj.second] = 0;
            }
        }
    }
    return count;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 1) {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<float> dist(0.5, 8.5);
        for (size_t i = 0; i < dim; ++i) {
            std::ostringstream oss;
            for (size_t j = 0; j < dim; ++j) {
                oss << int(dist(mt) + 0.5);
            }
            lines.emplace_back(oss.str());
        }
    } else if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }
    if (lines.size() != dim ||
        std::any_of(lines.cbegin(), lines.cend(), [](const auto& line) { return line.size() != dim; })) {
        return EXIT_FAILURE;
    }

    Map map{};
    for (size_t i = 0; i < dim; ++i) {
        for (size_t j = 0; j < dim; ++j) {
            map[i + 1][j + 1] = lines[i][j] - '0';
        }
    }

#if defined(_MSC_VER)
    conmanip::console_out_context ctxout;
    conmanip::console_out conout(ctxout);
    auto conPos{conout.getpos()};
#endif

    size_t firstDay{0};
    uint64_t total{0};
    bool isEscape{false};
    {  // Part 1
        for (size_t day = 0; day < 100; ++day) {
            const auto count{simulate(map)};
            total += count;
            if (firstDay == 0 && count == dim * dim) {
                // Early case for part 2
                firstDay = day + 1;
            }
#if defined(_MSC_VER)
            isEscape = print(map, day, count, conPos);
            if (isEscape) {
                break;
            }
#endif
        }
    }
    {  // Part 2
        if (firstDay == 0 && !isEscape) {
            uint64_t count{0};
            size_t day{100};
            do {
                ++day;
                count = simulate(map);
                if (count == dim * dim) {
                    firstDay = day;
                }
#if defined(_MSC_VER)
                isEscape = print(map, day, count, conPos);
#endif
            } while (firstDay == 0 && !isEscape);
        }
    }

#if defined(_MSC_VER)
    std::cout << std::endl;
    ctxout.restore(conmanip::console_cleanup_options::restore_attibutes);
#endif
    std::cout << total << std::endl;
    std::cout << firstDay << std::endl;

    return EXIT_SUCCESS;
}
