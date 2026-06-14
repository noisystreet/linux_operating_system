#include <cstring>
#include <iostream>
#include <sys/syscall.h>
#include <unistd.h>

static long my_syscall3(long nr, long a1, long a2, long a3) {
    long ret;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "a"(nr), "D"(a1), "S"(a2), "d"(a3)
        : "rcx", "r11", "memory");
    return ret;
}

int main() {
    long pid = my_syscall3(SYS_getpid, 0, 0, 0);
    std::cout << "getpid (syscall): " << pid << "\n";
    std::cout << "getpid (libc):    " << getpid() << "\n";

    const char* msg = "Hello via raw syscall!\n";
    long n = my_syscall3(SYS_write, STDOUT_FILENO,
                         reinterpret_cast<long>(msg), strlen(msg));
    std::cout << "write 返回: " << n << " 字节\n";

    return 0;
}
