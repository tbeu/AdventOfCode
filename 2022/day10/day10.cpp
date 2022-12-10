// Advent of Code 2022
// Day 10: Cathode-Ray Tube
// https://adventofcode.com/2022/day/10

#include <algorithm>
#include <assert.h>
#include <array>
#include <fstream>
#include <iostream>
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

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    {
        // Part 1
        std::vector<int> cycles{};
        int X{1};
        auto tick = [&cycles, &X] { cycles.push_back(X); };
        for (const auto& line : lines) {
            if (line.compare("noop") == 0) {
                tick();
                continue;
            }
            std::istringstream iss{line};
            std::string op;
            int incr;
            iss >> op >> incr;
            if (op.compare("addx") == 0) {
                tick();
                tick();
                X += incr;
            }
        }

        const std::array<int, 6> poss{20, 60, 100, 140, 180, 220};
        auto sum = std::accumulate(poss.cbegin(), poss.cend(), int64_t{0},
                                   [&cycles](auto& s, const auto& pos) { return s + cycles[pos - 1] * pos; });
        std::cout << sum << std::endl;
    }
    {  // Part 2
        constexpr int px{40};
        constexpr int py{6};
        std::array<char, px * py> CRT;
        std::fill_n(CRT.begin(), px * py, '.');
        int X{1};
        int pos{0};
        auto tick = [&CRT, &pos, &X, px] {
            if (X - 1 <= (pos % px) && (pos % px) <= X + 1) {
                CRT[pos] = '#';
            }
            pos++;
        };
        for (const auto& line : lines) {
            assert(pos < px * py);
            if (line.compare("noop") == 0) {
                tick();
                continue;
            }
            std::istringstream iss{line};
            std::string op;
            int incr;
            iss >> op >> incr;
            if (op.compare("addx") == 0) {
                tick();
                tick();
                X += incr;
            }
        }

        for (size_t i = 0; i < py; ++i) {
            for (size_t j = 0; j < px; ++j) {
                std::cout << CRT[i * px + j];
            }
            std::cout << '\n';
        }
    }

    return EXIT_SUCCESS;
}
