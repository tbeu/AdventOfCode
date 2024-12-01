// Advent of Code 2024
// Day 1: Historian Hysteria
// https://adventofcode.com/2024/day/1

#include <algorithm>
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

int main(int argc, char* argv[])
{
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    std::vector<std::string> lines{};
    if (!readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    std::vector<uint32_t> left;
    std::vector<uint32_t> right;
    left.reserve(lines.size());
    right.reserve(lines.size());
    for (const auto& line : lines) {
        std::istringstream iss{line};
        uint32_t i1{};
        uint32_t i2{};
        iss >> i1 >> i2;
        left.push_back(i1);
        right.push_back(i2);
    }
    std::sort(left.begin(), left.end());
    std::sort(right.begin(), right.end());

    {  // Part 1
        uint64_t sum{};
        for (size_t i = 0; i < left.size(); ++i) {
            sum += left[i] > right[i] ? left[i] - right[i] : right[i] - left[i];
        }
        std::cout << sum << '\n';
    }
    {  // Part 2
        uint64_t sum{};
        size_t last_j{};
        for (size_t i = 0; i < left.size(); ++i) {
            for (size_t j = last_j; j < right.size(); ++j) {
                if (right[j] < left[i]) {
                    last_j = j + 1;
                    continue;
                }
                if (right[j] == left[i]) {
                    sum += left[i];
                    continue;
                }
                break;
            }
        }
        std::cout << sum << '\n';
    }

    return EXIT_SUCCESS;
}
