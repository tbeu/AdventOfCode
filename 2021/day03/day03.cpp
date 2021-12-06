#include <array>
#include <bitset>
#include <fstream>
#include <iostream>
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

constexpr size_t bits{12};

inline static std::vector<std::bitset<bits> > toBitset(std::vector<std::string>& lines)
{
    std::vector<std::bitset<bits> > data;
    data.reserve(lines.size());
    std::transform(lines.cbegin(), lines.cend(), std::back_inserter(data),
                   [](const std::string& line) { return std::bitset<bits>(line); });
    return data;
}

template <typename Cmp>
static unsigned long eraseAllButOne(std::vector<std::bitset<bits> >& data)
{
    Cmp cmp;
    for (auto i = bits - 1; i > 0; i--) {
        if (data.size() == 1) {
            break;
        }
        const auto count{std::count_if(data.cbegin(), data.cend(), [i](const auto& b) { return b[i - 1]; })};
        const bool remove{cmp(count, data.size() / 2.)};
        data.erase(std::remove_if(data.begin(), data.end(), [&](const auto& b) { return b[i - 1] == remove; }),
                   data.end());
    }
    return data[0].to_ulong();
}

constexpr auto keepMostCommonBit = eraseAllButOne<std::less<double> >;
constexpr auto keepLeastCommonBit = eraseAllButOne<std::greater_equal<double> >;

int main(int argc, char* argv[])
{
    std::vector<std::string> lines;
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    auto data{toBitset(lines)};
    {
        auto size{data.size()};
        std::bitset<bits> rate;
        for (size_t i = bits; i > 0; i--) {
            const auto count{std::count_if(data.cbegin(), data.cend(), [i](const auto& b) { return b[i - 1]; })};
            rate[i - 1] = count >= size / 2.;
        }
        const auto gamma{rate.to_ulong()};
        const auto eps{rate.flip().to_ulong()};
        std::cout << gamma * eps << std::endl;
    }
    {
        // Use first selection to split data into two parts
        const auto count{std::count_if(data.cbegin(), data.cend(), [](const auto& b) { return b[bits - 1]; })};
        const auto remove{count < data.size() / 2.};
        auto p{std::stable_partition(data.begin(), data.end(), [&](const auto& b) { return b[bits - 1] != remove; })};
        std::vector<std::bitset<bits> > data2;
        data2.insert(data2.end(), std::make_move_iterator(p), std::make_move_iterator(data.end()));
        data.erase(p, data.end());

        const auto o2{keepMostCommonBit(data)};
        const auto co2{keepLeastCommonBit(data2)};

        std::cout << o2 * co2 << std::endl;
    }
    return EXIT_SUCCESS;
}
