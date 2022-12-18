// Advent of Code 2022
// Day 16: Proboscidea Volcanium
// https://adventofcode.com/2022/day/16

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
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
using NodeRef = std::weak_ptr<Node>;

struct Node
{
    Node(const std::string& name) : name{name}
    {
    }

    uint16_t rate{};
    std::string name{};
    uint16_t index{};
    std::vector<NodeRef> adjs{};
    std::vector<uint16_t> steps{};
};

struct Graph
{
    std::vector<std::shared_ptr<Node> > nodes{};

    void setIndexes()
    {
        for (auto& node : nodes) {
            node->index = index(node->name);
        }
    }

    uint16_t index(const std::string& i) const
    {
        auto node{std::find_if(nodes.cbegin(), nodes.cend(), [&](const auto node) { return node.get()->name == i; })};
        return static_cast<uint16_t>(node - nodes.cbegin());
    }

    NodeRef get(const std::string& i) const
    {
        auto ret = NodeRef{};
        auto node{std::find_if(nodes.cbegin(), nodes.cend(), [&](const auto node) { return node.get()->name == i; })};
        if (node == nodes.cend()) {
            return ret;
        }
        ret = *node;
        return ret;
    }

    void addEdge(const std::string& i, const std::string& j)
    {
        auto nodeI = get(i);
        if (nodeI.expired()) {
            nodeI = nodes.emplace_back(std::make_shared<Node>(i));
        }
        auto nodeJ = get(j);
        if (nodeJ.expired()) {
            nodeJ = nodes.emplace_back(std::make_shared<Node>(j));
        }
        nodeI.lock()->adjs.emplace_back(nodeJ);
        nodeI.lock()->steps.emplace_back(1);
    }

    void removeEdgesToZeroRateNodes()
    {
        for (auto& nodeI : nodes) {
            if (nodeI->rate == 0) {
                continue;
            }
            const auto& nameI = nodeI->name;
            std::vector<NodeRef> newAdjsI{};
            std::vector<uint16_t> newStepsI{};
            for (size_t j = 0; j < nodeI->adjs.size(); ++j) {
                auto& nodeJ = nodeI->adjs[j];
                //const auto& nameJ = nodeJ.lock()->name;
                if (nodeJ.lock()->rate == 0) {
                    for (size_t k = 0; k < nodeJ.lock()->adjs.size(); ++k) {
                        auto& nodeK = nodeJ.lock()->adjs[k];
                        const auto& nameK = nodeK.lock()->name;
                        if (nameI == nameK) {
                            continue;
                        }
                        newAdjsI.emplace_back(nodeK);
                        newStepsI.emplace_back(nodeI->steps[j] + nodeJ.lock()->steps[k]);
                    }
                } else {
                    newAdjsI.emplace_back(nodeJ);
                    newStepsI.emplace_back(nodeI->steps[j]);
                }
            }
            nodeI->adjs = std::move(newAdjsI);
            nodeI->steps = std::move(newStepsI);
        }
    }
};

static Graph g{};

struct System
{
    System()
    {
        current = g.index("AA");
        open = std::vector<uint16_t>(g.nodes.size());
    }

    // Open state of each valve by index
    std::vector<uint16_t> open{};
    // Current valve index
    uint16_t current{};
    uint16_t step{0};

    uint16_t pressure() const
    {
        uint16_t p{0};
        for (size_t i = 0; i < open.size(); ++i) {
            if (open[i] > 0) {
                p += (30 - open[i]) * g.nodes[i]->rate;
            }
        }
        return p;
    }

    uint16_t maxExpectedPressure() const
    {
        uint16_t p{0};
        for (size_t i = 0; i < open.size(); ++i) {
            if (open[i] > 0) {
                p += (30 - open[i]) * g.nodes[i]->rate;
            } else {
                p += (29 - step) * g.nodes[i]->rate;
            }
        }
        return p;
    }

    bool allOpen() const
    {
        for (size_t i = 0; i < open.size(); ++i) {
            if (open[i] == 0 && g.nodes[i]->rate > 0) {
                return false;
            }
        }
        return true;
    }
};

struct SystemHash
{
    size_t operator()(const System& sys) const
    {
        size_t hash{sys.open.size()};
        for (size_t i = 0; i < sys.open.size(); ++i) {
            if (g.nodes[i]->rate > 0) {
                hash ^= sys.open[i] + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
        }
        hash ^= sys.current + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }
};

struct SystemEq
{
    bool operator()(System const& sys1, System const& sys2) const
    {
        return sys1.open == sys2.open && sys1.current == sys2.current;
    }
};

using Pressures = std::unordered_map<System, size_t, SystemHash, SystemEq>;

static Pressures dijkstra()
{
    g.setIndexes();
    Pressures ps{};
    uint16_t maxPressure{};
    auto cmp = [&ps](const auto& a, const auto& b) { return ps[a] < ps[b]; };
    std::priority_queue<System, std::vector<System>, decltype(cmp)> q(cmp);
    {
        System start = System();
        ps[start] = 0;
        q.push(std::move(start));
    }
    auto pushQ = [&](const System& next) {
        const auto pressure = next.pressure();
        const auto& it = ps.find(next);
        if (it == ps.cend() || pressure > it->second) {
            ps.insert_or_assign(next, pressure);
            if (pressure > maxPressure) {
                maxPressure = pressure;
                if (verbose) {
                    std::cout << maxPressure << std::endl;
                }
            }
            q.push(std::move(next));
        }
    };
    while (!q.empty()) {
        auto sys = q.top();
        q.pop();
        if (sys.step >= 30) {
            continue;
        }
        if (sys.allOpen()) {
            continue;
        }
        if (sys.maxExpectedPressure() <= maxPressure) {
            continue;
        }

        //const auto& name = g.nodes[sys.current]->name;
        if (sys.open[sys.current] == 0 && g.nodes[sys.current]->rate > 0) {
            System next(sys);
            next.step++;
            next.open[next.current] = next.step;
            pushQ(next);
        }
        for (size_t i = 0; i < g.nodes[sys.current]->adjs.size(); ++i) {
            System next(sys);
            //const auto& nextName = g.nodes[next.current]->adjs[i].lock()->name;
            next.step += g.nodes[next.current]->steps[i];
            next.current = g.nodes[next.current]->adjs[i].lock()->index;
            pushQ(next);
        }
    }
    return ps;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    for (const auto& line : lines) {
        std::istringstream iss{line.substr(6, line.size() - 6)};
        std::string first, second;
        iss >> first;
        iss.ignore(15);
        uint32_t rate;
        iss >> rate;
        iss.ignore(24);
        while (iss >> second) {
            if (second[second.size() - 1] == ',') {
                second = second.substr(0, second.size() - 1);
            }
            g.addEdge(first, second);
        }
        g.get(first).lock()->rate = rate;
    }
    g.removeEdgesToZeroRateNodes();

    {  // Part 1
        auto ps = dijkstra();
        auto max =
            std::max_element(ps.cbegin(), ps.cend(), [](const auto& a, const auto& b) { return a.second < b.second; });
        if (verbose) {
            std::ostringstream oss{};
            for (size_t i = 0; i < max->first.open.size(); ++i) {
                oss << g.nodes[i]->name << ": " << max->first.open[i] << "\n";
            }
            std::cout << oss.str() << std::endl;
        }
        std::cout << max->second << std::endl;
    }

    return EXIT_SUCCESS;
}
