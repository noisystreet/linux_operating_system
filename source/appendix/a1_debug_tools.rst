======================
调试工具
======================

理解操作系统离不开观察内核与进程行为。本附录汇总教程中涉及的及常用的 Linux 调试与追踪工具，并按场景提供端到端 walkthrough。

strace / ltrace
========================

**strace** ：跟踪 **系统调用** 和信号。

.. code-block:: bash

   strace ls
   strace -p <pid>
   strace -c -f command

**ltrace** ：跟踪 **库函数** 调用（动态链接）。

.. code-block:: bash

   ltrace ls

适用：程序行为异常、权限拒绝、文件未找到、慢在哪些 syscall。

GDB
========================

GNU 调试器，断点、单步、查看内存和栈：

.. code-block:: bash

   gdb ./program
   gdb -p <pid>              # 附加进程
   gdb ./program core        # 分析 core dump

内核调试需 ``kgdb`` 或虚拟机 + 双机调试，进阶主题见内核文档。

ftrace
========================

内核内置追踪框架，跟踪函数调用、延迟、事件：

.. code-block:: bash

   # 需 root，且内核启用 CONFIG_FUNCTION_TRACER
   sudo cat /sys/kernel/debug/tracing/available_tracers
   sudo bash -c 'echo function > /sys/kernel/debug/tracing/current_tracer'
   sudo cat /sys/kernel/debug/tracing/trace

``trace-cmd``、``KernelShark`` 提供图形分析。适合内核开发者分析启动、调度、I/O 路径。

perf
========================

性能分析与追踪，CPU 采样、硬件事件、tracepoint：

.. code-block:: bash

   perf stat ./program
   perf record -g ./program
   perf report
   perf trace

``perf probe`` 可动态添加内核/用户探针。教程第 8 章用 ``perf`` 统计 syscall。

场景一：strace 排查权限拒绝
================================

第 9 章权限实验失败时，用 ``strace`` 定位被拒绝的 syscall：

.. code-block:: bash

   touch /tmp/secret && chmod 000 /tmp/secret
   strace cat /tmp/secret 2>&1 | tail -5

典型输出含 ``openat(...) = -1 EACCES (Permission denied)``。结合 ``errno`` 与 ``man 2 openat`` 理解 DAC 检查顺序。若启用 SELinux，还需 ``ausearch -m avc`` 查看 MAC 拒绝。

场景二：GDB 分析 SIGSEGV（呼应第 4 章）
==========================================

.. code-block:: bash

   cd source/code/chap04
   make segv_demo
   ulimit -c unlimited
   ./segv_demo    # 产生 core

   gdb ./segv_demo core
   (gdb) bt
   (gdb) list
   (gdb) info registers

``bt`` 显示崩溃栈；空指针解引用通常落在 ``main`` 附近。生产环境需开启 ``kernel.core_pattern`` 并注意 core 文件可能含敏感数据。

场景三：perf 分析多线程程序（呼应第 3 章）
============================================

.. code-block:: bash

   cd source/code/chap03 && make thread_demo
   perf record -g ./thread_demo
   perf report --stdio | head -40

查看 CPU 时间是否均匀分布在 ``pthread_mutex_lock`` 与各线程函数。若锁竞争严重，``perf`` 会显示 mutex 占用高比例，可考虑缩小临界区或读写锁。

其他工具
========================

.. list-table::
   :header-rows: 1
   :widths: 18 42

   * - 工具
     - 用途
   * - tcpdump / Wireshark
     - 网络抓包
   * - bpftrace / bcc
     - eBPF 动态追踪
   * - vmstat / pidstat / sar
     - 系统资源统计
   * - /proc、/sys
     - 内核状态只读接口
   * - dmesg / journalctl
     - 内核与用户日志

选择建议：用户态问题先 ``strace``/``gdb``；性能问题 ``perf``；内核路径 ``ftrace``/``bpftrace``；网络 ``tcpdump``。

与正文章节对照
========================

.. list-table::
   :header-rows: 1
   :widths: 22 48

   * - 教程章节
     - 推荐工具
   * - 第 3 章 进程/线程
     - ``strace -f``、``gdb info threads``、``perf record -g``
   * - 第 4 章 内存
     - ``gdb`` core、``/proc/self/maps``、``valgrind``
   * - 第 6 章 设备
     - ``dmesg``、``udevadm monitor``、``modprobe -v``
   * - 第 7 章 网络
     - ``tcpdump``、``ss``、``bpftrace``
   * - 第 8 章 系统调用
     - ``strace -c``、``perf trace``
   * - 第 10 章 容器
     - ``nsenter``、``systemd-cgls``、``bpftool``
