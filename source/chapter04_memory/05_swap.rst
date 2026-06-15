======================
Swap 与内存压缩
======================

物理内存有限，当进程需要的内存超过可用 RAM 时，操作系统必须把部分页:strong:`换出` （swap out）到磁盘，需要时再:strong:`换入` （swap in）。本节介绍 swap 分区/文件、swappiness 策略，以及 zram 等内存压缩技术。

为什么需要 Swap
========================

Swap 的主要作用：

- :strong:`扩展有效内存` ：虚拟内存总和可超过物理 RAM
- :strong:`回收闲置页` ：长时间未访问的匿名页可换出，腾出 RAM 给活跃进程
- :strong:`休眠` （hibernate）：将内存内容写入 swap 后关机，开机时恢复

没有 swap 时，物理内存耗尽会触发 OOM Killer。有 swap 时，系统可通过换页缓解压力，但过度依赖 swap 会导致性能急剧下降——磁盘比内存慢几个数量级。

Swap 的实现
========================

Linux 支持两种 swap 载体：

:strong:`Swap 分区`
  磁盘上的独立分区，类型为 ``0x82`` （Linux swap）。性能较好，无文件系统开销。

:strong:`Swap 文件`
  普通文件作为 swap，灵活性高，可动态调整大小，无需重新分区。

.. code-block:: bash

   # 查看当前 swap
   swapon --show
   free -h

   # 创建 swap 文件（示例：1 GB）
   sudo dd if=/dev/zero of=/swapfile bs=1M count=1024
   sudo chmod 600 /swapfile
   sudo mkswap /swapfile
   sudo swapon /swapfile

   # 永久生效：写入 /etc/fstab
   # /swapfile none swap sw 0 0

``mkswap`` 在文件或分区头部写入 swap 元数据，``swapon`` 将其加入内核的 swap 设备列表。

换页流程
========================

当进程访问已被换出的页：

1. MMU 触发缺页，页表项标记为"在 swap 中"
2. 内核 ``do_swap_page()`` 从 swap 设备读入该页
3. 分配物理页框，更新页表，进程继续执行

换出时，内核选择 LRU 中非活跃的匿名页，写入 swap 槽位（swap slot），释放物理页框。``vmstat`` 的 ``si`` （swap in）和 ``so`` （swap out）反映换入换出速率。

.. code-block:: bash

   vmstat 1
   # si、so 持续非零说明内存压力大

swappiness
========================

:strong:`swappiness` 控制内核换出页的积极程度，取值 0–100：

- :strong:`0` ：尽量不换出，除非内存极度紧张
- :strong:`60` ：默认值，平衡匿名页和文件缓存的回收
- :strong:`100` ：积极换出匿名页

.. code-block:: bash

   cat /proc/sys/vm/swappiness
   # 临时修改
   sudo sysctl vm.swappiness=10

服务器工作负载常降低 swappiness，避免数据库等进程的内存被换出；桌面系统可保持默认。swappiness 不影响文件缓存的回收——脏页仍会写回磁盘。

ZRAM：内存压缩
========================

:strong:`ZRAM` （原 compcache）在内存中创建压缩块设备作为 swap 后端。换出时数据先压缩再存入 RAM 中的 zram 设备，而非磁盘。适合内存有限、无 SSD 或希望减少磁盘磨损的设备（如手机、树莓派）。

.. code-block:: bash

   # 查看 zram 状态（若启用）
   cat /sys/block/zram0/disksize
   zramctl

Ubuntu、Fedora 等在某些安装中默认启用 zram swap。压缩/解压消耗 CPU，但避免了慢速磁盘 I/O，在内存轻度超用时往往更优。

zswap 与 zsmalloc
========================

:strong:`zswap` 是另一种压缩 swap 前端：换出时先尝试压缩，仅当压缩后仍放不进预留的内存池才真正写入磁盘。与 zram 类似，但可与传统 swap 分区配合。

内核使用:strong:`zsmalloc` 等专用分配器管理压缩页，平衡压缩率和分配效率。

.. code-block:: bash

   # 查看 zswap 是否启用（需内核 CONFIG_ZSWAP=y）
   cat /sys/module/zswap/parameters/enabled
   # 压缩算法、占最大内存比例等
   ls /sys/module/zswap/parameters/

``max_pool_percent`` 限制 zswap 池占物理内存的比例，避免压缩本身耗尽 RAM。容器场景下，cgroup ``memory.max`` 触发回收时，zswap 可延缓对磁盘的换出，但 CPU 开销上升——需在延迟与容量之间权衡。

zram 与 zswap 选型参考：

.. list-table::
   :header-rows: 1
   :widths: 18 42

   * - 方案
     - 适用场景
   * - zram
     - 无磁盘或磁盘极慢的嵌入式/云主机 swap
   * - zswap
     - 有 swap 分区，希望减少磁盘 I/O 的桌面/服务器
   * - 传统 swap 文件
     - 大内存压力、需 hibernate 镜像

Swap 与性能
========================

.. warning::

   频繁 swap 是性能问题的信号。若 ``vmstat`` 中 ``si``/``so`` 持续较高，或 ``free`` 显示 swap 大量使用，应检查是否有内存泄漏、进程占用过大，或考虑增加物理内存。

监控命令：

.. code-block:: bash

   free -h
   vmstat 1
   cat /proc/meminfo | grep -i swap

``/proc/meminfo`` 中的 ``SwapTotal``、``SwapFree``、``SwapCached`` 提供详细 swap 统计。

休眠与 resume
========================

系统休眠（hibernate）时，内核将内存镜像写入 swap 分区/文件，然后关机。开机时引导加载程序加载该镜像，恢复内存状态，进程从休眠前继续运行。这需要 swap 大小至少等于物理内存。

Linux 内存管理从虚拟地址到物理页框、从伙伴系统到 swap，构成了完整的内存子系统。下一节通过命令和 C++ 程序观察内存布局与行为。
