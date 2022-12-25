// Advent of Code 2022
// Day 25: Full of Hot Air
// https://adventofcode.com/2022/day/25

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <gsl/util>

static bool readFile(std::string fileName, std::vector<std::string>& lines)
{
    std::ifstream in{fileName.c_str()};
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

void printSNAFU(uint64_t num)
{
    if (num < 3) {
        std::cout << num;
        return;
    }

    const uint64_t rem = num % 5;
    printSNAFU((num + 2) / 5);
    if (rem < 3) {
        std::cout << rem;
    } else if (rem == 3) {
        std::cout << "=";
    } else if (rem == 4) {
        std::cout << "-";
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

    uint64_t sum{};
    for (const auto& line : lines) {
        uint64_t num{};
        for (size_t i = line.size(); i > 0; --i) {
            const auto c = line[i - 1];
            uint64_t digit;
            if (c == '=') {
                digit = -2;
            } else if (c == '-') {
                digit = -1;
            } else {
                digit = c - '0';
            }
            num += digit * static_cast<uint64_t>(std::pow(5, line.size() - i));
        }
        sum += num;
    }

    {  // Part 1
        printSNAFU(sum);
        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}
