// Advent of Code 2024
// Day 3: Mull It Over
// https://adventofcode.com/2024/day/3

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <optional>
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

std::optional<uint32_t> getFactor(const std::string& line, size_t& i, char end)
{
    uint32_t num{};
    constexpr const size_t maxDigits{3};
    const auto imax = i + maxDigits;
    while (i < imax) {
        if (std::isdigit(line[i]) != 0) {
            num *= 10;
            num += line[i++] - '0';
            continue;
        }
        break;
    }
    return line[i] == end ? num : std::optional<uint32_t>();
}

template <bool checkEnabled = true>
uint64_t count(const std::string& line, bool& isEnabled)
{
    if constexpr (!checkEnabled) {
        isEnabled = true;
    }
    uint64_t sum{};
    size_t i{};
    while (i < line.size() - std::strlen("mul(x,y)")) {
        if constexpr (checkEnabled) {
            if (line[i] == 'd' && line[i + 1] == 'o') {
                i += 2;
                if (line[i] == '(' && line[i + 1] == ')') {
                    i += 2;
                    isEnabled = true;
                } else if (line[i] == 'n' && line[i + 1] == '\'' && line[i + 2] == 't' && line[i + 3] == '(' &&
                           line[i + 4] == ')') {
                    i += 5;
                    isEnabled = false;
                }
            }
        }
        if (isEnabled && line[i] == 'm' && line[i + 1] == 'u' && line[i + 2] == 'l' && line[i + 3] == '(') {
            i += 4;
            const auto first = getFactor(line, i, ',');
            if (!first.has_value()) {
                continue;
            }
            i++;
            const auto second = getFactor(line, i, ')');
            if (!second.has_value()) {
                continue;
            }
            sum += static_cast<uint64_t>(first.value() * second.value());
        }
        i++;
    }
    return sum;
}

uint64_t count(const std::string& line)
{
    auto isEnabled{true};
    constexpr const auto checkEnabled{false};
    return count<checkEnabled>(line, isEnabled);
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
        uint64_t sum{};
        for (const auto& line : lines) {
            sum += count(line);
        }
        std::cout << sum << '\n';
    }
    {  // Part 2
        uint64_t sum{};
        auto isEnabled{true};
        for (const auto& line : lines) {
            sum += count(line, isEnabled);
        }
        std::cout << sum << '\n';
    }

    return EXIT_SUCCESS;
}
