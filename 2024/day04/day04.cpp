// Advent of Code 2024
// Day 4: Ceres Search
// https://adventofcode.com/2024/day/4

#include <fstream>
#include <iostream>
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
        uint64_t count{};
        for (size_t r = 0; r < lines.size(); ++r) {
            for (size_t c = 0; c < lines[r].size(); ++c) {
                if (lines[r][c] != 'X') {
                    continue;
                }
                if (c < lines[r].size() - 3 && lines[r][c + 1] == 'M' && lines[r][c + 2] == 'A' &&
                    lines[r][c + 3] == 'S') {  //E
                    count++;
                }
                if (c < lines[r].size() - 3 && r < lines.size() - 3 && lines[r + 1][c + 1] == 'M' &&
                    lines[r + 2][c + 2] == 'A' && lines[r + 3][c + 3] == 'S') {  //SE
                    count++;
                }
                if (r < lines.size() - 3 && lines[r + 1][c] == 'M' && lines[r + 2][c] == 'A' &&
                    lines[r + 3][c] == 'S') {  //S
                    count++;
                }
                if (c >= 3 && r < lines.size() - 3 && lines[r + 1][c - 1] == 'M' && lines[r + 2][c - 2] == 'A' &&
                    lines[r + 3][c - 3] == 'S') {  //SW
                    count++;
                }
                if (c >= 3 && lines[r][c - 1] == 'M' && lines[r][c - 2] == 'A' && lines[r][c - 3] == 'S') {  //W
                    count++;
                }
                if (c >= 3 && r >= 3 && lines[r - 1][c - 1] == 'M' && lines[r - 2][c - 2] == 'A' &&
                    lines[r - 3][c - 3] == 'S') {  //NW
                    count++;
                }
                if (r >= 3 && lines[r - 1][c] == 'M' && lines[r - 2][c] == 'A' && lines[r - 3][c] == 'S') {  //N
                    count++;
                }
                if (c < lines[r].size() - 3 && r >= 3 && lines[r - 1][c + 1] == 'M' && lines[r - 2][c + 2] == 'A' &&
                    lines[r - 3][c + 3] == 'S') {  //NE
                    count++;
                }
            }
        }
        std::cout << count << '\n';
    }
    {  // Part 2
        uint64_t count{};
        for (size_t r = 1; r < lines.size() - 1; ++r) {
            for (size_t c = 1; c < lines[r].size() - 1; ++c) {
                if (lines[r][c] != 'A') {
                    continue;
                }
                if (((lines[r - 1][c - 1] == 'M' && lines[r + 1][c + 1] == 'S') ||
                     (lines[r + 1][c + 1] == 'M' && lines[r - 1][c - 1] == 'S')) &&
                    ((lines[r + 1][c - 1] == 'M' && lines[r - 1][c + 1] == 'S') ||
                     (lines[r - 1][c + 1] == 'M' && lines[r + 1][c - 1] == 'S'))) {
                    count++;
                }
            }
        }
        std::cout << count << '\n';
    }

    return EXIT_SUCCESS;
}
