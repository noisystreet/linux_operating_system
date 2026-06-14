======================
系统调用发展史
======================

用户程序不能直接操作硬件或访问内核内存。:strong:`系统调用` （system call）是用户态进入内核态、请求内核服务的唯一受控入口。系统调用机制的设计与演进，体现了操作系统对安全与性能的持续权衡。

早期：监控程序调用
========================

1960 年代，IBM System/360 通过:strong:`监控程序调用` （SVC）从用户态陷入操作系统。参数通过寄存器传递，内核执行后返回用户态。这一模式成为后续系统的原型。

PDP-11 上的 Unix 使用:strong:`trap` 指令触发系统调用，系统调用号放在寄存器中。简洁的接口设计（open、read、write、close）沿用至今。

x86 的 int 0x80
========================

早期 Linux/x86 使用 ``int 0x80`` 触发系统调用：

.. code-block:: text

   用户程序: mov eax, __NR_write; int 0x80
   CPU: 跳转到中断描述符表 IDT 中 0x80 号项
   内核: 系统调用处理程序

``int 0x80`` 是软件中断，开销较大（需查 IDT、切换栈），且与硬件中断共用机制。

x86-64 的 syscall 指令
========================

AMD 在 x86-64 中引入:strong:`syscall` 指令，Intel 通过 ``sysenter`` 支持类似功能。Linux x86-64 采用 ``syscall``/``sysret`` 配对：

- 更快：专用指令，无需查 IDT
- 系统调用号放 ``rax``，参数放 ``rdi``、``rsi``、``rdx``、``r10``、``r8``、``r9``
- 内核入口在 ``entry_SYSCALL_64`` （``arch/x86/entry/entry_64.S``）

这是现代 Linux 桌面和服务器的主要路径。

vDSO 与快速路径
========================

部分系统调用无需进入内核，或可在用户态完成：

- :strong:`vsyscall` 页（已废弃）：旧式快速 gettimeofday
- :strong:`vDSO` （virtual Dynamic Shared Object）：内核映射到用户空间的共享库，提供 ``clock_gettime``、``getcpu`` 等，避免上下文切换

vDSO 是性能优化的典范——高频、只读类调用在用户态完成。

系统调用数量与稳定 ABI
========================

Linux 系统调用号由架构定义，x86-64 见 ``arch/x86/entry/syscalls/syscall_64.tbl``。新系统调用追加在末尾，:strong:`不删除、不改变已有号` ，保证二进制兼容性。

.. code-block:: bash

   # 查看系统调用号定义（若安装了内核头文件）
   grep write /usr/include/asm/unistd_64.h

POSIX 定义标准 API（如 ``pthread_create``），部分由库函数实现，部分直接对应系统调用。glibc 的 ``write()`` 最终调用 ``syscall(SYS_write, ...)``。

seccomp 与过滤
========================

:strong:`seccomp` （secure computing）限制进程可使用的系统调用，容器和沙箱（Chrome、systemd）用它缩小攻击面。违反规则时进程被 ``SIGKILL`` 终止。

系统调用是用户与内核的边界。下一节介绍系统调用的概念、POSIX 接口和 Linux 的实现概览。
