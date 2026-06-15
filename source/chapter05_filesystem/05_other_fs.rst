======================================
其他文件系统：XFS、Btrfs 与 tmpfs
======================================

ext4 是多数发行版的默认选择，但不同场景需要不同文件系统。本节深入介绍企业级常用的 XFS、现代化的 Btrfs，以及基于内存的 tmpfs。

选型概览
========================

.. list-table::
   :header-rows: 1
   :widths: 18 38 38

   * - 文件系统
     - 典型场景
     - 关键特性
   * - ext4
     - 通用桌面/服务器根分区
     - 成熟稳定、延迟分配、extent
   * - XFS
     - 大文件、高并发 I/O、数据库
     - B+ 树空闲空间、在线扩容
   * - Btrfs
     - 快照、子卷、校验、家庭 NAS
     - COW、压缩、RAID（谨慎用于生产）
   * - tmpfs
     - ``/dev/shm``、临时文件、容器可写层
     - 纯内存、极快、重启丢失

.. code-block:: bash

   df -Th
   lsblk -f
   findmnt -D

XFS
========================

**XFS** 由 SGI 为 IRIX 开发，现是 RHEL/Rocky/Alma 的默认文件系统。设计目标是大文件和高吞吐并行 I/O。

核心特点
------------------------

- **B+ 树** 管理空闲 extent 和 inode，分配与查找效率高
- **分配组** （allocation groups）将磁盘分区为多个独立单元，减少锁竞争
- **延迟日志** ：元数据日志，崩溃后快速恢复
- **在线扩容** ：``xfs_growfs`` 无需卸载即可扩大文件系统

常用命令
------------------------

.. code-block:: bash

   # 创建 XFS（在分区或 loop 设备上）
   sudo mkfs.xfs /dev/sdb1

   # 挂载
   sudo mount /dev/sdb1 /mnt/xfs

   # 查看超级块与分配组信息
   sudo xfs_info /mnt/xfs
   sudo xfs_db -r -c "sb 0" -c "print" /dev/sdb1

   # 在线扩容（先扩展分区/LV）
   sudo xfs_growfs /mnt/xfs

   # 碎片整理（通常较少需要）
   sudo xfs_fsr /mnt/xfs

内核实现位于 ``fs/xfs/``。XFS 对大量小文件的场景不如 ext4 普遍，但在视频存储、HPC、数据库数据目录中常见。

Btrfs
========================

**Btrfs** （B-tree FS）是 COW（写时复制）文件系统：修改数据时写入新块，旧块保留至无引用，天然支持快照与克隆。

核心特点
------------------------

- **子卷** （subvolume）：逻辑分区，可独立快照
- **快照** ：只读或读写，瞬间创建，空间按实际变更增长
- **校验和** ：检测静默数据损坏
- **压缩** ：透明 zstd/lzo 压缩
- **RAID 0/1/10/5/6** ：用户态 RAID，RAID5/6 仍有已知风险，生产需谨慎

常用命令
------------------------

.. code-block:: bash

   # 创建 Btrfs
   sudo mkfs.btrfs /dev/sdc1

   # 挂载（可指定子卷）
   sudo mount /dev/sdc1 /mnt/btrfs
   sudo btrfs subvolume create /mnt/btrfs/data
   sudo btrfs subvolume list /mnt/btrfs

   # 快照
   sudo btrfs subvolume snapshot /mnt/btrfs/data /mnt/btrfs/data_snap

   # 查看使用率（含压缩比）
   sudo btrfs filesystem df /mnt/btrfs
   sudo btrfs filesystem usage /mnt/btrfs

   # 启用压缩挂载
   sudo mount -o compress=zstd /dev/sdc1 /mnt/btrfs

内核实现位于 ``fs/btrfs/``。openSUSE、Fedora 可选 Btrfs 为根文件系统；Docker 的 ``btrfs`` 存储驱动曾直接使用 Btrfs 子卷（现多 overlay2）。

tmpfs
========================

**tmpfs** 将文件存储在 **页缓存/内存** 中，不落盘（swap 除外）。速度快，适合临时数据；重启或卸载后内容丢失。

典型挂载点
------------------------

.. code-block:: bash

   mount | grep tmpfs
   # /dev/shm      —— POSIX 共享内存
   # /run          —— 运行时状态（PID 文件、socket）
   # /tmp          —— 部分发行版将 /tmp 设为 tmpfs

手工挂载示例
------------------------

.. code-block:: bash

   sudo mkdir -p /mnt/ramdisk
   sudo mount -t tmpfs -o size=256M tmpfs /mnt/ramdisk
   df -h /mnt/ramdisk

   # 写入测试
   dd if=/dev/zero of=/mnt/ramdisk/test bs=1M count=100
   free -h    # 可见内存占用上升

**size=** 限制最大用量，防止耗尽内存。容器中的 ``tmpfs`` 挂载（如 ``/dev/shm``）同样受此约束。

与 page cache 的关系
------------------------

tmpfs 页面参与正常的 LRU 回收；内存紧张时可被换出到 swap。因此 tmpfs 并非"免费内存"——大文件仍会占用物理 RAM 或 swap。

内核实现位于 ``mm/shmem.c``，通过 VFS 接口挂载为 ``tmpfs`` 类型。

对比实验（可选）
========================

在测试环境用 loop 设备对比 ext4 与 tmpfs 顺序写：

.. code-block:: bash

   # ext4 on loop
   dd if=/dev/zero of=/tmp/ext4.img bs=1M count=512
   mkfs.ext4 /tmp/ext4.img
   mkdir -p /mnt/ext4test && sudo mount -o loop /tmp/ext4.img /mnt/ext4test
   dd if=/dev/zero of=/mnt/ext4test/big bs=1M count=256 oflag=direct

   # tmpfs
   sudo mount -t tmpfs -o size=512M tmpfs /mnt/ramdisk
   dd if=/dev/zero of=/mnt/ramdisk/big bs=1M count=256

tmpfs 通常显著更快，但不持久。数据库临时表空间、编译缓存（``ccache`` 目录放 tmpfs）是常见用法。

文件内容读写如何经过内核到达磁盘？涉及 page cache 和 I/O 路径——下一节介绍 io_uring 与现代异步 I/O。
