#include <iostream>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    int* shared = static_cast<int*>(
        mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE,
             MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    if (shared == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    *shared = 100;

    if (fork() == 0) {
        (*shared)++;
        std::cout << "子进程: " << *shared << "\n";
        munmap(shared, sizeof(int));
        return 0;
    }

    wait(nullptr);
    std::cout << "父进程: " << *shared << "\n";
    munmap(shared, sizeof(int));
    return 0;
}
