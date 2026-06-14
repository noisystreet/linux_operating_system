#include <iostream>
#include <unistd.h>

int main() {
    std::cout << "真实 UID (uid):  " << getuid() << "\n"
              << "有效 UID (euid): " << geteuid() << "\n"
              << "真实 GID (gid):  " << getgid() << "\n"
              << "有效 GID (egid): " << getegid() << "\n";
    return 0;
}
