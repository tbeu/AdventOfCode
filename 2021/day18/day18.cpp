// Advent of Code 2021
// Day 18: Snailfish
// https://adventofcode.com/2021/day/18

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
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

struct Node;
using NodeRef = std::shared_ptr<Node>;
using NodePair = std::pair<NodeRef, NodeRef>;

enum class Position
{
    LEFT,
    RIGHT,
    UNDEFINED
};

struct Node
{
    Node()
    {
    }
    Node(Position pos, const NodeRef& parent) : pos{pos}, parent{parent}
    {
    }
    std::variant<NodePair, int> p{};
    std::weak_ptr<Node> parent{};
    Position pos{Position::UNDEFINED};
};

static std::ostream& print(std::ostream& stream, const NodeRef& node, int depth = 0)
{
    if (std::holds_alternative<int>(node->p)) {
        auto& val = std::get<int>(node->p);
        if (val >= 10) {
            stream << "*" << val << "*";
        } else {
            stream << val;
        }
    } else if (std::holds_alternative<NodePair>(node->p)) {
        auto& p = std::get<NodePair>(node->p);
        if (depth > 3) {
            stream << "[~";
        } else {
            stream << "[";
        }
        print(stream, p.first, depth + 1);
        stream << ",";
        print(stream, p.second, depth + 1);
        if (depth > 3) {
            stream << "~]";
        } else {
            stream << "]";
        }
    }
    return stream;
}

static std::ostream& operator<<(std::ostream& stream, const NodeRef& node)
{
    return print(stream, node);
}

static void parse(NodeRef& node, const std::string& s, size_t& pos)
{
    while (pos < s.size()) {
        if (s[pos] == '[') {
            ++pos;
            auto left = std::make_shared<Node>(Position::LEFT, node);
            auto right = std::make_shared<Node>(Position::RIGHT, node);
            node->p = std::make_pair(left, right);
            parse(left, s, pos);
            parse(right, s, pos);
        } else if (s[pos] == ',' || s[pos] == ']') {
            ++pos;
            break;
        } else if (s[pos] >= '0' && s[pos] <= '9') {
            node->p = s[pos++] - '0';
        }
    }
}

static void incrementLeft(std::weak_ptr<Node> iter, int incr)
{
    while (!iter.expired() && iter.lock()->pos != Position::RIGHT) {
        iter = iter.lock()->parent;
    }
    if (!iter.expired()) {
        iter = iter.lock()->parent;
    }
    if (!iter.expired()) {
        if (std::holds_alternative<NodePair>(iter.lock()->p)) {
            auto& q = std::get<NodePair>(iter.lock()->p);
            iter = q.first;
        }
        while (true) {
            if (std::holds_alternative<int>(iter.lock()->p)) {
                auto& val = std::get<int>(iter.lock()->p);
                val += incr;
                break;
            } else {
                auto& q = std::get<NodePair>(iter.lock()->p);
                iter = q.second;
            }
        }
    }
}

static void incrementRight(std::weak_ptr<Node> iter, int incr)
{
    while (!iter.expired() && iter.lock()->pos != Position::LEFT) {
        iter = iter.lock()->parent;
    }
    if (!iter.expired()) {
        iter = iter.lock()->parent;
    }
    if (!iter.expired()) {
        if (std::holds_alternative<NodePair>(iter.lock()->p)) {
            auto& q = std::get<NodePair>(iter.lock()->p);
            iter = q.second;
        }
        while (true) {
            if (std::holds_alternative<int>(iter.lock()->p)) {
                auto& val = std::get<int>(iter.lock()->p);
                val += incr;
                break;
            } else {
                auto& q = std::get<NodePair>(iter.lock()->p);
                iter = q.first;
            }
        }
    }
}

static void explode(NodeRef& node, bool& isDone, int depth = 0)
{
    if (std::holds_alternative<NodePair>(node->p)) {
        auto& p = std::get<NodePair>(node->p);
        if (depth > 3 && !isDone && std::holds_alternative<int>(p.first->p) &&
            std::holds_alternative<int>(p.second->p)) {
            isDone = true;
            incrementLeft(node, std::get<int>(p.first->p));
            incrementRight(node, std::get<int>(p.second->p));
            node->p = 0;
            return;
        }
        explode(p.first, isDone, depth + 1);
        explode(p.second, isDone, depth + 1);
    }
}

static void split(NodeRef& node, bool& isDone)
{
    if (std::holds_alternative<NodePair>(node->p)) {
        auto& p = std::get<NodePair>(node->p);
        split(p.first, isDone);
        split(p.second, isDone);
    } else if (std::holds_alternative<int>(node->p)) {
        auto val = std::get<int>(node->p);
        if (val >= 10 && !isDone) {
            auto left = std::make_shared<Node>(Position::LEFT, node);
            auto right = std::make_shared<Node>(Position::RIGHT, node);
            node->p = std::make_pair(left, right);
            left->p = static_cast<int>(floor(val / 2.0));
            right->p = static_cast<int>(ceil(val / 2.0));
            isDone = true;
        }
    }
}

static NodeRef reduce(NodeRef& node, bool verbose = false)
{
    if (verbose)
        std::cout << "R: " << node << std::endl;
    while (true) {
        bool isExploded{false};
        do {
            isExploded = false;
            explode(node, isExploded);
            if (verbose)
                std::cout << "E: " << node << std::endl;
        } while (isExploded);
        bool isSplit{false};
        split(node, isSplit);
        if (!isSplit) {
            break;
        }
        if (verbose)
            std::cout << "S: " << node << std::endl;
    }
    return node;
}

static NodeRef add(NodeRef& left, NodeRef& right)
{
    auto sum = std::make_shared<Node>();
    sum->p = std::make_pair(left, right);
    left->parent = sum;
    left->pos = Position::LEFT;
    right->parent = sum;
    right->pos = Position::RIGHT;
    bool verbose{false};
    return reduce(sum, verbose);
}

static int magnitude(const NodeRef& node)
{
    if (std::holds_alternative<NodePair>(node->p)) {
        auto& p = std::get<NodePair>(node->p);
        return 3 * magnitude(p.first) + 2 * magnitude(p.second);
    } else if (std::holds_alternative<int>(node->p)) {
        return std::get<int>(node->p);
    }
    return 0;
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
        std::vector<NodeRef> numbers{};
        numbers.reserve(lines.size());
        for (const auto& line : lines) {
            size_t pos{0};
            auto root = std::make_shared<Node>();
            numbers.emplace_back(root);
            parse(numbers.back(), line, pos);
        }

        auto sum = numbers[0];
        for (size_t i = 0; i < numbers.size() - 1; ++i) {
            sum = add(sum, numbers[i + 1]);
        }
        std::cout << magnitude(sum) << std::endl;
    }
    {  // Part 2
        int maxMagnitude{0};
        for (size_t i = 0; i < lines.size(); ++i) {
            for (size_t j = 0; j < lines.size(); j++) {
                if (i == j) {
                    continue;
                }
                size_t pos{0};
                auto left = std::make_shared<Node>();
                parse(left, lines[i], pos);
                pos = 0;
                auto right = std::make_shared<Node>();
                parse(right, lines[j], pos);
                auto sum = add(left, right);
                maxMagnitude = std::max(magnitude(sum), maxMagnitude);
            }
        }
        std::cout << maxMagnitude << std::endl;
    }

    return EXIT_SUCCESS;
}
