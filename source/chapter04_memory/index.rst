==============
内存管理
==============

内存管理是操作系统的核心职责之一。本章从硬件寻址讲起，
逐步深入到 Linux 的虚拟内存、物理内存分配和页面回收机制。

学习目标
========

- 说明虚拟地址、页表、TLB 与 x86-64 四级页表翻译流程
- 解释伙伴系统、slab、page cache、swap 与 OOM 行为
- 用 ``free``、``vmstat``、``/proc/maps`` 观察内存（``lab_mem_*``）

先修要求：第 3 章进程地址空间；实验含 ``malloc``/``mmap`` C++ 程序。

.. toctree::
   :maxdepth: 2

   01_history
   02_addressing
   03_virtual_memory
   04_linux_mm
   05_swap
   lab_mem_tools
   lab_mem_program
   outlook
   references