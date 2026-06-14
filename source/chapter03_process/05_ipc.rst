======================
进程间通信
======================

进程拥有独立的地址空间，不能直接访问彼此的内存。当多个进程需要协作时，必须通过内核提供的:strong:`进程间通信` （IPC，Inter-Process Communication）机制交换数据。本节介绍管道、信号、共享内存和消息队列等 Linux IPC 方式。

IPC 的分类
========================

.. list-table::
   :header-rows: 1
   :widths: 18 22 40

   * - 类型
     - 代表
     - 特点
   * - 匿名管道
     - ``pipe()``
     - 亲缘进程间，单向，字节流
   * - 命名管道
     - FIFO
     - 任意进程，文件系统路径
   * - 信号
     - ``kill()``、``signal()``
     - 异步通知，信息量小
   * - 共享内存
     - ``shmget()``、``mmap(MAP_SHARED)``
     - 最快，需同步
   * - 消息队列
     - ``mq_open()``
     - 结构化消息，POSIX 标准
   * - 套接字
     - ``socket()``
     - 可跨机器，第 7 章详述

管道（Pipe）
========================

:strong:`管道` 是最古老的 IPC 形式。``pipe()`` 创建匿名管道，返回两个文件描述符：``fd[0]`` 读端，``fd[1]`` 写端。数据单向流动，内核缓冲区有限（通常 64KB），写满则写端阻塞。

典型用法：``fork()`` 后，父进程关闭读端、子进程关闭写端，实现父写子读（或反之）：

.. code-block:: cpp

   int fd[2];
   pipe(fd);
   if (fork() == 0) {
       close(fd[1]);
       // 从 fd[0] 读取
   } else {
       close(fd[0]);
       // 向 fd[1] 写入
   }

Shell 的管道符 ``|`` 即基于此：``cmd1 | cmd2`` 将 cmd1 的 stdout 连接到 cmd2 的 stdin，通过管道传递。

:strong:`命名管道` （FIFO）在文件系统中有路径，任意进程可通过 ``open()`` 打开：

.. code-block:: bash

   mkfifo /tmp/myfifo
   # 终端 1: echo hello > /tmp/myfifo
   # 终端 2: cat /tmp/myfifo

信号（Signal）
========================

:strong:`信号` 是异步通知机制。内核或进程可向目标进程发送信号，目标进程在下次从内核态返回用户态时处理。常见信号：

.. list-table::
   :header-rows: 1
   :widths: 15 45

   * - 信号
     - 含义
   * - ``SIGINT`` （2）
     - 中断（Ctrl+C）
   * - ``SIGTERM`` （15）
     - 终止请求（kill 默认）
   * - ``SIGKILL`` （9）
     - 强制杀死，不可捕获
   * - ``SIGCHLD`` （17）
     - 子进程状态改变
   * - ``SIGSEGV`` （11）
     - 段错误，非法内存访问
   * - ``SIGUSR1/2``
     - 用户自定义

.. code-block:: bash

   kill -9 <pid>      # 发送 SIGKILL
   kill -TERM <pid>   # 发送 SIGTERM

进程可注册:strong:`信号处理函数` 改变默认行为（忽略、捕获、恢复默认）。``SIGKILL`` 和 ``SIGSTOP`` 不可捕获。信号携带的信息有限（仅信号编号），不适合传递大量数据，多用于控制和通知。

共享内存
========================

:strong:`共享内存` 允许多个进程映射同一块物理内存，是:strong:`最快` 的 IPC——数据无需在内核和用户空间之间拷贝。

方式一：POSIX 共享内存 ``shm_open()`` + ``mmap()``

方式二：匿名共享 ``mmap(MAP_SHARED | MAP_ANONYMOUS)``，常用于 ``fork()`` 后的父子进程

.. code-block:: cpp

   void* addr = mmap(nullptr, size, PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);
   if (fork() == 0) {
       // 子进程通过 addr 读写
   } else {
       // 父进程通过 addr 读写
   }

共享内存本身不提供同步，多个进程同时写会导致数据竞争，必须配合互斥锁或信号量。

System V IPC
========================

较早的 System V IPC 包括消息队列、信号量、共享内存，通过 ``ipcs``、``ipcrm`` 管理：

.. code-block:: bash

   ipcs -a    # 查看所有 IPC 对象

System V 接口较繁琐，且使用全局键值，易冲突。现代应用更倾向 POSIX API（``mq_*``、``sem_*``、``shm_*``）或 Unix 域套接字。

Unix 域套接字
========================

:strong:`Unix 域套接字` （AF_UNIX）在同一主机进程间通信，可双向、可靠，支持流式和数据报。比网络 socket 少一层网络协议栈，效率更高。systemd、Docker 等大量使用。

.. code-block:: cpp

   int fd = socket(AF_UNIX, SOCK_STREAM, 0);
   bind(fd, (struct sockaddr*)&addr, sizeof(addr));
   listen(fd, 5);
   // accept, read, write...

选择 IPC 方式
========================

.. list-table::
   :header-rows: 1
   :widths: 20 50

   * - 场景
     - 推荐
   * - 父子进程简单数据流
     - 管道
   * - 任意进程单向流
     - FIFO
   * - 控制、通知
     - 信号
   * - 大块数据、高性能
     - 共享内存 + 同步原语
   * - 结构化消息
     - 消息队列或套接字
   * - 跨机器
     - 网络套接字

无论哪种 IPC，若涉及共享可变数据，都需要同步机制避免竞态。下一节介绍互斥锁、信号量和 RCU 等同步原语。
