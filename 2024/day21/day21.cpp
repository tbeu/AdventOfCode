// Advent of Code 2024
// Day 21: Keypad Conundrum
// https://adventofcode.com/2024/day/21

#include <algorithm>
#include <array>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
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

using Coord = int16_t;
using Pos = std::array<Coord, 2>;
using P = std::array<char, 2>;

bool addKeys(const Pos& start, const Pos& end, const Pos& bad, std::vector<std::vector<char> >& outs)
{
    auto rr = end[0] - start[0];
    auto cc = end[1] - start[1];
    if (start == bad) {
        return false;
    }
    if (rr == 0 && cc == 0) {
        for (auto& out : outs) {
            out.push_back('A');
        }
        return true;
    }
    if (rr == 0 && cc != 0) {
        if (cc > 0) {
            for (auto& out : outs) {
                auto c = cc;
                while (c-- > 0) {
                    out.push_back('>');
                }
                out.push_back('A');
            }
        } else {
            for (auto& out : outs) {
                auto c = cc;
                while (c++ < 0) {
                    out.push_back('<');
                }
                out.push_back('A');
            }
        }
        return true;
    }
    if (rr != 0 && cc == 0) {
        if (rr > 0) {
            for (auto& out : outs) {
                auto r = rr;
                while (r-- > 0) {
                    out.push_back('v');
                }
                out.push_back('A');
            }
        } else {
            for (auto& out : outs) {
                auto r = rr;
                while (r++ < 0) {
                    out.push_back('^');
                }
                out.push_back('A');
            }
        }
        return true;
    }
    if (rr > 0 && cc > 0) {
        auto nexts1 = outs;
        for (auto& out : nexts1) {
            out.push_back('>');
        }
        if (!addKeys(Pos{start[0], start[1] + 1}, end, bad, nexts1)) {
            nexts1.clear();
        }
        auto nexts2 = outs;
        for (auto& out : nexts2) {
            out.push_back('v');
        }
        if (!addKeys(Pos{start[0] + 1, start[1]}, end, bad, nexts2)) {
            nexts2.clear();
        }
        outs = std::move(nexts1);
        outs.insert(outs.end(), nexts2.begin(), nexts2.end());
        return true;
    }
    if (rr > 0 && cc < 0) {
        auto nexts1 = outs;
        for (auto& out : nexts1) {
            out.push_back('<');
        }
        if (!addKeys(Pos{start[0], start[1] - 1}, end, bad, nexts1)) {
            nexts1.clear();
        }
        auto nexts2 = outs;
        for (auto& out : nexts2) {
            out.push_back('v');
        }
        if (!addKeys(Pos{start[0] + 1, start[1]}, end, bad, nexts2)) {
            nexts2.clear();
        }
        outs = std::move(nexts1);
        outs.insert(outs.end(), nexts2.begin(), nexts2.end());
        return true;
    }
    if (rr < 0 && cc > 0) {
        auto nexts1 = outs;
        for (auto& out : nexts1) {
            out.push_back('>');
        }
        if (!addKeys(Pos{start[0], start[1] + 1}, end, bad, nexts1)) {
            nexts1.clear();
        }
        auto nexts2 = outs;
        for (auto& out : nexts2) {
            out.push_back('^');
        }
        if (!addKeys(Pos{start[0] - 1, start[1]}, end, bad, nexts2)) {
            nexts2.clear();
        }
        outs = std::move(nexts1);
        outs.insert(outs.end(), nexts2.begin(), nexts2.end());
        return true;
    }
    if (rr < 0 && cc < 0) {
        auto nexts1 = outs;
        for (auto& out : nexts1) {
            out.push_back('<');
        }
        if (!addKeys(Pos{start[0], start[1] - 1}, end, bad, nexts1)) {
            nexts1.clear();
        }
        auto nexts2 = outs;
        for (auto& out : nexts2) {
            out.push_back('^');
        }
        if (!addKeys(Pos{start[0] - 1, start[1]}, end, bad, nexts2)) {
            nexts2.clear();
        }
        outs = std::move(nexts1);
        outs.insert(outs.end(), nexts2.begin(), nexts2.end());
        return true;
    }
    assert(false);
    return false;
}

