================================
实践：进程与线程编程
================================

理论结合代码，本节编写三个 C++ 程序：演示 ``fork()`` 多进程、管道通信，以及 ``pthread`` 多线程与互斥锁同步。

.. note::

   本节全部示例（``fork_demo``、``pipe_demo``、``thread_demo``、``mmap_shared``）源码见 ``source/code/chap03/`` 。在 ``source/code`` 目录执行 ``make user`` 可一键编译。

fork 与 wait
==========================

:strong:`示例 1` ：父进程创建子进程，子进程打印信息后退出，父进程 ``wait()`` 回收。

.. code-block:: cpp

   #include <iostream>
   #include <unistd.h>
   #include <sys/wait.h>
   #include <cstdlib>

   int main() {
       pid_t pid = fork();

       if (pid < 0) {
           std::cerr << "fork 失败\n";
           return 1;
       }

       if (pid == 0) {
           std::cout << "子进程: PID=" << getpid()
                     << ", PPID=" << getppid() << "\n";
           return 0;
       }

       std::cout << "父进程: PID=" << getpid()
                 << ", 子进程 PID=" << pid << "\n";

       int status;
       waitpid(pid, &status, 0);
       if (WIFEXITED(status))
           std::cout << "子进程退出码: " << WEXITSTATUS(status) << "\n";

       return 0;
   }

编译运行：

.. code-block:: bash

   g++ -std=c++17 -Wall -o fork_demo fork_demo.cpp
   ./fork_demo

管道通信
==========================

:strong:`示例 2` ：父进程通过管道向子进程发送字符串。

.. code-block:: cpp

   #include <iostream>
   #include <unistd.h>
   #include <cstring>
   #include <sys/wait.h>

   int main() {
       int fd[2];
       if (pipe(fd) < 0) {
           perror("pipe");
           return 1;
       }

       pid_t pid = fork();
       if (pid < 0) {
           perror("fork");
           return 1;
       }

       if (pid == 0) {
           close(fd[1]);
           char buf[64];
           ssize_t n = read(fd[0], buf, sizeof(buf) - 1);
           if (n > 0) {
               buf[n] = '\0';
               std::cout << "子进程收到: " << buf << "\n";
           }
           close(fd[0]);
           return 0;
       }

       close(fd[0]);
       const char* msg = "Hello from parent";
       write(fd[1], msg, strlen(msg));
       close(fd[1]);
       wait(nullptr);
       return 0;
   }

编译运行：

.. code-block:: bash

   g++ -std=c++17 -Wall -o pipe_demo pipe_demo.cpp
   ./pipe_demo

多线程与互斥锁
==========================

:strong:`示例 3` ：两个线程各对共享计数器加 100 万次，用互斥锁保证正确性。

.. code-block:: cpp

   #include <iostream>
   #include <pthread.h>

   const int N = 1'000'000;
   long counter = 0;
   pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

   void* thread_func(void* arg) {
       for (int i = 0; i < N; ++i) {
           pthread_mutex_lock(&mutex);
           ++counter;
           pthread_mutex_unlock(&mutex);
       }
       return nullptr;
   }

   int main() {
       pthread_t t1, t2;
       pthread_create(&t1, nullptr, thread_func, nullptr);
       pthread_create(&t2, nullptr, thread_func, nullptr);
       pthread_join(t1, nullptr);
       pthread_join(t2, nullptr);

       std::cout << "counter = " << counter
                 << " (期望 " << 2 * N << ")\n";
       return 0;
   }

编译运行：

.. code-block:: bash

   g++ -std=c++17 -Wall -pthread -o thread_demo thread_demo.cpp
   ./thread_demo

尝试注释掉 ``pthread_mutex_lock/unlock``，多次运行观察结果是否小于 200 万——直观感受数据竞争。

共享内存示例
==========================

:strong:`示例 4` （可选）：父子进程通过 ``mmap`` 共享内存。

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
       if (shared == MAP_FAILED) {
           perror("mmap");
           return 1;
       }

       *shared = 0;
       if (fork() == 0) {
           (*shared)++;
           std::cout << "子进程: *shared = " << *shared << "\n";
           return 0;
       }

       wait(nullptr);
       std::cout << "父进程: *shared = " << *shared << "\n";
       munmap(shared, sizeof(int));
       return 0;
   }

父子进程对同一 ``int`` 的修改互相可见，因为 ``MAP_SHARED`` 映射到同一物理页。

调试技巧
==========================

.. code-block:: bash

   # 用 gdb 调试多线程程序
   gdb ./thread_demo
   (gdb) run
   (gdb) info threads
   (gdb) thread apply all bt

进程与线程是操作系统调度的对象，它们运行的代码和数据存放在内存中。下一章深入内存管理——从寻址方式到虚拟内存，再到 Linux 的伙伴系统和 swap。

拓展阅读
==========

- ``man 2 fork``、``man 2 pipe``、``man 3 pthread_create``
- ``man 5 proc`` —— /proc 文件系统
- 内核源码 ``kernel/fork.c``、``kernel/exit.c``

本章完整参考资料与各节交叉索引见 :doc:`references`。
