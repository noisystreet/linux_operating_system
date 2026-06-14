#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    const char* host = argc > 1 ? argv[1] : "127.0.0.1";
    const char* msg = argc > 2 ? argv[2] : "Hello, TCP!\n";

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        std::cerr << "无效地址\n";
        return 1;
    }

    if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        perror("connect");
        return 1;
    }

    write(fd, msg, strlen(msg));

    char buf[1024];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        std::cout << "回显: " << buf;
    }

    close(fd);
    return 0;
}
