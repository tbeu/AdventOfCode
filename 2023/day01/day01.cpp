// Advent of Code 2023
// Day 1: Trebuchet?!
// https://adventofcode.com/2023/day/1

#include <array>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
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
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    std::vector<std::string> lines{};
    if (!readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    {  // Part 1
        uint32_t sum{};
        for (const auto& line : lines) {
            uint32_t d{};
            for (const auto c : line) {
                if (std::isdigit(c)) {
                    if (0 == d) {
                        sum += 10 * (c - '0');
                    }
                    d = c - '0';
                }
            }
            sum += d;
        }
        std::cout << sum << std::endl;
    }
    {  // Part 2
        constexpr std::array<std::string_view, 9> digitWords = {"one", "two",   "three", "four", "five",
                                                                "six", "seven", "eight", "nine"};

        uint64_t sum{};
        for (const auto& line : lines) {
            uint64_t d{};
            for (size_t i = 0; i < line.size(); ++i) {
                if (const auto c = line[i]; std::isdigit(c)) {
                    if (0 == d) {
                        sum += 10 * (c - '0');
                    }
                    d = c - '0';
                } else {
                    for (uint32_t j = 0; j < digitWords.size(); ++j) {
                        if (const auto& w = digitWords[j]; 0 == line.compare(i, w.size(), w)) {
                            i += w.size() - 2;
                            if (0 == d) {
                                sum += 10 * (j + 1);
                            }
                            d = j + 1;
                            break;
                        }
                    }
                }
            }
            sum += d;
        }
        std::cout << sum << std::endl;
    }

    return EXIT_SUCCESS;
}
