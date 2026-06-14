==============
系统调用
==============

系统调用是用户程序进入内核的唯一入口。本章详解 Linux 系统调用的
实现机制，包括入口处理、参数传递和返回流程。

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