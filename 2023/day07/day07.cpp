// Advent of Code 2023
// Day 7: Camel Cards
// https://adventofcode.com/2023/day/7

#include <algorithm>
#include <array>
#include <assert.h>
#include <cctype>
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

constexpr size_t NCARDS{5};
using Cards = std::array<char, 13>;
using Count = std::array<uint8_t, NCARDS>;

const auto cmpTie = [](const std::string& a, const std::string& b, const Cards& cards) {
    for (size_t i = 0; i < NCARDS; ++i) {
        for (const auto card : cards) {
            if (a[i] == card && b[i] != card) {
                return true;
            }
            if (a[i] != card && b[i] == card) {
                return false;
            }
        }
    }
    assert(false);
    return true;
};

struct
{
    constexpr static inline Cards cards = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
    bool operator()(const std::string& a, const std::string& b) const
    {
        const auto count = [](const std::string& s) {
            Count sc{0, 0, 0, 0, 0};
            for (const auto card : cards) {
                if (const auto c = std::count(s.begin(), s.begin() + NCARDS, card); c > 0) {
                    sc[NCARDS - c]++;
                }
            }
            assert(5 * sc[0] + 4 * sc[1] + 3 * sc[2] + 2 * sc[3] + sc[4] == NCARDS);
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
    constexpr static inline char Joker = 'J';
    constexpr static inline Cards cards = {Joker, '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'Q', 'K', 'A'};
    bool operator()(const std::string& a, const std::string& b) const
    {
        const auto applyJoker = [](const std::string& s, Count& sc) {
            if (const auto csJ = std::count(s.begin(), s.begin() + NCARDS, Joker); csJ > 0) {
                if (NCARDS == csJ) {
                    assert(std::all_of(sc.begin(), sc.end(), [](auto i) { return i == 0; }));
                    sc[0] = 1;  // s = "JJJJJ"
                    return;
                }
                for (size_t i = 0; i < NCARDS; ++i) {
                    if (sc[i] > 0) {
                        sc[i]--;
                        sc[i - csJ]++;
                        break;
                    }
                }
            }
        };

        const auto count = [&applyJoker](const std::string& s) {
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
            assert(5 * sc[0] + 4 * sc[1] + 3 * sc[2] + 2 * sc[3] + sc[4] == NCARDS);
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
        for (size_t i = NCARDS + 1; i < line.size(); ++i) {
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
