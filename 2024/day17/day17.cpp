// Advent of Code 2024
// Day 17: Chronospatial Computer
// https://adventofcode.com/2024/day/17

#include <array>
#include <assert.h>
#include <fstream>
#include <iostream>
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

enum class Operation : int32_t
{
    adv = 0,
    bxl = 1,
    bst = 2,
    jnz = 3,
    bxc = 4,
    out = 5,
    bdv = 6,
    cdv = 7,
};

using IntCode = std::vector<uint16_t>;
using State = std::array<int64_t, 3>;

static void runCode(const IntCode& intCode, IntCode& output, State& state)
{
    auto& [A, B, C] = state;
    size_t pos = 0;
    while (pos < intCode.size() - 1) {
        const auto op = static_cast<Operation>(intCode[pos++]);
        const auto operand = intCode[pos++];
        switch (op) {
            case Operation::adv:
                if (operand < 4) {
                    A /= (1ull << operand);
                } else if (operand < 7) {
                    A /= (1ull << state[operand - 4]);
                }
                break;
            case Operation::bxl:
                B ^= operand;
                break;
            case Operation::bst:
                assert(operand >= 4);
                B = state[operand - 4] % 8;
                break;
            case Operation::jnz:
                assert(operand < 4);
                if (A != 0) {
                    pos = operand;
                }
                break;
            case Operation::bxc:
                B ^= C;
                break;
            case Operation::out:
                assert(operand >= 4);
                output.push_back(state[operand - 4] % 8);
                break;
            case Operation::bdv:
                if (operand < 4) {
                    B = A / (1ull << operand);
                } else if (operand < 7) {
                    B = A / (1ull << state[operand - 4]);
                }
                break;
            case Operation::cdv:
                if (operand < 4) {
                    C = A / (1ull << operand);
                } else if (operand < 7) {
                    C = A / (1ull << state[operand - 4]);
                }
                break;
            default:
                assert(false);
                break;
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

    const auto intCode = [&lines] {
        IntCode intCode;
        std::istringstream iss(lines[4]);
        iss.ignore(9);
        IntCode::value_type i;
        while (iss >> i) {
            intCode.push_back(i);
            iss.ignore(1);
        }
        return intCode;
    }();

    {  // Part 1
        State state;
        for (size_t i : {0, 1, 2}) {
            std::istringstream iss(lines[i]);
            iss.ignore(12);
            iss >> state[i];
        }
        IntCode output;
        runCode(intCode, output, state);
        bool first = true;
        while (!output.empty()) {
            if (!first) {
                std::cout << ',';
            } else {
                first = false;
            }
            std::cout << output.front();
            output.erase(output.begin());
        }
        std::cout << '\n';
    }
    {  // Part 2
        State::value_type A = 0;
        for (size_t i = 0; i < intCode.size(); ++i) {
            A *= 8;
            bool found = false;
            for (uint16_t j = 0; j < 8; ++j) {
                State::value_type B = A % 8 ^ 2;
                const State::value_type C = A / (1ull << B);
                B ^= C;
                B ^= 7;
                if (B % 8 == intCode[intCode.size() - 1 - i]) {
                    found = true;
                    break;
                }
                ++A;
            }
            assert(found);
        }
        IntCode output;
        std::cout << A << '\n';
        auto state = State{A, 0, 0};
        runCode(intCode, output, state);
        assert(output == intCode);
    }

    return EXIT_SUCCESS;
}
