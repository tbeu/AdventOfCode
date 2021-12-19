// Advent of Code 2021
// Day 19: Beacon Scanner
// https://adventofcode.com/2021/day/19

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
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

constexpr bool verbose{false};
constexpr int minIntersectionSize{12};
using Point = std::array<int, 3>;
using Points = std::vector<Point>;
using Scanners = std::vector<Points>;
using Matrix = std::array<Point, 3>;
using Configurations = std::array<Matrix, 24>;

constexpr Matrix c01{{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};
constexpr Matrix c02{{{0, 0, -1}, {0, -1, 0}, {-1, 0, 0}}};
constexpr Matrix c03{{{-1, 0, 0}, {0, -1, 0}, {0, 0, 1}}};
constexpr Matrix c04{{{0, 0, -1}, {0, 1, 0}, {1, 0, 0}}};
constexpr Matrix c05{{{-1, 0, 0}, {0, 1, 0}, {0, 0, -1}}};
constexpr Matrix c06{{{0, 0, 1}, {0, -1, 0}, {1, 0, 0}}};
constexpr Matrix c07{{{1, 0, 0}, {0, -1, 0}, {0, 0, -1}}};
constexpr Matrix c08{{{0, 0, 1}, {0, 1, 0}, {-1, 0, 0}}};
constexpr Matrix c09{{{0, 1, 0}, {0, 0, 1}, {1, 0, 0}}};
constexpr Matrix c10{{{-1, 0, 0}, {0, 0, -1}, {0, -1, 0}}};
constexpr Matrix c11{{{0, -1, 0}, {0, 0, -1}, {1, 0, 0}}};
constexpr Matrix c12{{{-1, 0, 0}, {0, 0, 1}, {0, 1, 0}}};
constexpr Matrix c13{{{0, -1, 0}, {0, 0, 1}, {-1, 0, 0}}};
constexpr Matrix c14{{{1, 0, 0}, {0, 0, -1}, {0, 1, 0}}};
constexpr Matrix c15{{{0, 1, 0}, {0, 0, -1}, {-1, 0, 0}}};
constexpr Matrix c16{{{1, 0, 0}, {0, 0, 1}, {0, -1, 0}}};
constexpr Matrix c17{{{0, 0, 1}, {1, 0, 0}, {0, 1, 0}}};
constexpr Matrix c18{{{0, -1, 0}, {-1, 0, 0}, {0, 0, -1}}};
constexpr Matrix c19{{{0, 0, -1}, {-1, 0, 0}, {0, 1, 0}}};
constexpr Matrix c20{{{0, -1, 0}, {1, 0, 0}, {0, 0, 1}}};
constexpr Matrix c21{{{0, 0, -1}, {1, 0, 0}, {0, -1, 0}}};
constexpr Matrix c22{{{0, 1, 0}, {-1, 0, 0}, {0, 0, 1}}};
constexpr Matrix c23{{{0, 0, 1}, {-1, 0, 0}, {0, -1, 0}}};
constexpr Matrix c24{{{0, 1, 0}, {1, 0, 0}, {0, 0, -1}}};

constexpr Configurations configs{c01, c02, c03, c04, c05, c06, c07, c08, c09, c10, c11, c12,
                                 c13, c14, c15, c16, c17, c18, c19, c20, c21, c22, c23, c24};

inline static Point matVecMul(const Matrix& m, const Point& v)
{
    return Point{m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2], m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2],
                 m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2]};
}

static std::ostream& operator<<(std::ostream& stream, const Point& v)
{
    stream << v[0] << "," << v[1] << "," << v[2];
    return stream;
}

static std::ostream& operator<<(std::ostream& stream, const Points& s)
{
    for (const auto& v : s) {
        stream << v << '\n';
    }
    return stream;
}

inline static Points rotateScanner(const Points& s, const Matrix& m)
{
    Points res{};
    res.reserve(s.size());
    std::transform(s.begin(), s.end(), std::back_inserter(res), [&](auto& v) { return matVecMul(m, v); });
    return res;
}

