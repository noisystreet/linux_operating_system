==========================
参考资料与补充阅读
==========================

本章内容索引
==========================

.. list-table::
   :header-rows: 1
   :widths: 28 52

   * - 节
     - 主题
   * - :doc:`01_history`
     - 系统调用演进
   * - :doc:`02_syscall_overview`
     - syscall 表、POSIX 与内核接口
   * - :doc:`03_impl_x86_64`
     - ``syscall`` 指令、寄存器约定
   * - :doc:`04_vdso`
     - vDSO、避免 syscall 的快速路径
   * - :doc:`05_write_syscall`
     - 添加自定义 syscall（文档说明）
   * - :doc:`lab_syscall_tools`
     - ``strace``、``perf``、seccomp 简介
   * - :doc:`lab_syscall_asm`
     - 内联汇编 ``syscall``、自定义 syscall 补丁
   * - :doc:`outlook`
     - seccomp、io_uring 与 syscall 批处理

示例代码
==========================

- ``source/code/chap08/raw_syscall.cpp`` —— :doc:`lab_syscall_asm`

手册页与内核文档
==========================

- ``man 2 syscalls``、``man 2 syscall`` —— 总览
- ``man 2 getpid``、``man 2 write`` —— :doc:`lab_syscall_asm` 示例
- ``man 1 strace``、``man 1 perf`` —— :doc:`lab_syscall_tools`
- 内核 ``arch/x86/entry/entry_64.S``、``arch/x86/entry/syscalls/syscall_64.tbl`` —— 附录 :doc:`/appendix/a3_references` 第 8 章路线

书籍与在线资料
==========================

- 《TLPI》第 6 章 —— 系统调用深入
- `Linux 内核文档：syscall user dispatch <https://docs.kernel.org/admin-guide/syscall-user-dispatch.html>`_
- 附录 :doc:`/appendix/a1_debug_tools` —— :doc:`lab_syscall_tools` 调试场景

相关章节
==========================

- :doc:`/chapter01_intro/lab_sysinfo` —— ``uname()``、``sysinfo()`` 的 syscall 本质
- :doc:`/chapter09_security/index` —— seccomp 过滤 syscall（:doc:`outlook`）
- :doc:`/chapter05_filesystem/06_io_uring` —— 异步 I/O 与 syscall 边界
