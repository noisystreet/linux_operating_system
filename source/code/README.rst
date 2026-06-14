示例代码
========

各章节的 C++ 示例程序与内核模块源码，与教程 ``lab_*.rst`` 实践节对应。
许可协议见仓库根目录 ``LICENSE-code`` （MIT）。

快速开始
========

在 ``source/code`` 目录下：

.. code-block:: bash

   make user    # 编译全部用户态示例
   make test    # 编译并运行冒烟测试
   make clean   # 清理编译产物

依赖：``g++``（C++17）、``make``。第 3 章 ``thread_demo`` 需要 ``-pthread`` （Makefile 已配置）。

内核模块（第 6 章）需额外安装内核头文件：

.. code-block:: bash

   sudo apt install build-essential linux-headers-$(uname -r)
   make kernel    # 在 chap06/ 生成 hello_chardev.ko

目录与程序索引
==============

chap01 — 第 1 章：系统信息
----------------------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 程序
     - 说明
   * - ``sysinfo``
     - 调用 ``uname()``、``sysinfo()`` 获取内核与内存信息

chap02 — 第 2 章：启动流程
----------------------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 程序
     - 说明
   * - ``mini_init``
     - 最小 init：SIGCHLD 回收、fork + exec 启动 shell

chap03 — 第 3 章：进程与线程
------------------------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 程序
     - 说明
   * - ``fork_demo``
     - ``fork()`` 与 ``waitpid()``
   * - ``pipe_demo``
     - 匿名管道父子通信
   * - ``thread_demo``
     - ``pthread`` 互斥锁保护共享计数器
   * - ``mmap_shared``
     - ``MAP_SHARED`` 匿名映射，父子共享内存

chap04 — 第 4 章：内存管理
----------------------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 程序
     - 说明
   * - ``malloc_demo``
     - 堆分配并打印 ``/proc/self/maps`` 中的 heap 行
   * - ``mmap_demo``
     - 文件 ``mmap`` 映射
   * - ``maps_dump``
     - 转储当前进程 maps（前若干行）
   * - ``segv_demo``
     - 故意空指针解引用（需 gdb 分析，勿纳入 ``make test``）
   * - ``mmap_cow``
     - 写时复制（CoW）行为演示

chap05 — 第 5 章：文件系统
----------------------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 程序
     - 说明
   * - ``io_demo``
     - ``open/read/write`` 基本文件 I/O
   * - ``sync_demo``
     - ``fsync()`` 刷盘语义
   * - ``seek_demo``
     - ``lseek()`` 随机访问

chap06 — 第 6 章：设备驱动
----------------------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 文件
     - 说明
   * - ``hello_chardev.ko``
     - 字符设备内核模块（``make kernel``）
   * - ``chardev_test``
     - 用户态读写 ``/dev/hello_chardev`` 的测试程序

chap07 — 第 7 章：网络
------------------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 程序
     - 说明
   * - ``echo_server``
     - TCP echo 服务端（默认端口 9876）
   * - ``echo_client``
     - TCP echo 客户端

chap08 — 第 8 章：系统调用
----------------------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 程序
     - 说明
   * - ``raw_syscall``
     - x86-64 内联汇编直接 ``syscall`` （``getpid``、``write``）

chap09 — 第 9 章：安全
------------------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 程序
     - 说明
   * - ``print_ids``
     - 打印真实/有效 UID、GID
   * - ``setuid_demo``
     - setuid 行为演示（需 root 安装 setuid 位后运行）
   * - ``keepcaps_demo``
     - ``prctl(PR_SET_KEEPCAPS)`` 与 capability
   * - ``secure_file``
     - 创建权限为 0600 的私有文件

chap10 — 第 10 章：虚拟化与容器
----------------------------------

实践以 ``unshare``、``docker`` 等 shell 命令为主，见 ``chap10/README.rst`` 与教程 ``lab_namespaces.rst``、``lab_cgroups.rst``。

单程序编译
==========

也可在对应章节目录内单独编译，例如：

.. code-block:: bash

   cd chap03
   make
   ./fork_demo

各章 ``Makefile`` 引用顶层 ``Makefile.common``；需要链接选项的程序（如 ``thread_demo``）在章内 Makefile 中单独指定 ``LDFLAGS``。
