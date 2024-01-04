// Advent of Code 2023
// Day 25: Snowverload
// https://adventofcode.com/2023/day/25

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
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

using Node = std::string;
using NodeRef = std::weak_ptr<Node>;

struct Graph
{
    std::vector<std::shared_ptr<Node> > nodes{};
    std::vector<std::vector<size_t> > adj{};

    size_t index(const std::string& name) const
    {
        auto node = std::find_if(nodes.cbegin(), nodes.cend(), [&](const auto& node) { return *node == name; });
        return node - nodes.cbegin();
    }

    NodeRef get(const std::string& name) const
    {
        auto ret = NodeRef{};
        auto node = std::find_if(nodes.cbegin(), nodes.cend(), [&](const auto& node) { return *node == name; });
        if (node == nodes.cend()) {
            return ret;
        }
        ret = *node;
        return ret;
    }

    void addEdge(const std::string& i, const std::string& j)
    {
        if (get(i).expired()) {
            nodes.emplace_back(std::make_shared<Node>(i));
        }
        if (get(j).expired()) {
            nodes.emplace_back(std::make_shared<Node>(j));
        }
        const auto& ii = index(i);
        const auto& jj = index(j);
        adj.resize(nodes.size());
        adj[ii].push_back(jj);
        adj[jj].push_back(ii);
    }

    std::pair<size_t, std::vector<size_t> > minCut(size_t cutSize = 3) const
    {
        std::vector<std::vector<int16_t> > mat(nodes.size(), std::vector<int16_t>(nodes.size(), 0));
        for (size_t i = 0; i < adj.size(); ++i) {
            for (size_t j = 0; j < adj[i].size(); ++j) {
                mat[i][adj[i][j]] += 1;
            }
        }

        std::pair<size_t, std::vector<size_t> > best = {SIZE_MAX, {}};
        const auto n = nodes.size();
        std::vector<std::vector<size_t> > co(n);

        for (size_t i = 0; i < n; i++) {
            co[i] = {i};
        }

        for (size_t ph = 1; ph < n; ph++) {
            auto w = mat[0];
            size_t s = 0, t = 0;
            for (size_t it = 0; it < n - ph; it++) {
                w[t] = INT16_MIN;
                s = t;
                t = 0;
                auto max = INT16_MIN;
                for (size_t j = 0; j < n; ++j) {
                    if (w[j] > max) {
                        max = w[j];
                        t = j;
                    }
                }
                for (size_t i = 0; i < n; i++) {
                    w[i] += mat[t][i];
                }
            }
            best = std::min(best, {static_cast<size_t>(w[t] - mat[t][t]), co[t]});
            if (best.first <= cutSize) {
                return best;
            }
            co[s].insert(co[s].end(), co[t].begin(), co[t].end());
            for (size_t i = 0; i < n; i++) {
                mat[s][i] += mat[t][i];
            }
            for (size_t i = 0; i < n; i++) {
                mat[i][s] = mat[s][i];
            }
            mat[0][t] = INT16_MIN;
        }
        return best;
    }
};

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (2 != argc || !readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    {  // Part 1
        Graph g;
        for (const auto& line : lines) {
            std::string i, j;
            std::istringstream iss{line};
            iss >> i;
            i = i.substr(0, i.size() - 1);
            while (iss >> j) {
                g.addEdge(i, j);
            }
        }

        const auto& [minCutWeight, minCutNodes] = g.minCut();
        if (minCutWeight == 3) {
            std::cout << minCutNodes.size() * (g.nodes.size() - minCutNodes.size()) << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
