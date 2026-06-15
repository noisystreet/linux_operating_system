==============
系统调用
==============

系统调用是用户程序进入内核的唯一入口。本章详解 Linux 系统调用的
实现机制，包括入口处理、参数传递和返回流程。

学习目标
========

- 说明 x86-64 ``syscall`` 指令、寄存器 ABI 与 ``sys_call_table`` 查表流程
- 区分 glibc 封装、vDSO 快速路径与真实陷入内核的调用
- 用 ``strace``/``perf`` 观察 syscall，并用内联汇编直接调用（``lab_syscall_*``）

先修要求：第 5 章 VFS 有助于理解 ``write`` 追踪；建议先读第 3 章进程概念。

.. toctree::
   :maxdepth: 2

   01_history
   02_syscall_overview
   03_impl_x86_64
   04_vdso
   05_write_syscall
   lab_syscall_tools
   lab_syscall_asm
   outlook
   references