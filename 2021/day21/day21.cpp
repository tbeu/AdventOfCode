// Advent of Code 2021
// Day 21: Dirac Dice
// https://adventofcode.com/2021/day/21

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
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

struct Player
{
    Player()
    {
    }
    Player(size_t pos) : pos{pos}
    {
    }
    size_t pos{};
    size_t score{};
    uint64_t wins{};
};

constexpr std::array<std::pair<size_t, size_t>, 7> eyesDist{{{3, 1}, {4, 3}, {5, 6}, {6, 7}, {7, 6}, {8, 3}, {9, 1}}};

static size_t play1(std::array<Player, 2>& players)
{
    size_t eyes{0};
    size_t turns{0};
    size_t res{0};
    bool done{false};
    while (!done) {
        for (size_t i = 0; i < 2; ++i) {
            auto& player = players[i];
            turns += 3;
            eyes += 3;
            const auto newPos{(player.pos + 3 * eyes - 4) % 10 + 1};
            player.pos = newPos;
            player.score += newPos;
            if (player.score >= 1000) {
                ++player.wins;
                const auto& otherPlayer = players[(i + 1) % 2];
                res = turns * otherPlayer.score;
                done = true;
                break;
            }
        }
    }
    return res;
}

static void play2(std::array<Player, 2>& players, uint64_t mul = 1, size_t i = 0)
{
    for (auto& [eyes, dist] : eyesDist) {
        auto& player = players[i];
        const auto newPos{(player.pos + eyes - 1) % 10 + 1};
        const auto newScore{player.score + newPos};
        if (newScore >= 21) {
            player.wins += mul * dist;
            continue;
        }
        const auto oldPos{player.pos};
        const auto oldScore{player.score};
        player.pos = newPos;
        player.score = newScore;
        play2(players, mul * dist, (i + 1) % 2);
        player.pos = oldPos;
        player.score = oldScore;
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

    if (lines.size() != 2) {
        return EXIT_FAILURE;
    }

    const auto p0{lines[0].rfind(' ')};
    const auto p1{lines[1].rfind(' ')};
    const auto startPos0{std::stoi(lines[0].substr(p0, lines.size() - p0 - 1))};
    const auto startPos1{std::stoi(lines[1].substr(p1, lines.size() - p1 - 1))};

    {  // Part 1
        std::array<Player, 2> players{Player(startPos0), Player(startPos1)};
        std::cout << play1(players) << std::endl;
    }
    {  // Part 2
        std::array<Player, 2> players{Player(startPos0), Player(startPos1)};
        play2(players);
        std::cout << std::max(players[0].wins, players[1].wins) << std::endl;
    }

    return EXIT_SUCCESS;
}
