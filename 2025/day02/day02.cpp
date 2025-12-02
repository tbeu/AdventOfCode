// Advent of Code 2025
// Day 2: Gift Shop
// https://adventofcode.com/2025/day/2

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

bool isInvalid(uint64_t id, uint64_t numDigits)
{
    uint64_t div = 1;
    for (size_t i = 0; i < numDigits; ++i) {
        div *= 10;
    }
    const auto num = id % div;
    id /= div;
    while (id > 0) {
        auto next = id % div;
        if (next != num) {
            return false;
        }
        id /= div;
    }
    return true;
}

size_t numDigits(uint64_t num)
{
    size_t count{};
    while (num != 0) {
        num /= 10;
        ++count;
    }
    return count;
}

template <bool onlyTwice>
bool isInvalid(uint64_t id)
{
    const auto nd = numDigits(id);
    if constexpr (onlyTwice) {
        if (0 != nd % 2) {
            return false;
        }
        return isInvalid(id, nd / 2);
    } else {
        for (size_t i = 1; i < nd; ++i) {
            if (0 != nd % i) {
                continue;
            }
            if (isInvalid(id, i)) {
                return true;
            }
        }
        return false;
    }
}

template <bool onlyTwice>
std::vector<uint64_t> getInvalidIds(uint64_t s, uint64_t e)
{
    std::vector<uint64_t> invalidIds;
    for (uint64_t id = s; id <= e; ++id) {
        if (isInvalid<onlyTwice>(id)) {
            invalidIds.push_back(id);
        }
    }
    return invalidIds;
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

    std::vector<std::pair<uint64_t, uint64_t> > ranges;
    std::istringstream iss{lines[0]};
    int64_t s{};
    int64_t e{};
    while (iss >> s) {
        iss.ignore(1);
        iss >> e;
        iss.ignore(1);
        ranges.push_back({s, e});
    }

    {  // Part 1
        int64_t sum = 0;
        for (const auto& [s, e] : ranges) {
            auto invalidIds = getInvalidIds<true>(s, e);
            for (const auto& id : invalidIds) {
                sum += id;
            }
        }
        std::cout << sum << '\n';
    }
    {  // Part 2
        int64_t sum = 0;
        for (const auto& [s, e] : ranges) {
            auto invalidIds = getInvalidIds<false>(s, e);
            for (const auto& id : invalidIds) {
                sum += id;
            }
        }
        std::cout << sum << '\n';
    }

    return EXIT_SUCCESS;
}
