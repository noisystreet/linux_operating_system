======================
延伸与趋势
======================

进程、线程、调度与同步是操作系统永恒主题。Linux 的 ``fork``、CFS、``futex``、``pthread`` 仍是服务器与桌面程序运行的基石；前沿工作主要在:strong:`可观测性` 、:strong:`可插拔调度` 与:strong:`异步执行模型` 上扩展。

工业界现状
==========================

CFS 公平调度器处理绝大多数通用负载；``NPTL`` 线程库基于 ``clone`` 与 ``futex`` 实现。本章 ``fork_demo``、``thread_demo`` 演示的 API 在生产代码中仍广泛使用。

值得关注的变化
==========================

- :strong:`sched_ext` ：基于 BPF 的可插拔调度框架，允许在不修改核心 CFS 的前提下实验调度策略，与第 7 章 eBPF、第 3 章 ``06_sync`` 中 RCU 读多写少模型形成技术栈呼应。
- :strong:`eBPF 调度观测` ：``bpftrace`` 跟踪 ``sched_switch``、``sched_wakeup`` 等 tracepoint，替代部分 ``ftrace`` 脚本，低开销分析延迟与锁竞争（附录 :doc:`/appendix/a1_debug_tools`）。
- :strong:`io_uring 与任务工作` ：高并发服务将部分「等待 I/O」与任务提交合并到 io_uring 环（第 5 章），减少线程数与上下文切换，改变「一连接一线程」传统模型。
- :strong:`用户态运行时` ：Go、Rust async runtime 在用户态调度 goroutine/task，底层仍依赖内核线程与 ``futex``；理解内核调度有助于解释 GOMAXPROCS、p99 延迟尖刺。

与本教程的衔接
==========================

``lab_process_program`` 与 ``source/code/chap03/`` 覆盖 fork、管道、互斥锁；进阶可结合 ``perf record -g ./thread_demo`` 观察锁竞争（附录场景三）。

进一步了解
==========================

- 内核 ``Documentation/scheduler/``、``Documentation/bpf/sched-ext.rst``
- LWN 系列：CFS、runqueues、PI futex
- 第 4 章内存与第 8 章 syscall 是理解 ``fork`` COW、``futex`` 进入内核的下一站
- 本章 :doc:`references` —— 进程/线程 API 手册页与源码阅读路线

.. note::

   本节约 2025–2026 年主线内核与工业实践撰写。
