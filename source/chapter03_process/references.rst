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
     - 进程模型演进、Multics → Unix
   * - :doc:`02_process_concept`
     - ``task_struct``、进程状态、``fork``、``/proc``
   * - :doc:`03_scheduling`
     - O(1)、CFS、实时调度、``nice``
   * - :doc:`04_threads`
     - NPTL、``clone``、线程与进程区别
   * - :doc:`05_ipc`
     - 管道、信号、共享内存、消息队列
   * - :doc:`06_sync`
     - 互斥锁、信号量、RCU、futex
   * - :doc:`lab_process_tools`
     - ``ps``、``strace``、进程 ``/proc`` 目录
   * - :doc:`lab_process_program`
     - ``fork``、管道、``pthread``、``mmap`` 共享
   * - :doc:`outlook`
     - sched_ext、eBPF 调度观测

示例代码
==========================

- ``source/code/chap03/`` —— 见 :doc:`lab_process_program`

手册页与内核文档
==========================

- ``man 2 fork``、``man 2 wait``、``man 2 pipe`` —— :doc:`lab_process_program`
- ``man 3 pthread_create``、``man 3 pthread_mutex_lock`` —— :doc:`04_threads`、:doc:`06_sync`
- ``man 2 mmap``、``man 5 proc`` —— :doc:`05_ipc`、:doc:`02_process_concept`
- ``man 7 signal`` —— :doc:`05_ipc` 信号机制
- 内核 ``kernel/fork.c``、``kernel/sched/fair.c``、``kernel/futex.c`` —— 附录 :doc:`/appendix/a3_references` 第 3 章路线

书籍与在线资料
==========================

- 《Unix 环境高级编程》第 8–12、14 章 —— 进程、线程、IPC
- `OSTEP 第 4–7 章 <https://pages.cs.wisc.edu/~remzi/OSTEP/threads-intro.pdf>`_ —— 线程与调度概念
- 附录 :doc:`/appendix/a1_debug_tools` —— :doc:`lab_process_tools` 中 ``strace``、``gdb`` 场景

相关章节
==========================

- :doc:`/chapter04_memory/index` —— :doc:`02_process_concept` 地址空间、COW（:doc:`lab_process_program` 后文）
- :doc:`/chapter08_syscall/index` —— ``fork``、``futex`` 的 syscall 路径
- :doc:`/chapter10_virt/index` —— PID namespace（:doc:`outlook` 中容器）
