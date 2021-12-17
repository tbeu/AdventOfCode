// Advent of Code 2021
// Day 17: Trick Shot
// https ://adventofcode.com/2021/day/17

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
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

using Point = std::pair<int, int>;
using Velocity = std::pair<int, int>;

template <typename T>
static int sgn(T val)
{
    return (T{0} < val) - (val < T{0});
}

static void increment(Point& point, Velocity& velocity)
{
    auto& [x, y] = point;
    auto& [vx, vy] = velocity;
    x += vx;
    y += vy;
    vx -= sgn<int>(vx);
    vy -= 1;
}

inline static bool isHit(const Point& point, const Point& xtarget, const Point& ytarget)
{
    auto& [x, y] = point;
    auto& [xmin, xmax] = xtarget;
    auto& [ymin, ymax] = ytarget;
    return x >= xmin && x <= xmax && y >= ymin && y <= ymax;
}

static std::ostream& operator<<(std::ostream& stream, const Velocity& velocity)
{
    auto& [vx, vy] = velocity;
    stream << vx << ", " << vy;
    return stream;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    if (lines.size() != 1) {
        return EXIT_FAILURE;
    }

    auto p1{lines[0].find('=')};
    auto p2{lines[0].find('.')};
    auto p3{lines[0].find(',')};
    auto p4{lines[0].rfind('=')};
    auto p5{lines[0].rfind('.')};

    Point xtarget{std::stoi(lines[0].substr(p1 + 1, p2 - p1 - 1)), std::stoi(lines[0].substr(p2 + 2, p3 - p2 - 2))};
    Point ytarget{std::stoi(lines[0].substr(p4 + 1, p5 - p4 - 2)),
                  std::stoi(lines[0].substr(p5 + 1, lines[0].size() - p5))};

    int vxmin{1};
    for (int n = 1; n <= xtarget.second; ++n) {
        if (n * (n + 1) >= 2 * xtarget.first) {
            vxmin = n;
            break;
        }
    }

    int ymaxGlobal{0};
    size_t count{0};
    const int vymin{-std::max<int>(std::abs(ytarget.first), std::abs(ytarget.second))};
    for (int vx = vxmin; vx <= xtarget.second; ++vx) {
        for (int vy = vymin; vy <= -vymin; ++vy) {
            int ymax{0};
            Point point{};
            //Velocity v0{vx, vy};
            Velocity v{vx, vy};
            auto& [x, y] = point;
            while (x <= xtarget.second && y >= ytarget.first) {
                increment(point, v);
                ymax = std::max<int>(ymax, point.second);
                if (isHit(point, xtarget, ytarget)) {
                    count++;
                    ymaxGlobal = std::max<int>(ymax, ymaxGlobal);
                    //std::cout << v0 << std::endl;
                    break;
                }
            }
        }
    }
    std::cout << ymaxGlobal << std::endl;
    std::cout << count << std::endl;

    return EXIT_SUCCESS;
}
