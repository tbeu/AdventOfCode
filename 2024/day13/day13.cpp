// Advent of Code 2024
// Day 13: Claw Contraption
// https://adventofcode.com/2024/day/13

#include <array>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <sstream>
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

using Button = std::array<int64_t, 2>;
using Prize = std::array<int64_t, 2>;
using Game = std::tuple<Button, Button, Prize>;
using Games = std::vector<Game>;

uint64_t play1(const Game& game)
{
    const auto& [A, B, P] = game;
    for (uint64_t a = 0; a < 100; ++a) {
        for (uint64_t b = 0; b < 100; ++b) {
            if (A[0] * a + B[0] * b == P[0] && A[1] * a + B[1] * b == P[1]) {
                return 3 * a + b;
            }
        }
    }
    return UINT64_MAX;
}

uint64_t play2(const Game& game, int64_t shift = 10000000000000)
{
    const auto& [A, B, Pold] = game;
    const auto P = Prize{Pold[0] + shift, Pold[1] + shift};
    const auto det = B[0] * A[1] - B[1] * A[0];
    assert(det != 0);
    auto p = P[0] * A[1] - P[1] * A[0];
    if (p % det != 0)
        return UINT64_MAX;
    const auto b = p / det;
    p = P[0] - B[0] * b;
    if (p % A[0] != 0)
        return UINT64_MAX;
    const auto a = p / A[0];
    return 3 * a + b;
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    std::vector<std::string> lines{};
    if (!readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    const auto toButton = [](const std::string& line) {
        Button button;
        std::istringstream iss{line};
        iss.ignore(7);
        char b, x, y;
        iss >> b;
        iss.ignore(1);
        iss >> x;
        iss >> button[0];
        iss.ignore(1);
        iss >> y;
        iss >> button[1];
        return button;
    };

    const auto toPrize = [](const std::string& line) {
        Prize prize;
        std::istringstream iss{line};
        iss.ignore(7);
        char x, y;
        iss >> x;
        iss.ignore(1);
        iss >> prize[0];
        iss.ignore(2);
        iss >> y;
        iss.ignore(1);
        iss >> prize[1];
        return prize;
    };

    Games games;
    for (size_t i = 0; i < lines.size(); ++i) {
        const auto A = toButton(lines[i++]);
        const auto B = toButton(lines[i++]);
        const auto P = toPrize(lines[i++]);
        games.emplace_back(Game{A, B, P});
    }

    {  // Part 1
        uint64_t sum{};
        for (const auto& game : games) {
            if (auto p = play1(game); p < UINT64_MAX) {
                assert(p == play2(game, 0));
                sum += p;
            }
        }
        std::cout << sum << '\n';
    }
    {  // Part 2
        uint64_t sum{};
        for (const auto& game : games) {
            if (auto p = play2(game); p < UINT64_MAX) {
                sum += p;
            }
        }
        std::cout << sum << '\n';
    }

    return EXIT_SUCCESS;
}
