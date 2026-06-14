#include <iostream>
#include <sys/prctl.h>
#include <unistd.h>

int main() {
    if (prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0) < 0) {
        perror("prctl");
        return 1;
    }

    if (setuid(1000) < 0) {
        perror("setuid");
        return 1;
    }

    std::cout << "setuid 后 euid=" << geteuid() << "\n";
    return 0;
}
