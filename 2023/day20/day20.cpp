// Advent of Code 2023
// Day 20: Pulse Propagation
// https://adventofcode.com/2023/day/20

#include <assert.h>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
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

enum class ModuleType
{
    Undefined,
    Broadcast,
    FlipFlop,
    Conjunction
};

struct Module
{
    uint64_t cycle{};
    std::map<std::string, uint64_t> cycles;
    bool state{false};
    std::map<std::string, bool> inputs;
    std::vector<std::string> outputs;
    ModuleType type{ModuleType::Undefined};
};

using Count = std::array<uint64_t, 2>;
using Graph = std::map<std::string, Module>;
using Pulse = std::tuple<std::string, std::string, bool>;

static Count simulate(Graph& g, uint64_t step)
{
    Count count{1, 0};
    std::queue<Pulse> q;
    {
        const auto& broadcaster = g.at("broadcaster");
        count[0] += static_cast<uint64_t>(broadcaster.outputs.size());
        for (const auto& next : broadcaster.outputs) {
            q.push(Pulse{"broadcaster", next, false});
        }
    }
    while (!q.empty()) {
        const auto [src, dst, high] = q.front();
        q.pop();
        auto& mod = g.at(dst);
        if (ModuleType::FlipFlop == mod.type) {
            if (high) {
                continue;
            }
            auto& state = mod.state;
            state = !state;
            for (const auto& next : mod.outputs) {
                q.push(Pulse{dst, next, state});
            }
            count[state ? 1 : 0] += static_cast<uint64_t>(mod.outputs.size());
        } else if (ModuleType::Conjunction == mod.type) {
            mod.inputs.at(src) = high;
            const auto state =
                !std::all_of(mod.inputs.begin(), mod.inputs.end(), [](const auto& it) { return it.second; });
            for (const auto& next : mod.outputs) {
                q.push(Pulse{dst, next, state});
            }
            count[state ? 1 : 0] += static_cast<uint64_t>(mod.outputs.size());
            if (high) {
                mod.cycles.at(src) = step;
                if (0 == mod.cycle &&
                    std::all_of(mod.cycles.begin(), mod.cycles.end(), [](const auto& it) { return 0 != it.second; })) {
                    mod.cycle = 1;
                    for (const auto& [_, cycle] : mod.cycles) {
                        mod.cycle = std::lcm(mod.cycle, cycle);
                    }
                }
            }
        }
    }
    return count;
}

static Graph toGraph(const std::vector<std::string>& lines)
{
    Graph g;
    for (const auto& line : lines) {
        std::string name, out;
        std::vector<std::string> outputs;
        std::istringstream iss{line};
        iss >> name;
        ModuleType type{ModuleType::Undefined};
        if (name == "broadcaster") {
            type = ModuleType::Broadcast;
        } else if ('%' == name[0]) {
            type = ModuleType::FlipFlop;
            name = name.substr(1, name.size() - 1);
        } else if ('&' == name[0]) {
            type = ModuleType::Conjunction;
            name = name.substr(1, name.size() - 1);
        }
        iss.ignore(4);
        while (iss >> out) {
            if (',' == out[out.size() - 1]) {
                out = out.substr(0, out.size() - 1);
            }
            if (auto it = g.find(out); it != g.end()) {
                it->second.inputs[name] = false;
                it->second.cycles[name] = 0;
            } else {
                auto mod = Module{};
                mod.inputs[name] = false;
                mod.cycles[name] = 0;
                g[out] = mod;
            }
            outputs.push_back(out);
        }
        if (auto it = g.find(name); it != g.end()) {
            it->second.type = type;
            it->second.outputs = std::move(outputs);
        } else {
            auto mod = Module{};
            mod.type = type;
            mod.outputs = std::move(outputs);
            g[name] = mod;
        }
    }
    return g;
}

template <typename T, size_t N>
std::array<T, N>& operator+=(std::array<T, N>& lhs, const std::array<T, N>& rhs)
{
    for (size_t i = 0; i < N; ++i) {
        lhs[i] += rhs[i];
    }
    return lhs;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (2 != argc || !readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    auto g = toGraph(lines);

    {  // Part 1
        Count count{0, 0};
        for (uint64_t step = 1; step < 1001; ++step) {
            count += simulate(g, step);
        }
        std::cout << count[0] * count[1] << std::endl;
    }
    {  // Part 2
        if (const auto rx = g.find("rx"); rx != g.end()) {
            // there only is one source module for "rx"
            const auto& singleSrc = g.at(rx->second.inputs.begin()->first);
            // this soure module of "rx" is a conjunction module
            assert(ModuleType::Conjunction == singleSrc.type);
            uint64_t step{1000};
            while (0 == singleSrc.cycle) {
                simulate(g, ++step);
            }
            std::cout << singleSrc.cycle << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
