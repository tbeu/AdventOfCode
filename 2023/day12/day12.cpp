// Advent of Code 2023
// Day 12: Hot Springs
// https://adventofcode.com/2023/day/12

#include <assert.h>
#include <fstream>
#include <iostream>
#include <map>
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

using Group = std::vector<uint32_t>;
using Puzzle = std::pair<std::string, Group>;
using Puzzles = std::vector<Puzzle>;

template <size_t FOLD = 1, bool WITH_CACHE = true>
static size_t count(const std::string& p, const Group& g, size_t pos = 0, size_t k = 0,
                    std::shared_ptr<std::map<std::pair<size_t, size_t>, size_t> > cache = nullptr)
{
    if constexpr (WITH_CACHE) {
        assert(nullptr != cache);
        if (const auto it = cache->find({pos, k}); it != cache->end()) {
            return it->second;
        }
    }
    if (pos == p.size() && k == FOLD * g.size()) {
        return 1;
    }
    if (pos >= p.size()) {
        return 0;
    }
    if ('#' == p[pos]) {
        if (k >= FOLD * g.size() || pos + g[k % g.size()] >= p.size()) {
            return 0;
        }
        for (size_t c = 1; c < g[k % g.size()]; ++c) {
            if ('.' == p[pos + c]) {
                return 0;
            }
        }
        pos += g[k % g.size()];
        k++;
        if ('#' == p[pos]) {
            return 0;
        }
        const auto s = count<FOLD, WITH_CACHE>(p, g, pos + 1, k, cache);
        if constexpr (WITH_CACHE) {
            cache->insert({{pos + 1, k}, s});
        }
        return s;
    }
    if ('.' == p[pos]) {
        return count<FOLD, WITH_CACHE>(p, g, pos + 1, k, cache);
    }
    const std::string p1 = p.substr(0, pos) + '#' + p.substr(pos + 1, p.size() - pos - 1);
    const std::string p2 = p.substr(0, pos) + '.' + p.substr(pos + 1, p.size() - pos - 1);
    return count<FOLD, WITH_CACHE>(p1, g, pos, k, cache) + count<FOLD, WITH_CACHE>(p2, g, pos, k, cache);
}

static inline size_t count1(const std::string& p, const Group& g)
{
    return count<1, false>(p, g);
}

static inline size_t count5(const std::string& p, const Group& g)
{
    return count<5>(p, g, 0, 0, std::make_shared<std::map<std::pair<size_t, size_t>, size_t> >());
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (2 != argc || !readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    {  // Part 1
        Puzzles puzzles;
        for (const auto& line : lines) {
            uint32_t x;
            std::string p;
            std::istringstream iss{line};
            iss >> p;
            p += '.';
            Group g;
            while (iss >> x) {
                g.push_back(x);
                iss.ignore(1);
            }
            puzzles.push_back({p, g});
        }

        size_t sum{};
        for (const auto& [p, g] : puzzles) {
            sum += count1(p, g);
        }
        std::cout << sum << std::endl;
    }
    {  // Part 2
        Puzzles puzzles;
        for (const auto& line : lines) {
            uint32_t x;
            std::string p;
            std::istringstream iss{line};
            iss >> p;
            p = p + '?' + p + '?' + p + '?' + p + '?' + p + '.';
            Group g;
            while (iss >> x) {
                g.push_back(x);
                iss.ignore(1);
            }
            puzzles.push_back({p, g});
        }

        size_t sum{};
        for (const auto& [p, g] : puzzles) {
            sum += count5(p, g);
        }
        std::cout << sum << std::endl;
    }

    return EXIT_SUCCESS;
}
