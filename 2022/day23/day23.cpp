// Advent of Code 2022
// Day 23: Unstable Diffusion
// https://adventofcode.com/2022/day/23

#include "../../MinGL/MinGL.cpp"
#include "../../MinGL/glad/glad.c"

#if defined(_WIN64)
#pragma comment(lib, "glfw3_64")
#else
#pragma comment(lib, "glfw3")
#endif

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
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

enum class Move : int32_t
{
    None = 0,
    North = 1,
    South = 2,
    West = 3,
    East = 4
};

constexpr char elve{'#'};
using Pos = std::array<int32_t, 2>;

struct PosHash
{
    size_t operator()(const Pos& pos) const
    {
        return pos[0] * 10000 + pos[1];
    }
};

using Map = std::unordered_map<Pos, Move, PosHash>;

constexpr bool writePpmFiles{false};
constexpr MinGLColor darkgreen{0.f, 130 / 255.f, 0.f, 1.f};
constexpr MinGLColor bg{0.66f, 0.66f, 0.66f, 1.f};
constexpr unsigned px{2};

bool isFree(const Map& map, const Pos& pos)
{
    if (const auto it = map.find(pos); it == map.cend()) {
        return true;
    }
    return false;
}

bool move(const Map& in, Map& out, size_t round)
{
    std::vector<Map::iterator> blocked{};
    std::vector<Pos> none{};
    auto tryInsert = [&](const Pos& pos, Move move) {
        if (auto [it, inserted] = out.try_emplace(pos, move); !inserted) {
            blocked.push_back(it);
            const auto [i, j] = pos;
            if (move == Move::North) {
                none.push_back({i + 1, j});
            } else if (move == Move::South) {
                none.push_back({i - 1, j});
            } else if (move == Move::West) {
                none.push_back({i, j + 1});
            } else if (move == Move::East) {
                none.push_back({i, j - 1});
            }
        }
    };
    for (const auto& [pos, val] : in) {
        const auto [i, j] = pos;
        auto nw = isFree(in, {i - 1, j - 1});
        auto n = isFree(in, {i - 1, j});
        auto ne = isFree(in, {i - 1, j + 1});
        auto e = isFree(in, {i, j + 1});
        auto se = isFree(in, {i + 1, j + 1});
        auto s = isFree(in, {i + 1, j});
        auto sw = isFree(in, {i + 1, j - 1});
        auto w = isFree(in, {i, j - 1});
        if (nw && n && ne && e && se && s && sw && w) {
            none.push_back(pos);
            continue;
        }
        bool moved{false};
        for (size_t count = 0; count < 4; ++count) {
            const auto check = (count + round) % 4;
            if (check == 0 && nw && n && ne) {  // N
                tryInsert({i - 1, j}, Move::North);
                moved = true;
                break;
            }
            if (check == 1 && se && s && sw) {  // S
                tryInsert({i + 1, j}, Move::South);
                moved = true;
                break;
            }
            if (check == 2 && nw && w && sw) {  // W
                tryInsert({i, j - 1}, Move::West);
                moved = true;
                break;
            }
            if (check == 3 && ne && e && se) {  // E
                tryInsert({i, j + 1}, Move::East);
                moved = true;
                break;
            }
        }
        if (!moved) {
            none.push_back(pos);
        }
    }

    for (auto it : blocked) {
        auto [i, j] = it->first;
        auto move = it->second;
        out.erase(it);
        if (move == Move::North) {
            out[{i + 1, j}] = Move::None;
        } else if (move == Move::South) {
            out[{i - 1, j}] = Move::None;
        } else if (move == Move::West) {
            out[{i, j + 1}] = Move::None;
        } else if (move == Move::East) {
            out[{i, j - 1}] = Move::None;
        }
    }
    for (const auto& pos : none) {
        out[pos] = Move::None;
    }
    return std::any_of(out.cbegin(), out.cend(), [](const auto it) { return it.second != Move::None; });
}

std::ostream& operator<<(std::ostream& stream, const MinGLColor& color)
{
    stream << static_cast<int>(color.rgba[0] * 255) << ' ' << static_cast<int>(color.rgba[1] * 255) << ' '
           << static_cast<int>(color.rgba[2] * 255) << '\n';
    return stream;
}

