#include <cstdlib>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "fork 失败\n";
        return 1;
    }

    if (pid == 0) {
        std::cout << "子进程: PID=" << getpid()
                  << ", PPID=" << getppid() << "\n";
        return 0;
    }

    std::cout << "父进程: PID=" << getpid()
              << ", 子进程 PID=" << pid << "\n";

    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        std::cout << "子进程退出码: " << WEXITSTATUS(status) << "\n";

    return 0;
}
