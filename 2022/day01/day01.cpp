// Advent of Code 2022
// Day 1: Calorie Counting
// https://adventofcode.com/2022/day/1

#include <algorithm>
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

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    lines.push_back(std::string());
    {  // Part 1
        uint64_t max{};
        uint64_t sum{};
        for (const auto& line : lines) {
            if (line.empty()) {
                if (sum > max) {
                    max = sum;
                }
                sum = 0;
            } else {
                sum += std::stoull(line);
            }
        }
        std::cout << max << std::endl;
    }
    {  // Part 2
        uint64_t max1{};
        uint64_t max2{};
        uint64_t max3{};
        uint64_t sum{};
        for (const auto& line : lines) {
            if (line.empty()) {
                if (sum > max1) {
                    max3 = max2;
                    max2 = max1;
                    max1 = sum;
                } else if (sum > max2) {
                    max3 = max2;
                    max2 = sum;
                } else if (sum > max3) {
                    max3 = sum;
                }
                sum = 0;
            } else {
                sum += std::stoull(line);
            }
        }
        std::cout << max1 + max2 + max3 << std::endl;
    }

    return EXIT_SUCCESS;
}
