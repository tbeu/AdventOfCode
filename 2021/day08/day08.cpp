#include <algorithm>
#include <array>
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

constexpr size_t n = 4;
using Code = std::string;
using ICodes = std::array<Code, 10>;
using OCodes = std::array<Code, n>;
using Puzzle = std::pair<ICodes, OCodes>;
using Puzzles = std::vector<Puzzle>;

static bool containsAllOf(const Code& s1, const Code& s2)
{
    return std::all_of(s1.cbegin(), s1.cend(), [&](const auto& c) { return s2.find(c) != Code::npos; });
}

static std::array<ICodes::iterator, 10> decode(ICodes& codes)
{
    std::array<ICodes::iterator, 10> s{};
    // First the four unique ones: 1, 4, 7 and 8
    s[1] = std::find_if(codes.begin(), codes.end(), [](const auto& c) { return c.size() == 2; });
    s[7] = std::find_if(codes.begin(), codes.end(), [](const auto& c) { return c.size() == 3; });
    s[4] = std::find_if(codes.begin(), codes.end(), [](const auto& c) { return c.size() == 4; });
    s[8] = std::find_if(codes.begin(), codes.end(), [](const auto& c) { return c.size() == 7; });
    // 6 is the only 6-segment number not containing all segments of 1
    s[6] = std::find_if(codes.begin(), codes.end(),
                        [&](const auto& c) { return c.size() == 6 && !containsAllOf(*s[1], c); });
    // 3 is the only 5-segment number containing all the segments of 1
    s[3] = std::find_if(codes.begin(), codes.end(),
                        [&](const auto& c) { return c.size() == 5 && containsAllOf(*s[1], c); });
    // 5 is the only 5-segment number where all segments are also in 6
    s[5] = std::find_if(codes.begin(), codes.end(),
                        [&](const auto& c) { return c.size() == 5 && containsAllOf(c, *s[6]); });
    // 2 is the remaining 5-segment number
    s[2] = std::find_if(codes.begin(), codes.end(),
                        [&](const auto& c) { return c.size() == 5 && 0 != s[3]->compare(c) && 0 != s[5]->compare(c); });
    // 9 is the only 6-segment number containing all the segments of 4
    s[9] = std::find_if(codes.begin(), codes.end(),
                        [&](const auto& c) { return c.size() == 6 && containsAllOf(*s[4], c); });
    // 0 is the remaining 6-segment number
    s[0] = std::find_if(codes.begin(), codes.end(),
                        [&](const auto& c) { return c.size() == 6 && 0 != s[6]->compare(c) && 0 != s[9]->compare(c); });

    // Sort the segments alphabetically
    std::for_each(s.begin(), s.end(), [](auto& c) { std::sort(c->begin(), c->end()); });

    return s;
}

static size_t solve(Puzzle& puzzle)
{
    size_t num{0};
    const auto& s{decode(puzzle.first)};

    // Sort the segments alphabetically
    auto& codes{puzzle.second};
    std::for_each(codes.begin(), codes.end(), [](auto& c) { std::sort(c.begin(), c.end()); });

    std::stringstream oss{};
    for (const auto& code : codes) {
        if (const auto& it{std::find_if(s.cbegin(), s.cend(), [&](const auto& c) { return 0 == c->compare(code); })};
            it != s.cend()) {
            oss << std::distance(s.cbegin(), it);
        }
    }
    oss >> num;
    return num;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    Puzzles puzzles{};
    for (const auto& line : lines) {
        std::istringstream iss(line);
        ICodes icodes{};
        OCodes ocodes{};
        for (size_t i = 0; i < 10; ++i) {
            iss >> icodes[i];
        }
        Code c{};
        iss >> c;
        for (size_t i = 0; i < n; ++i) {
            iss >> ocodes[i];
        }
        puzzles.emplace_back(Puzzle(icodes, ocodes));
    }

    {  // Part 1
        size_t sum{0};
        for (const auto& puzzle : puzzles) {
            const auto& codes = puzzle.second;
            sum += std::count_if(codes.cbegin(), codes.cend(), [](const auto& c) {
                return c.size() == 2 || c.size() == 3 || c.size() == 4 || c.size() == 7;
            });
        }
        std::cout << sum << std::endl;
    }
    {  // Part 2
        size_t sum{0};
        for (auto& puzzle : puzzles) {
            sum += solve(puzzle);
        }
        std::cout << sum << std::endl;
    }

    return EXIT_SUCCESS;
}
