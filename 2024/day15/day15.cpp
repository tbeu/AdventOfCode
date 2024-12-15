// Advent of Code 2024
// Day 15: Warehouse Woes
// https://adventofcode.com/2024/day/15

#include <array>
#include <assert.h>
#include <fstream>
#include <iostream>
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

using Coord = uint8_t;
using Pos = std::array<Coord, 2>;

void simulate(std::vector<std::string>& grid, char move, Pos& pos)
{
    auto& [r, c] = pos;
    if (move == '<') {
        auto cc = c;
        bool found = false;
        while (cc > 0) {
            if (grid[r][cc - 1] == '#') {
                break;
            }
            if (grid[r][cc - 1] == '.') {
                --cc;
                found = true;
                break;
            }
            --cc;
        }
        if (found) {
            for (size_t i = cc; i < c - 1; ++i) {
                grid[r][i] = 'O';
            }
            grid[r][c - 1] = '@';
            grid[r][c] = '.';
            --c;
        }
    } else if (move == '^') {
        auto rr = r;
        bool found = false;
        while (rr > 0) {
            if (grid[rr - 1][c] == '#') {
                break;
            }
            if (grid[rr - 1][c] == '.') {
                --rr;
                found = true;
                break;
            }
            --rr;
        }
        if (found) {
            for (size_t i = rr; i < r - 1; ++i) {
                grid[i][c] = 'O';
            }
            grid[r - 1][c] = '@';
            grid[r][c] = '.';
            --r;
        }
    } else if (move == '>') {
        auto cc = c;
        bool found = false;
        while (cc < grid[r].size() - 1) {
            if (grid[r][cc + 1] == '#') {
                break;
            }
            if (grid[r][cc + 1] == '.') {
                ++cc;
                found = true;
                break;
            }
            ++cc;
        }
        if (found) {
            grid[r][c] = '.';
            grid[r][c + 1] = '@';
            for (size_t i = c + 2; i <= cc; ++i) {
                grid[r][i] = 'O';
            }
            ++c;
        }
    } else if (move == 'v') {
        auto rr = r;
        bool found = false;
        while (rr < grid.size() - 1) {
            if (grid[rr + 1][c] == '#') {
                break;
            }
            if (grid[rr + 1][c] == '.') {
                ++rr;
                found = true;
                break;
            }
            ++rr;
        }
        if (found) {
            grid[r][c] = '.';
            grid[r + 1][c] = '@';
            for (size_t i = r + 2; i <= rr; ++i) {
                grid[i][c] = 'O';
            }
            ++r;
        }
    }
}

bool canMoveN(const std::vector<std::vector<char> >& grid, const Pos& pos)
{
    const auto [r, c] = pos;
    assert(grid[r][c] == '[');
    if (r == 0) {
        return false;
    }
    if (grid[r - 1][c] == '.' && grid[r - 1][c + 1] == '.') {
        return true;
    }
    if (grid[r - 1][c] == '[' && grid[r - 1][c + 1] == ']' && canMoveN(grid, Pos{static_cast<Coord>(r - 1), c})) {
        return true;
    }
    if (grid[r - 1][c] == ']' && grid[r - 1][c + 1] == '[' &&
        canMoveN(grid, Pos{static_cast<Coord>(r - 1), static_cast<Coord>(c - 1)}) &&
        canMoveN(grid, Pos{static_cast<Coord>(r - 1), static_cast<Coord>(c + 1)})) {
        return true;
    }
    if (grid[r - 1][c] == ']' && grid[r - 1][c + 1] == '.' &&
        canMoveN(grid, Pos{static_cast<Coord>(r - 1), static_cast<Coord>(c - 1)})) {
        return true;
    }
    if (grid[r - 1][c] == '.' && grid[r - 1][c + 1] == '[' &&
        canMoveN(grid, Pos{static_cast<Coord>(r - 1), static_cast<Coord>(c + 1)})) {
        return true;
    }
    return false;
}

