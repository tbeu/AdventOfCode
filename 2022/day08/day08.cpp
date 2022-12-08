// Advent of Code 2022
// Day 8: Treetop Tree House
// https://adventofcode.com/2022/day/8

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
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

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    const size_t len{lines[0].size()};

    {  // Part 1
        size_t countVisible{4 * (len - 1)};
        for (size_t i = 1; i < len - 1; ++i) {
            for (size_t j = 1; j < len - 1; ++j) {
                const auto& h = lines[i][j];
                bool done{true};
                for (size_t k = 0; k < i; ++k) {
                    if (h <= lines[k][j]) {
                        done = false;
                        break;
                    }
                }
                if (done) {
                    ++countVisible;
                    continue;
                }

                done = true;
                for (size_t k = i + 1; k < len; ++k) {
                    if (h <= lines[k][j]) {
                        done = false;
                        break;
                    }
                }
                if (done) {
                    ++countVisible;
                    continue;
                }

                done = true;
                for (size_t k = 0; k < j; ++k) {
                    if (h <= lines[i][k]) {
                        done = false;
                        break;
                    }
                }
                if (done) {
                    ++countVisible;
                    continue;
                }

                done = true;
                for (size_t k = j + 1; k < len; ++k) {
                    if (h <= lines[i][k]) {
                        done = false;
                        break;
                    }
                }
                if (done) {
                    ++countVisible;
                    continue;
                }
            }
        }
        std::cout << countVisible << std::endl;
    }
    {  // Part 2
        size_t maxScore{0};
        for (size_t i = 1; i < len - 1; ++i) {
            for (size_t j = 1; j < len - 1; ++j) {
                size_t score{0};
                const auto& h = lines[i][j];
                for (size_t k = i; k > 0; --k) {
                    ++score;
                    if (h <= lines[k - 1][j]) {
                        break;
                    }
                }

                size_t dist{0};
                for (size_t k = i + 1; k < len; ++k) {
                    ++dist;
                    if (h <= lines[k][j]) {
                        break;
                    }
                }
                score *= dist;

                dist = 0;
                for (size_t k = j; k > 0; --k) {
                    ++dist;
                    if (h <= lines[i][k - 1]) {
                        break;
                    }
                }
                score *= dist;

                dist = 0;
                for (size_t k = j + 1; k < len; ++k) {
                    ++dist;
                    if (h <= lines[i][k]) {
                        break;
                    }
                }
                score *= dist;

                maxScore = std::max(maxScore, score);
            }
        }
        std::cout << maxScore << std::endl;
    }
    return EXIT_SUCCESS;
}
