// Advent of Code 2021
// Day 23: Amphipod
// https://adventofcode.com/2021/day/23

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <unordered_map>
#include <variant>
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

constexpr bool verbose{false};
enum class Pod
{
    NONE = 0,
    A = 1,
    B = 10,
    C = 100,
    D = 1000
};

template <size_t goalSize>
using Room = std::variant<Pod, std::array<Pod, goalSize> >;
template <size_t goalSize>
using World = std::array<Room<goalSize>, 11>;

template <size_t goalSize>
inline static bool isGoal(const Room<goalSize>& room)
{
    return std::holds_alternative<std::array<Pod, goalSize> >(room);
}

template <size_t goalSize>
struct WorldHash
{
    size_t operator()(const World<goalSize>& world) const
    {
        size_t hash{0};
        for (uint8_t pos = 0; pos < world.size(); ++pos) {
            const auto& room = world[pos];
            if (!isGoal(room)) {
                Pod pod = std::get<Pod>(room);
                if (pod == Pod::A) {
                    hash += 1 * static_cast<size_t>(std::pow(5, pos));
                } else if (pod == Pod::B) {
                    hash += 2 * static_cast<size_t>(std::pow(5, pos));
                } else if (pod == Pod::C) {
                    hash += 3 * static_cast<size_t>(std::pow(5, pos));
                } else if (pod == Pod::D) {
                    hash += 4 * static_cast<size_t>(std::pow(5, pos));
                }
            }
        }
        return hash;
    }
};

template <size_t goalSize>
using Costs = std::unordered_map<World<goalSize>, uint32_t, WorldHash<goalSize> >;
using Goals = std::map<Pod, uint8_t>;
constexpr std::array<uint8_t, 4> goalRooms{2, 4, 6, 8};

inline static bool isGoal(uint8_t pos)
{
    return std::any_of(goalRooms.begin(), goalRooms.end(), [&](const auto& p) { return p == pos; });
}

static Pod toPod(std::string::value_type c)
{
    if (c == 'A') {
        return Pod::A;
    }
    if (c == 'B') {
        return Pod::B;
    }
    if (c == 'C') {
        return Pod::C;
    }
    if (c == 'D') {
        return Pod::D;
    }
    return Pod::NONE;
}

inline std::ostream& operator<<(std::ostream& stream, Pod pod)
{
    if (pod == Pod::A) {
        stream << "A";
    } else if (pod == Pod::B) {
        stream << "B";
    } else if (pod == Pod::C) {
        stream << "C";
    } else if (pod == Pod::D) {
        stream << "D";
    } else {
        stream << ".";
    }
    return stream;
}

template <size_t goalSize>
static std::ostream& operator<<(std::ostream& stream, const World<goalSize>& world)
{
    stream << "#############\n#";
    for (uint8_t pos = 0; pos < world.size(); ++pos) {
        const auto& room = world[pos];
        if (isGoal(room)) {
            stream << ".";
        } else {
            Pod pod = std::get<Pod>(room);
            stream << pod;
        }
    }
    stream << "#\n###";
    for (auto& pos : goalRooms) {
        const auto& room = world[pos];
        const auto pods = std::get<std::array<Pod, goalSize> >(room);
        stream << pods[0] << "#";
    }
    for (size_t i = 1; i < goalSize; ++i) {
        stream << "##\n  #";
        for (auto& pos : goalRooms) {
            const auto& room = world[pos];
            const auto pods = std::get<std::array<Pod, goalSize> >(room);
            stream << pods[i] << "#";
        }
    }
    stream << "\n  #########";
    return stream;
}

template <size_t goalSize>
static bool isFree(const Room<goalSize>& room)
{
    if (isGoal(room)) {
        auto pods = std::get<std::array<Pod, goalSize> >(room);
        return std::all_of(pods.begin(), pods.end(), [](const auto& pod) { return pod == Pod::NONE; });
    }
    return std::get<Pod>(room) == Pod::NONE;
}

