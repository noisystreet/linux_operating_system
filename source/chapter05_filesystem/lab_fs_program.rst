================================
实践：文件 I/O 编程
================================

用 C++ 调用 POSIX 文件 I/O 接口，理解文件描述符、缓冲 I/O，以及 ``fsync`` 刷盘语义。

基本读写
==========================

:strong:`示例 1` ：创建文件并写入、读回。

.. code-block:: cpp

   #include <iostream>
   #include <fcntl.h>
   #include <unistd.h>
   #include <cstring>

   int main() {
       const char* path = "/tmp/io_demo.txt";
       const char* msg = "Hello, filesystem!\n";

       int fd = open(path, O_CREAT | O_RDWR | O_TRUNC, 0644);
       if (fd < 0) { perror("open"); return 1; }

       ssize_t n = write(fd, msg, strlen(msg));
       if (n < 0) { perror("write"); close(fd); return 1; }

       lseek(fd, 0, SEEK_SET);   // 回到文件开头

       char buf[64];
       n = read(fd, buf, sizeof(buf) - 1);
       if (n > 0) {
           buf[n] = '\0';
           std::cout << "读取: " << buf;
       }

       close(fd);
       return 0;
   }

编译运行：

.. code-block:: bash

   g++ -std=c++17 -Wall -o io_demo io_demo.cpp
   ./io_demo

标准 I/O 与系统 I/O
==========================

C 标准库的 ``fopen``/``fread``/``fwrite`` 在用户态有额外缓冲；``open``/``read``/``write`` 是系统调用封装，数据进入内核的 page cache。对性能敏感或需要特定标志（如 ``O_DIRECT``）时用系统 I/O。

:strong:`示例 2` ：对比 ``write`` 后是否 ``fsync`` 的持久性（简化演示）。

.. code-block:: cpp

   #include <iostream>
   #include <fcntl.h>
   #include <unistd.h>
   #include <cstring>

   int main() {
       int fd = open("/tmp/sync_demo.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
       if (fd < 0) { perror("open"); return 1; }

       const char* msg = "data before sync\n";
       write(fd, msg, strlen(msg));

       if (fsync(fd) == 0)
           std::cout << "fsync 成功，数据已刷到磁盘（或设备缓存）\n";

       close(fd);
       return 0;
   }

``fsync`` 确保该文件的所有修改数据写入存储设备；``fdatasync`` 仅刷数据，不刷元数据（更快）。

lseek 与文件偏移
==========================

:strong:`示例 3` ：在文件中随机位置读写。

.. code-block:: cpp

   #include <iostream>
   #include <fcntl.h>
   #include <unistd.h>
   #include <cstring>

   int main() {
       int fd = open("/tmp/seek_demo.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
       write(fd, "ABCDEFGHIJ", 10);

       char c;
       lseek(fd, 3, SEEK_SET);   // 定位到 'D'
       read(fd, &c, 1);
       std::cout << "偏移 3 处字符: " << c << "\n";

       lseek(fd, 0, SEEK_END);
       off_t end = lseek(fd, 0, SEEK_CUR);
       std::cout << "文件大小: " << end << " 字节\n";

       close(fd);
       return 0;
   }

O_DIRECT 说明
==========================

``O_DIRECT`` 要求用户缓冲区按扇区对齐（通常 512 字节），且 ``open`` 时需文件系统支持。示例如下（在部分环境可能因对齐失败）：

.. code-block:: cpp

   // 需 posix_memalign 分配对齐缓冲
   void* buf = nullptr;
   posix_memalign(&buf, 512, 4096);
   int fd = open("bigfile.dat", O_RDONLY | O_DIRECT);
   read(fd, buf, 4096);

数据库、分布式存储等常自建缓存并使用 O_DIRECT。一般应用使用默认缓冲 I/O 即可。

观察进程的打开文件
==========================

程序运行时在另一终端：

.. code-block:: bash

   ls -l /proc/<pid>/fd/
   readlink /proc/<pid>/fd/3

每个 fd 对应内核 ``struct file``，链接到实际文件或管道、socket。

文件由 VFS 和具体文件系统管理，最终读写落到:strong:`块设备` 上。块设备由设备驱动和 I/O 子系统管理——下一章进入设备与 I/O 管理。

拓展阅读
==========

- ``man 2 open``、``man 2 read``、``man 2 write``、``man 2 fsync``
- 内核文档 ``Documentation/filesystems/vfs.rst``
- ``liburing`` 官方文档（io_uring）
