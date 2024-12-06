// Advent of Code 2024
// Day 6: Guard Gallivant
// https://adventofcode.com/2024/day/6

#include <array>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include <gsl/util>

static bool readFile(const std::string& fileName, std::vector<std::string>& grid)
{
    std::ifstream in{fileName};
    if (!in) {
        std::cerr << "Cannot open file " << fileName << '\n';
        return false;
    }
    auto closeStream = gsl::finally([&in] { in.close(); });
    std::string str;
    while (std::getline(in, str)) {
        grid.push_back(str);
    }
    return true;
}

constexpr const auto guards = std::array<char, 4>{'>', 'v', '<', '^'};
constexpr const auto empty = '.';
using Coord = uint16_t;
using Pos = std::array<Coord, 2>;
enum class Dir : uint8_t
{
    E = 0,
    S,
    W,
    N,
};

Dir& operator++(Dir& dir)
{
    dir = static_cast<Dir>((static_cast<uint8_t>(dir) + 1) % guards.size());
    return dir;
}

using State = std::pair<Pos, Dir>;

bool tryGetNext(State& state, const std::vector<std::string>& grid)
{
    auto& [pos, dir] = state;
    auto& [r, c] = pos;
    switch (dir) {
        case Dir::E:
            if (c == grid[r].size() - 1) {
                return false;
            }
            if (grid[r][c + 1] == empty) {
                c++;
                return true;
            }
            break;
        case Dir::S:
            if (r == grid.size() - 1) {
                return false;
            }
            if (grid[r + 1][c] == empty) {
                r++;
                return true;
            }
            break;
        case Dir::W:
            if (c == 0) {
                return false;
            }
            if (grid[r][c - 1] == empty) {
                c--;
                return true;
            }
            break;
        case Dir::N:
            if (r == 0) {
                return false;
            }
            if (grid[r - 1][c] == empty) {
                r--;
                return true;
            }
            break;
    }
    ++dir;
    return true;
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    std::vector<std::string> grid{};
    if (!readFile(argv[1], grid)) {
        return EXIT_FAILURE;
    }

    const auto startState = [&grid] {
        for (Coord r = 0; r < static_cast<Coord>(grid.size()); ++r) {
            for (Coord c = 0; c < static_cast<Coord>(grid[r].size()); ++c) {
                for (uint8_t dir = 0; dir < static_cast<uint8_t>(guards.size()); ++dir) {
                    if (grid[r][c] == guards[dir]) {
                        return State{Pos{r, c}, static_cast<Dir>(dir)};
                    }
                }
            }
        }
        return State{Pos{0, 0}, Dir::E};
    }();

    // remove the guard from the start position to simplify the check for accessibility
    grid[startState.first[0]][startState.first[1]] = empty;

    auto pathPos = std::set<Pos>{startState.first};

    {  // Part 1
        auto state = startState;
        while (tryGetNext(state, grid)) {
            pathPos.emplace(state.first);
        }
        std::cout << pathPos.size() << '\n';
    }
    {  // Part 2
        uint32_t count{};
        // put obstruction on each of the guard's path positions, except start position
        pathPos.erase(startState.first);
        for (const auto& obs : pathPos) {
            constexpr const auto arbitraryObstruction = '*';
            grid[obs[0]][obs[1]] = arbitraryObstruction;
            using GuardState = std::array<bool, guards.size()>;
            auto pathState = std::vector<std::vector<GuardState> >(
                grid.size(), std::vector<GuardState>(grid[0].size(), GuardState{}));
            auto state = startState;
            while (tryGetNext(state, grid)) {
                if (pathState[state.first[0]][state.first[1]][static_cast<uint8_t>(state.second)]) {
                    // loop detected
                    count++;
                    break;
                }
                pathState[state.first[0]][state.first[1]][static_cast<uint8_t>(state.second)] = true;
            }
            // reset grid
            grid[obs[0]][obs[1]] = empty;
        }
        std::cout << count << '\n';
    }

    return EXIT_SUCCESS;
}
