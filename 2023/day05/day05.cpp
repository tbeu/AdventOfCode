// Advent of Code 2023
// Day 5: If You Give A Seed A Fertilizer
// https://adventofcode.com/2023/day/5

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <gsl/util>

using Range = std::array<uint64_t, 3>;
using Map = std::vector<Range>;
using Maps = std::vector<Map>;

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

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (2 != argc || !readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    Maps maps;
    {
        lines.push_back("");
        Map map;
        for (size_t i = 3; i < lines.size(); ++i) {
            if (lines[i].empty()) {
                maps.push_back(map);
                map.clear();
                i++;
                continue;
            }
            std::istringstream iss{lines[i]};
            uint64_t d, s, l;
            iss >> d >> s >> l;
            map.push_back({d, s, l});
        }
    }

    {
        // Part 1
        std::vector<uint64_t> seeds;
        std::istringstream iss{lines[0]};
        iss.ignore(6);
        uint64_t x;
        while (iss >> x) {
            seeds.push_back(x);
        }

        auto vals = seeds;
        for (const auto& map : maps) {
            for (auto& v : vals) {
                for (const auto& [d, s, l] : map) {
                    if (v >= s && v < s + l) {
                        v += d - s;
                        break;
                    }
                }
            }
        }
        const auto min = *std::min_element(vals.begin(), vals.end());
        std::cout << min << std::endl;
    }
    {  // Part 2
        std::vector<std::array<uint64_t, 2> > seeds;
        std::istringstream iss{lines[0]};
        iss.ignore(6);
        uint64_t x, y;
        while (iss >> x >> y) {
            seeds.push_back({x, y});
        }

        uint64_t min{UINT64_MAX};
        for (const auto& [vs, vl] : seeds) {
            for (uint64_t j = 0; j < vl; ++j) {
                uint64_t v = vs + j;
                std::vector<uint64_t> toSkip;
                for (size_t i = 0; i < maps.size(); ++i) {
                    uint64_t minSkip{UINT64_MAX};
                    for (const auto& [d, s, l] : maps[i]) {
                        if (v >= s) {
                            if (v < s + l) {
                                toSkip.push_back(s + l - v - 1);
                                v += d - s;
                                break;
                            }
                        } else {
                            minSkip = std::min(minSkip, s - v - 1);
                        }
                    }
                    if (minSkip < UINT64_MAX) {
                        toSkip.push_back(minSkip);
                    }
                }
                if (!toSkip.empty()) {
                    j += *std::min_element(toSkip.begin(), toSkip.end());
                }
                min = std::min(min, v);
            }
        }
        std::cout << min << std::endl;
    }

    return EXIT_SUCCESS;
}
