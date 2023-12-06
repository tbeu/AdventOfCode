// Advent of Code 2023
// Day 6: Wait For It
// https://adventofcode.com/2023/day/6

#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
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
    if (2 != argc || !readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    auto countWins = [](const uint64_t t, const uint64_t d) {
        uint64_t wins{};
        for (uint64_t v = 0; v <= t; ++v) {
            if (v * (t - v) > d) {
                wins++;
            }
        }
        return wins;
    };

    auto countWinsAnalytic = [](const uint64_t t, const uint64_t d) {
        // number of integers between roots of quadratic equation x * (t - x) = d
        const auto s = std::sqrt(t * t / 4. - d);
        auto x1 = static_cast<uint64_t>(std::ceil(t / 2. - s));
        auto x2 = static_cast<uint64_t>(std::floor(t / 2. + s));
        x1 += (x1 * (t - x1) <= d) ? 1 : 0;
        x2 -= (x2 * (t - x2) <= d) ? 1 : 0;
        return x2 - x1 + 1;
    };

    {  // Part 1
        auto toNumbers = [](const std::string& line) {
            std::vector<uint64_t> numbers;
            std::istringstream iss{line};
            iss.ignore(9);
            uint64_t x;
            while (iss >> x) {
                numbers.push_back(x);
            }
            return numbers;
        };

        const auto times = toNumbers(lines[0]);
        const auto distances = toNumbers(lines[1]);
        uint64_t p{1};
        for (size_t i = 0; i < times.size(); ++i) {
            p *= countWinsAnalytic(times[i], distances[i]);
        }
        std::cout << p << std::endl;
    }
    {  // Part 2
        auto toNumber = [](const std::string& line) {
            uint64_t n{};
            for (const auto c : line) {
                if (std::isdigit(c)) {
                    n = n * 10 + c - '0';
                }
            }
            return n;
        };

        std::cout << countWinsAnalytic(toNumber(lines[0]), toNumber(lines[1])) << std::endl;
    }

    return EXIT_SUCCESS;
}
