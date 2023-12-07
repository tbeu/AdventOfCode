// Advent of Code 2023
// Day 7: Camel Cards
// https://adventofcode.com/2023/day/7

#include <algorithm>
#include <array>
#include <assert.h>
#include <cctype>
#include <fstream>
#include <iostream>
#include <iterator>
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

constexpr size_t NCARDS{5};
using Cards = std::array<char, 13>;
using Count = std::array<uint16_t, NCARDS>;

const auto cmpTie = [](const std::string& a, const std::string& b, const Cards& cards) {
    for (size_t i = 0; i < 5; ++i) {
        const auto ad = cards.size() - std::distance(cards.begin(), std::find(cards.begin(), cards.end(), a[i]));
        const auto bd = cards.size() - std::distance(cards.begin(), std::find(cards.begin(), cards.end(), b[i]));
        if (ad < bd) {
            return true;
        }
        if (bd < ad) {
            return false;
        }
    }
    assert(false);
    return true;
};

struct
{
    constexpr static inline Cards cards = {'A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3', '2'};
    bool operator()(const std::string& a, const std::string& b) const
    {
        const auto count = [](const std::string& s) {
            Count sc{0, 0, 0, 0, 0};
            for (const auto card : cards) {
                if (const auto c = std::count(s.begin(), s.begin() + NCARDS, card); c > 0) {
                    sc[NCARDS - c]++;
                }
            }
            assert(5 * sc[0] + 4 * sc[1] + 3 * sc[2] + 2 * sc[3] + sc[4] == 5);
            return sc;
        };

        const auto ac = count(a);
        const auto bc = count(b);
        if (ac < bc) {
            return true;
        }
        if (bc < ac) {
            return false;
        }
        // tie
        return cmpTie(a, b, cards);
    }
} Strength1;

struct
{
    constexpr static inline Cards cards = {'A', 'K', 'Q', 'T', '9', '8', '7', '6', '5', '4', '3', '2', 'J'};
    constexpr static inline char Joker = 'J';
    bool operator()(const std::string& a, const std::string& b) const
    {
        auto applyJoker = [](const std::string& s, Count& sc) {
            if (const auto csJ = std::count(s.begin(), s.begin() + 5, Joker); csJ > 0) {
                bool applied{false};
                for (size_t i = 0; i < NCARDS; ++i) {
                    if (sc[i] > 0) {
                        sc[i]--;
                        sc[i - csJ]++;
                        applied = true;
                        break;
                    }
                }
                if (!applied) {
                    sc = {1, 0, 0, 0, 0};  // s = "JJJJJ"
                }
            }
        };

        const auto count = [&](const std::string& s) {
            Count sc{0, 0, 0, 0, 0};
            for (const auto card : cards) {
                if (Joker == card) {
                    continue;
                }
                if (const auto c = std::count(s.begin(), s.begin() + NCARDS, card); c > 0) {
                    sc[NCARDS - c]++;
                }
            }
            applyJoker(s, sc);
            assert(5 * sc[0] + 4 * sc[1] + 3 * sc[2] + 2 * sc[3] + sc[4] == 5);
            return sc;
        };

        const auto ac = count(a);
        const auto bc = count(b);
        if (ac < bc) {
            return true;
        }
        if (bc < ac) {
            return false;
        }
        // tie
        return cmpTie(a, b, cards);
    }
} Strength2;

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (2 != argc || !readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    auto toBid = [](const std::string& line) {
        uint64_t n{};
        for (size_t i = 6; i < line.size(); ++i) {
            assert(std::isdigit(line[i]));
            n = n * 10 + line[i] - '0';
        }
        return n;
    };

    {  // Part 1
        std::sort(lines.begin(), lines.end(), Strength1);
        uint64_t sum{};
        for (uint64_t rank = 0; rank < lines.size(); ++rank) {
            sum += (rank + 1) * toBid(lines[rank]);
        }
        std::cout << sum << std::endl;
    }
    {  // Part 2
        std::sort(lines.begin(), lines.end(), Strength2);
        uint64_t sum{};
        for (uint64_t rank = 0; rank < lines.size(); ++rank) {
            sum += (rank + 1) * toBid(lines[rank]);
        }
        std::cout << sum << std::endl;
    }

    return EXIT_SUCCESS;
}
