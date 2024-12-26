// Advent of Code 2024
// Day 24: Crossed Wires
// https://adventofcode.com/2024/day/24

#include <algorithm>
#include <array>
#include <assert.h>
#include <bitset>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <tuple>
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

enum class GateType
{
    AND,
    OR,
    XOR,
};

struct Gate
{
    std::string name;
    std::optional<bool> state;
};

using Graph = std::map<std::string, std::tuple<std::string, std::string, GateType> >;
using Gates = std::map<std::string, Gate>;
constexpr const size_t bitSize = 45;
using B = std::bitset<bitSize>;

void reinit(Gates& gates, const B& x, const B& y)
{
    for (auto& [name, gate] : gates) {
        if (name[0] == 'x') {
            gate.state = x[(name[1] - '0') * 10 + name[2] - '0'];
        } else if (name[0] == 'y') {
            gate.state = y[(name[1] - '0') * 10 + name[2] - '0'];
        } else {
            gate.state = std::nullopt;
        }
    }
}

std::optional<bool> get(Gates& gates, Gate& gate, const Graph& g, std::set<std::string>& visited)
{
    if (gate.state.has_value()) {
        return gate.state;
    }
    if (visited.find(gate.name) != visited.end()) {
        return std::nullopt;
    }
    visited.insert(gate.name);
    const auto& [in1, in2, type] = g.at(gate.name);
    const auto v1 = get(gates, gates.at(in1), g, visited);
    if (!v1.has_value()) {
        return std::nullopt;
    }
    const auto v2 = get(gates, gates.at(in2), g, visited);
    if (!v2.has_value()) {
        return std::nullopt;
    }
    switch (type) {
        case GateType::AND:
            gate.state = *v1 && *v2;
            break;
        case GateType::OR:
            gate.state = *v1 || *v2;
            break;
        case GateType::XOR:
            gate.state = *v1 != *v2;
            break;
    }
    return gate.state;
}

std::optional<uint64_t> add(Gates& gates, const B& x, const B& y, const Graph& g, std::set<std::string>& visited,
                            size_t bit = bitSize)
{
    reinit(gates, x, y);
    uint64_t z = 0;
    for (int32_t i = 0; i < bit + 1; ++i) {
        std::array<char, 4> name{};
        std::snprintf(name.data(), 4, "z%02d", i);
        if (const auto v = get(gates, gates.at(name.data()), g, visited); v.has_value()) {
            z += *v ? 1ULL << i : 0;
        } else {
            return std::nullopt;
        }
    }
    return z;
}

std::optional<uint64_t> add(Gates& gates, const B& x, const B& y, const Graph& g, size_t bit = bitSize)
{
    std::set<std::string> visited;
    return add(gates, x, y, g, visited, bit);
}

using Swaps = std::set<std::string>;

std::ostream& operator<<(std::ostream& stream, const Swaps& swaps)
{
    bool first = true;
    for (const auto& s : swaps) {
        if (!first) {
            stream << ',';
        } else {
            first = false;
        }
        stream << s;
    }
    return stream;
}

size_t checkHighestValidBit(Gates& gates, const Graph& g, std::set<std::string>& visited)
{
    // check how many bits of the adder circuit are valid
    size_t bit = 0;
    while (bit < bitSize) {
        B x;
        B y;
        x[bit] = true;
        auto z = add(gates, x, x, g, bit + 1);
        if (!z.has_value() || *z != 2 * x.to_ullong()) {
            break;
        }
        y[0] = true;
        for (size_t j = 0; j < bit; ++j) {
            x[j] = true;
        }
        std::set<std::string> visitedBit;
        z = add(gates, x, y, g, visitedBit, bit + 1);
        if (!z.has_value() || *z != x.to_ullong() + y.to_ullong()) {
            break;
        }
        ++bit;
        visited.insert(visitedBit.begin(), visitedBit.end());
    }
    return bit;
}

size_t checkHighestValidBit(Gates& gates, const Graph& g)
{
    std::set<std::string> visited;
    return checkHighestValidBit(gates, g, visited);
}

Swaps findSwaps(Gates& gates, Graph& g)
{
    Swaps swaps;
    std::set<std::string> goods;
    size_t step = 0;
    while (step++ < 4) {
        auto bit = checkHighestValidBit(gates, g, goods);
        std::string swapa;
        std::string swapb;
        for (const auto& [a, ta] : g) {
            const auto founda = goods.find(a) != goods.end();
            for (const auto& [b, tb] : g) {
                if (a >= b) {
                    continue;
                }
                if (founda && goods.find(b) != goods.end()) {
                    continue;
                }
                std::swap(g[a], g[b]);
                if (const auto next = checkHighestValidBit(gates, g); next > bit) {
                    swapa = a;
                    swapb = b;
                    bit = next;
                }
                std::swap(g[a], g[b]);
            }
            if (bit == bitSize) {
                break;
            }
        }
        assert(!swapa.empty());
        assert(!swapb.empty());
        std::swap(g[swapa], g[swapb]);
        swaps.insert({swapa, swapb});
        goods.insert({swapa, swapb});
    }
    return swaps;
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

    Gates gates;
    Graph g;
    B x;
    B y;

    bool isInit = true;
    for (const auto& line : lines) {
        if (line.empty()) {
            isInit = false;
            continue;
        }
        if (isInit) {
            if (line[0] == 'x') {
                x[(line[1] - '0') * 10 + line[2] - '0'] = static_cast<bool>(line[5] - '0');
            } else if (line[0] == 'y') {
                y[(line[1] - '0') * 10 + line[2] - '0'] = static_cast<bool>(line[5] - '0');
            }
            gates[line.substr(0, 3)] = Gate{line.substr(0, 3)};
        } else {
            std::string in1;
            std::string in2;
            std::string out;
            if (line.substr(4, 3) == "AND") {
                in1 = line.substr(0, 3);
                in2 = line.substr(8, 3);
                out = line.substr(15, 3);
                g[out] = std::make_tuple(in1, in2, GateType::AND);
            } else if (line.substr(4, 3) == "XOR") {
                in1 = line.substr(0, 3);
                in2 = line.substr(8, 3);
                out = line.substr(15, 3);
                g[out] = std::make_tuple(in1, in2, GateType::XOR);
            } else {
                in1 = line.substr(0, 3);
                in2 = line.substr(7, 3);
                out = line.substr(14, 3);
                g[out] = std::make_tuple(in1, in2, GateType::OR);
            }
            for (const auto& name : {in1, in2, out}) {
                if (gates.find(name) == gates.end()) {
                    gates[name] = Gate{name};
                }
            }
        }
    }

    {  // Part 1
        const auto z = add(gates, x, y, g);
        assert(z.has_value());
        std::cout << *z << '\n';
    }
    {  // Part 2
        const auto swaps = findSwaps(gates, g);
        assert(swaps.size() == 8);
        std::cout << swaps << '\n';
    }

    return EXIT_SUCCESS;
}
