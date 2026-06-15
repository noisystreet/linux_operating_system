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
     - 分页、虚拟内存发展史
   * - :doc:`02_addressing`
     - 分段/分页、x86-64 四级页表、大页
   * - :doc:`03_virtual_memory`
     - COW、``mmap``、缺页异常
   * - :doc:`04_linux_mm`
     - 伙伴系统、slab、zone、KSM
   * - :doc:`05_swap`
     - swap、zram、内存压力
   * - :doc:`lab_mem_tools`
     - ``free``、``/proc/meminfo``、``vmstat``、swap 实验
   * - :doc:`lab_mem_program`
     - ``malloc``、``maps``、段错误与 gdb
   * - :doc:`outlook`
     - MGLRU、CXL、DAMON

示例代码
==========================

- ``source/code/chap04/`` —— 见 :doc:`lab_mem_program`

手册页与内核文档
==========================

- ``man 2 mmap``、``man 2 brk``、``man 3 malloc`` —— :doc:`lab_mem_program`、:doc:`03_virtual_memory`
- ``man 5 proc`` —— ``/proc/self/maps``，见 :doc:`02_addressing` 与 :doc:`lab_mem_program`
- ``man 1 free``、``man 8 swapon`` —— :doc:`lab_mem_tools`、:doc:`05_swap`
- 内核 ``mm/page_alloc.c``、``mm/memory.c``、``mm/swap.c`` —— 附录 :doc:`/appendix/a3_references` 第 4 章路线

书籍与在线资料
==========================

- `OSTEP 第 13–22 章 <https://pages.cs.wisc.edu/~remzi/OSTEP/vm-intro.pdf>`_ —— 虚拟内存
- 《Linux 内核设计与实现》第 12–15 章 —— 内存管理
- 附录 :doc:`/appendix/a1_debug_tools` —— :doc:`lab_mem_program` SIGSEGV 与 core 分析

相关章节
==========================

- :doc:`/chapter03_process/index` —— 进程地址空间、COW 与 ``fork``
- :doc:`/chapter05_filesystem/index` —— page cache 与文件 I/O
- :doc:`/chapter10_virt/index` —— cgroup 内存限制（:doc:`outlook`）
