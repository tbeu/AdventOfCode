// Advent of Code 2024
// Day 9: Disk Fragmenter
// https://adventofcode.com/2024/day/9

#include <assert.h>
#include <cstddef>
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

class Block
{
public:
    Block() = default;
    Block(uint32_t id) : _id{id}
    {
    }
    bool empty() const
    {
        return !_id.has_value();
    }
    uint32_t id() const
    {
        assert(!empty());
        return *_id;
    }

private:
    std::optional<uint32_t> _id;
};

size_t frag(std::vector<Block>& disk)
{
    size_t first = 0;
    size_t last = disk.size() - 1;
    size_t swapCount{};
    while (true) {
        while (!disk[first].empty() && first < disk.size() - 1) {
            ++first;
        }
        assert(disk[first].empty());
        while (disk[last].empty() && last > 0) {
            --last;
        }
        assert(!disk[last].empty());
        assert(first != last);
        if (first > last) {
            break;
        }
        std::swap(disk[first], disk[last]);
        ++swapCount;
    }
    return swapCount;
}

class BlockWithLength : public Block
{
public:
    BlockWithLength(uint16_t len) : Block(), len{len}
    {
    }
    BlockWithLength(uint32_t id, uint16_t len) : Block(id), len{len}
    {
    }
    uint16_t len;
};

size_t moveBlocks(std::vector<BlockWithLength>& disk)
{
    size_t swapCount{};
    size_t last = disk.size() - 1;
    while (last > 0) {
        while (disk[last].empty() && last > 0) {
            --last;
        }
        assert(!disk[last].empty());
        size_t first = 0;
        while (true) {
            while (!disk[first].empty() && first < disk.size() - 1) {
                ++first;
            }
            assert(disk[first].empty());
            assert(first != last);
            if (first > last) {
                break;
            }
            if (disk[first].len < disk[last].len) {
                ++first;
                continue;
            }
            std::swap(disk[first], disk[last]);
            if (disk[first].len < disk[last].len) {
                disk.insert(disk.begin() + first + 1,
                            BlockWithLength(static_cast<uint16_t>(disk[last].len - disk[first].len)));
                disk[++last].len = disk[first].len;
            }
            ++swapCount;
            break;
        }
        --last;
    }
    return swapCount;
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

    {  // Part 1
        std::vector<Block> disk;
        for (uint32_t i = 0; i < lines[0].size(); ++i) {
            const auto len = lines[0][i] - '0';
            if (i % 2 == 0) {
                for (size_t j = 0; j < len; ++j) {
                    disk.emplace_back(Block(i / 2));
                }
            } else {
                for (size_t j = 0; j < len; ++j) {
                    disk.emplace_back(Block());
                }
            }
        }

        frag(disk);

        uint64_t checkSum{};
        for (uint64_t pos = 0; pos < disk.size(); ++pos) {
            if (disk[pos].empty()) {
                continue;
            }
            checkSum += pos * disk[pos].id();
        }
        std::cout << checkSum << '\n';
    }
    {  // Part 2
        std::vector<BlockWithLength> disk;
        for (uint32_t i = 0; i < lines[0].size(); ++i) {
            const uint16_t len = lines[0][i] - '0';
            if (i % 2 == 0) {
                disk.emplace_back(BlockWithLength(i / 2, len));
            } else {
                disk.emplace_back(BlockWithLength(len));
            }
        }

        moveBlocks(disk);

        uint64_t sum{};
        uint64_t pos{};
        for (uint32_t i = 0; i < disk.size(); ++i) {
            if (disk[i].empty()) {
                pos += disk[i].len;
                continue;
            }
            const auto id = disk[i].id();
            for (uint64_t _ = 0; _ < disk[i].len; ++_) {
                sum += pos++ * id;
            }
        }
        std::cout << sum << '\n';
    }

    return EXIT_SUCCESS;
}
