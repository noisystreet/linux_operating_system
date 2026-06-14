#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

int main() {
    int fd = open("/dev/hello_chardev", O_RDWR);
    if (fd < 0) {
        perror("open /dev/hello_chardev");
        std::cerr << "请先加载内核模块: sudo insmod hello_chardev.ko\n";
        return 1;
    }

    const char* msg = "test from cpp\n";
    write(fd, msg, strlen(msg));

    lseek(fd, 0, SEEK_SET);
    char buf[64]{};
    read(fd, buf, sizeof(buf) - 1);
    std::cout << "读到: " << buf;

    close(fd);
    return 0;
}
