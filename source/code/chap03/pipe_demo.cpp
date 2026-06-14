#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    int fd[2];
    if (pipe(fd) < 0) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        close(fd[1]);
        char buf[64];
        ssize_t n = read(fd[0], buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            std::cout << "子进程收到: " << buf << "\n";
        }
        close(fd[0]);
        return 0;
    }

    close(fd[0]);
    const char* msg = "Hello from parent";
    write(fd[1], msg, strlen(msg));
    close(fd[1]);
    wait(nullptr);
    return 0;
}