inline static Points shiftScanner(const Points& s, const Point& p)
{
    Points res{};
    res.reserve(s.size());
    std::transform(s.begin(), s.end(), std::back_inserter(res), [&](auto& v) {
        return Point{v[0] + p[0], v[1] + p[1], v[2] + p[2]};
    });
    return res;
}

inline static Point diff(const Point& v1, const Point& v2)
{
    return Point{v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]};
}

inline static int distance(const Point& v)
{
    return std::abs(v[0]) + std::abs(v[1]) + std::abs(v[2]);
}

static bool intersection(const Points& s1, const Points& s2)
{
    std::set<Point> set1(s1.begin(), s1.end());
    std::set<Point> set2(s2.begin(), s2.end());
    Points res{};
    std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(res));
    return res.size() >= minIntersectionSize;
}

static std::tuple<Points, Point> checkScanners(const Points& s1, const Points& s2)
{
    const std::tuple<Points, Point> noMatch{};
    for (const auto& c : configs) {
        auto s2Rotated{rotateScanner(s2, c)};
        for (size_t i = 0; i <= s1.size() - minIntersectionSize; ++i) {
            for (size_t j = 0; j <= s2Rotated.size() - minIntersectionSize; ++j) {
                auto d{diff(s1[i], s2Rotated[j])};
                if (d[0] * d[0] + d[1] * d[1] + d[2] * d[2] > 3 * 4000 * 4000) {
                    // Optimization: Return early if the two points are too distant
                    return noMatch;
                }
                auto s2Transformed{shiftScanner(s2Rotated, d)};
                if (intersection(s2Transformed, s1)) {
                    return {s2Transformed, d};
                }
            }
        }
    }
    return noMatch;
}

static Scanners toScanners(const std::vector<std::string>& lines)
{
    Scanners scanners{};
    for (auto it = lines.begin(); it != lines.end(); ++it) {
        if (it->empty()) {
            continue;
        }
        if (it->at(1) == '-') {
            scanners.push_back(Points{});
            continue;
        }
        const auto p1{it->find(',')};
        const auto p2{it->rfind(',')};
        const auto x{it->substr(0, p1)};
        const auto y{it->substr(p1 + 1, p2 - p1 - 1)};
        const auto z{it->substr(p2 + 1, it->size() - p2)};
        Point p{std::stoi(x), std::stoi(y), std::stoi(z)};
        scanners.back().emplace_back(p);
    }
    return scanners;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    Scanners scanners{toScanners(lines)};
    if (scanners.empty()) {
        return EXIT_FAILURE;
    }

    constexpr size_t origin{0};
    Points beacons{scanners[origin]};
    std::vector<std::vector<bool> > visited{scanners.size(), std::vector<bool>(scanners.size())};
    std::vector<Point> distances(scanners.size());
    std::queue<size_t> q{};
    q.push(origin);
    while (!q.empty()) {
        auto i = q.front();
        q.pop();
        visited[i][i] = true;
        for (size_t j = 0; j < scanners.size(); ++j) {
            if (visited[i][j] || visited[j][j]) {
                continue;
            }
            if (verbose)
                std::cout << i << "," << j;
            visited[i][j] = visited[j][i] = true;
            if (auto [transformed, dist] = checkScanners(scanners[i], scanners[j]); !transformed.empty()) {
                if (verbose)
                    std::cout << " -> " << dist;
                distances[j] = dist;
                scanners[j] = transformed;  // Scanner j has now same orientation and origin as scanner i
                beacons.insert(beacons.end(), transformed.begin(), transformed.end());
                visited[j][j] = true;
                q.push(j);
            }
            if (verbose)
                std::cout << std::endl;
        }
    }

    {  // Part 1
        std::set<Point> beaconsUnique(beacons.begin(), beacons.end());
        std::cout << beaconsUnique.size() << std::endl;
    }
    {  // Part 2
        int maxDistance{0};
        for (size_t i = 0; i < distances.size(); ++i) {
            for (size_t j = i + 1; j < distances.size(); ++j) {
                maxDistance = std::max<int>(maxDistance, distance(diff(distances[i], distances[j])));
            }
        }
        std::cout << maxDistance << std::endl;
    }

    return EXIT_SUCCESS;
}
