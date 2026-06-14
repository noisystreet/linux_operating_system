======================
Linux 内存管理
======================

虚拟内存需要内核在底层管理物理页框的分配、回收和映射。Linux 通过:strong:`伙伴系统` 管理物理页、:strong:`slab/SLUB` 分配小对象、:strong:`zone` 划分内存区域，并借助:strong:`KSM` 等技术提高利用率。本节介绍这些机制。

物理内存的组织
========================

Linux 将物理内存划分为:strong:`节点` （NUMA node）和:strong:`区域` （zone）。常见 zone 类型：

.. list-table::
   :header-rows: 1
   :widths: 18 52

   * - Zone
     - 用途
   * - ZONE_DMA
     - 低端内存，供老旧 ISA 设备 DMA 使用
   * - ZONE_DMA32
     - 32 位可寻址的 DMA 区域
   * - ZONE_NORMAL
     - 直接映射区，内核和部分用户页
   * - ZONE_MOVABLE
     - 可迁移页，便于内存热插拔和碎片整理

查看区域信息：

.. code-block:: bash

   cat /proc/zoneinfo

``zoneinfo`` 中的 ``nr_free_pages``、``low``、``min`` 等是页面回收的:strong:`水位线` （watermark）——空闲页低于 ``low`` 时内核开始回收，低于 ``min`` 时分配可能阻塞等待回收。

伙伴系统
========================

:strong:`伙伴系统` （buddy system）管理物理页框的分配与释放。空闲页按 2 的幂次大小组织成链表：order 0 为单页（4 KB），order 1 为 2 页连续块，order 10 为 1024 页（4 MB）块。

分配时：若请求 order-k 块，从对应链表取；若为空，从更高 order 分裂；释放时：尝试与:strong:`伙伴` 页合并为更大块。

伙伴系统实现位于 ``mm/page_alloc.c``，``alloc_pages()`` 是核心接口。分配 2^n 页时效率高，但可能产生:strong:`内部碎片` —— 申请 5 KB 需分配 8 KB（2 页）。

slab 与 SLUB 分配器
========================

内核大量分配小于一页的小对象（如 ``task_struct``、``inode``、网络缓冲区）。伙伴系统对此效率低。:strong:`slab 分配器` 为每种对象类型维护:strong:`缓存` （cache），预分配若干页并切成固定大小对象，分配释放几乎无碎片。

Linux 2.6.23 起默认使用:strong:`SLUB` （Unqueued Slab），简化了 slab 的队列管理。查看 slab 缓存：

.. code-block:: bash

   cat /proc/slabinfo | head -20
   slabtop    # 需安装，实时查看

``/proc/slabinfo`` 列出各缓存名称、活跃对象数、对象大小等。

进程的内存描述
========================

每个进程通过 ``mm_struct`` 描述其虚拟地址空间，通过 ``mmap`` 红黑树和链表管理各虚拟内存区域（VMA）。``/proc/<pid>/maps`` 和 ``smaps`` 反映这些信息。

关键指标：

.. list-table::
   :header-rows: 1
   :widths: 20 50

   * - 指标
     - 含义
   * - VmSize
     - 虚拟地址空间总大小
   * - VmRSS
     - 常驻物理内存（Resident Set Size）
   * - VmSwap
     - 被换出的页大小
   * - RssAnon
     - 匿名页（堆、栈）占用
   * - RssFile
     - 文件映射页占用

``VmSize`` 可远大于物理内存，``VmRSS`` 才是实际占用的 RAM。

页面回收
========================

当物理内存紧张，内核通过:strong:`页面回收` （page reclaim）释放页框：

1. :strong:`LRU` 算法：将页分为活跃/非活跃链表，优先换出非活跃页
2. :strong:`kswapd` 内核线程：后台异步回收，维持水位线
3. :strong:`direct reclaim` ：分配失败时，当前进程同步参与回收

可回收的页包括：文件缓存（page cache，干净页直接丢弃，脏页写回）、匿名页（需换出到 swap）。回收策略由 ``vm.swappiness`` 等参数调节。

KSM：内核同页合并
========================

:strong:`KSM` （Kernel Same-page Merging）扫描进程内存，发现内容相同的页则合并为一份只读共享页，写入时 COW 分裂。主要用于虚拟化场景——多个虚拟机运行相同操作系统时，大量页内容相同，KSM 可显著节省内存。

.. code-block:: bash

   # 查看 KSM 状态
   cat /sys/kernel/mm/ksm/run
   cat /sys/kernel/mm/ksm/pages_shared

CMA：连续内存分配器
========================

:strong:`CMA` （Contiguous Memory Allocator）预留一块物理连续内存，供 DMA 设备使用，同时允许用户态分配其中未使用的页。解决了"设备需要连续物理内存"与"伙伴系统易产生碎片"的矛盾，常见于 ARM 嵌入式和多媒体设备。

透明大页（THP）
========================

:strong:`透明大页` （Transparent Huge Pages）由内核自动将合适的 4 KB 页合并为 2 MB 大页，无需应用修改。可减少 TLB 缺失，但可能增加内存碎片和延迟抖动。部分数据库建议关闭 THP。

.. code-block:: bash

   cat /sys/kernel/mm/transparent_hugepage/enabled

Linux 内存管理涉及众多子系统。当物理内存不足时，换出到磁盘成为必要手段。下一节介绍 swap 和 zram 机制。
