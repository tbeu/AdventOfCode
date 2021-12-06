#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
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

static uint64_t count(const std::vector<uint16_t>& numbers, size_t end)
{
    std::array<uint64_t, 9> d{};
    for (const auto& num : numbers) {
        ++d[num];
    }
    uint64_t len{numbers.size()};
    for (size_t i = 0; i < end; ++i) {
        auto z{d.front()};
        std::rotate(d.begin(), d.begin() + 1, d.end());
        d[6] += z;
        len += z;
    }
    return len;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines;
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    std::vector<uint16_t> numbers{};
    {
        std::istringstream iss(lines[0]);
        uint16_t num;
        while (iss >> num) {
            numbers.push_back(num);
            iss.ignore(1);
        }
    }

    std::cout << count(numbers, 80) << std::endl;
    std::cout << count(numbers, 256) << std::endl;

    return EXIT_SUCCESS;
}
