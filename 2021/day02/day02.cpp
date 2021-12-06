#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <gsl/util>

static bool readFile(std::string fileName, std::vector<std::string> &lines)
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

int main(int argc, char *argv[])
{
    std::vector<std::string> lines;
    if (argc == 2) {
        if (!readFile(argv[1], lines)) {
            return EXIT_FAILURE;
        }
    }

    {
        int x{0}, y{0};
        for (const auto &line : lines) {
            std::istringstream iss(line);
            std::string dir;
            int val;
            iss >> dir >> val;
            if (dir == "forward") {
                x += val;
            } else if (dir == "down") {
                y += val;
            } else if (dir == "up") {
                y -= val;
            }
        }
        std::cout << x * y << std::endl;
    }
    {
        int x{0}, y{0}, aim{0};
        for (const auto &line : lines) {
            std::istringstream iss(line);
            std::string dir;
            int val;
            iss >> dir >> val;
            if (dir == "forward") {
                x += val;
                y += aim * val;
            } else if (dir == "down") {
                aim += val;
            } else if (dir == "up") {
                aim -= val;
            }
        }
        std::cout << x * y << std::endl;
    }
    return EXIT_SUCCESS;
}
