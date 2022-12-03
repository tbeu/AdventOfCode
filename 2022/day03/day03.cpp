// Advent of Code 2022
// Day 3: Rucksack Reorganization
// https://adventofcode.com/2022/day/3

#include <algorithm>
#include <cctype>
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

using Rucksack = std::string;
using Item = Rucksack::value_type;

uint64_t errorPriorityItem(Item item)
{
    if (std::isupper(item)) {
        return item - 'A' + 27;
    }
    return item - 'a' + 1;
}

uint64_t errorPriority1(const Rucksack& line)
{
    const auto size2{line.size() / 2};
    const auto item = std::find_first_of(line.cbegin(), line.cbegin() + size2, line.cbegin() + size2, line.cend());
    if (item != line.cbegin() + size2) {
        return errorPriorityItem(*item);
    }
    return 0;
}

uint64_t errorPriority3(const Rucksack& line1, const Rucksack& line2, const Rucksack& line3)
{
    for (auto item : line1) {
        if (line2.find(item) != Rucksack::npos && line3.find(item) != Rucksack::npos) {
            return errorPriorityItem(item);
        }
    }
    return 0;
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
        auto acc = [](auto& s, const auto& line) { return s + errorPriority1(line); };
        auto error = std::accumulate(lines.cbegin(), lines.cend(), uint64_t{0}, acc);
        std::cout << error << std::endl;
    }
    {  // Part 2
        uint64_t error{};
        for (size_t i = 0; i < lines.size() - 2; i += 3) {
            error += errorPriority3(lines[i], lines[i + 1], lines[i + 2]);
        }
        std::cout << error << std::endl;
    }

    return EXIT_SUCCESS;
}
