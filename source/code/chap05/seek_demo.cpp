#include <fcntl.h>
#include <iostream>
#include <unistd.h>

int main() {
    int fd = open("/tmp/seek_demo.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    write(fd, "ABCDEFGHIJ", 10);

    char c;
    lseek(fd, 3, SEEK_SET);
    read(fd, &c, 1);
    std::cout << "偏移 3 处字符: " << c << "\n";

    lseek(fd, 0, SEEK_END);
    off_t end = lseek(fd, 0, SEEK_CUR);
    std::cout << "文件大小: " << end << " 字节\n";

    close(fd);
    return 0;
}
