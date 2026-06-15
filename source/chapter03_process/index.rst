==================
进程与线程管理
==================

进程是操作系统资源分配的基本单位，线程是 CPU 调度的基本单位。
本章深入 Linux 的进程管理、调度算法、同步机制和进程间通信。

学习目标
========

- 解释 ``fork``/``exec``、进程状态、僵尸进程与 ``task_struct`` 概念
- 理解 CFS/EEVDF 调度与 nice、cgroup CPU 限制的关系
- 使用 pthread 与管道/共享内存，掌握互斥锁与信号量（``lab_process_*``）

先修要求：第 4 章内存基础有助于理解 COW；C++17 与 ``-pthread``。

.. toctree::
   :maxdepth: 2

   01_history
   02_process_concept
   03_scheduling
   04_threads
   05_ipc
   06_sync
   lab_process_tools
   lab_process_program
   outlook
   references