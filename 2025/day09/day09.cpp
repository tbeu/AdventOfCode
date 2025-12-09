// Advent of Code 2025
// Day 9: Movie Theater
// https://adventofcode.com/2025/day/9

#include <array>
#include <cassert>
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

using Pos = std::array<uint64_t, 2>;
using Poly = std::vector<Pos>;

bool pointOnEdge(const Pos& p, const Poly& poly)
{
    const auto n = poly.size();
    for (size_t i = 0; i < n; ++i) {
        const auto& a = poly[i];
        const auto& b = poly[(i + 1) % n];
        if (a[0] == b[0]) {
            // Vertical edge
            if (p[0] == a[0]) {
                if (p[1] >= std::min(a[1], b[1]) && p[1] <= std::max(a[1], b[1])) {
                    return true;
                }
            }
        } else {
            assert(a[1] == b[1]);
            // Horizontal edge
            if (p[1] == a[1]) {
                if (p[0] >= std::min(a[0], b[0]) && p[0] <= std::max(a[0], b[0])) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool pointInPoly(const Pos& p, const Poly& poly)
{
    const auto n = poly.size();
    uint64_t crossings = 0;
    for (size_t i = 0; i < n; ++i) {
        const auto& p1 = poly[i];
        const auto& p2 = poly[(i + 1) % n];
        if (p1[0] == p2[0]) {
            // Vertical polygon edges
            if (p[1] >= std::min(p1[1], p2[1]) && p[1] < std::max(p1[1], p2[1])) {
                crossings += p1[0] > p[0] ? 1 : 0;
            }
        }
    }

    return 1 == crossings % 2;  // odd ? inside
}

bool pointInOrOn(const Pos& p, const Poly& poly)
{
    return pointOnEdge(p, poly) || pointInPoly(p, poly);
}

bool segmentInsidePoly(const Pos& a, const Pos& b, const Poly& poly)
{
    assert(pointInOrOn(a, poly));
    assert(pointInOrOn(b, poly));

    if (a[1] == b[1]) {
        // Horizontal segment
        const auto x1 = std::min(a[0], b[0]);
        const auto x2 = std::max(a[0], b[0]);
        for (size_t i = 0; i < poly.size(); ++i) {
            const auto& p1 = poly[i];
            const auto& p2 = poly[(i + 1) % poly.size()];
            if (p1[0] == p2[0]) {
                // Vertical polygon edges
                if (a[1] >= std::min(p1[1], p2[1]) && a[1] <= std::max(p1[1], p2[1])) {
                    if (p1[0] > x1 && p1[0] < x2) {
                        return false;  // crosses polygon boundary
                    }
                }
            }
        }
        return true;
    }

    assert(a[0] == b[0]);  // Vertical segment
    const auto y1 = std::min(a[1], b[1]);
    const auto y2 = std::max(a[1], b[1]);
    for (size_t i = 0; i < poly.size(); ++i) {
        const auto& p1 = poly[i];
        const auto& p2 = poly[(i + 1) % poly.size()];
        if (p1[1] == p2[1]) {
            // Horizontal polygon edges
            if (a[0] >= std::min(p1[0], p2[0]) && a[0] <= std::max(p1[0], p2[0])) {
                if (p1[1] > y1 && p1[1] < y2) {
                    return false;  // crosses polygon boundary
                }
            }
        }
    }
    return true;
}

bool rectangleInsidePoly(const Pos& p1, const Pos& p3, const Poly& poly)
{
    assert(pointOnEdge(p1, poly));
    assert(pointOnEdge(p3, poly));

    const auto& [s1, e1] = p1;
    const auto& [s3, e3] = p3;
    const auto p2 = Pos{s1, e3};
    if (!pointInOrOn(p2, poly)) {
        return false;
    }
    const auto p4 = Pos{s3, e1};
    if (!pointInOrOn(p4, poly)) {
        return false;
    }
    if (!segmentInsidePoly(p1, p2, poly)) {
        return false;
    }
    if (!segmentInsidePoly(p2, p3, poly)) {
        return false;
    }
    if (!segmentInsidePoly(p3, p4, poly)) {
        return false;
    }
    if (!segmentInsidePoly(p4, p1, poly)) {
        return false;
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

    Poly tiles;
    for (const auto& line : lines) {
        std::istringstream iss{line};
        uint64_t x = 0;
        uint64_t y = 0;
        iss >> x;
        iss.ignore(1);
        iss >> y;
        tiles.push_back({x, y});
    }

    uint64_t maxArea1 = 0;
    uint64_t maxArea2 = 0;
    for (size_t i = 0; i < tiles.size(); ++i) {
        const auto& [si, ei] = tiles[i];
        for (size_t j = i + 1; j < tiles.size(); ++j) {
            const auto& [sj, ej] = tiles[j];
            const uint64_t d1 = si >= sj ? si - sj : sj - si;
            const uint64_t d2 = ei >= ej ? ei - ej : ej - ei;
            const uint64_t area = (d1 + 1) * (d2 + 1);
            maxArea1 = std::max(maxArea1, area);
            if (!rectangleInsidePoly(tiles[i], tiles[j], tiles)) {
                continue;
            }
            maxArea2 = std::max(maxArea2, area);
        }
    }

    {  // Part 1
        std::cout << maxArea1 << '\n';
    }
    {  // Part 2
        std::cout << maxArea2 << '\n';
    }

    return EXIT_SUCCESS;
}
