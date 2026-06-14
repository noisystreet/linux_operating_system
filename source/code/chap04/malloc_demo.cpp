#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

void print_maps() {
    std::ifstream maps("/proc/self/maps");
    std::string line;
    while (std::getline(maps, line)) {
        if (line.find("[heap]") != std::string::npos ||
            line.find("anon") != std::string::npos)
            std::cout << line << "\n";
    }
}

int main() {
    std::cout << "分配前:\n";
    print_maps();

    const size_t size = 10 * 1024 * 1024;
    void* p = malloc(size);
    if (!p) return 1;

    std::cout << "\n分配 10 MB 后:\n";
    print_maps();

    free(p);
    return 0;
}
