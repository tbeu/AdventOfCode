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
using State = std::array<uint64_t, 3>;

static void runCode(const IntCode& intCode, IntCode& output, State state)
{
    auto& [A, B, C] = state;
    size_t pos = 0;
    while (pos < intCode.size() - 1) {
        const auto op = static_cast<Operation>(intCode[pos++]);
        const auto operand = intCode[pos++];
        switch (op) {
            case Operation::adv:
                if (operand < 4) {
                    A >>= operand;
                } else if (operand < 7) {
                    A >>= state[operand - 4];
                }
                break;
            case Operation::bxl:
                B ^= operand;
                break;
            case Operation::bst:
                if (operand < 4) {
                    B = operand % 8;
                } else if (operand < 7) {
                    B = state[operand - 4] % 8;
                }
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
                if (operand < 4) {
                    output.push_back(operand % 8);
                } else if (operand < 7) {
                    output.push_back(state[operand - 4] % 8);
                }
                break;
            case Operation::bdv:
                if (operand < 4) {
                    B = A >> operand;
                } else if (operand < 7) {
                    B = A >> state[operand - 4];
                }
                break;
            case Operation::cdv:
                if (operand < 4) {
                    C = A >> operand;
                } else if (operand < 7) {
                    C = A >> state[operand - 4];
                }
                break;
            default:
                assert(false);
                break;
        }
    }
}

static State::value_type getA(const IntCode& intCode, State::value_type A = 0, size_t i = 0)
{
    assert(intCode.size() == 16);
    assert(intCode[0] == 2);
    assert(intCode[1] == 4);
    assert(intCode[intCode.size() - 4] == 5);
    assert(intCode[intCode.size() - 3] == 5);
    assert(intCode[intCode.size() - 2] == 3);
    assert(intCode[intCode.size() - 1] == 0);
    if (i > intCode.size() - 1) {
        return A / 8;
    }
    // skip the trailing "jnz 0" (to reset the instruction pointer), but only produce one single output value
    const auto intCodeBody = IntCode(intCode.begin(), intCode.end() - 2);
    for (uint16_t j = 0; j < 8; ++j) {
        IntCode output;
        runCode(intCodeBody, output, State{A, 0, 0});
        assert(output.size() == 1);
        if (output.back() == intCode[intCode.size() - 1 - i]) {
            if (const auto a = getA(intCode, A * 8, i + 1); a > 0) {
                return a;
            }
        }
        ++A;
    }
    return 0;
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
        const auto A = getA(intCode);
#ifdef _DEBUG
        IntCode output;
        runCode(intCode, output, State{A, 0, 0});
        assert(output == intCode);
#endif
        std::cout << A << '\n';
    }

    return EXIT_SUCCESS;
}
