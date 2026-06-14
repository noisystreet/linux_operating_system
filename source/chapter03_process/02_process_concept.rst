======================
进程概念
======================

打开任务管理器或运行 ``ps aux``，你会看到一行行进程信息。但"进程"在操作系统内部究竟是什么？本节介绍进程的定义、进程控制块（PCB）和进程状态，以及 Linux 如何通过 ``task_struct`` 描述一个进程。

进程是什么
========================

:strong:`进程` 是程序的一次执行实例。同一个程序（如 ``/usr/bin/bash``）可以被多次启动，每次启动产生一个独立的进程，拥有各自的 PID、内存空间和文件描述符。

进程与程序的区别：

.. list-table::
   :header-rows: 1
   :widths: 15 35 35

   * - 维度
     - 程序
     - 进程
   * - 本质
     - 静态文件（磁盘上的二进制）
     - 动态执行实体（内存中的运行实例）
   * - 生命周期
     - 持久存储
     - 创建 → 运行 → 退出
   * - 数量
     - 一份代码可对应多个进程
     - 每个实例独立
   * - 组成
     - 指令 + 数据（文件）
     - 地址空间 + 寄存器 + 资源 + PCB

操作系统为每个进程分配:strong:`独立的虚拟地址空间`，使进程彼此隔离。进程只能通过内核提供的 IPC 机制（管道、socket 等）通信，不能直接读写对方的内存。

进程控制块（PCB）
========================

内核用:strong:`进程控制块` （PCB，Process Control Block）描述和管理进程。在 Linux 中，PCB 体现为 ``task_struct`` 结构体（定义在 ``include/linux/sched.h``），包含：

.. list-table::
   :header-rows: 1
   :widths: 25 55

   * - 字段类别
     - 内容
   * - 标识
     - PID、TGID（线程组 ID）、PPID（父进程 ID）、UID/GID
   * - 状态
     - 运行、睡眠、僵尸等；调度优先级
   * - 内存
     - 指向 ``mm_struct`` 的指针，描述虚拟地址空间
   * - 文件
     - ``files_struct``，打开的文件描述符表
   * - 信号
     - 待处理信号、信号处理函数、信号掩码
   * - CPU 上下文
     - 寄存器值、程序计数器、栈指针（切换时保存）

``task_struct`` 通过 ``pid`` 哈希表和 ``tasks`` 链表组织。你可以在内核源码中搜索 ``struct task_struct`` 查看完整定义——它是 Linux 内核中最核心的数据结构之一。

进程状态
========================

进程在其生命周期中经历多种状态。经典五状态模型：

.. code-block:: text

   新建(New) → 就绪(Ready) ⇄ 运行(Running) → 终止(Terminated)
                    ↕
                 阻塞(Blocked)

Linux 用更细粒度的状态常量（``include/linux/sched.h``）：

.. list-table::
   :header-rows: 1
   :widths: 20 50

   * - 状态
     - 含义
   * - ``TASK_RUNNING``
     - 正在运行或就绪（在运行队列中等待 CPU）
   * - ``TASK_INTERRUPTIBLE``
     - 可中断睡眠（等待 I/O、锁等，可被信号唤醒）
   * - ``TASK_UNINTERRUPTIBLE``
     - 不可中断睡眠（通常等待磁盘 I/O，不能被信号打断）
   * - ``TASK_STOPPED``
     - 停止（收到 SIGSTOP 等）
   * - ``TASK_ZOMBIE``
     - 僵尸：已退出但父进程尚未 wait，保留退出状态
   * - ``TASK_DEAD``
     - 死亡，即将被回收

查看进程状态：

.. code-block:: bash

   ps aux | head -5
   # STAT 列：R=运行 S=睡眠 D=不可中断睡眠 Z=僵尸 T=停止

:strong:`僵尸进程` （zombie）是常见考点：子进程退出后，内核保留其 ``task_struct`` 中的退出码，直到父进程调用 ``wait()`` 回收。若父进程不 wait，僵尸会占用 PID 资源。大量僵尸通常意味着父进程有 bug。

上下文切换
========================

当调度器选择新进程运行时，需要:strong:`上下文切换` （context switch）：

1. 保存当前进程的寄存器、程序计数器等到其 ``task_struct``
2. 切换页表（``CR3`` 寄存器指向新进程的页目录）
3. 恢复新进程的寄存器，跳转到其上次暂停的位置

上下文切换开销来自：保存/恢复寄存器、刷新 TLB、缓存失效。线程切换比进程切换轻量，因为同一进程内的线程共享页表，无需切换 ``CR3``。

可以用 ``vmstat 1`` 观察 ``cs`` （context switch）列，了解系统切换频率。

进程的创建：fork
========================

Linux 通过 ``fork()`` 创建进程。``fork()`` 复制父进程的地址空间、文件描述符、信号处理等，子进程获得新的 PID，从 ``fork()`` 返回处继续执行——父进程返回子 PID，子进程返回 0。

.. code-block:: c

   pid_t pid = fork();
   if (pid == 0) {
       // 子进程
   } else if (pid > 0) {
       // 父进程，pid 为子进程 ID
   } else {
       // fork 失败
   }

现代 Linux 的 ``fork()`` 使用:strong:`写时复制` （Copy-On-Write，COW）：并不立即复制全部内存，而是共享物理页，直到某一方写入时才复制该页。这使 fork 非常高效，尤其对大进程（如 fork 后立刻 exec 的 shell 启动命令）。

进程的终止
========================

进程通过 ``exit()`` 或 ``main`` 返回退出，内核：

1. 关闭打开的文件描述符（若设置了 ``FD_CLOEXEC`` 等）
2. 向父进程发送 ``SIGCHLD``
3. 子进程变为僵尸，等待父进程 ``wait()``
4. 父进程 wait 后，内核彻底释放 ``task_struct`` 和地址空间

若父进程先于子进程退出，子进程被 init（PID 1）收养，由 init 负责 wait 回收。

/proc 中的进程信息
========================

Linux 通过 ``/proc`` 虚拟文件系统暴露进程信息。每个进程对应 ``/proc/<pid>/`` 目录：

.. code-block:: bash

   ls /proc/self/
   cat /proc/self/status    # 进程状态、内存、信号等
   cat /proc/self/maps      # 虚拟内存映射
   ls -l /proc/self/fd/     # 打开的文件描述符

``/proc/<pid>/status`` 中的 ``State``、``VmRSS``、``Threads`` 等字段直接反映 ``task_struct`` 中的信息，是调试和理解进程行为的窗口。

进程是资源分配的容器，而 CPU 时间如何在多个进程之间分配，由调度器决定。下一节介绍 Linux 的调度算法。
