// Advent of Code 2024
// Day 18: RAM Run
// https://adventofcode.com/2024/day/18

#include <array>
#include <fstream>
#include <iostream>
#include <optional>
#include <queue>
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

using Coord = uint16_t;
using Pos = std::array<Coord, 2>;
using Dir = uint8_t;
using Grid = std::vector<std::vector<bool> >;

struct State
{
    Pos pos{};
    uint16_t step{};
};

static std::optional<uint16_t> bfs(const Grid& grid)
{
    auto visited = grid;
    constexpr std::array<std::array<int8_t, 2>, 4> adjs{{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};
    std::queue<State> q;
    q.push(State{Pos{0, 0}, 0});
    auto minSteps = std::optional<uint16_t>();
    const auto end = Pos{static_cast<Coord>(grid.size() - 1), static_cast<Coord>(grid.size() - 1)};
    while (!q.empty()) {
        const auto state = q.front();
        if (state.pos == end) {
            minSteps = state.step;
            break;
        }
        q.pop();
        const auto step = static_cast<uint16_t>(state.step + 1);
        const auto [r, c] = state.pos;
        for (Dir dir = 0; dir < adjs.size(); ++dir) {
            if (0 == dir && grid[0].size() - 1 == c) {  // E
                continue;
            }
            if (1 == dir && grid.size() - 1 == r) {  // S
                continue;
            }
            if (2 == dir && 0 == c) {  // W
                continue;
            }
            if (3 == dir && 0 == r) {  // N
                continue;
            }
            const auto& [rr, cc] = adjs[dir];
            const auto pos = Pos{static_cast<Coord>(r + rr), static_cast<Coord>(c + cc)};
            if (visited[pos[0]][pos[1]]) {
                continue;
            }
            visited[pos[0]][pos[1]] = true;
            q.push(State{pos, step});
        }
    }
    return minSteps;
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

    auto grid = Grid(71, std::vector<bool>(71, false));

    for (size_t i = 0; i < 1024; ++i) {
        std::istringstream iss{lines[i]};
        Coord r, c;
        char comma;
        iss >> c >> comma >> r;
        grid[r][c] = true;
    }

    {  // Part 1
        std::cout << bfs(grid).value() << '\n';
    }
    {  // Part 2
        for (size_t i = 1024; i < lines.size(); ++i) {
            std::istringstream iss{lines[i]};
            Coord r, c;
            char comma;
            iss >> c >> comma >> r;
            grid[r][c] = true;
            if (!bfs(grid).has_value()) {
                std::cout << c << ',' << r << '\n';
                break;
            }
        }
    }

    return EXIT_SUCCESS;
}
