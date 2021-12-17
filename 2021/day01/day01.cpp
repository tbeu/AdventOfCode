// Advent of Code 2021
// Day 1: Sonar Sweep
// https://adventofcode.com/2021/day/1

#include <charconv>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include <gsl/util>

static bool readFile(std::string fileName, std::vector<std::string> &lines)
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

inline static std::vector<int> toInt(std::vector<std::string> &lines)
{
    std::vector<int> data;
    data.reserve(lines.size());
    std::transform(lines.cbegin(), lines.cend(), std::back_inserter(data), [](const std::string &line) {
        int val{0};
        std::from_chars(line.data(), line.data() + line.size(), val);
        return val;
    });
    return data;
}

static size_t countIncreases(std::vector<int> &data)
{
    std::adjacent_difference(data.begin(), data.end(), data.begin());
    data.erase(data.begin());
    return std::count_if(data.cbegin(), data.cend(), [](int i) { return i > 0; });
}

int main(int argc, char *argv[])
{
    std::vector<std::string> lines;
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    {
        auto data{toInt(lines)};
        std::cout << countIncreases(data) << std::endl;
    }
    {
        auto data{toInt(lines)};
        std::vector<int> data3;
        data3.reserve(data.size() - 2);
        for (auto val = data.cbegin(); val != data.cend() - 2; val++) {
            data3.push_back(std::accumulate(val, val + 3, 0));
        }
        std::cout << countIncreases(data3) << std::endl;
    }
    return EXIT_SUCCESS;
}
