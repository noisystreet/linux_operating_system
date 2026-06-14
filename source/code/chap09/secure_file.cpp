#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

int main() {
    const char* path = "/tmp/secure_file.txt";
    int fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0600);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    write(fd, "secret\n", 7);
    close(fd);

    struct stat st;
    stat(path, &st);
    std::cout << "mode: " << std::oct << (st.st_mode & 0777) << std::dec << "\n";
    return 0;
}
