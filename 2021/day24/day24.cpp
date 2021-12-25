// Advent of Code 2021
// Day 24: Arithmetic Logic Unit
// https://adventofcode.com/2021/day/24

#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
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

enum class Operation
{
    Inp,
    Add,
    Mul,
    Div,
    Mod,
    Eql
};

struct Instruction
{
    Operation op{};
    int32_t op1{};
    int32_t op2{};
    bool isExpr{false};
};

static constexpr size_t dim{14};
using Code = std::vector<Instruction>;
using Codes = std::array<Code, dim>;
using Unit = std::array<int32_t, 4>;
using Cache = std::map<std::pair<size_t, Unit>, bool>;
using Number = std::array<int32_t, dim>;

static Cache cache{};

static int32_t index(std::string::value_type c)
{
    return c - 'w';
}

static bool isExpr(std::string::value_type c)
{
    return c == 'w' || c == 'x' || c == 'y' || c == 'z';
}

static void runCode(Unit& unit, int32_t input, const Code& code)
{
    for (const auto& instr : code) {
        const int32_t op2{instr.isExpr ? unit[instr.op2] : instr.op2};
        switch (instr.op) {
            case Operation::Inp:
                unit[instr.op1] = input;
                break;
            case Operation::Add:
                unit[instr.op1] += op2;
                break;
            case Operation::Mul:
                unit[instr.op1] *= op2;
                break;
            case Operation::Div:
                unit[instr.op1] /= op2;
                break;
            case Operation::Eql:
                unit[instr.op1] = (unit[instr.op1] == op2) ? 1 : 0;
                break;
            case Operation::Mod:
                unit[instr.op1] %= op2;
                break;
            default:
                break;
        }
    }
}

static bool calc(const Unit unit, const Codes& codes, const std::array<int32_t, 9>& range, Number& number, size_t part)
{
    if (part == dim) {
        return unit[3] == 0;
    }
    auto [it, isInserted] = cache.try_emplace(std::pair(part, unit), false);
    if (!isInserted) {
        return it->second;
    }
    for (int32_t digit : range) {
        Unit u{unit};
        runCode(u, digit, codes[part]);
        if (calc(u, codes, range, number, part + 1)) {
            cache.insert_or_assign(std::pair(part, unit), true);
            number[part] = digit;
            return true;
        }
    }
    return false;
}

static Number calcLargest(const Codes& codes)
{
    Unit unit{};
    const std::array<int32_t, 9> largest{9, 8, 7, 6, 5, 4, 3, 2, 1};
    Number number{};
    cache.clear();
    calc(unit, codes, largest, number, 0);
    return number;
}

static Number calcSmallest(const Codes& codes)
{
    Unit unit{};
    const std::array<int32_t, 9> smallest{1, 2, 3, 4, 5, 6, 7, 8, 9};
    Number number{};
    cache.clear();
    calc(unit, codes, smallest, number, 0);
    return number;
}

static Codes toCodes(std::vector<std::string>& lines)
{
    lines.erase(std::remove_if(lines.begin(), lines.end(),
                               [&](const auto& line) { return line == "div z 1" || line == "add x 0"; }),
                lines.end());

    size_t part{0};
    bool isFirst{true};
    Codes codes{};
    for (const auto& line : lines) {
        std::array<std::string::value_type, 4> op{};
        std::array<std::string::value_type, 2> op1{};
        std::array<std::string::value_type, 3> op2{};
        std::sscanf(line.c_str(), "%3s %1s %3s", op.data(), op1.data(), op2.data());
        if (op[0] == 'i' && op[1] == 'n' && op[2] == 'p') {
            if (isFirst) {
                codes[0].push_back({Operation::Inp, index(op1[0]), 0, false});
                isFirst = false;
            } else {
                codes[++part].push_back({Operation::Inp, index(op1[0]), 0, false});
            }
        } else if (op[0] == 'a' && op[1] == 'd' && op[2] == 'd') {
            if (isExpr(op2[0])) {
                codes[part].push_back({Operation::Add, index(op1[0]), index(op2[0]), true});
            } else {
                codes[part].push_back({Operation::Add, index(op1[0]), std::stoi(op2.data()), false});
            }
        } else if (op[0] == 'm' && op[1] == 'u' && op[2] == 'l') {
            if (isExpr(op2[0])) {
                codes[part].push_back({Operation::Mul, index(op1[0]), index(op2[0]), true});
            } else {
                codes[part].push_back({Operation::Mul, index(op1[0]), std::stoi(op2.data()), false});
            }
        } else if (op[0] == 'd' && op[1] == 'i' && op[2] == 'v') {
            if (isExpr(op2[0])) {
                codes[part].push_back({Operation::Div, index(op1[0]), index(op2[0]), true});
            } else {
                codes[part].push_back({Operation::Div, index(op1[0]), std::stoi(op2.data()), false});
            }
        } else if (op[0] == 'e' && op[1] == 'q' && op[2] == 'l') {
            if (isExpr(op2[0])) {
                codes[part].push_back({Operation::Eql, index(op1[0]), index(op2[0]), true});
            } else {
                codes[part].push_back({Operation::Eql, index(op1[0]), std::stoi(op2.data()), false});
            }
        } else if (op[0] == 'm' && op[1] == 'o' && op[2] == 'd') {
            if (isExpr(op2[0])) {
                codes[part].push_back({Operation::Mod, index(op1[0]), index(op2[0]), true});
            } else {
                codes[part].push_back({Operation::Mod, index(op1[0]), std::stoi(op2.data()), false});
            }
        }
    }
    return codes;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    Codes codes{toCodes(lines)};
    {  // Part 1
        auto number{calcLargest(codes)};
        for (size_t i = 0; i < dim; ++i) {
            std::cout << number[i];
        }
        std::cout << std::endl;
    }
    {  // Part 2
        auto number{calcSmallest(codes)};
        for (size_t i = 0; i < dim; ++i) {
            std::cout << number[i];
        }
        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}
