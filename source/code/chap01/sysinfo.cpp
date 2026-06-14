#include <iostream>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

int main() {
    struct utsname buf;
    if (uname(&buf) == 0) {
        std::cout << "系统: " << buf.sysname << "\n"
                  << "主机名: " << buf.nodename << "\n"
                  << "内核版本: " << buf.release << "\n"
                  << "架构: " << buf.machine << "\n";
    }

    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        std::cout << "已运行: " << info.uptime << " 秒\n"
                  << "总内存: " << info.totalram / (1024 * 1024) << " MB\n"
                  << "空闲内存: " << info.freeram / (1024 * 1024) << " MB\n"
                  << "进程数: " << info.procs << "\n";
    }

    return 0;
}
