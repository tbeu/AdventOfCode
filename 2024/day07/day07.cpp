// Advent of Code 2024
// Day 7: Bridge Repair
// https://adventofcode.com/2024/day/7

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

uint64_t concat(uint64_t a, const uint64_t b)
{
    auto c = b;
    while (c != 0u) {
        c /= 10;
        a *= 10;
    }
    return a + b;
}

template <bool enableConcatenation = false>
bool check(const std::vector<uint64_t>& ops, uint64_t res, uint64_t interim, size_t i = 1)
{
    if (i == ops.size()) {
        return res == interim;
    }
    if (interim > res) {
        return false;
    }
    if (check<enableConcatenation>(ops, res, interim * ops[i], i + 1) ||
        check<enableConcatenation>(ops, res, interim + ops[i], i + 1)) {
        return true;
    }
    if constexpr (enableConcatenation) {
        return check<true>(ops, res, concat(interim, ops[i]), i + 1);
    } else {
        return false;
    }
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

    uint64_t sum1{};
    uint64_t sum2{};
    for (const auto& line : lines) {
        std::istringstream iss{line};
        uint64_t res = 0;
        iss >> res;
        std::vector<uint64_t> ops;
        iss.ignore(1);
        uint64_t op = 0;
        while (iss >> op) {
            ops.push_back(op);
        }
        if (check(ops, res, ops[0])) {
            sum1 += res;
        } else if (check<true>(ops, res, ops[0])) {
            sum2 += res;
        }
    }

    {  // Part 1
        std::cout << sum1 << '\n';
    }
    {  // Part 2
        std::cout << sum1 + sum2 << '\n';
    }

    return EXIT_SUCCESS;
}
