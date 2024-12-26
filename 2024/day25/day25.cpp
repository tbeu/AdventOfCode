// Advent of Code 2024
// Day 25: Code Chronicle
// https://adventofcode.com/2024/day/25

#include <algorithm>
#include <array>
#include <assert.h>
#include <forward_list>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <gsl/util>

static bool readFile(const std::string& fileName, std::vector<std::string>& lines)
{
    std::ifstream in{fileName};
    if (!in) {
        std::cerr << "Cannot open file " << fileName << '\n';
        return false;
    }
    auto closeStream = gsl::finally([&in] { in.close(); });
    std::string str;
    while (std::getline(in, str)) {
        lines.push_back(str);
    }
    return true;
}

constexpr const size_t columnWidth = 5;
using KeyOrLock = std::array<int16_t, columnWidth>;
using KeysOrLocks = std::forward_list<KeyOrLock>;

std::pair<KeysOrLocks, KeysOrLocks> toKeysAndLocks(const std::vector<std::string>& lines)
{
    KeysOrLocks keys;
    KeysOrLocks locks;
    std::optional<std::forward_list<KeyOrLock>::iterator> cur;
    for (const auto& line : lines) {
        if (line.empty()) {
            cur = std::nullopt;
            continue;
        }
        if (!cur.has_value()) {
            if (std::all_of(line.begin(), line.end(), [](const auto c) { return c == '#'; })) {
                locks.push_front(KeyOrLock{});
                cur = locks.begin();
            } else {
                assert(std::all_of(line.begin(), line.end(), [](const auto c) { return c == '.'; }));
                keys.push_front(KeyOrLock{});
                cur = keys.begin();
            }
            continue;
        }
        assert(line.size() == columnWidth);
        for (size_t i = 0; i < columnWidth; ++i) {
            (**cur)[i] += line[i] == '#' ? 1 : 0;
        }
    }
    return {keys, locks};
}

bool isMatch(const KeyOrLock& key, const KeyOrLock& lock)
{
    for (size_t i = 0; i < columnWidth; ++i) {
        if (6 < key[i] + lock[i]) {
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    std::vector<std::string> lines{};
    if (!readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    const auto [keys, locks] = toKeysAndLocks(lines);

    {  // Part 1
        uint64_t sum{};
        for (const auto& key : keys) {
            for (const auto& lock : locks) {
                sum += isMatch(key, lock) ? 1 : 0;
            }
        }
        std::cout << sum << '\n';
    }

    return EXIT_SUCCESS;
}
