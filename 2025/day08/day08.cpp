// Advent of Code 2025
// Day 8: Playground
// https://adventofcode.com/2025/day/8

#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <sstream>
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

using Pos = std::array<int32_t, 3>;
struct Node
{
    Pos pos;
    size_t id;
};
struct Pair
{
    Node first;
    Node second;
    uint64_t d2;
};
using Circuit = std::set<size_t>;
using Circuits = std::vector<Circuit>;

struct Compare
{
    bool operator()(const Pair& a, const Pair& b) const
    {
        return a.d2 > b.d2;
    }
};
using Q = std::priority_queue<Pair, std::vector<Pair>, Compare>;

std::vector<Pos> parse(const std::vector<std::string>& lines)
{
    std::vector<Pos> poss;
    for (const auto& line : lines) {
        std::istringstream iss{line};
        int32_t x = 0;
        int32_t y = 0;
        int32_t z = 0;
        iss >> x;
        iss.ignore(1);
        iss >> y;
        iss.ignore(1);
        iss >> z;
        poss.push_back({x, y, z});
    }
    return poss;
}

Q getPairs(const std::vector<Pos>& poss)
{
    auto distance2 = [](const Pos& a, const Pos& b) -> uint64_t {
        const int64_t d0 = a[0] - b[0];
        const int64_t d1 = a[1] - b[1];
        const int64_t d2 = a[2] - b[2];
        return d0 * d0 + d1 * d1 + d2 * d2;
    };

    Q pairs;
    for (size_t i = 0; i < poss.size(); ++i) {
        for (size_t j = i + 1; j < poss.size(); ++j) {
            pairs.push({{poss[i], i}, {poss[j], j}, distance2(poss[i], poss[j])});
        }
    }
    return pairs;
}

void connect(Circuits& circuits, const Pair& pair)
{
    const auto& [a, b, _] = pair;
    std::vector<size_t> found{};
    for (size_t j = 0; j < circuits.size(); ++j) {
        const auto& circuit = circuits[j];
        if (circuit.find(a.id) != circuit.end() || circuit.find(b.id) != circuit.end()) {
            found.push_back(j);
        }
    }
    if (found.empty()) {
        circuits.push_back(Circuit{a.id, b.id});
    } else if (1 == found.size()) {
        auto& circuit = circuits[found.front()];
        circuit.insert(a.id);
        circuit.insert(b.id);
    } else {
        assert(2 == found.size());
        auto& circuit = circuits[found.front()];
        const auto& other = circuits[found.back()];
        circuit.insert(other.begin(), other.end());
        circuit.insert(a.id);
        circuit.insert(b.id);
        circuits.erase(circuits.begin() + found.back());
    }
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

    auto pairs = getPairs(parse(lines));
    Circuits circuits;

    {  // Part 1
        size_t step = 0;
        while (step++ < 1000) {
            assert(!pairs.empty());
            connect(circuits, pairs.top());
            pairs.pop();
        }
        std::nth_element(circuits.begin(), circuits.begin() + 2, circuits.end(),
                         [](const auto& a, const auto& b) { return a.size() > b.size(); });
        std::cout << circuits[0].size() * circuits[1].size() * circuits[2].size() << '\n';
    }
    {  // Part 2
        Pair pair{};
        while (circuits.size() != 1 || circuits.front().size() != lines.size()) {
            assert(!pairs.empty());
            pair = pairs.top();
            connect(circuits, pair);
            pairs.pop();
        }
        std::cout << pair.first.pos[0] * pair.second.pos[0] << '\n';
    }

    return EXIT_SUCCESS;
}
