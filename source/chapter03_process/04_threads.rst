======================
线程模型
======================

进程是资源分配的单元，但切换开销大。**线程** （thread）在同一进程内共享地址空间和资源，是 CPU 调度的基本单位。本节介绍用户级与内核级线程、Linux 的 NPTL 实现，以及线程与进程的关系。

为什么需要线程
========================

考虑一个 Web 服务器同时处理上千个连接。为每个连接创建一个进程，内存和切换开销都难以接受。线程允许多个执行流共享同一进程的：

- 代码段和数据段（全局变量、堆）
- 打开的文件描述符
- 信号处理设置
- 工作目录、用户 ID 等

每个线程拥有独立的:

- 程序计数器、栈指针、寄存器
- 栈空间
- 线程 ID（TID）
- 调度状态和优先级

创建线程比创建进程快得多——无需复制地址空间，无需建立新的页表。

用户级线程 vs 内核级线程
========================

.. list-table::
   :header-rows: 1
   :widths: 18 38 38

   * - 模型
     - 用户级线程（ULT）
     - 内核级线程（KLT）
   * - 管理
     - 用户态线程库（如 GNU Pth）
     - 内核直接调度
   * - 切换开销
     - 小（无系统调用）
     - 较大（需陷入内核）
   * - 多核利用
     - 难（内核只见一个进程）
     - 天然支持
   * - 阻塞问题
     - 一个线程阻塞，整个进程阻塞
     - 仅该线程阻塞

早期 Linux 使用 **LinuxThreads** ，每个线程对应一个轻量级进程，通过 ``clone()`` 实现，但不符合 POSIX 语义（如信号处理）。2003 年 **NPTL** （Native POSIX Threads Library）成为标准，线程与内核调度实体一一对应，完全兼容 POSIX 线程 API。

Linux 的线程实现
========================

Linux 不区分"进程"和"线程"的数据结构——两者都用 ``task_struct`` 表示。区别通过 ``clone()`` 系统调用的标志位控制：

.. list-table::
   :header-rows: 1
   :widths: 30 40

   * - clone 标志
     - 含义
   * - ``CLONE_VM``
     - 共享地址空间（线程）
   * - ``CLONE_FILES``
     - 共享文件描述符表
   * - ``CLONE_SIGHAND``
     - 共享信号处理
   * - ``CLONE_THREAD``
     - 同一线程组，共享 TGID

``fork()`` 等价于 ``clone(SIGCHLD, 0)``，不共享地址空间。``pthread_create()`` 底层调用 ``clone()`` 并设置 ``CLONE_VM | CLONE_FILES | ...``。

同一进程的所有线程共享 **TGID** （Thread Group ID），在 ``ps`` 中显示为同一 PID。每个线程有独立的 **TID** ，可通过 ``gettid()`` 获取（glibc 未直接暴露，需 ``syscall(SYS_gettid)``）。

查看线程
========================

.. code-block:: bash

   # 显示线程
   ps -eLf | head -5
   top -H -p <pid>

   # 查看进程的线程
   ls /proc/<pid>/task/
   cat /proc/<pid>/task/<tid>/status

``/proc/<pid>/task/`` 下列出该进程所有线程的 TID。每个线程有独立的 ``status``、``maps`` （共享同一地址空间时 maps 相同）等。

POSIX 线程 API
========================

应用层通过 **pthread** API 使用线程：

.. code-block:: cpp

   #include <pthread.h>

   void* thread_func(void* arg) {
       // 线程入口
       return nullptr;
   }

   pthread_t tid;
   pthread_create(&tid, nullptr, thread_func, nullptr);
   pthread_join(tid, nullptr);   // 等待线程结束

常用函数：

.. list-table::
   :header-rows: 1
   :widths: 25 45

   * - 函数
     - 作用
   * - ``pthread_create``
     - 创建线程
   * - ``pthread_join``
     - 等待线程退出，回收资源
   * - ``pthread_detach``
     - 分离线程，退出时自动回收
   * - ``pthread_mutex_lock/unlock``
     - 互斥锁
   * - ``pthread_cond_wait/signal``
     - 条件变量

编译时需链接 pthread：``g++ -std=c++17 -pthread -o prog prog.cpp``

线程安全问题
========================

多线程共享内存，对同一变量的并发访问可能导致 **数据竞争** （data race）。例如两个线程同时执行 ``counter++``，可能丢失更新。需要同步机制保护共享数据——下一节的 IPC 与再下一节的同步原语将详细讨论。

.. note::

   C++11 起提供 ``std::thread``、``std::mutex`` 等标准库支持，底层通常仍基于 pthread。本教程以 POSIX API 为主，因其与 Linux 内核关系更直接。

主线程与分离线程
========================

进程启动时有一个 **主线程** ，``main()`` 在其上运行。主线程退出且其他线程未 join 时，整个进程终止（可用 ``pthread_exit()`` 让主线程先结束，工作线程继续）。**分离线程** （detached）退出后自动回收，无需 join，适合"发后即忘"的任务。

线程让单进程能充分利用多核，但进程间如何交换数据？下一节介绍进程间通信（IPC）机制。
