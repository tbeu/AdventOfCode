// Advent of Code 2022
// Day 2: Rock Paper Scissors
// https://adventofcode.com/2022/day/2

#include <algorithm>
#include <fstream>
#include <iostream>
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
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    {  // Part 1
        uint64_t score{};
        for (const auto& line : lines) {
            if (line[0] == 'A') {
                if (line[2] == 'X') {
                    // Rock vs Rock
                    score += 4;
                } else if (line[2] == 'Y') {
                    // Rock vs Paper
                    score += 8;
                } else if (line[2] == 'Z') {
                    // Rock vs Scissors
                    score += 3;
                }
            } else if (line[0] == 'B') {
                if (line[2] == 'X') {
                    // Paper vs Rock
                    score += 1;
                } else if (line[2] == 'Y') {
                    // Paper vs Paper
                    score += 5;
                } else if (line[2] == 'Z') {
                    // Paper vs Scissors
                    score += 9;
                }
            } else if (line[0] == 'C') {
                if (line[2] == 'X') {
                    // Scissors vs Rock
                    score += 7;
                } else if (line[2] == 'Y') {
                    // Scissors vs Paper
                    score += 2;
                } else if (line[2] == 'Z') {
                    // Scissors vs Scissors
                    score += 6;
                }
            }
        }
        std::cout << score << std::endl;
    }
    {  // Part 2
        uint64_t score{};
        for (const auto& line : lines) {
            if (line[0] == 'A') {
                if (line[2] == 'X') {
                    // Rock vs Scissors
                    score += 3;
                } else if (line[2] == 'Y') {
                    // Rock vs Rock
                    score += 4;
                } else if (line[2] == 'Z') {
                    // Rock vs Paper
                    score += 8;
                }
            } else if (line[0] == 'B') {
                if (line[2] == 'X') {
                    // Paper vs Rock
                    score += 1;
                } else if (line[2] == 'Y') {
                    // Paper vs Paper
                    score += 5;
                } else if (line[2] == 'Z') {
                    // Paper vs Scissors
                    score += 9;
                }
            } else if (line[0] == 'C') {
                if (line[2] == 'X') {
                    // Scissors vs Paper
                    score += 2;
                } else if (line[2] == 'Y') {
                    // Scissors vs Scissors
                    score += 6;
                } else if (line[2] == 'Z') {
                    // Scissors vs Rock
                    score += 7;
                }
            }
        }
        std::cout << score << std::endl;
    }
    return EXIT_SUCCESS;
}
