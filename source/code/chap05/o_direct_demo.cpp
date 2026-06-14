#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

int main() {
    const char* path = "/tmp/o_direct_demo.dat";
    constexpr size_t align = 512;
    constexpr size_t size = 4096;

    int wfd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (wfd < 0) {
        perror("open write");
        return 1;
    }

    char data[size];
    std::memset(data, 'D', sizeof(data));
    if (write(wfd, data, sizeof(data)) != static_cast<ssize_t>(sizeof(data))) {
        perror("write");
        close(wfd);
        return 1;
    }
    close(wfd);

    void* buf = nullptr;
    if (posix_memalign(&buf, align, size) != 0) {
        std::cerr << "posix_memalign 失败\n";
        return 1;
    }

    int fd = open(path, O_RDONLY | O_DIRECT);
    if (fd < 0) {
        std::cout << "O_DIRECT open 失败: " << std::strerror(errno)
                  << "（部分文件系统或挂载选项不支持，属正常情况）\n";
        std::free(buf);
        unlink(path);
        return 0;
    }

    ssize_t n = read(fd, buf, size);
    if (n < 0) {
        std::cout << "O_DIRECT read 失败: " << std::strerror(errno) << "\n";
    } else {
        std::cout << "O_DIRECT 读取 " << n << " 字节，首字符: "
                  << static_cast<char*>(buf)[0] << "\n";
    }

    close(fd);
    std::free(buf);
    unlink(path);
    return 0;
}
