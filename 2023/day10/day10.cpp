// Advent of Code 2023
// Day 10: Pipe Maze
// https://adventofcode.com/2023/day/10

#include <array>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
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

using Pos = std::array<int16_t, 2>;
using Pipes = std::set<std::pair<Pos, Pos> >;

size_t bfs(const Pipes& pipes, std::vector<std::string>& map)
{
    const Pos dim{static_cast<int16_t>(map.size()), static_cast<int16_t>(map[0].size())};
    std::vector<std::vector<bool> > visited(map.size(), std::vector<bool>(map[0].size(), false));
    std::queue<Pos> q{};
    q.push({0, 0});
    const auto isVisited = [&](const Pos& pos) {
        assert(pos[0] >= 0);
        assert(pos[0] < dim[0]);
        assert(pos[1] >= 0);
        assert(pos[1] < dim[1]);
        return visited[pos[0]][pos[1]];
    };
    const auto isPipe = [&pipes](const Pos&& src, const Pos&& dst) {
        assert(src < dst);
        return pipes.find(std::pair(src, dst)) != pipes.end();
    };
    const auto updateQ = [&](const Pos&& next) {
        if (!isVisited(next)) {
            visited[next[0]][next[1]] = true;
            q.push(next);
        }
    };

    while (!q.empty()) {
        auto pos = q.front();
        q.pop();
        const auto [r, c] = pos;
        if (c < dim[1] - 1) {  // E
            if (!isPipe({r, c + 1}, {r + 1, c + 1})) {
                updateQ({r, c + 1});
            }
        }
        if (r < dim[0] - 1) {  // S
            if (!isPipe({r + 1, c}, {r + 1, c + 1})) {
                updateQ({r + 1, c});
            }
        }
        if (c > 0) {  // W
            if (!isPipe({r, c}, {r + 1, c})) {
                updateQ({r, c - 1});
            }
        }
        if (r > 0) {  // N
            if (!isPipe({r, c}, {r, c + 1})) {
                updateQ({r - 1, c});
            }
        }
    }

    size_t count{};
    for (int16_t r = 0; r < dim[0] - 1; ++r) {
        for (int16_t c = 0; c < dim[1] - 1; ++c) {
            if (!isVisited({r, c}) && !isVisited({r, c + 1}) && !isVisited({r + 1, c}) && !isVisited({r + 1, c + 1})) {
                count++;
            }
        }
    }
    return count;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> map{};
    if (2 != argc || !readFile(argv[1], map)) {
        return EXIT_FAILURE;
    }

    const Pos dim{static_cast<int16_t>(map.size()), static_cast<int16_t>(map[0].size())};
    map.insert(map.begin(), std::string(dim[1], '.'));
    map.emplace_back(std::string(dim[1], '.'));
    std::transform(map.begin(), map.end(), map.begin(), [](auto& row) { return '.' + std::move(row) + '.'; });

    const auto getStart = [&]() -> Pos {
        for (int16_t r = 1; r < dim[0] - 1; ++r) {
            for (int16_t c = 1; c < dim[1] - 1; ++c) {
                if ('S' == map[r][c]) {
                    return {r, c};
                }
            }
        }
        assert(false);
        return {0, 0};
    };

    const auto start = getStart();
    Pipes pipes;
    {
        // Part 1
        constexpr auto adjs = std::array<Pos, 4>{{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};
        size_t max{};
        std::vector<uint8_t> dirs;
        for (uint8_t i = 0; i < adjs.size(); ++i) {
            auto dir = i;
            if (std::find(dirs.begin(), dirs.end(), dir) != dirs.end()) {
                continue;
            }
            dirs.push_back(dir);
            Pos pos = {start[0] + adjs[i][0], start[1] + adjs[i][1]};
            auto& [r, c] = pos;
            size_t steps{1};
            Pipes loop{{start, pos}};
            while (pos != start) {
                auto src = pos;
                if (0 == dir) {  // E
                    if ('-' == map[r][c]) {
                        c++;
                    } else if ('7' == map[r][c]) {
                        r++;
                        dir = 1;
                    } else if ('J' == map[r][c]) {
                        r--;
                        dir = 3;
                    } else {
                        steps = 0;
                        break;
                    }
                } else if (1 == dir) {  // S
                    if ('|' == map[r][c]) {
                        r++;
                    } else if ('J' == map[r][c]) {
                        c--;
                        dir = 2;
                    } else if ('L' == map[r][c]) {
                        c++;
                        dir = 0;
                    } else {
                        steps = 0;
                        break;
                    }
                } else if (2 == dir) {  // W
                    if ('-' == map[r][c]) {
                        c--;
                    } else if ('L' == map[r][c]) {
                        r--;
                        dir = 3;
                    } else if ('F' == map[r][c]) {
                        r++;
                        dir = 1;
                    } else {
                        steps = 0;
                        break;
                    }
                } else if (3 == dir) {  // N
                    if ('|' == map[r][c]) {
                        r--;
                    } else if ('F' == map[r][c]) {
                        c++;
                        dir = 0;
                    } else if ('7' == map[r][c]) {
                        c--;
                        dir = 2;
                    } else {
                        steps = 0;
                        break;
                    }
                }
                steps++;
                // ensure first < second for sorted insertion
                if (0 == dir || 1 == dir) {
                    assert(src < pos);
                    loop.insert({src, pos});
                } else {
                    assert(pos < src);
                    loop.insert({pos, src});
                }
            }
            if (const auto s2 = steps / 2; s2 > 0) {
                dirs.push_back((dir + 2) % 4);
                if (s2 > max) {
                    max = s2;
                    pipes = std::move(loop);
                }
            }
        }
        std::cout << max << std::endl;
    }
    {  // Part 2
        const auto count = bfs(pipes, map);
        std::cout << count << std::endl;
    }

    return EXIT_SUCCESS;
}
