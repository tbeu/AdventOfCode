// Advent of Code 2024
// Day 11: Plutonian Pebbles
// https://adventofcode.com/2024/day/11

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
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

size_t numDigits(uint64_t num)
{
    size_t count{};
    while (num != 0) {
        num /= 10;
        ++count;
    }
    return count;
}

uint64_t split(uint64_t& num, size_t numDigits)
{
    uint64_t div{1};
    while (numDigits-- > 0) {
        div *= 10;
    }
    const auto res = num / div;
    num %= div;
    return res;
}

std::optional<uint64_t> evolve(uint64_t& stone)
{
    if (stone == 0) {
        stone = 1;
        return std::optional<uint64_t>();
    }
    if (size_t nd = numDigits(stone); 0 == nd % 2) {
        return split(stone, nd / 2);
    }
    stone *= 2024;
    return std::optional<uint64_t>();
}

size_t evolve(std::unordered_map<uint64_t, size_t>& stones, size_t numSteps)
{
    while (numSteps-- > 0) {
        std::unordered_map<uint64_t, size_t> next;
        for (auto& kvp : stones) {
            auto stone = kvp.first;
            const auto n = kvp.second;
            if (const auto optStone = evolve(stone); optStone.has_value()) {
                next[*optStone] += n;
            }
            next[stone] += n;
        }
        std::swap(next, stones);
    }
    size_t numStones = 0;
    for (const auto& kvp : stones) {
        numStones += kvp.second;
    }
    return numStones;
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

    std::unordered_map<uint64_t, size_t> stones;
    {
        std::istringstream iss{lines[0]};
        uint64_t stone = 0;
        while (iss >> stone) {
            stones.insert_or_assign(stone, 1);
        }
    }

    std::cout << evolve(stones, 25) << '\n';
    std::cout << evolve(stones, 75 - 25) << '\n';

    return EXIT_SUCCESS;
}
