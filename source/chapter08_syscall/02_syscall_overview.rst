======================
系统调用概述
======================

应用程序通过系统调用请求内核服务：读写文件、创建进程、分配内存、发送网络数据等。本节介绍系统调用的概念、与库函数的关系、以及 Linux 系统调用的分类。

什么是系统调用
========================

**系统调用** 是用户程序主动触发的、从用户态切换到内核态的入口。触发后：

1. CPU 切换到内核栈和内核页表
2. 内核验证参数合法性
3. 执行对应内核函数
4. 返回结果到用户态寄存器

用户程序 **不能** 直接调用内核函数——内核运行在受保护的地址空间，只能通过 syscall 门进入。

系统调用 vs 库函数
========================

.. list-table::
   :header-rows: 1
   :widths: 20 38 38

   * - 类型
     - 示例
     - 说明
   * - 直接系统调用
     - write、read、open
     - 主要由 syscall 实现
   * - 库封装
     - printf、fopen
     - 用户态缓冲，最终可能调用 write
   * - 纯用户态
     - strlen、memcpy
     - 不进入内核

glibc 提供 C 标准库和 POSIX API，内部通过 ``syscall()`` 或汇编封装调用内核。``man 2 write`` 为系统调用，``man 3 printf`` 为库函数。

查看系统调用
========================

.. code-block:: bash

   strace -c ls
   strace -e trace=write echo hello

``strace`` 跟踪进程的系统调用，是理解程序行为的利器。``-c`` 统计各调用次数和耗时。

.. code-block:: bash

   perf stat -e 'syscalls:sys_enter_openat' ls

``perf`` 可统计特定系统调用的触发次数。

Linux 系统调用分类
========================

Linux 约有 300+ 个系统调用（随版本增加），按功能大致分为：

.. list-table::
   :header-rows: 1
   :widths: 22 48

   * - 类别
     - 示例
   * - 文件 I/O
     - openat、read、write、close、ioctl
   * - 进程
     - fork、execve、exit、wait4、getpid
   * - 内存
     - brk、mmap、munmap、mprotect
   * - 网络
     - socket、bind、connect、sendmsg
   * - 信号
     - kill、rt_sigaction、sigreturn
   * - 调度
     - sched_yield、clone
   * - 时间
     - clock_gettime、nanosleep

完整列表见内核文档 ``Documentation/admin-guide/syscall-user-dispatch.rst`` 或 ``man 2 syscalls``。

系统调用号
========================

每个系统调用有唯一 **系统调用号** 。x86-64 上 ``write`` 为 1，``read`` 为 0，``openat`` 为 257（具体以 ``unistd_64.h`` 为准）。用户将号放入 ``rax``，参数按 ABI 放入指定寄存器，执行 ``syscall`` 指令。

错误处理：内核返回负值表示错误码（如 ``-ENOENT``），glibc 包装为设置 ``errno`` 并返回 -1。

添加新系统调用
========================

添加系统调用需：

1. 在 ``syscall_64.tbl`` 分配新号
2. 实现 ``SYSCALL_DEFINE`` 内核函数
3. 在 ``unistd.h`` 添加 ``__NR_xxx``
4. 可选：glibc 包装为用户 API

内核模块 **不能** 动态添加系统调用——syscall 表在编译时确定。自定义 syscall 需修改内核源码并重新编译，或通过现有 syscall（如 ``ioctl``）扩展。

系统调用是架构相关的。下一节以 x86-64 为例，详解从 ``syscall`` 指令到内核处理函数的完整路径。
