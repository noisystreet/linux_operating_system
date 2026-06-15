==========
附录
==========

附录提供调试排障、内核编译、深度阅读路线与全书「延伸与趋势」索引，建议在完成对应章节实验后按需查阅。

附录导读
========

.. list-table::
   :header-rows: 1
   :widths: 22 48

   * - 文档
     - 适用场景
   * - :doc:`a1_debug_tools`
     - 实验失败、程序崩溃、性能异常；strace/perf/gdb 场景化 walkthrough
   * - :doc:`a2_build_kernel`
     - 需自定义内核或排查模块编译；**务必在虚拟机操作**
   * - :doc:`a3_references`
     - 按章节列出内核源码路径与外部文档，供专题深入
   * - :doc:`a4_outlook_index`
     - 全书各章 outlook 汇总入口，了解前沿方向与动手延伸

推荐查阅路径
============

- 第 3 章 ``lab_process_program`` 段错误 → :doc:`a1_debug_tools` 场景二（gdb + core）
- 第 6 章 ``hello_chardev`` 编译失败 → :doc:`a2_build_kernel` 中「仅编译模块」一节，确认 ``linux-headers`` 版本
- 第 7 章 Netfilter/eBPF 实验后 → :doc:`a3_references` 第 7 章表格，定位 ``net/`` 源码
- 第 8 章内联汇编 syscall → :doc:`a3_references` 第 8 章 + ``man 2 syscalls``
- 读完任意章 outlook → :doc:`a4_outlook_index` 跳转其他章趋势

环境与安全提示
==============

- 涉及 **root**、**内核模块**、**swap 文件**、**自定义内核** 的操作，默认在虚拟机或专用测试机进行
- 模块实验后务必 ``rmmod``；Netfilter 模块误设为 ``NF_DROP`` 可能导致本机 TCP 不通
- 自定义内核保留 GRUB 旧内核启动项，详见 :doc:`a2_build_kernel` 故障恢复节

.. toctree::
   :maxdepth: 2

   a1_debug_tools
   a2_build_kernel
   a3_references
   a4_outlook_index
