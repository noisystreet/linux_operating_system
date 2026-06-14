======================
虚拟文件系统（VFS）
======================

Linux 同时支持 ext4、XFS、Btrfs、proc、sysfs 等数十种文件系统。若每种都暴露不同接口，应用程序将无法通用。:strong:`VFS` （Virtual File System）在应用程序与具体文件系统之间插入抽象层，提供统一的 open/read/write 接口。本节介绍 VFS 的核心对象和调用路径。

VFS 的设计目标
========================

VFS 解决三个问题：

1. :strong:`统一接口` ：应用使用相同的系统调用访问任意文件系统
2. :strong:`可扩展` ：新文件系统只需实现 VFS 规定的操作集
3. :strong:`互操作` ：挂载在同一棵树上的不同 FS 对用户透明

当你 ``open("/home/user/file")`` 时，VFS 根据路径解析到对应挂载点，调用该文件系统的 ``->open()`` 实现，应用无需知道底层是 ext4 还是 NFS。

四大核心对象
========================

VFS 用四种数据结构描述文件系统状态：

:strong:`super_block` （超级块）
  代表一个已挂载的文件系统实例。包含块大小、inode 总数、魔数、文件系统特有的操作函数表（``super_operations``）等。

:strong:`inode`
  代表一个文件或目录。VFS 的 ``struct inode`` 是各具体 FS inode 的抽象，包含 mode、uid、size、指向 ``address_space`` 的指针等。磁盘 FS 的 inode 从磁盘读取后填入 VFS inode 缓存。

:strong:`dentry` （目录项缓存）
  代表路径中的一个分量（如 ``/home/user`` 中的 ``user``）。dentry 将路径名与 inode 关联，并缓存解析结果，加速后续访问。dentry 不持久化，重启后重建。

:strong:`file`
  代表一次打开操作（open 的返回值）。``struct file`` 包含当前读写偏移（f_pos）、打开模式、指向 ``file_operations`` 的指针。同一 inode 可被多次 open，每次对应不同的 file 对象。

.. code-block:: text

   路径: /home/user/doc.txt
   dentry: / → home → user → doc.txt
   inode:  doc.txt 的元数据
   file:   某进程的 open fd 对应的 file 结构

路径解析与挂载
========================

:strong:`路径解析` （path lookup）从根 dentry 或当前工作目录出发，逐级查找。dentry 缓存（dcache）命中则跳过磁盘读取；未命中则从父目录 inode 读取目录项，创建新 dentry。

:strong:`挂载` 时，挂载点目录的 dentry 被:strong:`遮盖` （covered），其下的 dentry 来自新挂载的 FS。``/home`` 挂载独立分区后，``/home/user`` 的解析进入该分区的根 inode。

.. code-block:: bash

   findmnt /home
   ls /proc/mounts

file_operations 与 inode_operations
==========================================

每种文件系统实现 VFS 规定的:strong:`操作函数表` ：

- ``inode_operations`` ：lookup、create、unlink、mkdir、getattr 等
- ``file_operations`` ：read、write、llseek、ioctl、mmap 等
- ``super_operations`` ：read_inode、write_inode、put_super 等

以 read 为例，调用链（简化）：

.. code-block:: text

   read() 系统调用
   → vfs_read()
   → file->f_op->read_iter()  或 generic_file_read_iter()
   → 具体 FS 或 page cache
   → 块设备层

块设备文件系统（ext4、XFS）通常通过:strong:`page cache` 缓存数据，实际读写经过通用层；字符设备、proc 等直接实现 ``file_operations``。

文件描述符表
========================

进程通过:strong:`文件描述符` （fd，0、1、2…）访问打开的文件。每个进程有 ``files_struct``，内含 fd 数组，指向内核的 ``struct file``。fork 后子进程复制 fd 表，父子可共享同一 file 的偏移（或独立，取决于 ``FD_CLOEXEC`` 等标志）。

.. code-block:: bash

   ls -l /proc/self/fd/

标准输入 0、标准输出 1、标准错误 2 在进程启动时由 shell 打开。

缓存：dcache 与 inode cache
==================================

VFS 维护:strong:`dentry 缓存` 和:strong:`inode 缓存` ，避免重复从磁盘读取元数据。内存紧张时，内核回收不常用的 dentry 和 inode。回收后再次访问需重新从磁盘加载。

``/proc/sys/fs/dentry-state``、``inode-state`` 可查看缓存统计。过多的 dentry 缓存可能占用大量 RAM，但通常能显著加速路径解析。

特殊文件系统
========================

:strong:`proc` 和:strong:`sysfs` 不关联块设备，其 ``read/write`` 由内核函数动态生成内容。例如 ``cat /proc/cpuinfo`` 触发内核收集 CPU 信息并格式化为文本返回。

:strong:`tmpfs` 数据存于 page cache，无后端块设备，卸载或重启即丢失。适合 ``/tmp``、``/dev/shm``。

:strong:`pipe`、:strong:`socket` 也通过 VFS 的 file 接口操作，体现 Unix 的统一抽象。

VFS 是文件子系统的枢纽。具体文件系统如何在磁盘上组织数据？下一节以 Linux 默认的 ext4 为例深入。
