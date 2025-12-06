// Advent of Code 2025
// Day 6: Trash Compactor
// https://adventofcode.com/2025/day/6

#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
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

using Sheet = std::vector<std::vector<uint64_t> >;

uint64_t grandTotal(const Sheet& sheet, const std::string& ops)
{
    uint64_t sum = 0;
    std::istringstream iss{ops};
    char op = 0;
    size_t c = 0;
    while (iss >> op) {
        if (' ' == op) {
            continue;
        }
        assert(c < sheet.size());
        const auto& col = sheet[c++];
        if ('+' == op) {
            sum += std::accumulate(col.begin(), col.end(), uint64_t{0}, std::plus<uint64_t>());
        } else {
            assert('*' == op);
            sum += std::accumulate(col.begin(), col.end(), uint64_t{1}, std::multiplies<uint64_t>());
        }
    }
    assert(c == sheet.size());
    return sum;
}

Sheet getSheet(const std::vector<std::string>& lines)
{
    Sheet sheet;
    for (const auto& line : lines) {
        std::istringstream iss{line};
        uint64_t num{};
        size_t c = 0;
        while (iss >> num) {
            if (c == sheet.size()) {
                sheet.push_back(std::vector<uint64_t>{});
            }
            assert(c < sheet.size());
            auto& col = sheet[c++];
            col.push_back(num);
        }
    }
    return sheet;
}

Sheet getSheetTransposed(const std::vector<std::string>& lines)
{
    const auto transposed = [&lines] {
        std::vector<std::string> transposed;
        for (size_t r = 0; r < lines.size(); ++r) {
            for (size_t c = 0; c < lines[r].size(); ++c) {
                if (c == transposed.size()) {
                    transposed.push_back(std::string{});
                }
                auto& row = transposed[c];
                if (' ' != lines[r][c]) {
                    row += lines[r][c];
                }
            }
        }
        return transposed;
    }();

    Sheet sheet;
    size_t c = 0;
    for (const auto& line : transposed) {
        if (line.empty()) {
            ++c;
        }
        if (c >= sheet.size()) {
            sheet.push_back(std::vector<uint64_t>{});
        }
        assert(c < sheet.size());
        auto& col = sheet[c];
        std::istringstream iss{line};
        uint64_t num{};
        if (iss >> num) {
            col.push_back(num);
        }
    }
    return sheet;
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

    std::string ops = lines.back();
    lines.pop_back();

    {  // Part 1
        const auto sheet = getSheet(lines);
        std::cout << grandTotal(sheet, ops) << '\n';
    }
    {  // Part 2
        const auto sheet = getSheetTransposed(lines);
        std::cout << grandTotal(sheet, ops) << '\n';
    }

    return EXIT_SUCCESS;
}