template <size_t goalSize>
static bool checkMove(const World<goalSize>& world, uint8_t src, uint8_t dst)
{
    auto left = std::min(src, dst);
    auto right = std::max(src, dst);
    for (uint8_t pos = left; pos <= right; ++pos) {
        if (pos == src || isGoal(pos)) {
            continue;
        }
        if (!isFree(world[pos])) {
            return false;
        }
    }
    return true;
}

template <size_t goalSize>
static bool checkGoal(const World<goalSize>& world, Pod pod, uint8_t dst)
{
    const auto pods = std::get<std::array<Pod, goalSize> >(world[dst]);
    return std::all_of(pods.begin(), pods.end(), [&](const auto& p) { return p == Pod::NONE || p == pod; });
}

template <size_t goalSize>
static std::vector<uint8_t> validMoves(const World<goalSize>& world, uint8_t pos, Goals& goals)
{
    std::vector<uint8_t> moves{};
    auto& room = world[pos];
    Pod pod{Pod::NONE};
    if (isGoal(room)) {
        auto pods = std::get<std::array<Pod, goalSize> >(room);
        for (size_t i = 0; i < goalSize; ++i) {
            if (pods[i] != Pod::NONE) {
                pod = pods[i];
                break;
            }
        }
    } else {
        pod = std::get<Pod>(room);
    }
    {
        auto dst = goals[pod];
        if (!isGoal(pos)) {
            if (checkMove(world, pos, dst) && checkGoal(world, pod, dst)) {
                moves.emplace_back(dst);
                return moves;
            }
            return moves;
        }
        if (pos == dst && checkGoal(world, pod, dst)) {
            return moves;
        }
    }
    for (uint8_t dst = 0; dst < world.size(); ++dst) {
        if (pos == dst) {
            continue;
        }
        if (isGoal(dst) && goals[pod] != dst) {
            continue;
        }
        if (goals[pod] == dst && !checkGoal(world, pod, dst)) {
            continue;
        }
        if (checkMove(world, pos, dst)) {
            moves.emplace_back(dst);
        }
    }
    return moves;
}

template <size_t goalSize>
static std::tuple<World<goalSize>, uint32_t> move(const World<goalSize>& world, uint8_t src, uint8_t dst)
{
    auto next = world;
    Pod pod{Pod::NONE};
    uint32_t cost{0};
    {
        auto& room = world[src];
        if (isGoal(room)) {
            auto pods = std::get<std::array<Pod, goalSize> >(room);
            for (size_t i = 0; i < goalSize; ++i) {
                if (pods[i] != Pod::NONE) {
                    pod = pods[i];
                    pods[i] = Pod::NONE;
                    next[src] = pods;
                    cost += uint32_t(i) + 1;
                    break;
                }
            }
        } else {
            pod = std::get<Pod>(room);
            next[src] = Pod::NONE;
        }
    }
    {
        auto& room = world[dst];
        if (isGoal(room)) {
            auto pods = std::get<std::array<Pod, goalSize> >(room);
            for (size_t i = goalSize; i > 0; --i) {
                if (pods[i - 1] == Pod::NONE) {
                    pods[i - 1] = pod;
                    next[dst] = pods;
                    cost += uint32_t(i);
                    break;
                }
            }
        } else {
            next[dst] = pod;
        }
    }
    cost += std::abs(dst - src);
    cost *= uint32_t(pod);
    return {next, cost};
}

template <size_t goalSize>
static std::tuple<Costs<goalSize>, std::unordered_map<World<goalSize>, World<goalSize>, WorldHash<goalSize> > >
dijkstra(const World<goalSize>& start, Goals& goals)
{
    Costs<goalSize> costs{};
    costs[start] = 0;
    std::unordered_map<World<goalSize>, World<goalSize>, WorldHash<goalSize> > path{};
    auto cmp = [&](const auto& a, const auto& b) { return costs[a] > costs[b]; };
    std::priority_queue<World<goalSize>, std::vector<World<goalSize> >, decltype(cmp)> q(cmp);
    q.push(start);
    while (!q.empty()) {
        auto world = q.top();
        q.pop();
        for (uint8_t pos = 0; pos < world.size(); ++pos) {
            auto& room = world[pos];
            if (isFree(room)) {
                continue;
            }
            auto dsts = validMoves(world, pos, goals);
            for (auto& dst : dsts) {
                auto [next, cost] = move(world, pos, dst);
                cost += costs[world];
                if (costs.find(next) == costs.end() || cost < costs[next]) {
                    costs[next] = cost;
                    q.push(next);
                    path[next] = world;
                }
            }
        }
    }
    return {costs, path};
}

