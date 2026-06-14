#include <iostream>
#include <sys/types.h>
#include <unistd.h>

int main() {
    std::cout << "切换前 euid: " << geteuid() << "\n";

    if (setuid(65534) != 0) {
        perror("setuid");
        return 1;
    }

    std::cout << "切换后 euid: " << geteuid() << "\n";
    return 0;
}
