// Advent of Code 2023
// Day 14: Parabolic Reflector Dish
// https://adventofcode.com/2023/day/14

#include <fstream>
#include <iostream>
#include <map>
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

static void rotateClockwise(std::vector<std::string>& grid)
{
    std::vector<std::string> rot(grid[0].size(), std::string(grid.size(), '\0'));
    for (size_t r = 0; r < grid.size(); ++r) {
        for (size_t c = 0; c < grid[0].size(); ++c) {
            rot[c][grid.size() - r - 1] = grid[r][c];
        }
    }
    grid = std::move(rot);
}

static void rotateAntiClockwise(std::vector<std::string>& grid)
{
    std::vector<std::string> rot(grid[0].size(), std::string(grid.size(), '\0'));
    for (size_t r = 0; r < grid.size(); ++r) {
        for (size_t c = 0; c < grid[0].size(); ++c) {
            rot[grid[0].size() - c - 1][r] = grid[r][c];
        }
    }
    grid = std::move(rot);
}

static void moveN(std::vector<std::string>& grid)
{
    for (size_t r = 1; r < grid.size(); ++r) {
        for (size_t c = 0; c < grid[r].size(); ++c) {
            if (grid[r][c] == 'O') {
                size_t rr{r};
                size_t cc{c};
                while (rr > 0 && grid[rr - 1][cc] == '.') {
                    grid[rr - 1][cc] = 'O';
                    grid[rr][cc] = '.';
                    rr--;
                }
            }
        }
    }
}

static void moveW(std::vector<std::string>& grid)
{
    for (size_t c = 1; c < grid[0].size(); ++c) {
        for (size_t r = 0; r < grid.size(); ++r) {
            if (grid[r][c] == 'O') {
                size_t rr{r};
                size_t cc{c};
                while (cc > 0 && grid[rr][cc - 1] == '.') {
                    grid[rr][cc - 1] = 'O';
                    grid[rr][cc] = '.';
                    cc--;
                }
            }
        }
    }
}

static void moveS(std::vector<std::string>& grid)
{
    for (size_t r = grid.size() - 1; r > 0; --r) {
        for (size_t c = 0; c < grid[r].size(); ++c) {
            if (grid[r - 1][c] == 'O') {
                size_t rr{r - 1};
                size_t cc{c};
                while (rr + 1 < grid.size() && grid[rr + 1][cc] == '.') {
                    grid[rr + 1][cc] = 'O';
                    grid[rr][cc] = '.';
                    rr++;
                }
            }
        }
    }
}

static void moveE(std::vector<std::string>& grid)
{
    for (size_t c = grid[0].size() - 1; c > 0; --c) {
        for (size_t r = 0; r < grid.size(); ++r) {
            if (grid[r][c - 1] == 'O') {
                size_t rr{r};
                size_t cc{c - 1};
                while (cc + 1 < grid[r].size() && grid[rr][cc + 1] == '.') {
                    grid[rr][cc + 1] = 'O';
                    grid[rr][cc] = '.';
                    cc++;
                }
            }
        }
    }
}

int main(int argc, char* argv[])
{
    std::vector<std::string> grid{};
    if (2 != argc || !readFile(argv[1], grid)) {
        return EXIT_FAILURE;
    }

    const auto load = [](const std::vector<std::string>& grid) {
        uint64_t sum{};
        for (size_t r = 0; r < grid.size(); ++r) {
            for (size_t c = 0; c < grid[r].size(); ++c) {
                if (grid[r][c] == 'O') {
                    sum += grid.size() - r;
                }
            }
        }
        return sum;
    };

    {  // Part 1
        moveN(grid);
        const auto sum = load(grid);
        std::cout << sum << std::endl;
    }
    {  // Part 2
        moveW(grid);
        moveS(grid);
        moveE(grid);
        size_t step{1};
        std::map<std::vector<std::string>, size_t> cache;
        cache.insert({grid, step});
        constexpr size_t stop{1000000000};
        while (step++ < stop) {
            moveN(grid);
            moveW(grid);
            moveS(grid);
            moveE(grid);
            if (auto it = cache.find(grid); it != cache.end()) {
                const auto period = step - it->second;
                step = stop - (stop - step) % period;
                cache.clear();
            } else if (!cache.empty()) {
                cache.insert({grid, step});
            }
        }
        const auto sum = load(grid);
        std::cout << sum << std::endl;
    }

    return EXIT_SUCCESS;
}
