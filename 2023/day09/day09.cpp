// Advent of Code 2023
// Day 9: Mirage Maintenance
// https://adventofcode.com/2023/day/9

#include <algorithm>
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

    {  // Part 1
        int64_t sum{};
        for (const auto& line : lines) {
            std::vector<int64_t> data;
            std::vector<int64_t> lasts;
            {
                int64_t x;
                std::istringstream iss{line};
                while (iss >> x) {
                    data.push_back(x);
                }
            }
            do {
                lasts.push_back(data.back());
                std::adjacent_difference(data.begin(), data.end(), data.begin());
                data.erase(data.begin());
            } while (!std::all_of(data.begin(), data.end(), [](auto d) { return d == 0; }));
            for (const auto& last : lasts) {
                sum += last;
            }
        }
        std::cout << sum << std::endl;
    }
    {  // Part 2
        int64_t sum{};
        for (const auto& line : lines) {
            std::vector<int64_t> data;
            std::vector<int64_t> firsts;
            {
                int64_t x;
                std::istringstream iss{line};
                while (iss >> x) {
                    data.push_back(x);
                }
            }
            do {
                firsts.push_back(data.front());
                std::adjacent_difference(data.begin(), data.end(), data.begin());
                data.erase(data.begin());
            } while (!std::all_of(data.begin(), data.end(), [](auto d) { return d == 0; }));
            int64_t first{};
            for (size_t i = firsts.size(); i > 0; --i) {
                first = firsts[i - 1] - first;
            }
            sum += first;
        }
        std::cout << sum << std::endl;
    }

    return EXIT_SUCCESS;
}
