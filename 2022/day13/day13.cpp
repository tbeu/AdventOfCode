// Advent of Code 2022
// Day 13: Distress Signal
// https://adventofcode.com/2022/day/13

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <optional>
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

constexpr bool verbose{false};
struct Node;
using NodeRef = std::shared_ptr<Node>;
using NodeRefs = std::vector<NodeRef>;

struct Node
{
    Node()
    {
    }
    Node(const std::string& s)
    {
        size_t pos{0};
        // Skip first "[" and last "]" for root
        parse(s.substr(1, s.size() - 2), pos);
    }
    NodeRefs nodes{};
    std::optional<int> v{std::nullopt};
    friend std::ostream& operator<<(std::ostream& stream, const NodeRef& node);

private:
    void parse(const std::string& s, size_t& pos)
    {
        while (pos < s.size()) {
            if (s[pos] == '[') {
                ++pos;
                auto next = std::make_shared<Node>();
                nodes.push_back(next);
                next->parse(s, pos);
            } else if (s[pos] == ']') {
                ++pos;
                break;
            } else if (s[pos] == ',') {
                ++pos;
            } else {
                auto next = std::make_shared<Node>();
                auto start = pos;
                while (s[pos] >= '0' && s[pos] <= '9') {
                    pos++;
                }
                next->v = std::stoi(s.substr(start, pos - start));
                nodes.push_back(next);
            }
        }
    }

    std::ostream& print(std::ostream& stream, int depth = 0)
    {
        if (v.has_value()) {
            stream << *v;
        } else {
            stream << "[";
            if (nodes.size() > 1) {
                for (size_t i = 0; i < nodes.size() - 1; ++i) {
                    nodes[i]->print(stream, depth + 1);
                    stream << ",";
                }
            }
            if (!nodes.empty()) {
                nodes.back()->print(stream, depth + 1);
            }
            stream << "]";
        }
        return stream;
    }
};

std::ostream& operator<<(std::ostream& stream, const NodeRef& node)
{
    return node->print(stream);
}

std::optional<bool> compare(const NodeRef& first, const NodeRef& second, int depth = 0)
{
    auto _ = gsl::finally([&depth] {
        if (verbose && depth == 0) {
            std::cout << std::endl;
        }
    });

    if (verbose) {
        std::cout << std::string(2 * depth, ' ') << "- Compare " << first << " vs " << second << std::endl;
    }
    if (first->v.has_value() && second->v.has_value()) {
        if (*first->v < *second->v) {
            if (verbose) {
                std::cout << std::string(2 * depth + 2, ' ') << "- Left side is smaller -> true" << std::endl;
            }
            return true;
        }
        if (*first->v > *second->v) {
            if (verbose) {
                std::cout << std::string(2 * depth + 2, ' ') << "- Right side is smaller -> false" << std::endl;
            }
            return false;
        }
    } else if (!first->v.has_value() && !second->v.has_value()) {
        for (size_t i = 0; i < first->nodes.size(); ++i) {
            if (i < second->nodes.size()) {
                if (auto cmp = compare(first->nodes[i], second->nodes[i], depth + 1); cmp.has_value()) {
                    return *cmp;
                }
            }
            if (i >= second->nodes.size()) {
                if (verbose) {
                    std::cout << std::string(2 * depth + 2, ' ') << "- Right side ran out of items -> false"
                              << std::endl;
                }
                return false;
            }
        }
        if (first->nodes.size() < second->nodes.size()) {
            if (verbose) {
                std::cout << std::string(2 * depth + 2, ' ') << "- Left side ran out of items -> true" << std::endl;
            }
            return true;
        }
    } else {
        if (verbose) {
            std::cout << std::string(2 * depth + 2, ' ') << "- Mixed types" << std::endl;
        }
        auto tmp = std::make_shared<Node>();
        auto next = std::make_shared<Node>();
        tmp->nodes.push_back(next);
        if (first->v.has_value()) {
            next->v = *first->v;
            if (auto cmp = compare(tmp, second, depth + 1); cmp.has_value()) {
                return *cmp;
            }
        } else {
            next->v = *second->v;
            if (auto cmp = compare(first, tmp, depth + 1); cmp.has_value()) {
                return *cmp;
            }
        }
    }
    return std::nullopt;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    std::vector<NodeRef> list{};
    for (const auto& line : lines) {
        if (!line.empty()) {
            list.emplace_back(std::make_shared<Node>(line));
        }
    }

    {  // Part 1
        size_t count{};
        for (size_t i = 0; i < list.size(); i += 2) {
            auto cmp = compare(list[i], list[i + 1]);
            assert(cmp.has_value());
            if (*cmp) {
                count += i / 2 + 1;
            }
        }
        std::cout << count << std::endl;
    }
    {  // Part 2
        size_t first{};
        size_t second{1};
        const auto node2 = std::make_shared<Node>("[[2]]");
        const auto node6 = std::make_shared<Node>("[[6]]");
        for (const auto& node : list) {
            if (auto cmp = compare(node, node2); cmp.has_value() && *cmp) {
                ++first;
            }
            if (auto cmp = compare(node, node6); cmp.has_value() && *cmp) {
                ++second;
            }
        }
        std::cout << (first + 1) * (second + 1) << std::endl;
    }

    return EXIT_SUCCESS;
}
