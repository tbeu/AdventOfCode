// Advent of Code 2021
// Day 16: Packet Decoder
// https://adventofcode.com/2021/day/16

#include "node.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <numeric>
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

static std::string word2Bin(std::string::value_type c)
{
    switch (std::toupper(c)) {
        case '0':
            return "0000";
        case '1':
            return "0001";
        case '2':
            return "0010";
        case '3':
            return "0011";
        case '4':
            return "0100";
        case '5':
            return "0101";
        case '6':
            return "0110";
        case '7':
            return "0111";
        case '8':
            return "1000";
        case '9':
            return "1001";
        case 'A':
            return "1010";
        case 'B':
            return "1011";
        case 'C':
            return "1100";
        case 'D':
            return "1101";
        case 'E':
            return "1110";
        case 'F':
            return "1111";
    }
    return "";
}

static std::string hex2Bin(const std::string& hex)
{
    std::stringstream bin{};
    std::transform(hex.cbegin(), hex.cend(), std::ostream_iterator<std::string>(bin),
                   [](const auto& c) { return word2Bin(c); });
    return bin.str();
}

enum class Kind
{
    Root,
    Literal,
    OperationSum,
    OperationProduct,
    OperationMin,
    OperationMax,
    OperationGreater,
    OperationLess,
    OperationEqual
};

struct Package
{
    Kind kind{Kind::Root};
    std::string ver{};
    uint64_t val{0};
    Package()
    {
    }
    Package(const std::string& ver, Kind kind, uint64_t val = 0) : ver{ver}, kind{kind}, val{val}
    {
    }
};

static std::ostream& operator<<(std::ostream& stream, const Package& package)
{
    if (!package.ver.empty()) {
        stream << 'v' << std::stoi(package.ver, nullptr, 2);
    }
    if (package.kind == Kind::OperationSum) {
        stream << " sum ";
    } else if (package.kind == Kind::OperationProduct) {
        stream << " prod ";
    } else if (package.kind == Kind::OperationMin) {
        stream << " min ";
    } else if (package.kind == Kind::OperationMax) {
        stream << " max ";
    } else if (package.kind == Kind::OperationGreater) {
        stream << " > ";
    } else if (package.kind == Kind::OperationLess) {
        stream << " < ";
    } else if (package.kind == Kind::OperationEqual) {
        stream << " == ";
    } else if (package.kind != Kind::Root) {
        stream << " ";
    }
    stream << package.val;
    return stream;
}

using PackageRef = NodeRef<Package>;
NodeAlloc<Package> alloc{};

static size_t parse(PackageRef& tree, const std::string& s, size_t& pos, size_t endPos, bool singlePass = false)
{
    size_t sum{0};
    while (pos < endPos - 7) {
        auto ver = s.substr(pos, 3);
        sum += std::stoi(ver, nullptr, 2);
        pos += ver.size();
        auto id = s.substr(pos, 3);
        pos += id.size();
        if (id == "100") {
            const auto kind{Kind::Literal};
            std::stringstream ss;
            while (s[pos++] == '1') {
                auto l = s.substr(pos, 4);
                ss << l;
                pos += l.size();
            }
            auto l = s.substr(pos, 4);
            ss << l;
            pos += l.size();
            tree->addNode(alloc, Package(ver, kind, std::stoull(ss.str(), nullptr, 2)));
        } else {
            // Operator
            auto kind{Kind::Root};
            if (id == "000") {
                kind = Kind::OperationSum;
            } else if (id == "001") {
                kind = Kind::OperationProduct;
            } else if (id == "010") {
                kind = Kind::OperationMin;
            } else if (id == "011") {
                kind = Kind::OperationMax;
            } else if (id == "101") {
                kind = Kind::OperationGreater;
            } else if (id == "110") {
                kind = Kind::OperationLess;
            } else if (id == "111") {
                kind = Kind::OperationEqual;
            }
            if (s[pos] == '0') {
                auto len = s.substr(++pos, 15);
                pos += len.size();
                auto sub = tree->addNode(alloc, Package(ver, kind));
                sum += parse(*sub, s, pos, pos + std::stoi(len, nullptr, 2));
            } else if (s[pos] == '1') {
                auto len = s.substr(++pos, 11);
                pos += len.size();
                auto sub = tree->addNode(alloc, Package(ver, kind));
                for (int count = 0; count < std::stoi(len, nullptr, 2); ++count) {
                    sum += parse(*sub, s, pos, endPos, true);
                }
            }
        }
        if (singlePass) {
            break;
        }
    }
    return sum;
}

static uint64_t calc(PackageRef& tree)
{
    uint64_t res{tree->val.val};
    std::vector<uint64_t> resVec{};
    for (size_t i = 0; i < tree->nodes.size(); ++i) {
        tree->nodes[i]->val.val = calc(tree->nodes[i]);
        resVec.push_back(tree->nodes[i]->val.val);
    }

    if (tree->val.kind == Kind::OperationMax) {
        res = *std::max_element(resVec.begin(), resVec.end());
    } else if (tree->val.kind == Kind::OperationMin) {
        res = *std::min_element(resVec.begin(), resVec.end());
    } else if (tree->val.kind == Kind::OperationSum) {
        res = std::accumulate(resVec.begin(), resVec.end(), uint64_t{0}, std::plus<uint64_t>());
    } else if (tree->val.kind == Kind::OperationProduct) {
        res = std::accumulate(resVec.begin(), resVec.end(), uint64_t{1}, std::multiplies<uint64_t>());
    } else if (tree->val.kind == Kind::OperationGreater) {
        res = (resVec[0] > resVec[1]) ? 1 : 0;
    } else if (tree->val.kind == Kind::OperationLess) {
        res = (resVec[0] < resVec[1]) ? 1 : 0;
    } else if (tree->val.kind == Kind::OperationEqual) {
        res = (resVec[0] == resVec[1]) ? 1 : 0;
    } else if (tree->val.kind == Kind::Root) {
        tree->val.val = tree->nodes[0]->val.val;
        res = tree->val.val;
    }

    return res;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }
    if (lines.size() != 1) {
        return EXIT_FAILURE;
    }

    const auto s{hex2Bin(lines[0])};
    size_t pos{0};
    auto root{allocate_unique<Node<Package> >(alloc, Package())};
    auto ver = parse(root, s, pos, s.size());
    std::cout << ver << std::endl;
    calc(root);
    root->print();

    return EXIT_SUCCESS;
}
