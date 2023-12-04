// Advent of Code 2023
// Day 4: Scratchcards
// https://adventofcode.com/2023/day/4

#include <fstream>
#include <iostream>
#include <set>
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

struct Counter
{
    struct value_type
    {
        template <typename T>
        value_type(const T&)
        {
        }
    };
    void push_back(const value_type&)
    {
        ++count;
    }
    size_t count{0};
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

    {  // Part 1
        uint32_t sum{};
        for (const auto& line : lines) {
            std::set<std::string> s1, s2;
            std::istringstream iss{line};
            size_t id;
            std::string x;
            iss >> x;
            iss >> id;
            iss.ignore(1);
            bool sep{false};
            while (iss >> x) {
                if ('|' == x[0]) {
                    sep = true;
                    continue;
                }
                if (sep) {
                    s2.insert(x);
                } else {
                    s1.insert(x);
                }
            }
            Counter c;
            std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), std::back_inserter(c));
            sum += 1 << (c.count - 1);
        }
        std::cout << sum << std::endl;
    }
    {  // Part 2
        std::vector<uint32_t> cards(lines.size(), 1);
        uint32_t sum{};
        for (const auto& line : lines) {
            std::set<std::string> s1, s2;
            std::istringstream iss{line};
            size_t id;
            std::string x;
            iss >> x;
            iss >> id;
            iss.ignore(1);
            bool sep{false};
            while (iss >> x) {
                if ('|' == x[0]) {
                    sep = true;
                    continue;
                }
                if (sep) {
                    s2.insert(x);
                } else {
                    s1.insert(x);
                }
            }
            Counter c;
            std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), std::back_inserter(c));
            for (size_t i = 0; i < c.count; ++i) {
                cards[id + i] += cards[id - 1];
            }
            sum += cards[id - 1];
        }
        std::cout << sum << std::endl;
    }

    return EXIT_SUCCESS;
}
