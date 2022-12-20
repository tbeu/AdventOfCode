// Advent of Code 2022
// Day 20: Grove Positioning System
// https://adventofcode.com/2022/day/20

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
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

template <typename T>
void move(std::vector<T>& v, size_t oldIndex, size_t newIndex)
{
    if (oldIndex > newIndex) {
        std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
    } else {
        std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);
    }
}

int32_t mix(const std::vector<int32_t>& v, int32_t scale = 1, size_t loopCount = 1)
{
    std::vector<int32_t> vIndex(v.size());
    std::iota(vIndex.begin(), vIndex.end(), 0);
    const int32_t div{static_cast<int32_t>(v.size() - 1)};
    size_t count{};
    do {
        for (size_t i = 0; i < v.size(); ++i) {
            const auto oldIndex = static_cast<int32_t>(std::find(vIndex.begin(), vIndex.end(), i) - vIndex.begin());
            auto newIndex = oldIndex + v[i] % div * scale;
            if (newIndex < 0 || newIndex > div) {
                newIndex = (newIndex % div + div) % div;
            }
            move<int32_t>(vIndex, static_cast<size_t>(oldIndex), static_cast<size_t>(newIndex));
        }
    } while (++count < loopCount);
    auto zeroIndex = static_cast<int32_t>(std::find(v.begin(), v.end(), 0) - v.begin());
    zeroIndex = static_cast<int32_t>(std::find(vIndex.begin(), vIndex.end(), zeroIndex) - vIndex.begin());
    int32_t level{};
    for (size_t i = 1; i < 4; ++i) {
        level += v[vIndex[(1000 * i + zeroIndex) % (div + 1)]];
    }
    return level;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    std::vector<int32_t> v{};
    for (const auto& line : lines) {
        v.push_back(std::stoi(line));
    }

    {  // Part 1
        std::cout << mix(v) << std::endl;
    }
    {  // Part 2
        const int64_t div{static_cast<int64_t>(v.size() - 1)};
        const int64_t code{811589153};
        const int32_t scale{code % div};
        std::cout << code * mix(v, scale, 10) << std::endl;
    }

    return EXIT_SUCCESS;
}