template <size_t goalSize>
static void print(const World<goalSize>& world, const Costs<goalSize>& costs,
                  const std::unordered_map<World<goalSize>, World<goalSize>, WorldHash<goalSize> >& path)
{
    std::vector<World<goalSize> > minPath{};
    auto iter = world;
    while (true) {
        minPath.push_back(iter);
        if (path.find(iter) == path.cend()) {
            break;
        }
        iter = path.at(iter);
    }
    for (auto it = minPath.rbegin(); it != minPath.rend(); ++it) {
        std::cout << *it << "   Cost: " << costs.at(*it) << "\n" << std::endl;
    }
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    {  //Part1
        constexpr const size_t goalSize{2};
        World<goalSize> world{};
        for (auto& room : world) {
            room = Pod::NONE;
        }
        world[2] = std::array<Pod, goalSize>{toPod(lines[2][3]), toPod(lines[3][3])};
        world[4] = std::array<Pod, goalSize>{toPod(lines[2][5]), toPod(lines[3][5])};
        world[6] = std::array<Pod, goalSize>{toPod(lines[2][7]), toPod(lines[3][7])};
        world[8] = std::array<Pod, goalSize>{toPod(lines[2][9]), toPod(lines[3][9])};

        Goals goals{};
        goals[Pod::A] = 2;
        goals[Pod::B] = 4;
        goals[Pod::C] = 6;
        goals[Pod::D] = 8;
        const auto& [costs, path] = dijkstra(world, goals);

        world[2] = std::array<Pod, goalSize>{Pod::A, Pod::A};
        world[4] = std::array<Pod, goalSize>{Pod::B, Pod::B};
        world[6] = std::array<Pod, goalSize>{Pod::C, Pod::C};
        world[8] = std::array<Pod, goalSize>{Pod::D, Pod::D};

        if constexpr (verbose) {
            print(world, costs, path);
        }

        std::cout << costs.at(world) << std::endl;
    }
    {  // Part 2
        constexpr const size_t goalSize{4};
        World<goalSize> world{};
        for (auto& room : world) {
            room = Pod::NONE;
        }
        world[2] = std::array<Pod, goalSize>{toPod(lines[2][3]), Pod::D, Pod::D, toPod(lines[3][3])};
        world[4] = std::array<Pod, goalSize>{toPod(lines[2][5]), Pod::C, Pod::B, toPod(lines[3][5])};
        world[6] = std::array<Pod, goalSize>{toPod(lines[2][7]), Pod::B, Pod::A, toPod(lines[3][7])};
        world[8] = std::array<Pod, goalSize>{toPod(lines[2][9]), Pod::A, Pod::C, toPod(lines[3][9])};

        Goals goals{};
        goals[Pod::A] = 2;
        goals[Pod::B] = 4;
        goals[Pod::C] = 6;
        goals[Pod::D] = 8;
        const auto& [costs, path] = dijkstra(world, goals);

        world[2] = std::array<Pod, goalSize>{Pod::A, Pod::A, Pod::A, Pod::A};
        world[4] = std::array<Pod, goalSize>{Pod::B, Pod::B, Pod::B, Pod::B};
        world[6] = std::array<Pod, goalSize>{Pod::C, Pod::C, Pod::C, Pod::C};
        world[8] = std::array<Pod, goalSize>{Pod::D, Pod::D, Pod::D, Pod::D};

        if constexpr (verbose) {
            print(world, costs, path);
        }

        std::cout << costs.at(world) << std::endl;
    }

    return EXIT_SUCCESS;
}
