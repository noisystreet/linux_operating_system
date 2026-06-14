======================
延伸与趋势
======================

虚拟地址、页表、伙伴系统与 swap 构成的内存管理模型在可预见未来仍是 Linux 核心。前沿重点在:strong:`更大容量` 、:strong:`更智能回收` 与:strong:`内存/存储融合` 。

工业界现状
==========================

四级页表 + THP（透明大页）+ LRU 回收是云主机与物理服务器的默认路径。``/proc/meminfo``、``vmstat`` 仍是 SRE 日常工具（本章 ``lab_mem_tools``）。

值得关注的变化
==========================

- :strong:`MGLRU 与多代 LRU` ：改进文件缓存与匿名页的回收精度，减少「缓存回收过猛伤害性能」问题。阅读 ``mm/vmscan.c`` 时可对照经典 LRU 双链表模型（第 4 章 ``04_linux_mm``）。
- :strong:`CXL 与内存分层` ：Compute Express Link 使内存池可跨设备扩展，内核发展 memtier、HMAT 感知 NUMA 距离，模糊 DRAM 与扩展内存边界（呼应 ``01_history`` 中持久内存提及）。
- :strong:`DAMON` ：数据访问监控框架，按访问模式优化回收、预取或压缩，适合大内存服务器与容器密度场景。
- :strong:`zswap / zram 演进` ：嵌入式与云主机继续用压缩换容量；与 cgroup ``memory.high`` 配合做容器限流（第 10 章）。

与本教程的衔接
==========================

``malloc_demo``、``mmap_cow`` 帮助理解堆、映射与 COW；页表遍历见 ``02_addressing`` Mermaid 图。OOM 与 cgroup 杀进程见第 10 章 cgroup 实验。

进一步了解
==========================

- 内核 ``Documentation/admin-guide/mm/``、``Documentation/mm/damon/``
- 第 3 章进程地址空间、第 5 章 page cache 与内存压力联动
- LWN：MGLRU、memtiering 专题

.. note::

   本节约 2025–2026 年方向撰写；硬件支持因平台而异。
