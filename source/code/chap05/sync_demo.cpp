#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

int main() {
    int fd = open("/tmp/sync_demo.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    const char* msg = "data before sync\n";
    write(fd, msg, strlen(msg));

    if (fsync(fd) == 0)
        std::cout << "fsync 成功，数据已刷到磁盘（或设备缓存）\n";

    close(fd);
    return 0;
}
