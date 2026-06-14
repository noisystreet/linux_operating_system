================================
实践：内联汇编系统调用
================================

用 C++ 内联汇编直接执行 ``syscall`` 指令，调用 ``getpid`` 和 ``write``，理解寄存器约定和返回值处理。

.. note::

   本节 ``raw_syscall.cpp`` 完整源码见 ``source/code/chap08/`` 。在 ``source/code`` 目录执行 ``make user`` 可一键编译。

syscall 宏封装
==========================

x86-64 Linux 系统调用约定：号在 ``rax``，参数 ``rdi``、``rsi``、``rdx``、``r10``、``r8``、``r9``，返回值在 ``rax``。

.. code-block:: cpp

   #include <iostream>
   #include <unistd.h>
   #include <sys/syscall.h>
   #include <cstring>

   // 内联汇编直接 syscall
   static long my_syscall3(long nr, long a1, long a2, long a3) {
       long ret;
       __asm__ volatile (
           "syscall"
           : "=a"(ret)
           : "a"(nr), "D"(a1), "S"(a2), "d"(a3)
           : "rcx", "r11", "memory"
       );
       return ret;
   }

   int main() {
       // getpid: __NR_getpid 通常为 39
       long pid = my_syscall3(SYS_getpid, 0, 0, 0);
       std::cout << "getpid (syscall): " << pid << "\n";
       std::cout << "getpid (libc):    " << getpid() << "\n";

       const char* msg = "Hello via raw syscall!\n";
       long n = my_syscall3(SYS_write, STDOUT_FILENO,
                            (long)msg, strlen(msg));
       std::cout << "write 返回: " << n << " 字节\n";

       return 0;
   }

编译运行：

.. code-block:: bash

   g++ -std=c++17 -Wall -o raw_syscall raw_syscall.cpp
   ./raw_syscall

``SYS_getpid``、``SYS_write`` 来自 ``sys/syscall.h``，与内核 ``__NR_*`` 一致。

验证无 libc 封装（可选）
==========================

用 strace 对比：

.. code-block:: bash

   strace -e getpid,write ./raw_syscall

应看到与程序输出一致的 ``getpid()`` 和 ``write(1, ...)`` 调用。

错误处理
==========================

系统调用返回负值表示错误（-errno）：

.. code-block:: cpp

   long ret = my_syscall3(SYS_open, (long)"/nonexistent", O_RDONLY, 0);
   if (ret < 0)
       std::cout << "open 失败, errno=" << -ret << "\n";

glibc 的 ``syscall()`` 函数提供类似功能，生产代码应使用标准 API 而非手写汇编。

自定义 syscall 模块（进阶说明）
======================================

在内核源码树中新增系统调用需重新编译内核，步骤概览：

1. 编辑 ``arch/x86/entry/syscalls/syscall_64.tbl`` 添加行
2. 在 ``kernel/`` 下实现 ``SYSCALL_DEFINE*``
3. 编译安装内核，用户态 ``#include <asm/unistd.h>`` 使用新 ``__NR_``

.. warning::

   修改内核并加载自定义 syscall 仅适合实验环境。发行版内核通常不保留可插拔的 syscall 槽位，:strong:`ioctl` 和 netlink 是更常见的内核-用户扩展方式。

自定义 syscall 补丁示例（仅文档）
====================================

以下展示在主线内核源码中添加 ``hello`` 系统调用的典型步骤，:strong:`不纳入本仓库构建` ，仅供实验内核参考。

**1. 注册系统调用号** （``arch/x86/entry/syscalls/syscall_64.tbl``）：

.. code-block:: text

   548 common  hello   sys_hello

**2. 实现内核函数** （``kernel/hello.c``）：

.. code-block:: c

   #include <linux/syscalls.h>
   #include <linux/uaccess.h>

   SYSCALL_DEFINE2(hello, char __user *, buf, int, len)
   {
       char kbuf[64];
       if (len <= 0 || len >= sizeof(kbuf))
           return -EINVAL;
       if (copy_from_user(kbuf, buf, len))
           return -EFAULT;
       pr_info("hello syscall: %.*s\n", len, kbuf);
       return len;
   }

**3. 加入构建** （``kernel/Makefile``）：``obj-y += hello.o``

**4. 重新编译安装内核** 后，用户态调用：

.. code-block:: cpp

   #include <sys/syscall.h>
   #include <unistd.h>

   #ifndef __NR_hello
   #define __NR_hello 548   // 与 tbl 中编号一致
   #endif

   syscall(__NR_hello, "world", 5);

每次内核升级需重新打补丁。生产环境更推荐通过现有 syscall（``ioctl``、``netlink``、``bpf``）扩展功能。

与第 1 章的联系
==========================

第 1 章 ``lab_sysinfo`` 使用 ``uname()``、``sysinfo()``——底层即系统调用。本章揭示了这些调用的进入方式：用户态设置寄存器 → ``syscall`` → ``sys_call_table`` → 内核实现 → 返回用户态。

系统调用是用户与内核的边界。下一章讨论:strong:`安全与权限` —— 内核如何决定谁可以执行哪些操作。

拓展阅读
==========

- ``man 2 syscall``、``man 2 syscalls``
- 内核源码 ``arch/x86/entry/entry_64.S``、``include/linux/syscalls.h``
- 《Linux 内核设计与实现》系统调用章节
