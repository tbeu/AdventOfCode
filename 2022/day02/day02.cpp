// Advent of Code 2022
// Day 2: Rock Paper Scissors
// https://adventofcode.com/2022/day/2

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
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

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    {  // Part 1
        const uint16_t scoreMap[3][3] = {{4, 8, 3}, {1, 5, 9}, {7, 2, 6}};
        auto acc = [&](auto& s, const auto& line) { return s + scoreMap[line[0] - 'A'][line[2] - 'X']; };
        auto score = std::accumulate(lines.cbegin(), lines.cend(), uint64_t{0}, acc);
        std::cout << score << std::endl;
    }
    {  // Part 2
        const uint16_t scoreMap[3][3] = {{3, 4, 8}, {1, 5, 9}, {2, 6, 7}};
        auto acc = [&](auto& s, const auto& line) { return s + scoreMap[line[0] - 'A'][line[2] - 'X']; };
        auto score = std::accumulate(lines.cbegin(), lines.cend(), uint64_t{0}, acc);
        std::cout << score << std::endl;
    }

    return EXIT_SUCCESS;
}
