// Advent of Code 2023
// Day 13: Point of Incidence
// https://adventofcode.com/2023/day/13

#include <fstream>
#include <iostream>
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

template <size_t TARGETDIFF = 0>
static size_t isvMirror(const std::vector<std::string>& map)
{
    size_t c{}, diff{};
    for (c = 1; c < map[0].size(); ++c) {
        diff = 0;
        size_t mm{1}, cc{c};
        do {
            cc--;
            for (size_t rr = 0; rr < map.size(); ++rr) {
                if (map[rr][cc] != map[rr][cc + mm]) {
                    if (++diff > TARGETDIFF) {
                        break;
                    }
                }
            }
            mm += 2;
        } while (cc > 0 && cc + mm <= map[0].size() && diff <= TARGETDIFF);
        if (TARGETDIFF == diff) {
            break;
        }
    }
    if (TARGETDIFF != diff) {
        return 0;
    }
    return c;
}

template <size_t TARGETDIFF = 0>
static size_t ishMirror(const std::vector<std::string>& map)
{
    size_t r{}, diff{};
    for (r = 1; r < map.size(); ++r) {
        diff = 0;
        size_t mm{1}, rr{r};
        do {
            rr--;
            for (size_t cc = 0; cc < map[0].size(); ++cc) {
                if (map[rr][cc] != map[rr + mm][cc]) {
                    if (++diff > TARGETDIFF) {
                        break;
                    }
                }
            }
            mm += 2;
        } while (rr > 0 && rr + mm <= map.size() && diff <= TARGETDIFF);
        if (TARGETDIFF == diff) {
            break;
        }
    }
    if (TARGETDIFF != diff) {
        return 0;
    }
    return r;
}

template <size_t TARGETDIFF = 0>
static size_t pattern(size_t& s, const std::vector<std::string>& map)
{
    if (const auto r = ishMirror<TARGETDIFF>(map); r > 0) {
        s += 100 * r;
    } else {
        s += isvMirror<TARGETDIFF>(map);
    }
    return s;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (2 != argc || !readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    const auto toMaps = [&lines]() {
        std::vector<std::vector<std::string> > maps;
        std::vector<std::string> map;
        lines.push_back(std::string{});
        for (auto& line : lines) {
            if (line.empty() && !map.empty()) {
                maps.push_back(map);
                map.clear();
                continue;
            }
            map.push_back(std::move(line));
        }
        return maps;
    };
    const auto maps = toMaps();

    {
        // Part 1
        constexpr size_t targetDiff{0};
        const auto sum = std::accumulate(maps.cbegin(), maps.cend(), size_t{}, pattern<targetDiff>);
        std::cout << sum << std::endl;
    }
    {  // Part 2
        constexpr size_t targetDiff{1};
        const auto sum = std::accumulate(maps.cbegin(), maps.cend(), size_t{}, pattern<targetDiff>);
        std::cout << sum << std::endl;
    }

    return EXIT_SUCCESS;
}