void writePpm(const std::string& fileName, const Map& map, size_t m, size_t n)
{
    std::ofstream out{fileName};
    if (!out) {
        std::cerr << "Cannot open file " << fileName << std::endl;
        return;
    }
    auto closeStream = gsl::finally([&out] { out.close(); });
    out << "P3\n" << n << ' ' << m << '\n' << "255\n";
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            Pos pos{static_cast<int32_t>(i - 2 * m / 9), static_cast<int32_t>(j - 2 * n / 9)};
            if (!isFree(map, pos)) {
                out << darkgreen;
            } else {
                out << bg;
            }
        }
    }
}

void draw(MinGL& minGL, const Map& map, size_t m, size_t n)
{
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            Pos pos{static_cast<int32_t>(i - 2 * m / 9), static_cast<int32_t>(j - 2 * n / 9)};
            if (!isFree(map, pos)) {
                minGL.putPixel(static_cast<int>(px * j), static_cast<int>(px * (m - i - 1)), darkgreen, px, px);
            }
        }
    }
    minGL.flush(bg);
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    Map map{};
    for (int32_t i = 0; i < lines.size(); ++i) {
        if (lines[i].empty()) {
            break;
        }
        for (int32_t j = 0; j < lines[i].size(); ++j) {
            if (lines[i][j] == elve) {
                map[{i, j}] = Move::None;
            }
        }
    }
    const auto m{lines.size() * 3};
    const auto n{lines[0].size() * 3};

    MinGL minGL{};
    if (!minGL.init(static_cast<unsigned>(px * n), static_cast<unsigned>(px * m), "Unstable Diffusion")) {
        std::cerr << "Cannot initialize MinGL" << std::endl;
        return EXIT_FAILURE;
    }
    auto _ = gsl::finally([&minGL] { minGL.shutdown(); });

    minGL.flush(bg);
    draw(minGL, map, m, n);
    while (glfwGetKey(minGL.getWindow(), GLFW_KEY_SPACE) != GLFW_PRESS &&
           glfwGetKey(minGL.getWindow(), GLFW_KEY_ESCAPE) != GLFW_PRESS) {
        minGL.pollEvents();
        minGL.processInput();
    }

    size_t round{};
    {  // Part 1
        do {
            Map next{};
            auto hasMoved = move(map, next, round++);
            if (hasMoved) {
                map.swap(next);
            }
            {
                std::ostringstream oss;
                oss << "Round: " << round;
                glfwSetWindowTitle(minGL.getWindow(), oss.str().c_str());
            }
            minGL.pollEvents();
            minGL.processInput();
            draw(minGL, map, m, n);
            if (writePpmFiles) {
                std::ostringstream oss;
                oss << std::setfill('0') << std::setw(4) << round << ".ppm";
                writePpm(oss.str(), map, m, n);
            }
            if (!hasMoved) {
                break;
            }
        } while (round < 10 && !minGL.windowShouldClose());
        int32_t minI{INT32_MAX}, maxI{INT32_MIN}, minJ{INT32_MAX}, maxJ{INT32_MIN};
        for (const auto it : map) {
            auto [i, j] = it.first;
            minI = std::min<int32_t>(minI, i);
            maxI = std::max<int32_t>(maxI, i);
            minJ = std::min<int32_t>(minJ, j);
            maxJ = std::max<int32_t>(maxJ, j);
        }
        std::cout << (maxI - minI + 1) * (maxJ - minJ + 1) - map.size() << std::endl;
    }
    {  // Part 2
        while (!minGL.windowShouldClose()) {
            Map next{};
            auto hasMoved = move(map, next, round++);
            if (hasMoved) {
                map.swap(next);
            }
            {
                std::ostringstream oss;
                oss << "Round: " << round;
                glfwSetWindowTitle(minGL.getWindow(), oss.str().c_str());
            }
            minGL.pollEvents();
            minGL.processInput();
            draw(minGL, map, m, n);
            if (writePpmFiles) {
                std::ostringstream oss;
                oss << std::setfill('0') << std::setw(4) << round << ".ppm";
                writePpm(oss.str(), map, m, n);
            }
            if (!hasMoved) {
                break;
            }
        }
        std::cout << round << std::endl;
    }

    while (!minGL.windowShouldClose()) {
        minGL.pollEvents();
        minGL.processInput();
    }

    return EXIT_SUCCESS;
}
