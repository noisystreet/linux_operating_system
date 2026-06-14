#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    if (bind(listen_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }
    if (listen(listen_fd, SOMAXCONN) < 0) {
        perror("listen");
        return 1;
    }

    std::cout << "Echo 服务器监听 8080...\n";

    while (true) {
        int conn_fd = accept(listen_fd, nullptr, nullptr);
        if (conn_fd < 0) {
            perror("accept");
            continue;
        }

        char buf[1024];
        ssize_t n = read(conn_fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            std::cout << "收到: " << buf;
            write(conn_fd, buf, n);
        }
        close(conn_fd);
    }

    close(listen_fd);
    return 0;
}
