// Advent of Code 2023
// Day 15: Lens Library
// https://adventofcode.com/2023/day/15

#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
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
        for (const auto c : str) {
            hash += size_t(c);
            hash *= 17;
            hash %= 256;
        }
        return hash;
    }
} hash;

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (2 != argc || !readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    const auto& line = lines[0];

    {  // Part 1: HASH
        uint64_t sum{};
        size_t start{};  // start position of string
        for (size_t i = 0; i < line.size(); ++i) {
            const auto c = line[i];
            if (',' == c) {
                sum += hash(std::string_view(line.data() + start, i - start));
                start = i + 1;
            }
        }
        sum += hash(std::string_view(line.data() + start, line.size() - start));
        std::cout << sum << std::endl;
    }
    {  // Part 2: HASHMAP
        const auto walk = [](const std::string& line) {
            std::unordered_map<std::string_view, size_t, Hash> boxes;
            size_t start{};  // start position of label
            for (size_t i = 0; i < line.size(); ++i) {
                const auto c = line[i];
                if (',' == c) {
                    start = i + 1;
                    continue;
                }
                const auto label = std::string_view(line.data() + start, i - start);
                if ('=' == c) {
                    boxes[label] = line[++i] - '0';
                } else if ('-' == c) {
                    boxes.erase(label);
                }
            }
            return boxes;
        };
        const auto boxes = walk(line);
        uint64_t sum{};
        for (size_t i = 0; i < boxes.bucket_count(); ++i) {
            auto s = boxes.bucket_size(i);
            for (auto it = boxes.cbegin(i); it != boxes.cend(i); ++it) {
                const auto& [label, f] = *it;
                sum += (hash(label) + 1) * (s-- * f);
            }
        }
        std::cout << sum << std::endl;
    }

    return EXIT_SUCCESS;
}
