// Advent of Code 2021
// Day 13: Transparent Origami
// https://adventofcode.com/2021/day/13

#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <set>
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

using Coord = std::pair<long, long>;
using Coords = std::vector<Coord>;

static std::tuple<Coords, Coords> get(const std::vector<std::string>& lines)
{
    Coords coords{};
    auto it{lines.cbegin()};
    while (it != lines.cend()) {
        if (it->empty()) {
            break;
        }
        std::istringstream iss(*it);
        long x{0}, y{0};
        iss >> x;
        iss.ignore(1);
        iss >> y;
        coords.emplace_back(x, y);
        ++it;
    }
    ++it;
    Coords instrs{};
    std::regex reg("=");
    while (it != lines.cend()) {
        std::sregex_token_iterator begin(it->begin(), it->end(), reg, -1);
        std::sregex_token_iterator end{};
        std::vector<std::string> temp(begin, end);
        std::istringstream iss(temp[1]);
        long val{0};
        iss >> val;
        if (temp[0] == "fold along x") {
            instrs.emplace_back(val, 0);
        } else if (temp[0] == "fold along y") {
            instrs.emplace_back(0, val);
        }
        ++it;
    }
    if (instrs.empty()) {
        instrs.emplace_back(0, 0);
    }
    return {coords, instrs};
}

static void fold(Coords& coords, const Coord& instr)
{
    if (instr.first > 0) {
        std::for_each(coords.begin(), coords.end(), [&](auto& coord) {
            if (auto dx{coord.first - instr.first}; dx > 0) {
                coord.first = instr.first - dx;
            }
        });
    } else if (instr.second > 0) {
        std::for_each(coords.begin(), coords.end(), [&](auto& coord) {
            if (auto dy{coord.second - instr.second}; dy > 0) {
                coord.second = instr.second - dy;
            }
        });
    }
}

static void print(const std::set<Coord>& coords)
{
    auto dimx{std::max_element(coords.cbegin(), coords.cend(), [](const auto& a, const auto& b) {
                  return a.first < b.first;
              })->first};
    auto dimy{std::max_element(coords.cbegin(), coords.cend(), [](const auto& a, const auto& b) {
                  return a.second < b.second;
              })->second};
    for (long y = 0; y <= dimy; ++y) {
        for (long x = 0; x <= dimx; ++x) {
            if (coords.find({x, y}) != coords.end()) {
                std::cout << "#";
            } else {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    auto [coords, instrs] = get(lines);

    {  // Part 1
        fold(coords, instrs[0]);
        std::set<Coord> folded(coords.cbegin(), coords.cend());
        std::cout << folded.size() << std::endl;
    }
    {  // Part 2
        std::for_each(instrs.cbegin(), instrs.cend(), [&](const auto& instr) { fold(coords, instr); });
        std::set<Coord> folded(coords.cbegin(), coords.cend());
        print(folded);
    }

    return EXIT_SUCCESS;
}
