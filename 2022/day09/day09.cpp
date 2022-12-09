// Advent of Code 2022
// Day 9: Rope Bridge
// https://adventofcode.com/2022/day/9

#include <array>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <gsl/util>

using Pos = std::array<int, 2>;

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

template <typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

void moveHeadOneStep(char dir, Pos& head)
{
    if (dir == 'R') {
        ++head[0];
    } else if (dir == 'L') {
        --head[0];
    } else if (dir == 'U') {
        ++head[1];
    } else if (dir == 'D') {
        --head[1];
    }
}

void moveTail(const Pos& head, Pos& tail)
{
    constexpr size_t maxDist{1};
    const auto dh = head[0] - tail[0];
    const auto dv = head[1] - tail[1];
    if (std::abs(dh) > maxDist || std::abs(dv) > maxDist) {
        tail[0] += sgn<int>(dh);
        tail[1] += sgn<int>(dv);
    }
}

std::pair<char, size_t> parseLine(const std::string& line)
{
    std::istringstream iss{line};
    char dir;
    size_t steps;
    iss >> dir >> steps;
    return {dir, steps};
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
        Pos head{};
        Pos tail{};
        std::set<Pos> tailPoss{tail};
        for (const auto& line : lines) {
            auto [dir, steps] = parseLine(line);
            while (steps-- > 0) {
                moveHeadOneStep(dir, head);
                moveTail(head, tail);
                tailPoss.insert(tail);
            }
        }
        std::cout << tailPoss.size() << std::endl;
    }
    {  // Part 2
        constexpr size_t dim{10};
        std::array<Pos, dim> rope{};
        Pos& head = rope[0];
        Pos& tail = rope[dim - 1];
        std::set<Pos> tailPoss{tail};
        for (const auto& line : lines) {
            auto [dir, steps] = parseLine(line);
            while (steps-- > 0) {
                moveHeadOneStep(dir, head);
                for (size_t i = 0; i < dim - 1; ++i) {
                    moveTail(rope[i], rope[i + 1]);
                }
                tailPoss.insert(tail);
            }
        }
        std::cout << tailPoss.size() << std::endl;
    }

    return EXIT_SUCCESS;
}
