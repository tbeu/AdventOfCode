// Advent of Code 2023
// Day 11: Cosmic Expansion
// https://adventofcode.com/2023/day/11

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

template <uint64_t EXPANSION_FACTOR = 2>
static inline uint64_t distances(const std::vector<uint16_t>& p)
{
    uint64_t sum{};
    for (size_t i = 0; i < p.size(); ++i) {
        uint64_t d{};  // keep track of coordinate differences to be expanded
        for (size_t j = i + 1; j < p.size(); ++j) {
            if (p[j] > p[j - 1]) {
                d += p[j] - p[j - 1] - 1;
            }
            sum += p[j] - p[i] + (EXPANSION_FACTOR - 1) * d;
        }
    }
    return sum;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (2 != argc || !readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    // sorted row/col coordinates for galaxies
    std::vector<uint16_t> rg, cg;
    for (uint16_t r = 0; r < lines.size(); ++r) {
        for (uint16_t c = 0; c < lines[r].size(); ++c) {
            if (lines[r][c] == '#') {
                rg.insert(std::lower_bound(rg.begin(), rg.end(), r), r);
                cg.insert(std::lower_bound(cg.begin(), cg.end(), c), c);
            }
        }
    }

    {  // Part 1
        constexpr uint64_t expansion{2};
        const auto sum = distances<expansion>(rg) + distances<expansion>(cg);
        std::cout << sum << std::endl;
    }
    {  // Part 2
        constexpr uint64_t expansion{1000000};
        const auto sum = distances<expansion>(rg) + distances<expansion>(cg);
        std::cout << sum << std::endl;
    }

    return EXIT_SUCCESS;
}
