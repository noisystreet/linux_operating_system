================================
实践：内存管理命令操作
================================

内存是看不见摸不着的，但 Linux 通过 ``/proc`` 和各类命令暴露了丰富的内存信息。本节用命令观察系统内存布局、换页活动和区域水位线。

系统内存概览
==========================

.. code-block:: bash

   free -h

输出示例：

.. code-block:: text

                  total        used        free      shared  buff/cache   available
   Mem:           15Gi       4.2Gi       6.1Gi       256Mi       5.0Gi        10Gi
   Swap:         2.0Gi          0B       2.0Gi

:strong:`available` 比 ``free`` 更有意义——它估算在不触发 swap 的情况下可用于新申请的内存（含可回收的 cache）。``buff/cache`` 是文件缓存，需要时可被回收。

详细内存信息
==========================

.. code-block:: bash

   cat /proc/meminfo

常用字段：

.. list-table::
   :header-rows: 1
   :widths: 25 45

   * - 字段
     - 含义
   * - MemTotal
     - 总物理内存
   * - MemFree
     - 完全空闲
   * - MemAvailable
     - 可用内存估算
   * - Buffers / Cached
     - 缓冲区与页缓存
   * - SwapTotal / SwapFree
     - swap 总量与空闲
   * - Dirty
     - 待写回磁盘的脏页
   * - AnonPages
     - 匿名页（堆、栈）占用

观察换页活动
==========================

.. code-block:: bash

   vmstat 1 5

关注列：

- ``si`` / ``so`` ：每秒换入/换出页数（KB），非零表示内存压力
- ``free`` ：空闲内存
- ``buff`` / ``cache`` ：缓冲区与缓存
- ``us`` / ``sy`` / ``id`` / ``wa`` ：CPU 使用率

在另一个终端运行 ``stress-ng --vm 2 --vm-bytes 1G`` （需安装）制造内存压力，观察 ``si``/``so`` 和 ``free`` 的变化。

内存区域与水位线
==========================

.. code-block:: bash

   cat /proc/zoneinfo | head -80

每个 NUMA 节点、每个 zone 有独立的 ``nr_free_pages``、``low``、``min``、``high`` 水位线。当 ``nr_free_pages`` 低于 ``low`` 时，``kswapd`` 开始回收页面。

进程内存占用
==========================

.. code-block:: bash

   # 按内存排序
   ps aux --sort=-%mem | head -10

   # 某进程详细内存
   cat /proc/self/status | grep -E 'Vm|Rss'

   # 进程的内存映射
   cat /proc/self/maps

Swap 管理实验
==========================

.. warning::

   以下 swap 文件操作需要 root 权限。在虚拟机或测试环境中实验，勿在生产系统随意操作。

.. code-block:: bash

   # 创建 256 MB swap 文件
   sudo dd if=/dev/zero of=/tmp/test_swap bs=1M count=256
   sudo chmod 600 /tmp/test_swap
   sudo mkswap /tmp/test_swap
   sudo swapon /tmp/test_swap

   swapon --show
   free -h

   # 实验后关闭并删除
   sudo swapoff /tmp/test_swap
   sudo rm /tmp/test_swap

swappiness 实验
==========================

.. code-block:: bash

   cat /proc/sys/vm/swappiness
   # 临时设为 10（更倾向保留匿名页在内存）
   sudo sysctl vm.swappiness=10

大页与 slab
==========================

.. code-block:: bash

   grep -i huge /proc/meminfo
   cat /proc/slabinfo | head -15

下一节通过 C++ 程序分配内存、观察 ``/proc/self/maps``，并体验段错误与 core dump 分析。
