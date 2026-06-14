#include <cerrno>
#include <csignal>
#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

void sigchld_handler(int) {
    while (waitpid(-1, nullptr, WNOHANG) > 0) {}
}

int main() {
    struct sigaction sa{};
    sa.sa_handler = sigchld_handler;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, nullptr);

    std::cout << "mini-init: PID=" << getpid() << " 启动\n";

    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "fork 失败: " << std::strerror(errno) << "\n";
        return 1;
    }

    if (pid == 0) {
        execl("/bin/sh", "sh", nullptr);
        std::cerr << "exec 失败: " << std::strerror(errno) << "\n";
        return 1;
    }

    int status;
    waitpid(pid, &status, 0);
    std::cout << "mini-init: shell 退出，状态=" << status << "\n";
    return 0;
}
