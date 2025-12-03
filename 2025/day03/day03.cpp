// Advent of Code 2025
// Day 3: Lobby
// https://adventofcode.com/2025/day/3

#include <fstream>
#include <iostream>
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

int64_t mul10(int64_t num, size_t times)
{
    for (size_t i = 0; i < times; ++i) {
        num *= 10;
    }
    return num;
}

int64_t maxJoltage(const std::string& line, size_t n)
{
    int64_t max = 0;
    int64_t d = 0;
    size_t lastc = 0;
    for (size_t c = 0; c < line.size() - n + 1; ++c) {
        if (line[c] - '0' > d) {
            d = line[c] - '0';
            lastc = c;
        }
    }
    max += mul10(d, n - 1);
    d = 0;
    for (size_t i = 1; i < n; ++i) {
        for (size_t c = i; c < line.size() - n + 1 + i; ++c) {
            if (line[c] - '0' > d && c > lastc) {
                d = line[c] - '0';
                lastc = c;
            }
        }
        max += mul10(d, n - 1 - i);
        d = 0;
    }
    return max;
}

int64_t sumMaxJoltage(const std::vector<std::string>& lines, size_t n)
{
    int64_t sum = 0;
    for (const auto& line : lines) {
        sum += maxJoltage(line, n);
    }
    return sum;
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

    {  // Part 1
        std::cout << sumMaxJoltage(lines, 2) << '\n';
    }
    {  // Part 2
        std::cout << sumMaxJoltage(lines, 12) << '\n';
    }

    return EXIT_SUCCESS;
}
