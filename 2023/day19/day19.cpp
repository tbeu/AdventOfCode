// Advent of Code 2023
// Day 19: Aplenty
// https://adventofcode.com/2023/day/19

#define _CRT_SECURE_NO_WARNINGS

#include <assert.h>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <variant>
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

using RuleName = std::string;
using Index = uint8_t;
using Target = std::variant<std::monostate, RuleName, bool>;

struct Part
{
    Part(uint16_t x, uint16_t m, uint16_t a, uint16_t s) : items{std::array<uint16_t, 4>{x, m, a, s}}
    {
    }
    uint64_t sum() const
    {
        return items[0] + items[1] + items[2] + items[3];
    }

    std::array<uint16_t, 4> items;
};

enum class RuleType
{
    Undefined,
    Accept,
    Reject,
    Next,
    Less,
    Greater
};

struct Rule
{
    virtual Target eval(const Part& part) const
    {
        return Target();
    }

    RuleType type{RuleType::Undefined};
};

struct Accept : Rule
{
    Accept()
    {
        type = RuleType::Accept;
    }
    Target eval(const Part& part) const override
    {
        return true;
    }
};

struct Reject : Rule
{
    Reject()
    {
        type = RuleType::Reject;
    }
    Target eval(const Part& part) const override
    {
        return false;
    }
};

struct Next : Rule
{
    Next(RuleName target) : target{target}
    {
        assert(!target.empty());
        type = RuleType::Next;
    }
    Target eval(const Part& part) const override
    {
        return target;
    }

    RuleName target;
};

struct Less : Rule
{
    Less(Index index, uint16_t amt, Target target) : index{index}, amt{amt}, target{target}
    {
        type = RuleType::Less;
    }
    Target eval(const Part& part) const override
    {
        if (part.items[index] < amt) {
            return target;
        }
        return Target();
    }

    Index index;
    uint16_t amt;
    Target target;
};

struct Greater : Rule
{
    Greater(Index index, uint16_t amt, Target target) : index{index}, amt{amt}, target{target}
    {
        type = RuleType::Greater;
    }
    Target eval(const Part& part) const override
    {
        if (part.items[index] > amt) {
            return target;
        }
        return Target();
    }

    Index index;
    uint16_t amt;
    Target target;
};

using Parts = std::vector<Part>;
using Rules = std::vector<std::shared_ptr<Rule> >;
using RuleMap = std::map<RuleName, Rules>;
using Range = std::pair<uint16_t, uint16_t>;

struct RangePart
{
    RangePart(Range x, Range m, Range a, Range s) : items{std::array<Range, 4>{x, m, a, s}}
    {
    }
    RangePart(const RangePart& other, Range range, Index index)
    {
        items = other.items;
        items[index] = range;
    }
    uint64_t count() const
    {
        uint64_t prod{1};
        for (const auto& [s, e] : items) {
            prod *= (e - s);
        }
        return prod;
    }
    bool isValid() const
    {
        for (const auto& [s, e] : items) {
            if (s >= e) {
                return false;
            }
        }
        return true;
    }

    std::array<Range, 4> items;
};

static std::pair<Parts, RuleMap> toPartsRules(const std::vector<std::string>& lines)
{
    std::vector<Part> parts;
    RuleMap ruleMap;
    bool isParts{false};
    for (const auto& line : lines) {
        if (line.empty()) {
            isParts = true;
            continue;
        }
        if (isParts) {
            uint16_t x, m, a, s;
            std::sscanf(line.c_str(), "{x=%hu,m=%hu,a=%hu,s=%hu}", &x, &m, &a, &s);
            parts.emplace_back(Part{x, m, a, s});
        } else {
            RuleName name;
            std::vector<std::shared_ptr<Rule> > conds;
            bool isCond{false};
            Index index;
            for (size_t i = 0; i < line.size(); ++i) {
                if ('{' == line[i]) {
                    name = line.substr(0, i);
                    isCond = true;
                    continue;
                }
                if (!isCond) {
                    continue;
                }
                if (const auto isOp = i < line.size() - 1 && !std::isalpha(line[i + 1]); isOp) {
                    if ('x' == line[i]) {
                        index = 0;
                        i++;
                    } else if ('m' == line[i]) {
                        index = 1;
                        i++;
                    } else if ('a' == line[i]) {
                        index = 2;
                        i++;
                    } else if ('s' == line[i]) {
                        index = 3;
                        i++;
                    }
                }
                const auto isLess = '<' == line[i];
                const auto isGreater = '>' == line[i];
                uint16_t amt{};
                if (isLess || isGreater) {
                    while (line[++i] != ':') {
                        amt = 10 * amt + line[i] - '0';
                    }
                    i++;
                }
                RuleName target;
                size_t start = i;
                while (i < line.size() && std::isalpha(line[i])) {
                    i++;
                }
                target = line.substr(start, i - start);
                const auto isA = target == "A";
                const auto isR = target == "R";
                if (isLess) {
                    if (isA) {
                        conds.emplace_back(std::make_shared<Less>(index, amt, true));
                    } else if (isR) {
                        conds.emplace_back(std::make_shared<Less>(index, amt, false));
                    } else {
                        conds.emplace_back(std::make_shared<Less>(index, amt, target));
                    }
                } else if (isGreater) {
                    if (isA) {
                        conds.emplace_back(std::make_shared<Greater>(index, amt, true));
                    } else if (isR) {
                        conds.emplace_back(std::make_shared<Greater>(index, amt, false));
                    } else {
                        conds.emplace_back(std::make_shared<Greater>(index, amt, target));
                    }
                } else if (isA) {
                    conds.emplace_back(std::make_shared<Accept>());
                } else if (isR) {
                    conds.emplace_back(std::make_shared<Reject>());
                } else {
                    conds.emplace_back(std::make_shared<Next>(target));
                }
                assert(',' == line[i] || '}' == line[i]);
            }
            ruleMap[name] = std::move(conds);
        }
    }
    return {parts, ruleMap};
}

