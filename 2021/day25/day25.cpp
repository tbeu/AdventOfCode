// Advent of Code 2021
// Day 25: Sea Cucumber
// https://adventofcode.com/2021/day/25

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
#include <sstream>
#include <string>
#include <vector>

#include <gsl/util>

static bool readFile(const std::string& fileName, std::vector<std::string>& lines)
{
    std::ifstream in{fileName};
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

constexpr bool writePpmFiles{false};
constexpr int8_t E{1};
constexpr int8_t S{-1};
constexpr int8_t FREE{0};
constexpr unsigned px{2};
using Map = std::vector<std::vector<int8_t> >;

static bool moveE(Map& map)
{
    const auto m{map.size()};
    const auto n{map[0].size()};
    bool hasMoved{false};
    for (size_t i = 0; i < m; ++i) {
        auto first{map[i][0]};
        auto last{map[i][n - 1]};
        auto temp{first};
        for (size_t j = 0; j < n - 1; ++j) {
            if (temp == E && map[i][j + 1] == FREE) {
                map[i][j] = FREE;
                temp = FREE;
                map[i][j + 1] = E;
                hasMoved = true;
            } else {
                temp = map[i][j + 1];
            }
        }
        if (last == E && first == FREE) {
            map[i][n - 1] = FREE;
            map[i][0] = E;
            hasMoved = true;
        }
    }
    return hasMoved;
}

static bool moveS(Map& map)
{
    const auto m{map.size()};
    const auto n{map[0].size()};
    bool hasMoved{false};
    for (size_t j = 0; j < n; ++j) {
        auto first{map[0][j]};
        auto last{map[m - 1][j]};
        auto temp{first};
        for (size_t i = 0; i < m - 1; ++i) {
            if (temp == S && map[i + 1][j] == FREE) {
                map[i][j] = FREE;
                temp = FREE;
                map[i + 1][j] = S;
                hasMoved = true;
            } else {
                temp = map[i + 1][j];
            }
        }
        if (last == S && first == FREE) {
            map[m - 1][j] = FREE;
            map[0][j] = S;
            hasMoved = true;
        }
    }
    return hasMoved;
}

constexpr MinGLColor green{0.f, 180 / 255.0f, 0.f, 1.f};
constexpr MinGLColor darkgreen{0.f, 130 / 255.f, 0.f, 1.f};
constexpr MinGLColor bg{0.18f, 0.32f, 0.39f, 1.f};

static std::ostream& operator<<(std::ostream& stream, const MinGLColor& color)
{
    stream << static_cast<int>(color.rgba[0] * 255) << ' ' << static_cast<int>(color.rgba[1] * 255) << ' '
           << static_cast<int>(color.rgba[2] * 255) << '\n';
    return stream;
}

static void writePpm(const std::string& fileName, const Map& map)
{
    std::ofstream out{fileName};
    if (!out) {
        std::cerr << "Cannot open file " << fileName << std::endl;
        return;
    }
    auto closeStream = gsl::finally([&out] { out.close(); });
    const auto m{map.size()};
    const auto n{map[0].size()};
    out << "P3\n" << n << ' ' << m << '\n' << "255\n";
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (map[i][j] == E) {
                out << darkgreen;
            } else if (map[i][j] == S) {
                out << green;
            } else {
                out << bg;
            }
        }
    }
}

static void draw(MinGL& minGL, const Map& map)
{
    const auto m{map.size()};
    const auto n{map[0].size()};
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            MinGLColor color{};
            if (map[i][j] == E) {
                color = darkgreen;
            } else if (map[i][j] == S) {
                color = green;
            } else {
                continue;
            }
            minGL.putPixel(static_cast<int>(px * j), static_cast<int>(px * (m - i - 1)), color, px, px);
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

    const auto m{lines.size()};
    const auto n{lines[0].size()};

    if (std::any_of(lines.cbegin(), lines.cend(), [&](const auto& line) { return line.size() != n; })) {
        return EXIT_FAILURE;
    }

    Map map{m, std::vector<int8_t>(n)};
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (lines[i][j] == '>') {
                map[i][j] = E;
            } else if (lines[i][j] == 'v') {
                map[i][j] = S;
            }
        }
    }

    MinGL minGL{};
    if (!minGL.init(static_cast<unsigned>(px * n), static_cast<unsigned>(px * m), "Sea Cucumber")) {
        std::cerr << "Cannot initialize MinGL" << std::endl;
        return EXIT_FAILURE;
    }
    auto _ = gsl::finally([&minGL] { minGL.shutdown(); });

    minGL.flush(bg);
    draw(minGL, map);
    while (glfwGetKey(minGL.getWindow(), GLFW_KEY_SPACE) != GLFW_PRESS &&
           glfwGetKey(minGL.getWindow(), GLFW_KEY_ESCAPE) != GLFW_PRESS) {
        minGL.pollEvents();
        minGL.processInput();
    }

    bool hasMovedE{false};
    bool hasMovedS{false};
    size_t count{0};
    do {
        ++count;

        {
            std::ostringstream oss;
            oss << "Step: " << count;
            glfwSetWindowTitle(minGL.getWindow(), oss.str().c_str());
        }
        minGL.pollEvents();
        minGL.processInput();

        hasMovedE = moveE(map);
        draw(minGL, map);
        if (writePpmFiles) {
            std::ostringstream oss;
            oss << std::setfill('0') << std::setw(4) << count << "1.ppm";
            writePpm(oss.str(), map);
        }
        hasMovedS = moveS(map);
        draw(minGL, map);
        if (writePpmFiles) {
            std::ostringstream oss;
            oss << std::setfill('0') << std::setw(4) << count << "2.ppm";
            writePpm(oss.str(), map);
        }
    } while ((hasMovedE || hasMovedS) && !minGL.windowShouldClose());

    std::cout << count << std::endl;

    while (!minGL.windowShouldClose()) {
        minGL.pollEvents();
        minGL.processInput();
    }

    return EXIT_SUCCESS;
}
