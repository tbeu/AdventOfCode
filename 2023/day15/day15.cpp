// Advent of Code 2023
// Day 15: Lens Library
// https://adventofcode.com/2023/day/15

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
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

struct Hash
{
    size_t operator()(const std::string_view& str) const
    {
        size_t hash{};
        size_t v{};
        for (const auto c : str) {
            if (',' == c) {
                hash += v;
                v = 0;
                continue;
            }
            v += size_t(c);
            v *= 17;
            v %= 256;
        }
        return hash + v;
    }
} hash;

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (2 != argc || !readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    {  // Part 1
        const auto sum = hash(lines[0]);
        std::cout << sum << std::endl;
    }
    {  // Part 2
        const auto walk = [](const std ::string& line) {
            using Lens = std::pair<std::string_view, size_t>;
            std::map<size_t, std::vector<Lens> > boxes;
            size_t start{};  // start position of label
            for (size_t i = 0; i < line.size(); ++i) {
                const auto c = line[i];
                if (',' == c) {
                    start = i + 1;
                } else if ('=' == c) {
                    const auto label = std::string_view(line.data() + start, i - start);
                    const auto v = hash(label);
                    const auto cmp = [&label](const auto& lens) { return lens.first == label; };
                    const auto f = line[++i] - '0';
                    if (auto box = boxes.find(v); box != boxes.end()) {
                        auto& lenses = box->second;
                        if (auto lens = std::find_if(lenses.begin(), lenses.end(), cmp); lens != lenses.end()) {
                            lens->second = f;
                        } else {
                            lenses.push_back({label, f});
                        }
                    } else {
                        boxes.insert({v, std::vector<Lens>{{label, f}}});
                    }
                } else if ('-' == c) {
                    const auto label = std::string_view(line.data() + start, i - start);
                    const auto v = hash(label);
                    const auto cmp = [&label](const auto& lens) { return lens.first == label; };
                    if (auto box = boxes.find(v); box != boxes.end()) {
                        auto& lenses = box->second;
                        if (auto lens = std::find_if(lenses.begin(), lenses.end(), cmp); lens != lenses.end()) {
                            lenses.erase(lens);
                            if (lenses.empty()) {
                                boxes.erase(box);
                            }
                        }
                    }
                }
            }
            return boxes;
        };
        const auto boxes = walk(lines[0]);
        uint64_t sum{};
        for (const auto& [v, lenses] : boxes) {
            for (size_t i = 0; i < lenses.size(); ++i) {
                sum += (v + 1) * ((i + 1) * lenses[i].second);
            }
        }
        std::cout << sum << std::endl;
    }

    return EXIT_SUCCESS;
}
