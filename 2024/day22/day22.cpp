// Advent of Code 2024
// Day 22: Monkey Market
// https://adventofcode.com/2024/day/22

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_set>
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

using Seq = std::array<int16_t, 4>;

struct SeqHash
{
    size_t operator()(const Seq& seq) const
    {
        return (seq[0] + 9) * 19 * 19 * 19 + (seq[1] + 9) * 19 * 19 + (seq[2] + 9) * 19 + (seq[3] + 9);
    }
};

int main(int argc, char* argv[])
{
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    std::vector<std::string> lines{};
    if (!readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    const auto evolve = [](uint64_t& num) {
        num ^= num * 64;
        num %= 16777216;
        num ^= num / 32;
        num %= 16777216;
        num ^= num * 2048;
        num %= 16777216;
    };

    {  // Part 1
        uint64_t sum = 0;
        for (const auto& line : lines) {
            std::istringstream iss{line};
            uint64_t num = 0;
            iss >> num;
            int16_t step = 0;
            while (step++ < 2000) {
                evolve(num);
            }
            sum += num;
        }
        std::cout << sum << '\n';
    }
    {  // Part 2
        std::map<Seq, uint16_t> map;
        for (const auto& line : lines) {
            std::istringstream iss{line};
            uint64_t num = 0;
            iss >> num;
            int16_t last = num % 10;
            std::unordered_set<Seq, SeqHash> visited;
            auto seq = Seq{0, 0, 0, 0};
            for (int16_t i = 0; i < 2000; ++i) {
                evolve(num);
                const int16_t cur = num % 10;
                std::rotate(seq.begin(), seq.begin() + 1, seq.end());
                seq[3] = cur - last;
                last = cur;
                if (i < 3) {
                    continue;
                }
                if (visited.find(seq) != visited.end()) {
                    continue;
                }
                visited.insert(seq);
                map[seq] += cur;
            }
        }
        const auto max = std::max_element(map.begin(), map.end(), [](const auto& a, const auto& b) {
                             return a.second < b.second;
                         })->second;
        std::cout << max << '\n';
    }

    return EXIT_SUCCESS;
}
