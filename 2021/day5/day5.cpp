#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
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

using Point = std::pair<size_t, size_t>;
using Points = std::vector<Point>;
using Lines = std::vector<std::pair<Point, Point> >;
using PointMap = std::map<Point, size_t>;

static void addPoints(PointMap& counts, Points& points)
{
    for (const auto& point : points) {
        auto p = counts.find(point);
        if (p != counts.end()) {
            ++p->second;
        } else {
            counts.emplace(point, 1);
        }
    }
}

static size_t countPoints(const Lines& lines, bool withDiags = false)
{
    PointMap counts;
    for (const auto& line : lines) {
        const auto& x1 = line.first.first;
        const auto& y1 = line.first.second;
        const auto& x2 = line.second.first;
        const auto& y2 = line.second.second;
        Points points;
        if (x1 == x2) {
            const auto d = (y2 > y1) ? (y2 - y1) : (y1 - y2);
            points.resize(d + 1);
            std::generate(points.begin(), points.end(),
                          [x = x1, y = std::min<size_t>(y1, y2)]() mutable { return Point(x, y++); });
        } else if (y1 == y2) {
            const auto d = (x2 > x1) ? (x2 - x1) : (x1 - x2);
            points.resize(d + 1);
            std::generate(points.begin(), points.end(),
                          [x = std::min<size_t>(x1, x2), y = y1]() mutable { return Point(x++, y); });
        } else if (withDiags) {
            if (x2 > x1 && y2 > y1) {
                points.resize(x2 - x1 + 1);
                std::generate(points.begin(), points.end(), [x = x1, y = y1]() mutable { return Point(x++, y++); });
            } else if (x2 > x1) {
                points.resize(x2 - x1 + 1);
                std::generate(points.begin(), points.end(), [x = x1, y = y1]() mutable { return Point(x++, y--); });
            } else if (y2 > y1) {
                points.resize(y2 - y1 + 1);
                std::generate(points.begin(), points.end(), [x = x1, y = y1]() mutable { return Point(x--, y++); });
            } else {
                points.resize(y1 - y2 + 1);
                std::generate(points.begin(), points.end(), [x = x1, y = y1]() mutable { return Point(x--, y--); });
            }
        }
        addPoints(counts, points);
    }
    return std::count_if(counts.cbegin(), counts.cend(), [](const auto& c) { return c.second > 1; });
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines;
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    Lines segs;
    for (const auto& line : lines) {
        size_t x1, y1, x2, y2;
        std::sscanf(line.c_str(), "%u,%u -> %u,%u", &x1, &y1, &x2, &y2);
        segs.push_back({{x1, y1}, {x2, y2}});
    }

    std::cout << countPoints(segs) << std::endl;
    std::cout << countPoints(segs, true) << std::endl;
    return EXIT_SUCCESS;
}
