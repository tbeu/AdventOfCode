// Advent of Code 2024
// Day 23: LAN Party
// https://adventofcode.com/2024/day/23

#include <algorithm>
#include <forward_list>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "bron-kerbosch.h"

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

void _addEdge(BronKerbosch::Graph<std::string>& g, const std::string& a, const std::string& b)
{
    if (auto it = std::find_if(g.begin(), g.end(), [&a](auto& v) { return v.id == a; }); it != g.end()) {
        it->ns.insert(b);
    } else {
        auto v = BronKerbosch::Vertex<std::string>(a);
        v.ns.insert(b);
        g.push_front(v);
    }
}

void addEdge(BronKerbosch::Graph<std::string>& g, const std::string& a, const std::string& b)
{
    _addEdge(g, a, b);
    _addEdge(g, b, a);
}

std::ostream& operator<<(std::ostream& stream, const BronKerbosch::Clique<std::string>& g)
{
    std::set<std::string> set;
    for (const auto& s : g) {
        set.insert(s.id);
    }
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

bool allUnique(const std::forward_list<Node>& path, const Node& next)
{
    return std::none_of(path.begin(), path.end(), [&next](const auto& node) { return node == next; });
}

bool allConnected(const Graph& g, const Nodes& pathNodes, const Node& next)
{
    const auto& nodes = g.at(next);
    return std::includes(nodes.begin(), nodes.end(), pathNodes.begin(), pathNodes.end());
}

void find(const Graph& g, std::set<Nodes>& sets, const std::forward_list<Node>& path, const Node& node, size_t len,
          std::set<Nodes>& visited)
{
    if (!allUnique(path, node)) {
        return;
    }
    auto set = Nodes(path.begin(), path.end());
    if (!allConnected(g, set, node)) {
        return;
    }
    set.insert(node);
    if (visited.find(set) != visited.end()) {
        return;
    }
    visited.insert(set);
    if (set.size() == len) {
        sets.insert(set);
        return;
    }
    auto nextPath = path;
    nextPath.push_front(node);
    for (const auto& next : g.at(node)) {
        find(g, sets, nextPath, next, len, visited);
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

    {  // Part 1
        const auto g = [&lines] {
            Graph g;
            for (const auto& line : lines) {
                const auto pos = line.find('-');
                addEdge(g, line.substr(0, pos), line.substr(pos + 1, line.size() - pos - 1));
            }
            return g;
        }();
        std::set<Nodes> sets;
        std::set<Nodes> visited;
        for (const auto& it : g) {
            find(g, sets, {}, it.first, 3, visited);
        }
        uint64_t sum{};
        for (const auto& set : sets) {
            sum += std::any_of(set.begin(), set.end(), [](const auto& node) { return node[0] == 't'; }) ? 1 : 0;
        }
        std::cout << sum << '\n';
    }
    {  // Part 2
        const auto g = [&lines] {
            BronKerbosch::Graph<std::string> g;
            for (const auto& line : lines) {
                const auto pos = line.find('-');
                addEdge(g, line.substr(0, pos), line.substr(pos + 1, line.size() - pos - 1));
            }
            return g;
        }();
        auto clique = BronKerbosch::Clique<std::string>{};
        BronKerbosch::solve<std::string>({{}}, g, {{}}, [&clique](auto R, const auto&, const auto&) {
            if (std::distance(R.begin(), R.end()) > std::distance(clique.begin(), clique.end())) {
                clique = R;
            }
        });
        std::cout << clique << '\n';
    }

    return EXIT_SUCCESS;
}
