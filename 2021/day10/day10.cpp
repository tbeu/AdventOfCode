// Advent of Code 2021
// Day 10: Syntax Scoring
// https://adventofcode.com/2021/day/10

#include <algorithm>
#include <array>
#include <fstream>
#include <map>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

#include <gsl/util>

static bool readFile(std::string fileName, std::vector<std::string>& map)
{
    std::ifstream in{fileName.c_str()};
    if (!in) {
        std::cerr << "Cannot open file " << fileName << std::endl;
        return false;
    }
    auto closeStream = gsl::finally([&in] { in.close(); });
    std::string str;
    while (std::getline(in, str)) {
        map.push_back(str);
    }
    return true;
}

constexpr std::array<std::string::value_type, 4> openers{'(', '[', '{', '<'};

static inline bool isOpener(const std::string::value_type& c)
{
    return std::any_of(openers.cbegin(), openers.cend(), [&](const auto& o) { return o == c; });
}

static inline bool isValidClosener(const std::string::value_type& b, const std::string::value_type& c)
{
    return b == '(' && c == ')' || b == '[' && c == ']' || b == '{' && c == '}' || b == '<' && c == '>';
}

static std::stack<std::string::value_type> checkIncomplete(const std::string& line)
{
    std::stack<std::string::value_type> stack{};
    for (const auto& c : line) {
        if (isOpener(c)) {
            stack.push(c);
            continue;
        }
        const auto& b{stack.top()};
        stack.pop();
    }
    return stack;
}

static uint64_t calcScore(const std::string& line, std::map<std::string::value_type, uint64_t>& penalty)
{
    auto stack{checkIncomplete(line)};
    uint64_t score{0};
    while (!stack.empty()) {
        const auto& c{stack.top()};
        stack.pop();
        score *= 5;
        score += penalty[c];
    }
    return score;
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
        uint64_t err{0};
        std::map<std::string::value_type, uint64_t> penalty{{')', 3}, {']', 57}, {'}', 1197}, {'>', 25137}};
        auto line{lines.begin()};
        while (line != lines.end()) {
            std::stack<std::string::value_type> stack{};
            bool isValid{true};
            for (const auto& c : *line) {
                if (isOpener(c)) {
                    stack.push(c);
                    continue;
                }
                const auto& b{stack.top()};
                stack.pop();
                isValid = isValidClosener(b, c);
                if (!isValid) {
                    err += penalty[c];
                    break;
                }
            }
            if (isValid) {
                ++line;
            } else {
                line = lines.erase(line);
            }
        }
        std::cout << err << std::endl;
    }
    {  // Part 2
        std::vector<uint64_t> scores(lines.size());
        std::map<std::string::value_type, uint64_t> penalty{{'(', 1}, {'[', 2}, {'{', 3}, {'<', 4}};
        std::transform(lines.begin(), lines.end(), scores.begin(),
                       [&](const auto& line) { return calcScore(line, penalty); });
        // Median
        std::nth_element(scores.begin(), scores.begin() + scores.size() / 2, scores.end());
        std::cout << scores[scores.size() / 2] << std::endl;
    }

    return EXIT_SUCCESS;
}
