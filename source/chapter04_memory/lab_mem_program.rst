================================
实践：内存管理编程
================================

通过 C++ 程序分配内存、映射文件，观察 ``/proc/self/maps`` 的变化，并体验非法内存访问引发的段错误与 gdb 调试。

.. note::

   本节全部示例源码见 ``source/code/chap04/`` 。在 ``source/code`` 目录执行 ``make user`` 可一键编译。

malloc 与堆
==========================

:strong:`示例 1` ：分配大块内存，观察 maps 变化。

.. code-block:: cpp

   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   #include <string>
   #include <unistd.h>

   void print_maps() {
       std::ifstream maps("/proc/self/maps");
       std::string line;
       while (std::getline(maps, line)) {
           if (line.find("[heap]") != std::string::npos ||
               line.find("anon") != std::string::npos)
               std::cout << line << "\n";
       }
   }

   int main() {
       std::cout << "分配前:\n";
       print_maps();

       const size_t size = 10 * 1024 * 1024;  // 10 MB
       void* p = malloc(size);
       if (!p) return 1;

       std::cout << "\n分配 10 MB 后:\n";
       print_maps();

       std::cout << "\n按 Enter 释放...\n";
       std::cin.get();
       free(p);
       return 0;
   }

编译运行：

.. code-block:: bash

   g++ -std=c++17 -Wall -o malloc_demo malloc_demo.cpp
   ./malloc_demo

``[heap]`` 区域在 ``malloc`` 大分配后可能通过 ``mmap`` 扩展，观察 ``maps`` 中是否出现新的匿名映射区域。

mmap 映射文件
==========================

:strong:`示例 2` ：用 ``mmap`` 映射文件到内存。

.. code-block:: cpp

   #include <iostream>
   #include <fcntl.h>
   #include <sys/mman.h>
   #include <sys/stat.h>
   #include <unistd.h>
   #include <cstring>

   int main() {
       const char* path = "/tmp/mmap_test.dat";
       int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0644);
       if (fd < 0) { perror("open"); return 1; }

       const char* msg = "Hello mmap";
       ftruncate(fd, 4096);

       void* addr = mmap(nullptr, 4096, PROT_READ | PROT_WRITE,
                         MAP_SHARED, fd, 0);
       if (addr == MAP_FAILED) { perror("mmap"); return 1; }

       strcpy(static_cast<char*>(addr), msg);
       std::cout << "写入: " << static_cast<char*>(addr) << "\n";

       munmap(addr, 4096);
       close(fd);
       return 0;
   }

运行后可用 ``cat /tmp/mmap_test.dat`` 验证内容已写入文件。``MAP_SHARED`` 的修改会反映到文件。

观察 /proc/self/maps
==========================

:strong:`示例 3` ：打印完整 maps，理解地址空间布局。

.. code-block:: cpp

   #include <iostream>
   #include <fstream>
   #include <string>

   int main() {
       std::ifstream maps("/proc/self/maps");
       std::string line;
       while (std::getline(maps, line))
           std::cout << line << "\n";
       return 0;
   }

对照输出理解：代码段（``r-xp``）、数据段（``rw-p``）、堆、栈（``[stack]``）、vdso（``[vdso]``）、共享库（``.so``）。

段错误与 core dump
==========================

:strong:`示例 4` ：故意访问非法地址，触发 ``SIGSEGV``。

.. code-block:: cpp

   #include <iostream>

   int main() {
       int* p = nullptr;
       std::cout << "即将访问空指针...\n";
       *p = 42;   // SIGSEGV
       return 0;
   }

编译运行：

.. code-block:: bash

   g++ -std=c++17 -g -o segv_demo segv_demo.cpp
   ./segv_demo
   # 应看到 "Segmentation fault"

用 gdb 分析：

.. code-block:: bash

   ulimit -c unlimited    # 允许生成 core 文件
   ./segv_demo
   gdb ./segv_demo core
   (gdb) bt               # 查看崩溃时的调用栈
   (gdb) list             # 查看源码

``-g`` 选项保留调试符号，``bt`` 可定位到 ``*p = 42`` 那一行。

写时复制观察
==========================

:strong:`示例 5` （可选）：父子进程通过 ``MAP_SHARED`` 与 ``MAP_PRIVATE`` 对比 COW 行为。

.. code-block:: cpp

   #include <iostream>
   #include <sys/mman.h>
   #include <unistd.h>
   #include <sys/wait.h>
   #include <cstring>

   int main() {
       int* shared = static_cast<int*>(
           mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, -1, 0));
       *shared = 100;

       if (fork() == 0) {
           (*shared)++;
           std::cout << "子进程: " << *shared << "\n";
           return 0;
       }
       wait(nullptr);
       std::cout << "父进程: " << *shared << "\n";
       munmap(shared, sizeof(int));
       return 0;
   }

父子进程对 ``MAP_SHARED`` 的修改互相可见。若改为 ``MAP_PRIVATE``，子进程修改后父进程仍为 100。

内存与进程
==========================

进程是资源容器，内存是其核心资源之一。理解 ``/proc/self/maps``、``malloc``/``mmap`` 和缺页异常，是分析内存泄漏、段错误的基础。下一章进入文件系统——数据在磁盘上如何组织、如何持久化。

拓展阅读
==========

- ``man 2 mmap``、``man 3 malloc``、``man 5 proc``
- 内核文档：``Documentation/admin-guide/mm/``
- 内核源码 ``mm/page_alloc.c``、``mm/swap.c``