std::vector<std::vector<char> > getInitialKeys(const std::array<char, 4>& in)
{
    std::map<char, Pos> numKeyPad;
    numKeyPad['0'] = Pos{3, 1};
    numKeyPad['1'] = Pos{2, 0};
    numKeyPad['2'] = Pos{2, 1};
    numKeyPad['3'] = Pos{2, 2};
    numKeyPad['4'] = Pos{1, 0};
    numKeyPad['5'] = Pos{1, 1};
    numKeyPad['6'] = Pos{1, 2};
    numKeyPad['7'] = Pos{0, 0};
    numKeyPad['8'] = Pos{0, 1};
    numKeyPad['9'] = Pos{0, 2};
    numKeyPad['A'] = Pos{3, 2};

    std::vector<std::vector<char> > outs{std::vector<char>{}};
    addKeys(numKeyPad['A'], numKeyPad[in[0]], Pos{3, 0}, outs);
    for (size_t i = 1; i < in.size(); ++i) {
        addKeys(numKeyPad[in[i - 1]], numKeyPad[in[i]], Pos{3, 0}, outs);
    }
    return outs;
}

std::vector<std::vector<char> > iterateKeys(const std::vector<std::vector<char> >& ins)
{
    std::map<char, Pos> dirKeyPad;
    dirKeyPad['<'] = Pos{1, 0};
    dirKeyPad['v'] = Pos{1, 1};
    dirKeyPad['>'] = Pos{1, 2};
    dirKeyPad['^'] = Pos{0, 1};
    dirKeyPad['A'] = Pos{0, 2};

    std::vector<std::vector<char> > outs;
    for (const auto& in : ins) {
        std::vector<std::vector<char> > nexts{std::vector<char>{}};
        addKeys(dirKeyPad['A'], dirKeyPad[in[0]], Pos{0, 0}, nexts);
        for (size_t i = 1; i < in.size(); ++i) {
            addKeys(dirKeyPad[in[i - 1]], dirKeyPad[in[i]], Pos{0, 0}, nexts);
        }
        outs.insert(outs.end(), nexts.begin(), nexts.end());
    }
    return outs;
}

std::vector<std::vector<P> > getSeqs(const P& p)
{
    std::vector<std::vector<P> > seqs;
    const auto [s, e] = p;
    if (s == 'A') {
        if (e == '<') {
            seqs.push_back(std::vector<P>{P{'A', 'v'}, P{'v', '<'}, P{'<', '<'}, P{'<', 'A'}});
            //seqs.push_back(std::vector<P>{P{'A', '<'}, P{'<', 'v'}, P{'v', '<'}, P{'<', 'A'}});
        } else if (e == '^') {
            seqs.push_back(std::vector<P>{P{'A', '<'}, P{'<', 'A'}});
        } else if (e == '>') {
            seqs.push_back(std::vector<P>{P{'A', 'v'}, P{'v', 'A'}});
        } else if (e == 'v') {
            seqs.push_back(std::vector<P>{P{'A', '<'}, P{'<', 'v'}, P{'v', 'A'}});
            seqs.push_back(std::vector<P>{P{'A', 'v'}, P{'v', '<'}, P{'<', 'A'}});
        } else if (e == 'A') {
            seqs.push_back(std::vector<P>{P{'A', 'A'}});
        }
    } else if (s == '^') {
        if (e == '<') {
            seqs.push_back(std::vector<P>{P{'A', 'v'}, P{'v', '<'}, P{'<', 'A'}});
        } else if (e == '^') {
            seqs.push_back(std::vector<P>{P{'A', 'A'}});
        } else if (e == '>') {
            seqs.push_back(std::vector<P>{P{'A', 'v'}, P{'v', '>'}, P{'>', 'A'}});
            seqs.push_back(std::vector<P>{P{'A', '>'}, P{'>', 'v'}, P{'v', 'A'}});
        } else if (e == 'v') {
            seqs.push_back(std::vector<P>{P{'A', 'v'}, P{'v', 'A'}});
        } else if (e == 'A') {
            seqs.push_back(std::vector<P>{P{'A', '>'}, P{'>', 'A'}});
        }
    } else if (s == '>') {
        if (e == '<') {
            seqs.push_back(std::vector<P>{P{'A', '<'}, P{'<', '<'}, P{'<', 'A'}});
        } else if (e == '^') {
            seqs.push_back(std::vector<P>{P{'A', '<'}, P{'<', '^'}, P{'^', 'A'}});
            seqs.push_back(std::vector<P>{P{'A', '^'}, P{'^', '<'}, P{'<', 'A'}});
        } else if (e == '>') {
            seqs.push_back(std::vector<P>{P{'A', 'A'}});
        } else if (e == 'v') {
            seqs.push_back(std::vector<P>{P{'A', '<'}, P{'<', 'A'}});
        } else if (e == 'A') {
            seqs.push_back(std::vector<P>{P{'A', '^'}, P{'^', 'A'}});
        }
    } else if (s == 'v') {
        if (e == '<') {
            seqs.push_back(std::vector<P>{P{'A', '<'}, P{'<', 'A'}});
        } else if (e == '^') {
            seqs.push_back(std::vector<P>{P{'A', '^'}, P{'^', 'A'}});
        } else if (e == '>') {
            seqs.push_back(std::vector<P>{P{'A', '>'}, P{'>', 'A'}});
        } else if (e == 'v') {
            seqs.push_back(std::vector<P>{P{'A', 'A'}});
        } else if (e == 'A') {
            seqs.push_back(std::vector<P>{P{'A', '^'}, P{'^', '>'}, P{'>', 'A'}});
            seqs.push_back(std::vector<P>{P{'A', '>'}, P{'>', '^'}, P{'^', 'A'}});
        }
    } else if (s == '<') {
        if (e == '<') {
            seqs.push_back(std::vector<P>{P{'A', 'A'}});
        } else if (e == '^') {
            seqs.push_back(std::vector<P>{P{'A', '>'}, P{'>', '^'}, P{'^', 'A'}});
        } else if (e == '>') {
            seqs.push_back(std::vector<P>{P{'A', '>'}, P{'>', '>'}, P{'>', 'A'}});
        } else if (e == 'v') {
            seqs.push_back(std::vector<P>{P{'A', '>'}, P{'>', 'A'}});
        } else if (e == 'A') {
            seqs.push_back(std::vector<P>{P{'A', '>'}, P{'>', '>'}, P{'>', '^'}, P{'^', 'A'}});
            //seqs.push_back(std::vector<P>{P{'A', '>'}, P{'>', '^'}, P{'^', '>'}, P{'>', 'A'}});
        }
    }
    return seqs;
}

