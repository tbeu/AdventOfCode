#include <algorithm>
#include <fstream>
#include <iostream>
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

static void increment(std::unordered_map<std::string::value_type, uint64_t>& d,
                      std::unordered_map<std::string, uint64_t>& pairs,
                      std::unordered_map<std::string, std::string>& map)
{
    std::unordered_map<std::string, uint64_t> newPairs{};
    for (const auto& [pair, count] : pairs) {
        auto& ins = map[pair];
        d[ins[0]] += count;
        newPairs[pair[0] + ins] += count;
        newPairs[ins + pair[1]] += count;
    }
    pairs.swap(newPairs);
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    if (lines.size() < 3 ||
        std::any_of(lines.cbegin() + 2, lines.cend(), [](const auto& line) { return line.size() != 7; })) {
        return EXIT_FAILURE;
    }

    std::unordered_map<std::string, std::string> map{};
    for (size_t i = 2; i < lines.size(); ++i) {
        auto src{lines[i].substr(0, 2)};
        auto dst{lines[i].substr(6, 1)};
        map.insert_or_assign(src, dst);
    }

    const std::string start{lines[0]};
    std::unordered_map<std::string, uint64_t> pairs{};
    for (size_t i = 0; i < start.size() - 1; ++i) {
        ++pairs[start.substr(i, 2)];
    }

    std::unordered_map<std::string::value_type, uint64_t> d{};
    for (size_t i = 0; i < start.size(); ++i) {
        ++d[start[i]];
    }

    {  // Part 1
        for (size_t i = 0; i < 10; ++i) {
            increment(d, pairs, map);
        }
        const auto& [min, max] =
            std::minmax_element(d.cbegin(), d.cend(), [](const auto& a, const auto& b) { return a.second < b.second; });
        std::cout << max->second - min->second << std::endl;
    }
    {  // Part 2
        for (size_t i = 10; i < 40; ++i) {
            increment(d, pairs, map);
        }
        const auto& [min, max] =
            std::minmax_element(d.cbegin(), d.cend(), [](const auto& a, const auto& b) { return a.second < b.second; });
        std::cout << max->second - min->second << std::endl;
    }

    return EXIT_SUCCESS;
}
