// Advent of Code 2021
// Day 12: Passage Pathing
// https://adventofcode.com/2021/day/12

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <queue>
#include <regex>
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

template <typename T>
struct Node;

template <typename T>
using NodeRef = std::weak_ptr<Node<T> >;

template <typename T>
struct Node
{
    T value{};
    std::vector<NodeRef<T> > adjs{};
    Node(const T& value) : value{value}
    {
    }
};

template <typename T>
struct Graph
{
    std::vector<std::shared_ptr<Node<T> > > nodes{};
    NodeRef<T> get(const T& i) const
    {
        auto ret = NodeRef<T>{};
        auto node{std::find_if(nodes.cbegin(), nodes.cend(), [&](const auto node) { return node.get()->value == i; })};
        if (node == nodes.cend()) {
            return ret;
        }
        ret = *node;
        return ret;
    }

    void addEdge(const T& i, const T& j)
    {
        auto nodeI = get(i);
        if (nodeI.expired()) {
            nodeI = nodes.emplace_back(std::make_shared<Node<T> >(i));
        }
        auto nodeJ = get(j);
        if (nodeJ.expired()) {
            nodeJ = nodes.emplace_back(std::make_shared<Node<T> >(j));
        }
        nodeI.lock()->adjs.emplace_back(nodeJ);
    }
};

using Path = std::vector<NodeRef<std::string> >;
using VisitorFunc = bool(const NodeRef<std::string>, const Path&);

static void printPath(const Path& path)
{
    for (size_t i = 0; i < path.size() - 1; ++i) {
        std::cout << path[i].lock()->value << ",";
    }
    std::cout << path.back().lock()->value << std::endl;
}

static bool isVisited(const NodeRef<std::string> node, const Path& path)
{
    return std::find_if(path.cbegin(), path.cend(), [&](const auto& pathNode) {
               return pathNode.lock()->value == node.lock()->value;
           }) != path.cend();
}

static bool isVisited1(const NodeRef<std::string> node, const Path& path)
{
    if (std::isupper(node.lock()->value[0])) {
        return false;
    }
    return isVisited(node, path);
}

static bool isVisited2(const NodeRef<std::string> node, const Path& path)
{
    if (std::isupper(node.lock()->value[0])) {
        return false;
    }
    if (!isVisited(node, path)) {
        return false;
    }
    if (node.lock()->value == "start") {
        return true;
    }
    if (node.lock()->value == "end") {
        return true;
    }

    Path sortedPath(path);
    sortedPath.erase(std::remove_if(sortedPath.begin(), sortedPath.end(),
                                    [](const auto& pathNode) { return std::isupper(pathNode.lock()->value[0]); }),
                     sortedPath.end());
    std::sort(sortedPath.begin(), sortedPath.end(),
              [](const auto& a, const auto& b) { return a.lock()->value.compare(b.lock()->value) > 0; });

    for (size_t i = 0; i < sortedPath.size() - 1; ++i) {
        if (sortedPath[i].lock()->value == sortedPath[i + 1].lock()->value) {
            return true;
        }
    }
    return false;
}

static uint64_t countPathsBFS(NodeRef<std::string> start, NodeRef<std::string> end, VisitorFunc isVisited)
{
    uint64_t count{0};
    std::queue<Path> q{};
    Path path;
    path.push_back(start);
    q.push(path);
    while (!q.empty()) {
        path = q.front();
        q.pop();
        auto last{path.back()};
        if (last.lock()->value == end.lock()->value) {
            ++count;
        }
        for (auto adj : last.lock()->adjs) {
            if (!isVisited(adj, path)) {
                Path newPath(path);
                newPath.push_back(adj);
                q.push(newPath);
            }
        }
    }
    return count;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    Graph<std::string> g{};
    {
        std::regex reg("-");
        for (auto line : lines) {
            std::sregex_token_iterator begin(line.begin(), line.end(), reg, -1);
            std::sregex_token_iterator end{};
            std::vector<std::string> nodes(begin, end);
            g.addEdge(nodes[0], nodes[1]);
            if (nodes[0] != "start" && nodes[1] != "end") {
                g.addEdge(nodes[1], nodes[0]);
            }
        }
    }

    auto start{g.get("start")};
    auto end{g.get("end")};
    std::cout << countPathsBFS(start, end, isVisited1) << std::endl;
    std::cout << countPathsBFS(start, end, isVisited2) << std::endl;

    return EXIT_SUCCESS;
}
