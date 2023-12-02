// Advent of Code 2023
// Day 2: Cube Conundrum
// https://adventofcode.com/2023/day/2

#include <fstream>
#include <iostream>
#include <numeric>
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

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (2 != argc || !readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    {  // Part 1
        auto acc = [](auto& s, const auto& line) {
            uint32_t id, x;
            std::string c;
            std::istringstream iss{line};
            iss >> c;
            iss >> id;
            iss.ignore(1);
            while (iss >> x) {
                iss >> c;
                if (('r' == c[0] && x > 12) ||  // red
                    ('g' == c[0] && x > 13) ||  // green
                    ('b' == c[0] && x > 14)) {  // blue
                    return s;                   // impossible
                }
            }
            return s + id;
        };
        const auto sum = std::accumulate(lines.cbegin(), lines.cend(), uint32_t{0}, acc);
        std::cout << sum << std::endl;
    }
    {  // Part 2
        auto acc = [](auto& s, const auto& line) {
            uint32_t id, x, r{}, g{}, b{};
            std::string c;
            std::istringstream iss{line};
            iss >> c;
            iss >> id;
            iss.ignore(1);
            while (iss >> x) {
                iss >> c;
                if ('r' == c[0]) {  //red
                    r = std::max(r, x);
                } else if ('g' == c[0]) {  // green
                    g = std::max(g, x);
                } else if ('b' == c[0]) {  //blue
                    b = std::max(b, x);
                }
            }
            return s + r * g * b;
        };
        const auto sum = std::accumulate(lines.cbegin(), lines.cend(), uint32_t{0}, acc);
        std::cout << sum << std::endl;
    }

    return EXIT_SUCCESS;
}
