#include <iostream>

int main() {
    int* p = nullptr;
    std::cout << "即将访问空指针...\n";
    *p = 42;
    return 0;
}
