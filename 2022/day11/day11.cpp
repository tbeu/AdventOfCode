// Advent of Code 2022
// Day 11: Monkey in the Middle
// https://adventofcode.com/2022/day/11

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
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

enum class Operation : int32_t
{
    None = 0,
    Add = 1,
    Mul = 2,
    Sqr = 3,
};

struct Monkey
{
    std::queue<uint64_t> items{};
    uint64_t divisor{1};
    std::pair<uint16_t, uint16_t> dst{};
    Operation op{Operation::None};
    uint64_t update{0};
    uint64_t count{};

    uint64_t& updateItem()
    {
        auto& item = items.front();
        if (op == Operation::Add) {
            item += update;
        } else if (op == Operation::Mul) {
            item *= update;
        } else if (op == Operation::Sqr) {
            item *= item;
        }
        return item;
    }

    bool inspectItem1()
    {
        if (items.empty()) {
            return false;
        }
        ++count;
        auto& item = updateItem();
        item /= 3;
        return true;
    }

    bool inspectItem2(uint64_t div)
    {
        if (items.empty()) {
            return false;
        }
        ++count;
        auto& item = updateItem();
        item %= div;
        return true;
    }

    std::pair<uint16_t, uint64_t> throwItem()
    {
        auto item = items.front();
        items.pop();
        if (item % divisor == 0) {
            return {dst.first, item};
        }
        return {dst.second, item};
    }
};

using Monkeys = std::vector<Monkey>;

Monkeys toMonkeys(const std::vector<std::string>& lines)
{
    Monkeys monkeys{};
    monkeys.reserve((lines.size() + 1) / 7);
    for (const auto& line : lines) {
        if (line.empty()) {
            continue;
        }
        if (line.substr(0, 7) == "Monkey ") {
            Monkey monkey{};
            monkeys.emplace_back(monkey);
        } else if (line.substr(0, 18) == "  Starting items: ") {
            std::istringstream iss{line.substr(18, line.size() - 18)};
            int32_t item;
            while (iss >> item) {
                monkeys.back().items.push(item);
                iss.ignore(1);
            }
        } else if (line.substr(0, 21) == "  Test: divisible by ") {
            std::istringstream iss{line.substr(21, line.size() - 21)};
            iss >> monkeys.back().divisor;
        } else if (line.substr(0, 23) == "  Operation: new = old ") {
            std::istringstream iss{line.substr(23, line.size() - 23)};
            std::string op;
            std::string update;
            iss >> op >> update;
            if (op == "*" && update == "old") {
                monkeys.back().op = Operation::Sqr;
            } else if (op == "+") {
                monkeys.back().op = Operation::Add;
                monkeys.back().update = std::stoull(update);
            } else if (op == "*") {
                monkeys.back().op = Operation::Mul;
                monkeys.back().update = std::stoull(update);
            }
        } else if (line.substr(0, 29) == "    If true: throw to monkey ") {
            std::istringstream iss{line.substr(29, line.size() - 29)};
            iss >> monkeys.back().dst.first;
        } else if (line.substr(0, 30) == "    If false: throw to monkey ") {
            std::istringstream iss{line.substr(30, line.size() - 30)};
            iss >> monkeys.back().dst.second;
        }
    }
    return monkeys;
}

uint64_t businessValue(const Monkeys& monkeys)
{
    std::vector<uint64_t> inspects{};
    inspects.reserve(monkeys.size());
    std::transform(monkeys.cbegin(), monkeys.cend(), std::back_inserter(inspects),
                   [](const auto& monkey) { return monkey.count; });
    std::nth_element(inspects.begin(), inspects.begin() + 1, inspects.end(), std::greater{});
    return inspects[0] * inspects[1];
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    {
        // Part 1
        Monkeys monkeys{toMonkeys(lines)};
        assert(monkeys.size() > 1);
        size_t round{};
        while (round++ < 20) {
            for (auto& monkey : monkeys) {
                while (monkey.inspectItem1()) {
                    const auto& [dst, item] = monkey.throwItem();
                    monkeys[dst].items.push(item);
                }
            }
        }
        std::cout << businessValue(monkeys) << std::endl;
    }
    {  // Part 2
        Monkeys monkeys{toMonkeys(lines)};
        assert(monkeys.size() > 1);
        const auto div = std::accumulate(monkeys.cbegin(), monkeys.cend(), uint64_t{1},
                                         [](auto& p, const auto& monkey) { return p * monkey.divisor; });
        size_t round{};
        while (round++ < 10000) {
            for (auto& monkey : monkeys) {
                while (monkey.inspectItem2(div)) {
                    const auto& [dst, item] = monkey.throwItem();
                    monkeys[dst].items.push(item);
                }
            }
        }
        std::cout << businessValue(monkeys) << std::endl;
    }

    return EXIT_SUCCESS;
}
