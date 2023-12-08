// Advent of Code 2023
// Day 8: Haunted Wasteland
// https://adventofcode.com/2023/day/8

#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
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

using Map = std::map<std::string, std::pair<std::string, std::string> >;

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (2 != argc || !readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    Map map;
    for (size_t i = 2; i < lines.size(); ++i) {
        auto node = lines[i].substr(0, 3);
        auto l = lines[i].substr(7, 3);
        auto r = lines[i].substr(12, 3);
        map[node] = std::pair(l, r);
    }

    {  // Part 1
        std::string node = "AAA";
        uint64_t count{};
        do {
            const auto dir = lines[0][count % lines[0].size()];
            if ('L' == dir) {
                node = map.at(node).first;
            } else {
                node = map.at(node).second;
            }
            count++;
        } while (node != "ZZZ");
        std::cout << count << std::endl;
    }
    {  // Part 2
        std::vector<std::string> nodes;
        std::vector<std::string> endNodes;
        for (const auto& [node, _] : map) {
            if (node[2] == 'A') {
                nodes.push_back(node);
            }
            if (node[2] == 'Z') {
                endNodes.push_back(node);
            }
        }
        auto cycles = std::vector<uint64_t>(nodes.size(), 0);
        uint64_t count{};
        do {
            const auto dir = lines[0][count % lines[0].size()];
            for (size_t i = 0; i < nodes.size(); ++i) {
                if (cycles[i] > 0) {
                    continue;
                }
                auto& node = nodes[i];
                if ('L' == dir) {
                    node = map.at(node).first;
                } else {
                    node = map.at(node).second;
                }
                if (std::find(endNodes.begin(), endNodes.end(), node) != endNodes.end()) {
                    cycles[i] = count + 1;
                }
            }
            count++;
        } while (!std::all_of(cycles.begin(), cycles.end(), [](auto c) { return c > 0; }));

        count = 1;
        for (size_t i = 0; i < cycles.size(); ++i) {
            count = std::lcm(count, cycles[i]);
        }
        std::cout << count << std::endl;
    }

    return EXIT_SUCCESS;
}
