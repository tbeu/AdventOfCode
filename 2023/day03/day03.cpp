// Advent of Code 2023
// Day 3: Gear Ratios
// https://adventofcode.com/2023/day/3

#include <array>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
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

constexpr char dot{'.'};
constexpr char gear{'*'};
using Pos = std::array<uint32_t, 2>;
using SymbolMap = std::map<Pos, char>;
using NumberMap = std::map<Pos, std::vector<uint32_t> >;

int main(int argc, char* argv[])
{
    std::vector<std::string> map{};
    if (2 != argc || !readFile(argv[1], map)) {
        return EXIT_FAILURE;
    }

    const Pos dim{static_cast<uint32_t>(map.size()), static_cast<uint32_t>(map[0].size())};
    map.insert(map.begin(), std::string(dim[1], dot));
    std::transform(map.begin(), map.end(), map.begin(), [](auto& row) { return dot + std::move(row) + dot; });

    SymbolMap symbols;
    for (uint32_t i = 1; i < dim[0] + 1; ++i) {
        for (uint32_t j = 1; j < dim[1] + 1; ++j) {
            if (!std::isdigit(map[i][j]) && dot != map[i][j]) {
                symbols[{i, j}] = map[i][j];
            }
        }
    }

    NumberMap numbers;
    for (uint32_t i = 1; i < dim[0] + 1; ++i) {
        std::vector<char> number;
        std::vector<Pos> adjs;
        for (uint32_t j = 1; j < dim[1] + 2; ++j) {
            if (std::isdigit(map[i][j])) {
                if (number.empty()) {
                    adjs.push_back({i - 1, j - 1});
                    adjs.push_back({i, j - 1});
                    adjs.push_back({i + 1, j - 1});
                }
                number.push_back(map[i][j]);
                adjs.push_back({i - 1, j});
                adjs.push_back({i + 1, j});
            } else {
                if (!number.empty()) {
                    adjs.push_back({i - 1, j});
                    adjs.push_back({i, j});
                    adjs.push_back({i + 1, j});
                    uint32_t n{};
                    for (const auto c : number) {
                        n *= 10;
                        n += c - '0';
                    }
                    for (const auto& adj : adjs) {
                        if (auto it = numbers.find(adj); it != numbers.end()) {
                            it->second.push_back(n);
                        } else {
                            numbers[adj] = std::vector<uint32_t>{n};
                        }
                    }
                    number.clear();
                    adjs.clear();
                }
            }
        }
    }

    {  // Part 1
        uint64_t sum{};
        for (const auto& [pos, _] : symbols) {
            if (const auto it = numbers.find(pos); it != numbers.end()) {
                sum += std::accumulate(it->second.cbegin(), it->second.cend(), uint64_t{0});
            }
        }
        std::cout << sum << std::endl;
    }
    {  // Part 2
        uint64_t sum{};
        for (const auto& [pos, symbol] : symbols) {
            if (gear != symbol) {
                continue;
            }
            if (const auto it = numbers.find(pos); it != numbers.end() && 2 == it->second.size()) {
                sum += it->second[0] * it->second[1];
            }
        }
        std::cout << sum << std::endl;
    }
    return EXIT_SUCCESS;
}
