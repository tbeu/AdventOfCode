// Advent of Code 2022
// Day 21: Monkey Math
// https://adventofcode.com/2022/day/21

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <variant>
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

enum class Operation : int64_t
{
    None = 0,
    Add = 1,
    Sub = 2,
    Mul = 3,
    Div = 4,
    Eq = 5
};

using Pair = std::pair<std::string, std::string>;
using OperationPair = std::pair<Pair, Operation>;
using Number = std::variant<OperationPair, int64_t>;
using Results = std::map<std::string, Number>;

int64_t calc(Results& results, std::string name)
{
    auto& num = results[name];
    if (std::holds_alternative<int64_t>(num)) {
        return std::get<int64_t>(num);
    } else {
        auto& [pair, op] = std::get<OperationPair>(num);
        int64_t res{0};
        switch (op) {
            case Operation::Add:
                res = calc(results, pair.first) + calc(results, pair.second);
                break;
            case Operation::Sub:
                res = calc(results, pair.first) - calc(results, pair.second);
                break;
            case Operation::Mul:
                res = calc(results, pair.first) * calc(results, pair.second);
                break;
            case Operation::Div:
                res = calc(results, pair.first) / calc(results, pair.second);
                break;
            default:
                break;
        }
        results[name] = res;
        return res;
    }
}

std::optional<int64_t> precalc(Results& results, std::string name)
{
    if (name == "humn") {
        return std::nullopt;
    }
    auto& num = results[name];
    if (std::holds_alternative<int64_t>(num)) {
        return std::get<int64_t>(num);
    } else {
        auto& [pair, op] = std::get<OperationPair>(num);
        auto firstRes = precalc(results, pair.first);
        if (firstRes.has_value()) {
            results[pair.first] = *firstRes;
        }
        auto secondRes = precalc(results, pair.second);
        if (secondRes.has_value()) {
            results[pair.second] = *secondRes;
        }
        if (firstRes.has_value() && secondRes.has_value()) {
            int64_t res{0};
            switch (op) {
                case Operation::Add:
                    res = *firstRes + *secondRes;
                    break;
                case Operation::Sub:
                    res = *firstRes - *secondRes;
                    break;
                case Operation::Mul:
                    res = *firstRes * *secondRes;
                    break;
                case Operation::Div:
                    res = *firstRes / *secondRes;
                    break;
                default:
                    break;
            }
            results[name] = res;
            return res;
        }
        return std::nullopt;
    }
}

int64_t check(Results& results, Pair pair, Operation op, int64_t val = 0)
{
    auto firstRes = precalc(results, pair.first);
    auto secondRes = precalc(results, pair.second);
    if (!firstRes.has_value() && secondRes.has_value()) {
        if (pair.first == "humn") {
            switch (op) {
                case Operation::Add:
                    return val - *secondRes;
                case Operation::Sub:
                    return *secondRes + val;
                case Operation::Mul:
                    return val / *secondRes;
                case Operation::Div:
                    return *secondRes * val;
            }
        }
        auto& [pair1, op1] = std::get<OperationPair>(results[pair.first]);
        switch (op) {
            case Operation::Add:
                return check(results, pair1, op1, val - *secondRes);
            case Operation::Sub:
                return check(results, pair1, op1, *secondRes + val);
            case Operation::Mul:
                return check(results, pair1, op1, val / *secondRes);
            case Operation::Div:
                return check(results, pair1, op1, *secondRes * val);
            case Operation::Eq:
                return check(results, pair1, op1, *secondRes);
        }
    }
    if (firstRes.has_value() && !secondRes.has_value()) {
        if (pair.second == "humn") {
            switch (op) {
                case Operation::Add:
                    return val - *firstRes;
                case Operation::Sub:
                    return *firstRes - val;
                case Operation::Mul:
                    return val / *firstRes;
                case Operation::Div:
                    return *firstRes / val;
            }
        }
        auto& [pair2, op2] = std::get<OperationPair>(results[pair.second]);
        switch (op) {
            case Operation::Add:
                return check(results, pair2, op2, val - *firstRes);
            case Operation::Sub:
                return check(results, pair2, op2, *firstRes - val);
            case Operation::Mul:
                return check(results, pair2, op2, val / *firstRes);
            case Operation::Div:
                return check(results, pair2, op2, *firstRes / val);
            case Operation::Eq:
                return check(results, pair2, op2, *firstRes);
        }
    }
    return val;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    Results results{};
    for (const auto& line : lines) {
        if (line.size() != 17) {
            results[line.substr(0, 4)] = std::stoi(line.substr(6, line.size() - 6));
        } else {
            auto pair = std::pair(line.substr(6, 4), line.substr(13, 4));
            auto op = Operation::None;
            switch (line[11]) {
                case '+':
                    op = Operation::Add;
                    break;
                case '-':
                    op = Operation::Sub;
                    break;
                case '*':
                    op = Operation::Mul;
                    break;
                case '/':
                    op = Operation::Div;
                    break;
                default:
                    break;
            }
            results[line.substr(0, 4)] = std::pair(pair, op);
        }
    }

    precalc(results, "root");
    {  // Part 1
        auto results1 = results;
        auto res = calc(results1, "root");
        std::cout << res << std::endl;
    }
    {  // Part 2
        Pair pair;
        std::tie(pair, std::ignore) = std::get<OperationPair>(results["root"]);
        auto humn = check(results, pair, Operation::Eq);
        std::cout << humn << std::endl;
    }

    return EXIT_SUCCESS;
}