static bool validate(const RuleName& ruleName, const RuleMap& ruleMap, const Part& part, size_t ruleIndex = 0)
{
    const auto& rules = ruleMap.at(ruleName);
    assert(ruleIndex < rules.size());
    const auto result = rules[ruleIndex]->eval(part);
    if (std::holds_alternative<RuleName>(result)) {
        return validate(std::get<RuleName>(result), ruleMap, part);
    }
    if (std::holds_alternative<bool>(result)) {
        return std::get<bool>(result);
    }
    return validate(ruleName, ruleMap, part, ruleIndex + 1);
}

static uint64_t count(const RuleName& ruleName, const RuleMap& ruleMap, const RangePart& part, size_t ruleIndex = 0)
{
    if (!part.isValid()) {
        return 0;
    }
    const auto& rules = ruleMap.at(ruleName);
    assert(ruleIndex < rules.size());
    const auto& baseRule = rules[ruleIndex];
    if (RuleType::Accept == baseRule->type) {
        return part.count();
    }
    if (RuleType::Reject == baseRule->type) {
        return 0;
    }
    if (RuleType::Next == baseRule->type) {
        const auto rule = std::dynamic_pointer_cast<Next>(baseRule);
        return count(rule->target, ruleMap, part);
    }
    if (RuleType::Less == baseRule->type) {
        const auto rule = std::dynamic_pointer_cast<Less>(baseRule);
        const auto os = part.items[rule->index].first;
        const auto oe = std::min<uint16_t>(rule->amt, part.items[rule->index].second);
        const auto& result = rule->target;
        const auto& oPart = RangePart(part, {os, oe}, rule->index);                              // overlap range
        const auto& rPart = RangePart(part, {oe, part.items[rule->index].second}, rule->index);  // right range
        const auto rSum = count(ruleName, ruleMap, rPart, ruleIndex + 1);
        if (std::holds_alternative<RuleName>(result)) {
            return count(std::get<RuleName>(result), ruleMap, oPart) + rSum;
        } else if (std::holds_alternative<bool>(result)) {
            return (std::get<bool>(result) ? oPart.count() : 0) + rSum;
        }
        assert(false);
        return 0;
    }
    if (RuleType::Greater == baseRule->type) {
        const auto rule = std::dynamic_pointer_cast<Greater>(baseRule);
        const auto os = std::max<uint16_t>(rule->amt + 1, part.items[rule->index].first);
        const auto oe = part.items[rule->index].second;
        const auto& result = rule->target;
        const auto& oPart = RangePart(part, {os, oe}, rule->index);                             // overlap range
        const auto& lPart = RangePart(part, {part.items[rule->index].first, os}, rule->index);  // left range
        const auto lSum = count(ruleName, ruleMap, lPart, ruleIndex + 1);
        if (std::holds_alternative<RuleName>(result)) {
            return count(std::get<RuleName>(result), ruleMap, oPart) + lSum;
        } else if (std::holds_alternative<bool>(result)) {
            return (std::get<bool>(result) ? oPart.count() : 0) + lSum;
        }
        assert(false);
        return 0;
    }
    assert(false);
    return 0;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (2 != argc || !readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    const auto& [parts, ruleMap] = toPartsRules(lines);

    {  // Part 1
        uint64_t sum{};
        for (const auto& part : parts) {
            if (validate(RuleName{"in"}, ruleMap, part)) {
                sum += part.sum();
            }
        }
        std::cout << sum << std::endl;
    }
    {  // Part 2
        constexpr auto range = Range{1, 4001};
        const auto sum = count(RuleName{"in"}, ruleMap, RangePart{range, range, range, range});
        std::cout << sum << std::endl;
    }

    return EXIT_SUCCESS;
}
