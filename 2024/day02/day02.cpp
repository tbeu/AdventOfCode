// Advent of Code 2024
// Day 2: Red-Nosed Reports
// https://adventofcode.com/2024/day/2

#include <algorithm>
#include <fstream>
#include <iostream>
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

bool isInc(const std::vector<int32_t>& data, size_t start, size_t end)
{
    for (size_t i = start; i < end; ++i) {
        if (data[i] < data[i - 1] + 1 || data[i] > data[i - 1] + 3) {
            return false;
        }
    }
    return true;
}

bool isInc(const std::vector<int32_t>& data)
{
    return isInc(data, 1, data.size());
}

bool isDec(const std::vector<int32_t>& data, size_t start, size_t end)
{
    for (size_t i = start; i < end; ++i) {
        if (data[i - 1] < data[i] + 1 || data[i - 1] > data[i] + 3) {
            return false;
        }
    }
    return true;
}

bool isDec(const std::vector<int32_t>& data)
{
    return isDec(data, 1, data.size());
}

bool check1(const std::vector<int32_t>& data)
{
    return isInc(data) || isDec(data);
}

bool check2(const std::vector<int32_t>& data)
{
    // skip the i-th value and check the split intervals and the two newly joined neighbours
    for (size_t i = 0; i < data.size(); ++i) {
        const auto join =
            (i > 0 && i < data.size() - 1) ? std::vector<int32_t>{data[i - 1], data[i + 1]} : std::vector<int32_t>{};
        if (isInc(data, 1, i) && isInc(join) && isInc(data, i + 2, data.size()))
            return true;
        if (isDec(data, 1, i) && isDec(join) && isDec(data, i + 2, data.size()))
            return true;
    }
    return false;
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

    const auto toReport = [](const std::string& line) {
        std::vector<int32_t> data;
        std::istringstream iss{line};
        int32_t num = 0;
        while (iss >> num) {
            data.push_back(num);
        }
        return data;
    };

    {  // Part 1
        uint64_t count{};
        for (const auto& line : lines) {
            count += check1(toReport(line)) ? 1 : 0;
        }
        std::cout << count << '\n';
    }
    {  // Part 2
        uint64_t count{};
        for (const auto& line : lines) {
            count += check2(toReport(line)) ? 1 : 0;
        }
        std::cout << count << '\n';
    }

    return EXIT_SUCCESS;
}
