================================
实践：系统调用跟踪与分析
================================

用 strace 和 perf 观察程序的系统调用行为，查看 syscall 表和 vDSO 映射，巩固系统调用理论知识。

strace 统计
==========================

.. code-block:: bash

   strace -c ls /tmp
   strace -c -f sleep 1

``-c`` 汇总各系统调用的次数、错误、耗时百分比。观察 ``openat``、``getdents64``、``write`` 等文件相关调用。

跟踪特定调用
==========================

.. code-block:: bash

   strace -e trace=openat,read,write,close cat /etc/hostname
   strace -e trace=network curl -s http://example.com 2>&1 | head -20
   strace -e trace=process fork_demo    # 若有第 3 章 fork 程序

``-f`` 跟踪子进程，``-p PID`` 附加到运行中进程。

perf 跟踪 syscall
==========================

.. code-block:: bash

   perf stat -e 'syscalls:sys_enter_openat' ls
   perf trace -e syscalls:sys_enter_write echo test

``perf trace`` 类似 strace，基于内核 tracepoint，开销较低。需内核支持 ``CONFIG_FTRACE_SYSCALL``。

查看系统调用表符号
==========================

.. code-block:: bash

   sudo cat /proc/kallsyms | grep sys_call_table
   sudo cat /proc/kallsyms | grep ' sys_write$'

若输出为空，可能启用了 ``kptr_restrict``：

.. code-block:: bash

   cat /proc/sys/kernel/kptr_restrict
   # 临时允许（仅调试环境）
   sudo sysctl kernel.kptr_restrict=0

vDSO 映射
==========================

.. code-block:: bash

   cat /proc/self/maps | grep -E 'vdso|vvar'
   ldd `which date`

``[vdso]`` 和 ``[vvar]`` 为内核映射的虚拟 DSO 和时间变量页。

系统调用号头文件
==========================

.. code-block:: bash

   grep -E 'define __NR_(read|write|open|getpid)' \
       /usr/include/asm/unistd_64.h 2>/dev/null || \
   grep -E 'define __NR_(read|write|open|getpid)' \
       /usr/include/x86_64-linux-gnu/asm/unistd_64.h

确认 ``__NR_write`` 为 1，``__NR_read`` 为 0 等（具体以系统头文件为准）。

seccomp 示例（可选）
==========================

.. code-block:: bash

   # 限制只能使用 read/write/exit 等（需编写 seccomp 程序或使用 systemd-run）
   systemd-run --scope -p SystemCallFilter=@system-service true

容器默认带有 seccomp 配置文件，限制危险系统调用。

下一节用 C++ 内联汇编直接发起 ``syscall`` 指令，绕过 libc 封装。
