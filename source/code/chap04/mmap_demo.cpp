#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>

int main() {
    const char* path = "/tmp/mmap_test.dat";
    int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    const char* msg = "Hello mmap";
    ftruncate(fd, 4096);

    void* addr = mmap(nullptr, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    strcpy(static_cast<char*>(addr), msg);
    std::cout << "写入: " << static_cast<char*>(addr) << "\n";

    munmap(addr, 4096);
    close(fd);
    return 0;
}
