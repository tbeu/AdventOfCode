// Advent of Code 2024
// Day 23: LAN Party
// https://adventofcode.com/2024/day/23

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
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

using Node = std::string;
using Nodes = std::set<Node>;
using Graph = std::map<Node, Nodes>;

void addEdge(Graph& graph, const Node& a, const Node& b)
{
    graph[a].insert(b);
    graph[b].insert(a);
}

std::ostream& operator<<(std::ostream& stream, const Nodes& set)
{
    bool first = true;
    for (const auto& s : set) {
        if (!first) {
            stream << ',';
        } else {
            first = false;
        }
        stream << s;
    }
    return stream;
}

bool allUnique(const std::vector<Node>& path)
{
    const auto& last = path.back();
    for (size_t i = 0; i < path.size() - 1; ++i) {
        const auto& s = path[i];
        if (path[i] == last) {
            return false;
        }
    }
    return true;
}

bool allConnected(const Graph& g, const std::vector<Node>& path)
{
    const auto& last = path.back();
    for (size_t i = 0; i < path.size() - 1; ++i) {
        const auto& s = path[i];
        bool found = false;
        for (const auto& s : g.at(path[i])) {
            if (s == last) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

template <bool onlyFirst = false>
void find(const Graph& g, std::set<Nodes>& sets, const std::vector<Node>& path, size_t len, std::set<Nodes>& visited)
{
    if (!allUnique(path)) {
        return;
    }
    if (!allConnected(g, path)) {
        return;
    }
    const auto set = Nodes(path.begin(), path.end());
    if (visited.find(set) != visited.end()) {
        return;
    }
    visited.insert(set);
    if constexpr (onlyFirst) {
        if (sets.size() > 1) {
            return;
        }
    }
    if (path.size() == len) {
        sets.insert(set);
        return;
    }
    for (const auto& s : g.at(path.back())) {
        auto next = path;
        next.push_back(s);
        find(g, sets, next, len, visited);
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

    const auto g = [&lines] {
        Graph g;
        for (const auto& line : lines) {
            const auto pos = line.find('-');
            addEdge(g, line.substr(0, pos), line.substr(pos + 1, line.size() - pos - 1));
        }
        return g;
    }();

    {  // Part 1
        std::set<Nodes> sets;
        std::set<Nodes> visited;
        for (const auto& it : g) {
            find(g, sets, {it.first}, 3, visited);
        }
        uint64_t sum{};
        for (const auto& set : sets) {
            sum += std::any_of(set.begin(), set.end(), [](const auto& s) { return s[0] == 't'; }) ? 1 : 0;
        }
        std::cout << sum << '\n';
    }
    {  // Part 2
        size_t size = 4;
        Nodes set;
        while (true) {
            std::set<Nodes> sets;
            std::set<Nodes> visited;
            for (const auto& it : g) {
                constexpr const auto onlyFirst = true;
                find<onlyFirst>(g, sets, {it.first}, size, visited);
            }
            if (sets.empty()) {
                break;
            }
            set = *sets.begin();
            ++size;
        }
        std::cout << set << '\n';
    }

    return EXIT_SUCCESS;
}
