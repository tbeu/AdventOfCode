// Advent of Code 2025
// Day 5: Cafeteria
// https://adventofcode.com/2025/day/5

#include <cassert>
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

int main(int argc, char* argv[])
{
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    std::vector<std::string> lines{};
    if (!readFile(argv[1], lines)) {
        return EXIT_FAILURE;
    }

    std::vector<std::pair<uint64_t, uint64_t> > ranges;
    size_t lineIdx = 0;
    for (lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
        std::istringstream iss{lines[lineIdx]};
        uint64_t s{};
        uint64_t e{};
        if (iss >> s) {
            iss.ignore(1);
            iss >> e;
            assert(s <= e);
            ranges.push_back({s, e});
        } else {
            break;
        }
    }

    {  // Part 1
        uint64_t sum = 0;
        for (size_t i = ++lineIdx; i < lines.size(); ++i) {
            std::istringstream iss{lines[i]};
            uint64_t id{};
            if (iss >> id) {
                for (const auto [s, e] : ranges) {
                    if (s <= id && id <= e) {
                        ++sum;
                        break;
                    }
                }
            }
        }
        std::cout << sum << '\n';
    }
    {  // Part 2
        // make ranges disjunct while keeping invalid ones
        for (size_t i = 0; i < ranges.size(); ++i) {
            auto [si, ei] = ranges[i];
            if (si > ei) {
                continue;  // invalid -> skip
            }
            for (size_t j = i + 1; j < ranges.size(); ++j) {
                auto [sj, ej] = ranges[j];
                if (sj > ej) {
                    continue;  // invalid -> skip
                }
                if (sj > ei) {
                    continue;  // no overlap -> skip
                }
                if (si > ej) {
                    continue;  // no overlap -> skip
                }
                if (si >= sj && ei <= ej) {
                    ranges[i] = {1, 0};  // full inclusion -> invalidate
                    break;
                }
                if (sj >= si && ej <= ei) {
                    ranges[j] = {1, 0};  // full inclusion -> invalidate
                    continue;
                }
                if (ei >= sj && sj >= si) {
                    ranges[j] = {ei + 1, ej};  // partial overlap -> cut
                } else if (ej >= si && si >= sj) {
                    ranges[j] = {sj, si - 1};  // partial overlap -> cut
                }
            }
        }

        uint64_t sum = 0;
        for (const auto [s, e] : ranges) {
            if (s > e) {
                continue;
            }
            sum += e - s + 1;
        }
        std::cout << sum << '\n';
    }

    return EXIT_SUCCESS;
}
