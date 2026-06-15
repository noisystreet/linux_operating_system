======================
vDSO 与快速系统调用
======================

每次系统调用都涉及用户态与内核态切换，开销约数百纳秒到数微秒。对于 ``gettimeofday``、``clock_gettime`` 等高频调用，Linux 通过 **vDSO** 在用户态提供实现，避免陷入内核。本节介绍 vDSO 的原理和使用。

为什么需要 vDSO
========================

时间查询、CPU 编号等操作极其频繁。若每次都 ``syscall``：

- 保存/恢复寄存器
- 切换页表、栈
- TLB 刷新风险
- 内核路径执行

累积开销可观。vDSO 将部分逻辑放到用户态可执行的共享页中。

vDSO 是什么
========================

**vDSO** （virtual Dynamic Shared Object）是内核在进程地址空间映射的一个特殊 ELF 共享库，通常位于 ``[vdso]`` 区域：

.. code-block:: bash

   cat /proc/self/maps | grep vdso

每个进程自动获得 vDSO 映射，无需 ``dlopen``。动态链接器（ld.so）将 libc 中的 ``clock_gettime`` 等符号解析到 vDSO 中的实现。

vDSO 提供的函数
========================

常见 vDSO 导出（因架构和内核版本而异）：

.. list-table::
   :header-rows: 1
   :widths: 25 45

   * - 符号
     - 作用
   * - __vdso_clock_gettime
     - 高精度时间
   * - __vdso_gettimeofday
     - 传统时间（若支持）
   * - __vdso_getcpu
     - 当前 CPU 编号
   * - __vdso_time
     - time() 快速路径

实现通常读取 **vvar** 页（与 vDSO 配套的内核更新页）中的时间数据，由内核在时钟中断时更新，用户态只读无需 syscall。

查看 vDSO 符号
========================

.. code-block:: bash

   find /usr/lib -name '*vdso*' 2>/dev/null
   # 或查看进程 maps 中 vdso 地址后用 readelf
   readelf -s /proc/self/exe 2>/dev/null | grep vdso

更直接：

.. code-block:: bash

   getconf AUXV 33    # AT_SYSINFO_EHDR，vDSO 入口（部分系统）

对比 syscall 与 vDSO
========================

.. code-block:: bash

   # 用 strace 观察 clock_gettime 是否产生 syscall
   strace -e clock_gettime date

若 vDSO 生效，``strace`` 可能看不到 ``clock_gettime`` 系统调用——库直接调用了 vDSO 实现。

.. code-block:: bash

   ldd `which date`
   # 确认动态链接

静态链接程序无 vDSO，每次时间查询都可能 syscall。

vsyscall 页的废弃
========================

早期 x86-64 有固定地址的 **vsyscall** 页（``0xffffffffff600000``），提供快速 ``gettimeofday``。因安全顾虑（固定地址、无法 ASLR），已被 vDSO 取代。``CONFIG_X86_VSYSCALL_EMULATION`` 可禁用遗留模拟。

seccomp 与 vDSO
========================

seccomp 过滤系统调用时，vDSO 调用不受影响——它们未进入内核。沙箱设计时需知：禁止 ``clock_gettime`` syscall 不等于禁止获取时间，vDSO 仍可用。

手写 syscall 与 vDSO
========================

直接用汇编 ``syscall`` 调用 ``getpid`` 等无 vDSO 替代的函数，是理解系统调用机制的好练习——下一节实践将编写此类程序，并以 ``write`` 为例追踪完整内核路径。