void moveN(std::vector<std::vector<char> >& grid, const Pos& pos)
{
    assert(canMoveN(grid, pos));
    const auto [r, c] = pos;
    assert(grid[r][c] == '[');
    if (grid[r - 1][c] == '[') {
        moveN(grid, Pos{static_cast<Coord>(r - 1), c});
    } else if (grid[r - 1][c] == ']') {
        moveN(grid, Pos{static_cast<Coord>(r - 1), static_cast<Coord>(c - 1)});
    }
    if (grid[r - 1][c + 1] == '[') {
        moveN(grid, Pos{static_cast<Coord>(r - 1), static_cast<Coord>(c + 1)});
    }
    assert(grid[r - 1][c] == '.' && grid[r - 1][c + 1] == '.');
    grid[r - 1][c] = '[';
    grid[r - 1][c + 1] = ']';
    grid[r][c] = '.';
    grid[r][c + 1] = '.';
}

bool canMoveS(const std::vector<std::vector<char> >& grid, const Pos& pos)
{
    const auto [r, c] = pos;
    assert(grid[r][c] == '[');
    if (r >= grid.size() - 1) {
        return false;
    }
    if (grid[r + 1][c] == '.' && grid[r + 1][c + 1] == '.') {
        return true;
    }
    if (grid[r + 1][c] == '[' && grid[r + 1][c + 1] == ']' && canMoveS(grid, Pos{static_cast<Coord>(r + 1), c})) {
        return true;
    }
    if (grid[r + 1][c] == ']' && grid[r + 1][c + 1] == '[' &&
        canMoveS(grid, Pos{static_cast<Coord>(r + 1), static_cast<Coord>(c - 1)}) &&
        canMoveS(grid, Pos{static_cast<Coord>(r + 1), static_cast<Coord>(c + 1)})) {
        return true;
    }
    if (grid[r + 1][c] == ']' && grid[r + 1][c + 1] == '.' &&
        canMoveS(grid, Pos{static_cast<Coord>(r + 1), static_cast<Coord>(c - 1)})) {
        return true;
    }
    if (grid[r + 1][c] == '.' && grid[r + 1][c + 1] == '[' &&
        canMoveS(grid, Pos{static_cast<Coord>(r + 1), static_cast<Coord>(c + 1)})) {
        return true;
    }
    return false;
}

void moveS(std::vector<std::vector<char> >& grid, const Pos& pos)
{
    assert(canMoveS(grid, pos));
    const auto [r, c] = pos;
    assert(grid[r][c] == '[');
    if (grid[r + 1][c] == '[') {
        moveS(grid, Pos{static_cast<Coord>(r + 1), c});
    } else if (grid[r + 1][c] == ']') {
        moveS(grid, Pos{static_cast<Coord>(r + 1), static_cast<Coord>(c - 1)});
    }
    if (grid[r + 1][c + 1] == '[') {
        moveS(grid, Pos{static_cast<Coord>(r + 1), static_cast<Coord>(c + 1)});
    }
    assert(grid[r + 1][c] == '.' && grid[r + 1][c + 1] == '.');
    grid[r][c] = '.';
    grid[r][c + 1] = '.';
    grid[r + 1][c] = '[';
    grid[r + 1][c + 1] = ']';
}

