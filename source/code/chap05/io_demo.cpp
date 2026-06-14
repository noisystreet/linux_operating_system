#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

int main() {
    const char* path = "/tmp/io_demo.txt";
    const char* msg = "Hello, filesystem!\n";

    int fd = open(path, O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    ssize_t n = write(fd, msg, strlen(msg));
    if (n < 0) {
        perror("write");
        close(fd);
        return 1;
    }

    lseek(fd, 0, SEEK_SET);

    char buf[64];
    n = read(fd, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        std::cout << "读取: " << buf;
    }

    close(fd);
    return 0;
}
