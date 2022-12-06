// Advent of Code 2022
// Day 6: Tuning Trouble
// https://adventofcode.com/2022/day/6

#include <assert.h>
#include <fstream>
#include <iostream>
#include <set>
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

size_t findUnique(const std::string& data, size_t count)
{
    assert(count > 0);
    size_t pos{};
    for (size_t i = count - 1; i < data.size(); ++i) {
        std::set<std::string::value_type> set{};
        bool goOn;
        for (size_t j = 0; j < count; ++j) {
            std::tie(std::ignore, goOn) = set.insert(data[i - j]);
            if (!goOn) {
                break;
            }
        }
        if (!goOn) {
            continue;
        }
        if (set.size() == count) {
            pos = i;
            break;
        }
    }
    return pos;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }
    if (lines.empty()) {
        return EXIT_FAILURE;
    }

    {  // Part 1
        const size_t count{4};
        size_t pos{findUnique(lines[0], count)};
        assert(pos >= count - 1);
        std::cout << ++pos << std::endl;
    }
    {  // Part 2
        const size_t count{14};
        size_t pos{findUnique(lines[0], count)};
        assert(pos >= count - 1);
        std::cout << ++pos << std::endl;
    }

    return EXIT_SUCCESS;
}
