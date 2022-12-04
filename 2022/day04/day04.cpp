// Advent of Code 2022
// Day 4: Camp Cleanup
// https://adventofcode.com/2022/day/4

#define _CRT_SECURE_NO_WARNINGS

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

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    {  // Part 1
        uint64_t count{};
        for (const auto& line : lines) {
            int32_t x1, y1, x2, y2;
            std::sscanf(line.c_str(), "%d-%d,%d-%d", &x1, &y1, &x2, &y2);
            if ((x1 <= x2 && y2 <= y1) || (x2 <= x1 && y1 <= y2)) {
                count++;
            }
        }
        std::cout << count << std::endl;
    }
    {  // Part 2
        uint64_t count{};
        for (const auto& line : lines) {
            int32_t x1, y1, x2, y2;
            std::sscanf(line.c_str(), "%d-%d,%d-%d", &x1, &y1, &x2, &y2);
            if ((x1 <= x2 && x2 <= y1) || (x2 <= x1 && x1 <= y2)) {
                count++;
            }
        }
        std::cout << count << std::endl;
    }

    return EXIT_SUCCESS;
}