void simulate(std::vector<std::vector<char> >& grid, char move, Pos& pos)
{
    auto& [r, c] = pos;
    if (move == '<' && c > 0) {
        auto cc = c;
        bool found = false;
        while (cc > 0) {
            if (grid[r][cc - 1] == '#') {
                break;
            }
            if (grid[r][cc - 1] == '.') {
                --cc;
                found = true;
                break;
            }
            cc -= 2;
        }
        if (found) {
            for (size_t i = cc; i < c - 1; i += 2) {
                grid[r][i] = '[';
                grid[r][i + 1] = ']';
            }
            grid[r][c - 1] = '@';
            grid[r][c] = '.';
            --c;
        }
    } else if (move == 'v' && r < grid.size() - 1) {
        if (grid[r + 1][c] == '[' && canMoveS(grid, Pos{static_cast<Coord>(r + 1), c})) {
            moveS(grid, Pos{static_cast<Coord>(r + 1), c});
        } else if (grid[r + 1][c] == ']' && canMoveS(grid, Pos{static_cast<Coord>(r + 1), static_cast<Coord>(c - 1)})) {
            moveS(grid, Pos{static_cast<Coord>(r + 1), static_cast<Coord>(c - 1)});
        }
        if (grid[r + 1][c] == '.') {
            grid[r][c] = '.';
            grid[r + 1][c] = '@';
            ++r;
        }
    } else if (move == '>' && c < grid[r].size() - 1) {
        auto cc = c;
        bool found = false;
        while (cc < grid[r].size() - 1) {
            if (grid[r][cc + 1] == '#') {
                break;
            }
            if (grid[r][cc + 1] == '.') {
                ++cc;
                found = true;
                break;
            }
            cc += 2;
        }
        if (found) {
            grid[r][c] = '.';
            grid[r][c + 1] = '@';
            for (size_t i = c + 2; i <= cc; i += 2) {
                grid[r][i] = '[';
                grid[r][i + 1] = ']';
            }
            ++c;
        }
    } else if (move == '^' && r > 0) {
        if (grid[r - 1][c] == '[' && canMoveN(grid, Pos{static_cast<Coord>(r - 1), c})) {
            moveN(grid, Pos{static_cast<Coord>(r - 1), c});
        } else if (grid[r - 1][c] == ']' && canMoveN(grid, Pos{static_cast<Coord>(r - 1), static_cast<Coord>(c - 1)})) {
            moveN(grid, Pos{static_cast<Coord>(r - 1), static_cast<Coord>(c - 1)});
        }
        if (grid[r - 1][c] == '.') {
            grid[r - 1][c] = '@';
            grid[r][c] = '.';
            --r;
        }
    }
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

    const auto& [grid, moves] = [&lines]() -> std::pair<std::vector<std::string>, std::string> {
        std::vector<std::string> grid;
        std::string moves;
        bool isGrid = true;
        for (size_t i = 0; i < lines.size(); ++i) {
            if (lines[i].empty()) {
                isGrid = false;
                continue;
            }
            if (isGrid) {
                grid.push_back(lines[i]);
            } else {
                moves += lines[i];
            }
        }
        return {grid, moves};
    }();

    {  // Part 1
        auto pos = [&grid] {
            for (Coord r = 0; r < static_cast<Coord>(grid.size()); ++r) {
                for (Coord c = 0; c < static_cast<Coord>(grid[r].size()); ++c) {
                    if (grid[r][c] == '@') {
                        return Pos{r, c};
                    }
                }
            }
            return Pos{0, 0};
        }();
        auto grid1 = grid;
        for (const auto move : moves) {
            simulate(grid1, move, pos);
        }
        uint32_t sum = 0;
        for (uint32_t r = 0; r < grid1.size(); ++r) {
            for (uint32_t c = 0; c < grid1[r].size(); ++c) {
                sum += grid1[r][c] == 'O' ? 100 * r + c : 0;
            }
        }
        std::cout << sum << '\n';
    }
    {  // Part 2
        auto grid2 = std::vector<std::vector<char> >(grid.size(), std::vector<char>(grid[0].size() * 2, '.'));
        Pos pos;
        for (Coord r = 0; r < static_cast<Coord>(grid.size()); ++r) {
            for (Coord c = 0; c < static_cast<Coord>(grid[r].size()); ++c) {
                if (grid[r][c] == 'O') {
                    grid2[r][2 * c] = '[';
                    grid2[r][2 * c + 1] = ']';
                } else if (grid[r][c] == '#') {
                    grid2[r][2 * c] = '#';
                    grid2[r][2 * c + 1] = '#';
                } else if (grid[r][c] == '@') {
                    grid2[r][2 * c] = '@';
                    pos[0] = r;
                    pos[1] = 2 * c;
                }
            }
        }
        for (const auto move : moves) {
            simulate(grid2, move, pos);
        }
        uint32_t sum = 0;
        for (uint32_t r = 0; r < grid2.size(); ++r) {
            for (uint32_t c = 0; c < grid2[r].size(); ++c) {
                sum += grid2[r][c] == '[' ? 100 * r + c : 0;
            }
        }
        std::cout << sum << '\n';
    }

    return EXIT_SUCCESS;
}
