// Advent of Code 2022
// Day 7: No Space Left On Device
// https://adventofcode.com/2022/day/7

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
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

struct File
{
    std::string name{};
    uint64_t size{0};
};

struct Directory;

using Files = std::vector<File>;
using DirectoryRef = std::shared_ptr<Directory>;
using DirectoryWeakRef = std::weak_ptr<Directory>;
using DirectoryRefs = std::vector<DirectoryRef>;

struct Directory
{
    std::string name{};
    uint64_t size{0};
    Files files{};
    DirectoryRefs dirs{};
    DirectoryWeakRef parent{};
    Directory()
    {
    }
    Directory(const std::string& name, DirectoryWeakRef& parent) : name{name}, parent{parent}
    {
    }
    DirectoryWeakRef getDir(const std::string& name)
    {
        return *std::find_if(dirs.cbegin(), dirs.cend(), [&](const auto& dir) { return dir.get()->name == name; });
    }
};

static DirectoryWeakRef parse(DirectoryWeakRef& dir, const std::string& line)
{
    if (line.compare("$ cd /") == 0) {
        dir.lock()->name = "/";
        return dir;
    }
    if (line.compare("$ cd ..") == 0) {
        return dir.lock()->parent;
    }
    if (line.compare(0, 5, "$ cd ") == 0) {
        auto name = line.substr(5, line.size() - 5);
        return dir.lock()->getDir(name);
    }
    if (line.compare("$ ls") == 0) {
        return dir;
    }
    if (line.compare(0, 4, "dir ") == 0) {
        auto name = line.substr(4, line.size() - 4);
        auto subDir = std::make_shared<Directory>(name, dir);
        dir.lock()->dirs.push_back(subDir);
        return dir;
    }
    {
        File file{};
        std::istringstream iss(line);
        iss >> file.size >> file.name;
        dir.lock()->files.push_back(file);
    }
    return dir;
}

void calcSizes(DirectoryRef& dir)
{
    for (auto& subDir : dir.get()->dirs) {
        calcSizes(subDir);
        dir.get()->size += subDir.get()->size;
    }
    for (auto& file : dir.get()->files) {
        dir.get()->size += file.size;
    }
}

uint64_t sumSizes(DirectoryRef& dir, uint64_t threshold)
{
    uint64_t sizes{};
    for (auto& subDir : dir.get()->dirs) {
        if (subDir.get()->size <= threshold) {
            sizes += subDir.get()->size;
        }
        sizes += sumSizes(subDir, threshold);
    }
    return sizes;
}

uint64_t minSpaceToDelete(DirectoryRef& dir, uint64_t req, uint64_t min)
{
    uint64_t best{min};
    for (auto& subDir : dir.get()->dirs) {
        if (subDir.get()->size >= req && subDir.get()->size < best) {
            best = subDir.get()->size;
        }
        best = minSpaceToDelete(subDir, req, best);
    }
    return best;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> lines{};
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    auto root = std::make_shared<Directory>();
    {
        DirectoryWeakRef dir = root;
        for (const auto& line : lines) {
            dir = parse(dir, line);
        }
    }
    calcSizes(root);

    {  // Part 1
        auto sizes = sumSizes(root, 100000);
        std::cout << sizes << std::endl;
    }
    {  // Part 2
        auto freeSpace = 70000000 - root.get()->size;
        auto requiredSpace = 30000000 - freeSpace;
        auto minSpace = minSpaceToDelete(root, requiredSpace, root.get()->size);
        std::cout << minSpace << std::endl;
    }

    return EXIT_SUCCESS;
}
