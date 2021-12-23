// Advent of Code 2021
// Day 22: Reactor Reboot
// https://adventofcode.com/2021/day/22

#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
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

constexpr bool verbose{false};
using Box = std::array<int32_t, 6>;
using Rule = std::pair<bool, Box>;
using Boxes = std::vector<Box>;

static uint64_t volume(const Box& box)
{
    return uint64_t(box[1] - box[0]) * uint64_t(box[3] - box[2]) * uint64_t(box[5] - box[4]);
}

static uint64_t volume(const Boxes& boxes)
{
    return std::accumulate(boxes.cbegin(), boxes.cend(), uint64_t{0},
                           [](auto& v, const auto& box) { return v + volume(box); });
}

static std::optional<Boxes> clip(Box& box, Box& newBox)
{
    const auto v0{std::max(box[0], newBox[0])};
    const auto v1{std::min(box[1], newBox[1])};
    const auto v2{std::max(box[2], newBox[2])};
    const auto v3{std::min(box[3], newBox[3])};
    const auto v4{std::max(box[4], newBox[4])};
    const auto v5{std::min(box[5], newBox[5])};
    if (v0 < v1 && v2 < v3 && v4 < v5) {
        Boxes boxes{};
        if (box[0] < v0 && v0 < box[1]) {
            boxes.emplace_back(Box{{box[0], v0, box[2], box[3], box[4], box[5]}});
            box[0] = v0;
        }
        if (box[0] <= v1 && v1 < box[1]) {
            boxes.emplace_back(Box{v1, box[1], box[2], box[3], box[4], box[5]});
            box[1] = v1;
        }
        if (box[2] < v2 && v2 < box[3]) {
            boxes.emplace_back(Box{box[0], box[1], box[2], v2, box[4], box[5]});
            box[2] = v2;
        }
        if (box[2] <= v3 && v3 < box[3]) {
            boxes.emplace_back(Box{box[0], box[1], v3, box[3], box[4], box[5]});
            box[3] = v3;
        }
        if (box[4] < v4 && v4 < box[5]) {
            boxes.emplace_back(Box{box[0], box[1], box[2], box[3], box[4], v4});
            box[4] = v4;
        }
        if (box[4] <= v4 && v4 < box[5] && v5 < box[5]) {
            boxes.emplace_back(Box{box[0], box[1], box[2], box[3], v5, box[5]});
            //box[5] = v5;
        }
        if (verbose) {
            for (auto& box : boxes) {
                std::cout << box[0] << "," << box[1] << "," << box[2] << "," << box[3] << "," << box[4] << "," << box[5]
                          << std::endl;
            }
        }
        return boxes;
    }
    return std::nullopt;
}

static void clip(Boxes& boxes, Box& newBox)
{
    Boxes clipped{};
    for (auto box : boxes) {
        if (auto temp = clip(box, newBox); temp.has_value()) {
            clipped.insert(clipped.end(), temp->begin(), temp->end());
        } else {
            clipped.emplace_back(box);
        }
    }
    boxes.swap(clipped);
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    std::vector<Rule> rules{};
    for (const auto& line : lines) {
        Box box{};
        std::array<std::string::value_type, 4> state{};
        std::sscanf(line.c_str(), "%3s x=%d..%d,y=%d..%d,z=%d..%d", state.data(), &box[0], &box[1], &box[2], &box[3],
                    &box[4], &box[5]);
        ++box[1];
        ++box[3];
        ++box[5];
        rules.push_back({state[0] == 'o' && state[1] == 'n', box});
    }

    {  // Part 1
        Boxes boxes{};
        for (auto& [state, box] : rules) {
            if (box[0] < -50 || box[1] > 51 || box[2] < -50 || box[3] > 51 || box[4] < -50 || box[5] > 51) {
                continue;
            }
            clip(boxes, box);
            if (state) {
                boxes.emplace_back(box);
            }
            if (verbose) {
                std::cout << "V: " << volume(boxes) << std::endl;
            }
        }
        std::cout << volume(boxes) << std::endl;
    }
    {  // Part 2
        Boxes boxes{};
        for (auto& [state, box] : rules) {
            clip(boxes, box);
            if (state) {
                boxes.emplace_back(box);
            }
            if (verbose) {
                std::cout << "V: " << volume(boxes) << std::endl;
            }
        }
        std::cout << volume(boxes) << std::endl;
    }

    return EXIT_SUCCESS;
}