uint64_t getLen(const P& p, int depth, std::map<std::pair<P, int>, uint64_t>& cache)
{
    if (auto it = cache.find({p, depth}); it != cache.end()) {
        return it->second;
    }
    if (depth == 1) {
        return getSeqs(p).front().size();
    }
    uint64_t min = UINT64_MAX;
    for (const auto& seq : getSeqs(p)) {
        uint64_t len = 0;
        for (const auto& s : seq) {
            len += getLen(s, depth - 1, cache);
        }
        min = std::min<uint64_t>(min, len);
    }
    cache[{p, depth}] = min;
    return min;
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

    {  // Part 1
        uint64_t sum{};
        for (const auto& line : lines) {
            std::array<char, 4> buttons;
            std::istringstream iss{line};
            iss >> buttons[0] >> buttons[1] >> buttons[2] >> buttons[3];
            auto keys = getInitialKeys(buttons);
            uint64_t min = UINT64_MAX;
            for (size_t i = 0; i < 2; ++i) {
                keys = iterateKeys(keys);
            }
            for (const auto& key : keys) {
                min = std::min<uint64_t>(min, key.size());
            }
            sum += ((buttons[0] - '0') * 100 + (buttons[1] - '0') * 10 + buttons[2] - '0') * min;
        }
        std::cout << sum << '\n';
    }
    {  // Part 2
        uint64_t sum{};
        std::map<std::pair<P, int>, uint64_t> cache;
        for (const auto& line : lines) {
            std::array<char, 4> buttons;
            std::istringstream iss{line};
            iss >> buttons[0] >> buttons[1] >> buttons[2] >> buttons[3];
            const auto keys = getInitialKeys(buttons);
            uint64_t min = UINT64_MAX;
            for (const auto& key : keys) {
                auto ps = std::vector<P>{P{'A', key[0]}};
                for (size_t i = 1; i < key.size(); ++i) {
                    ps.push_back(P{key[i - 1], key[i]});
                }
                uint64_t len = 0;
                for (const auto& p : ps) {
                    len += getLen(p, 25, cache);
                }
                min = std::min<uint64_t>(min, len);
            }
            sum += ((buttons[0] - '0') * 100 + (buttons[1] - '0') * 10 + buttons[2] - '0') * min;
        }
        std::cout << sum << '\n';
    }

    return EXIT_SUCCESS;
}
