// Advent of Code 2025
// Day 1: Secret Entrance
// https://adventofcode.com/2025/day/1

#include <cassert>
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

    constexpr const int32_t N = 100;
    constexpr const int32_t startPos = 50;
    {  // Part 1
        int32_t sum = 0;
        int32_t pos = startPos;
        for (const auto& line : lines) {
            std::istringstream iss{line};
            char c = 0;
            int32_t i{};
            iss >> c >> i;
            if ('L' == c) {
                pos -= i;
            } else {
                pos += i;
            }
            pos %= N;
            if (0 == pos) {
                ++sum;
            }
        }
        std::cout << sum << '\n';
    }
    {  // Part 2
        int32_t sum = 0;
        int32_t pos = startPos;
        for (const auto& line : lines) {
            std::istringstream iss{line};
            char c = 0;
            int32_t i{};
            iss >> c >> i;
            if ('L' == c) {
                sum -= (pos - N - i) / N - (pos - N) / N;
                pos = ((pos - i) % N + N) % N;
            } else {
                sum += (pos + i) / N /*- pos / N*/;
                pos = (pos + i) % N;
            }
            assert(pos >= 0 && pos < N);
        }
        std::cout << sum << '\n';
    }

    return EXIT_SUCCESS;
}
