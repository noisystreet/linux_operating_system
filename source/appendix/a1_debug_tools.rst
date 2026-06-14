======================
调试工具
======================

理解操作系统离不开观察内核与进程行为。本附录汇总教程中涉及的及常用的 Linux 调试与追踪工具，便于按场景选用。

strace / ltrace
========================

:strong:`strace` ：跟踪:strong:`系统调用` 和信号。

.. code-block:: bash

   strace ls
   strace -p <pid>
   strace -c -f command

:strong:`ltrace` ：跟踪:strong:`库函数` 调用（动态链接）。

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
