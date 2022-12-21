// Advent of Code 2022
// Day 19: Not Enough Minerals
// https://adventofcode.com/2022/day/19

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <sstream>
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

constexpr bool benchmark{true};
constexpr uint16_t ore{0};
constexpr uint16_t clay{1};
constexpr uint16_t obs{2};
constexpr uint16_t geo{3};
using Resources = std::array<uint16_t, 4>;
using Robots = Resources;
using RobotPlans = std::array<Resources, 4>;

struct BluePrint
{
    uint16_t id{};
    RobotPlans plans{};
};

using BluePrints = std::vector<BluePrint>;

struct World
{
    Resources resources{};
    Robots robots{1, 0, 0, 0};
    uint16_t step{};

    uint64_t hash() const
    {
        uint64_t hash{};
        for (uint16_t i = 0; i < 4; ++i) {
            hash |= static_cast<uint64_t>(resources[i]) << (16 * i);
            hash |= static_cast<uint64_t>(robots[i]) << (16 * i + 8);
        }
        return hash;
    }

    void update()
    {
        for (uint16_t i = 0; i < 4; ++i) {
            resources[i] += robots[i];
        }
    }

    uint16_t maxExpectedGeods(uint16_t maxSteps) const
    {
        auto g = resources[geo];
        g += (maxSteps - step) * robots[geo];
        for (uint16_t i = 1; i < maxSteps - step; i += 2) {
            g += i;
        }
        return g;
    }

    std::ostream& print(std::ostream& stream) const
    {
        stream << step << ": ";
        stream << "[" << resources[ore] << "," << resources[clay] << "," << resources[obs] << "," << resources[geo]
               << "], ";
        stream << "[" << robots[ore] << "," << robots[clay] << "," << robots[obs] << "," << robots[geo] << "]";
        return stream;
    }
};

static std::ostream& operator<<(std::ostream& stream, const World& world)
{
    return world.print(stream);
}

static size_t bfs(const BluePrint& blueprint, uint16_t maxSteps)
{
    std::map<uint64_t, uint16_t> visited{};
    uint16_t maxGeods{};
    std::queue<World> q{};
    q.push(std::move(World()));
    auto pushQ = [&](const World& next) {
        const auto hash = next.hash();
        if (visited.find(hash) == visited.cend()) {
            visited[hash] = next.resources[geo];
            if (next.resources[geo] > maxGeods) {
                maxGeods = next.resources[geo];
            }
            q.push(std::move(next));
        }
    };
    uint16_t maxOre{};
    for (uint16_t k = 0; k < 4; ++k) {
        maxOre = std::max<int16_t>(maxOre, blueprint.plans[k][ore]);
    }
    while (!q.empty()) {
        World world = q.front();
        q.pop();
        if (world.step >= maxSteps) {
            continue;
        }
        if (world.maxExpectedGeods(maxSteps) <= maxGeods) {
            continue;
        }
        world.step++;
        for (uint16_t j = 4; j > 0; --j) {
            auto i = j - 1;
            bool canBuild{true};
            World next(world);
            for (uint16_t k = 0; k < 4; ++k) {
                if (next.resources[k] < blueprint.plans[i][k]) {
                    canBuild = false;
                    break;
                }
            }
            bool wantBuild = next.step + (3 - i) < maxSteps;
            if (canBuild) {
                if (i == geo) {
                    wantBuild = true;
                } else if (i == obs || i == clay) {
                    wantBuild = next.robots[i] < blueprint.plans[i + 1][i];
                } else {
                    wantBuild = next.robots[ore] < maxOre;
                }
            }
            if (canBuild && wantBuild) {
                for (uint16_t k = 0; k < 4; ++k) {
                    next.resources[k] -= blueprint.plans[i][k];
                }
                next.update();
                next.robots[i]++;
                pushQ(next);
            }
        }
        world.update();
        pushQ(world);
    }
    return maxGeods;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    BluePrints blueprints{};
    for (const auto& line : lines) {
        if (line.empty()) {
            continue;
        }
        BluePrint blueprint{};
        std::istringstream iss{line.substr(10, line.size() - 10)};
        iss >> blueprint.id;
        iss.ignore(23);
        iss >> blueprint.plans[ore][ore];
        iss.ignore(28);
        iss >> blueprint.plans[clay][ore];
        iss.ignore(32);
        iss >> blueprint.plans[obs][ore];
        iss.ignore(9);
        iss >> blueprint.plans[obs][clay];
        iss.ignore(30);
        iss >> blueprint.plans[geo][ore];
        iss.ignore(9);
        iss >> blueprint.plans[geo][obs];
        blueprints.emplace_back(blueprint);
    }

    auto startTimeTotal = std::chrono::high_resolution_clock::now();
    {  // Part 1
        if (benchmark) {
            std::cout << "=== PUZZLE A ===" << std::endl;
        }
        size_t sum{};
        for (const auto& blueprint : blueprints) {
            auto startTime = std::chrono::high_resolution_clock::now();
            auto maxGeods = bfs(blueprint, 24);
            auto endTime = std::chrono::high_resolution_clock::now();
            auto elapsedTime = endTime - startTime;
            if (benchmark) {
                std::cout << "BluePrint [" << blueprint.id << "]: " << maxGeods << " ("
                          << elapsedTime / std::chrono::milliseconds(1) << " ms)" << std::endl;
            }
            sum += blueprint.id * maxGeods;
        }
        if (benchmark) {
            std::cout << std::endl;
        }
        std::cout << sum << std::endl;
    }
    {  // Part 2
        if (benchmark) {
            std::cout << "=== PUZZLE B ===" << std::endl;
        }
        size_t prod{1};
        for (size_t i = 0; i < std::min<size_t>(3, blueprints.size()); ++i) {
            auto startTime = std::chrono::high_resolution_clock::now();
            auto maxGeods = bfs(blueprints[i], 32);
            auto endTime = std::chrono::high_resolution_clock::now();
            auto elapsedTime = endTime - startTime;
            if (benchmark) {
                std::cout << "BluePrint [" << blueprints[i].id << "]: " << maxGeods << " ("
                          << elapsedTime / std::chrono::milliseconds(1) << " ms)" << std::endl;
            }
            prod *= maxGeods;
        }
        if (benchmark) {
            std::cout << std::endl;
        }
        std::cout << prod << std::endl;
    }
    auto endTimeTotal = std::chrono::high_resolution_clock::now();
    auto elapsedTime = endTimeTotal - startTimeTotal;
    if (benchmark) {
        std::cout << "Time: " << elapsedTime / std::chrono::seconds(1) << " s" << std::endl;
    }

    return EXIT_SUCCESS;
}
