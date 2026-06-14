#include <fstream>
#include <iostream>
#include <string>

int main() {
    std::ifstream maps("/proc/self/maps");
    std::string line;
    while (std::getline(maps, line))
        std::cout << line << "\n";
    return 0;
}
