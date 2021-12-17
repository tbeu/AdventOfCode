// Advent of Code 2021
// Day 7: The Treachery of Whales
// https://adventofcode.com/2021/day/7

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
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

using Number = long;

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }
    if (lines.size() < 1) {
        return EXIT_FAILURE;
    }

    std::vector<Number> numbers{};
    {
        std::istringstream iss(lines[0]);
        Number num;
        while (iss >> num) {
            numbers.push_back(num);
            iss.ignore(1);
        }
    }

    {  // Part 1
        Number min{INT32_MAX};
        for (const auto& num : numbers) {
            std::vector<Number> s(numbers.size());
            std::transform(numbers.begin(), numbers.end(), s.begin(), [&](auto& v) { return std::abs(v - num); });
            const auto sum{std::accumulate(s.begin(), s.end(), 0, std::plus<Number>())};
            min = std::min<Number>(min, sum);
        }
        std::cout << min << std::endl;
    }
    {  // Part 2
        Number min{INT32_MAX};
        for (Number num = 0; num < static_cast<Number>(numbers.size()); ++num) {
            std::vector<Number> s(numbers.size());
            std::transform(numbers.begin(), numbers.end(), s.begin(), [&](auto& v) {
                const Number n{std::abs(v - num)};
                return n * (n + 1) / 2;
            });
            const auto sum{std::accumulate(s.begin(), s.end(), 0, std::plus<Number>())};
            min = std::min<Number>(min, sum);
        }
        std::cout << min << std::endl;
    }

    return EXIT_SUCCESS;
}
