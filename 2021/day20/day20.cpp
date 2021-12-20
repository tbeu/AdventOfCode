// Advent of Code 2021
// Day 20: Trench Map
// https://adventofcode.com/2021/day/20

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include <gsl/util>

static bool readFile(const std::string& fileName, std::vector<std::string>& lines)
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

constexpr bool verbose{true};
using Image = std::vector<std::string>;
constexpr std::string::value_type dark{'.'};
constexpr std::string::value_type light{'#'};

static std::ostream& operator<<(std::ostream& stream, const Image& image)
{
    std::for_each(image.cbegin(), image.cend(), [&](const auto& row) { stream << row << '\n'; });
    return stream;
}

static void shrink(Image& image)
{
    image.erase(image.begin());
    image.pop_back();
    std::transform(image.begin(), image.end(), image.begin(), [](auto& row) { return row.substr(1, row.size() - 2); });
}

static void enlarge(Image& image, std::string::value_type c)
{
    const std::string border(image[0].size(), c);
    image.insert(image.begin(), border);
    image.emplace_back(border);
    std::transform(image.begin(), image.end(), image.begin(), [c](auto& row) { return c + std::move(row) + c; });
}

static Image enhance(Image& image, const std::string& map, std::string::value_type c)
{
    enlarge(image, c);
    enlarge(image, c);
    Image copy{image};
    size_t dim{image[0].size()};
    for (size_t i = 1; i < dim - 1; ++i) {
        for (size_t j = 1; j < dim - 1; ++j) {
            size_t index{0};
            // clang-format off
            if (image[i - 1][j - 1] == light) index += 256;
            if (image[i - 1][j    ] == light) index += 128;
            if (image[i - 1][j + 1] == light) index += 64;
            if (image[i    ][j - 1] == light) index += 32;
            if (image[i    ][j    ] == light) index += 16;
            if (image[i    ][j + 1] == light) index += 8;
            if (image[i + 1][j - 1] == light) index += 4;
            if (image[i + 1][j    ] == light) index += 2;
            if (image[i + 1][j + 1] == light) index += 1;
            // clang-format on
            copy[i][j] = map[index];
        }
    }
    shrink(copy);
    return copy;
}

static size_t enhance(Image& image, const std::string& map, size_t end)
{
    for (size_t i = 0; i < end; ++i) {
        std::string::value_type c{dark};
        if (i % 2 == 1 && map[0] == light && map[map.size() - 1] == dark) {
            c = light;
        }
        image = enhance(image, map, c);
        if (verbose)
            std::cout << image << std::endl;
    }
    return std::accumulate(image.begin(), image.end(), size_t{0}, [](auto& a, auto& row) {
        return a + std::count_if(row.begin(), row.end(), [](const auto c) { return c == light; });
    });
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    Image image{};
    image.reserve(lines.size() - 2);
    for (size_t i = 2; i < lines.size(); ++i) {
        if (lines[i].empty()) {
            continue;
        }
        image.push_back(lines[i]);
    }
    if (image.empty()) {
        return EXIT_FAILURE;
    }
    const auto& map{lines[0]};
    if (verbose)
        std::cout << image << std::endl;
    auto count1{enhance(image, map, size_t{2})};
    auto count2{enhance(image, map, size_t{48})};
    std::cout << count1 << '\n' << count2 << std::endl;

    return EXIT_SUCCESS;
}
