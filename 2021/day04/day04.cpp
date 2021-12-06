#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <tuple>
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

using Number = unsigned long;
using BoolNumber = std::pair<Number, bool>;
using Row = std::array<BoolNumber, 5>;
using Field = std::array<Row, 5>;

struct Bingo
{
    void fillNextRow(Row& row)
    {
        if (n < 5) {
            field[n++].swap(row);
        }
    }

    bool isBingo() const
    {
        if (std::any_of(field.cbegin(), field.cend(), [](const auto& row) {
                return std::all_of(row.cbegin(), row.cend(), [](const auto& v) { return v.second; });
            })) {
            return true;
        }
        return (field[0][0].second && field[1][0].second && field[2][0].second && field[3][0].second &&
                field[4][0].second) ||
               (field[0][1].second && field[1][1].second && field[2][1].second && field[3][1].second &&
                field[4][1].second) ||
               (field[0][2].second && field[1][2].second && field[2][2].second && field[3][2].second &&
                field[4][2].second) ||
               (field[0][3].second && field[1][3].second && field[2][3].second && field[3][3].second &&
                field[4][3].second) ||
               (field[0][4].second && field[1][4].second && field[2][4].second && field[3][4].second &&
                field[4][4].second);
    }

    std::tuple<bool, Number> set(Number num)
    {
        for (auto& row : field) {
            auto p = std::find_if(row.begin(), row.end(), [&](const auto& v) { return v.first == num; });
            if (p != row.end()) {
                p->second = true;
            }
        }

        if (isBingo()) {
            Number score{0};
            auto acc = [](Number r, const BoolNumber& v) {
                if (!v.second) {
                    return r + v.first;
                }
                return r;
            };
            for (const auto& row : field) {
                score += std::accumulate(row.cbegin(), row.cend(), 0, acc);
            }
            return {true, score * num};
        }
        return {false, 0};
    }

private:
    Field field{};
    size_t n{0};
};

using Bingos = std::vector<Bingo>;

static Bingos toBingos(const std::vector<std::string>& lines)
{
    Bingos bingos{};
    for (auto it = lines.cbegin() + 1; it != lines.cend(); ++it) {
        if (it->empty()) {
            bingos.push_back(Bingo());
            continue;
        }
        std::istringstream iss(*it);
        Row row{};
        for (size_t i = 0; i < 5; ++i) {
            Number num;
            if (iss >> num) {
                row[i].first = num;
            }
        }
        bingos.back().fillNextRow(row);
    }
    return bingos;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines;
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    std::vector<size_t> numbers{};
    {
        std::istringstream iss(lines[0]);
        Number num;
        while (iss >> num) {
            numbers.push_back(num);
            iss.ignore(1);
        }
    }

    {
        Bingos bingos{toBingos(lines)};
        bool isBingo{false};
        for (auto& num : numbers) {
            if (isBingo) {
                break;
            }
            for (auto& bingo : bingos) {
                Number score;
                std::tie(isBingo, score) = bingo.set(num);
                if (isBingo) {
                    std::cout << score << std::endl;
                    break;
                }
            }
        }
    }
    {
        Bingos bingos{toBingos(lines)};
        bool areAllBingos{false};
        for (auto& num : numbers) {
            if (areAllBingos) {
                break;
            }
            for (auto& bingo : bingos) {
                auto [isBingo, score] = bingo.set(num);
                if (!isBingo) {
                    continue;
                }
                areAllBingos =
                    std::all_of(bingos.cbegin(), bingos.cend(), [](const auto& bingo) { return bingo.isBingo(); });
                if (areAllBingos) {
                    std::cout << score << std::endl;
                    break;
                }
            }
        }
    }

    return EXIT_SUCCESS;
}
