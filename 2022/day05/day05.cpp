// Advent of Code 2022
// Day 5: Supply Stacks
// https://adventofcode.com/2022/day/5

#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
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

struct Move
{
    size_t count{};
    size_t src{};
    size_t dst{};
};

using Moves = std::vector<Move>;
using Stack = std::vector<char>;
using Stacks = std::vector<Stack>;

Stacks initStacks(const std::vector<std::string>& lines, size_t pos, size_t count)
{
    assert(count < 10);
    Stacks stacks(count);
    for (size_t i = pos - 1; i > 0; --i) {
        for (size_t j = 0; j < count; ++j) {
            auto crate = lines[i - 1][4 * j + 1];
            if (crate != ' ') {
                stacks[j].push_back(crate);
            }
        }
    }
    return stacks;
}

std::tuple<Stacks, size_t> initStacks(const std::vector<std::string>& lines)
{
    size_t i{};
    for (i = 0; i < lines.size(); ++i) {
        if (lines[i].empty()) {
            break;
        }
    }
    const size_t pos{i};
    size_t count{};
    {
        std::istringstream iss(lines[pos - 1]);
        while (iss >> count) {
            iss.ignore(1);
        }
    }

    Stacks stacks{initStacks(lines, pos, count)};
    return {stacks, pos};
}

Moves initMoves(const std::vector<std::string>& lines, size_t pos)
{
    Moves moves{};
    for (size_t i = pos; i < lines.size(); ++i) {
        size_t count{}, src{}, dst{};
        std::sscanf(lines[i].c_str(), "move %zu from %zu to %zu", &count, &src, &dst);
        Move move{count, --src, --dst};
        moves.emplace_back(move);
    }
    return moves;
}

void operateCraneMover9000(const Moves& moves, Stacks& stacks)
{
    for (const auto& move : moves) {
        for (size_t i = 0; i < move.count; ++i) {
            stacks[move.dst].push_back(stacks[move.src].back());
            stacks[move.src].pop_back();
        }
    }
}

void operateCraneMover9001(const Moves& moves, Stacks& stacks)
{
    for (const auto& move : moves) {
        stacks[move.dst].insert(stacks[move.dst].end(), stacks[move.src].end() - move.count, stacks[move.src].end());
        stacks[move.src].erase(stacks[move.src].end() - move.count, stacks[move.src].end());
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

    auto printTopCrate = [](const Stack& stack) { std::cout << stack.back(); };

    auto [stacks, pos] = initStacks(lines);
    const Moves moves{initMoves(lines, pos + 1)};

    {  // Part 1
        operateCraneMover9000(moves, stacks);
        std::for_each(stacks.cbegin(), stacks.cend(), printTopCrate);
        std::cout << std::endl;
    }

    // Reinitiliaze stacks
    // stacks = initStacks(lines, pos, stacks.size());
    std::tie(stacks, std::ignore) = initStacks(lines);
    {  // Part 2
        operateCraneMover9001(moves, stacks);
        std::for_each(stacks.cbegin(), stacks.cend(), printTopCrate);
        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}
