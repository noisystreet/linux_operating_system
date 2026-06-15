======================
延伸与趋势
======================

VFS + inode/dentry + 具体文件系统（ext4/XFS/Btrfs）的分层仍是 Linux 存储栈主干。近年最显著的变化是:strong:`异步 I/O 接口` 与:strong:`面向闪存/云原生的文件系统特性` 。

工业界现状
==========================

ext4 与 XFS 占根分区与数据盘主流；容器镜像层多用 overlayfs。本章 ``06_io_uring``、``05_other_fs`` 已介绍 io_uring 与 XFS/Btrfs/tmpfs，实验见 ``lab_fs_program``。

值得关注的变化
==========================

- :strong:`io_uring 成熟化` ：块设备、网络、文件打开/读写在同一提交队列模型下统一，高 QPS 服务（数据库、网关）广泛评估 ``IORING_OP_*`` 替代 libaio。教程 ``o_direct_demo`` 与 io_uring 代表「绕过或重构传统 read/write 路径」两条路线。
- :strong:`fscrypt 与完整性` ：全磁盘加密（LUKS）之上，逐目录 ``fscrypt`` 在 Android 与桌面普及；Btrfs/XFS 校验和减少静默损坏，与第 9 章数据保密性相关。
- :strong:`EROFS / SquashFS 只读根` ：容器与嵌入式用只读压缩镜像减少镜像体积，只读根 + 可写 overlay 是容器标准模式（第 10 章 overlayfs）。
- :strong:`用户态文件系统` ：FUSE、virtio-fs（VM 与宿主机共享目录）在特定场景补充内核 FS，但热路径仍以内核 VFS 为主。

与本教程的衔接
==========================

理解 ``open/read/write`` 与 page cache 后，阅读 io_uring 文档可对比「同步 syscall」与「提交环」语义差异。附录源码路线列出 ``fs/ext4/``、``fs/xfs/``、``fs/btrfs/``。

动手延伸
========================

#. 用 ``06_io_uring`` 中的 liburing 示例编译运行，与 ``strace -c cat /etc/hostname`` 对比 syscall 次数。
#. ``dumpe2fs -h`` 查看根分区特性位，对照 ``04_ext4`` extent 与 journal 说明。
#. 在 ``lab_fs_program`` 中尝试 ``O_DIRECT`` 与缓冲 I/O 读取同一文件，比较 ``iostat`` 中 ``r/s`` 差异。

进一步了解
==========================

- 内核 ``Documentation/filesystems/``、``Documentation/io_uring/``
- 第 6 章块层 I/O 调度与 ``iostat`` 对照磁盘延迟
- LWN：io_uring 系列文章
- 本章 :doc:`references` —— 文件 I/O、各文件系统命令与 VFS 源码索引

.. note::

   io_uring 接口与特性随内核 6.x 持续扩展，请以当前 ``man io_uring`` 为准。
