======================
文件系统概念
======================

用户通过文件名访问数据，内核和磁盘却通过 inode 号和块地址打交道。本节介绍文件、目录、inode、挂载等核心概念，以及硬链接与符号链接的区别。

文件与目录
========================

:strong:`文件` 是字节的线性序列，操作系统不关心内容语义——文本、二进制、图片对内核而言都是字节流。:strong:`目录` 是特殊的文件，其内容是若干目录项（dirent），每项将文件名映射到 inode 号。

目录形成树形结构，根目录 ``/`` 为起点。路径 ``/home/user/doc.txt`` 表示从根出发，经 ``home``、``user``，到达 ``doc.txt``。:strong:`当前工作目录` 由每个进程维护，相对路径基于此解析。

inode：文件的元数据
========================

:strong:`inode` （index node）存储文件的元数据，不含文件名：

.. list-table::
   :header-rows: 1
   :widths: 22 48

   * - 字段
     - 含义
   * - 模式（mode）
     - 文件类型 + 权限位
   * - 所有者 UID/GID
     - 属主和属组
   * - 大小
     - 文件字节数
   * - 时间戳
     - atime、mtime、ctime
   * - 链接计数
     - 硬链接数量
   * - 数据块指针
     - 直接块、间接块、双重间接块等

每个文件系统有固定数量的 inode，创建时分配，删除时回收。inode 号在文件系统内唯一，不同文件系统可有相同 inode 号。

查看 inode 信息：

.. code-block:: bash

   ls -li /etc/passwd
   stat /etc/passwd

``ls -li`` 第一列为 inode 号；``stat`` 显示更完整的元数据。

硬链接与符号链接
========================

:strong:`硬链接` ：多个目录项指向同一 inode，共享数据块。删除一个硬链接仅减少链接计数，计数为 0 时内核才释放 inode 和数据。

.. code-block:: bash

   ln file hardlink    # 创建硬链接
   ls -li              # 两文件 inode 号相同

硬链接不能跨文件系统，不能链接目录（防止环）。

:strong:`符号链接` （软链接）：特殊文件，内容为目标路径。可跨文件系统、可链接目录，但目标删除后成为悬空链接。

.. code-block:: bash

   ln -s /path/to/target symlink

块、扇区与簇
========================

磁盘硬件以:strong:`扇区` （sector，通常 512 或 4096 字节）为读写单位。文件系统以:strong:`块` （block，通常 4 KB）为分配单位，一块可包含多个扇区。FAT 的:strong:`簇` （cluster）是类似概念。

文件末尾未用满的最后一块产生:strong:`内部碎片` ；块之间不连续产生:strong:`外部碎片` 。现代文件系统通过延迟分配、 extents 等减少碎片。

挂载与文件系统树
========================

:strong:`挂载` （mount）将一块存储设备上的文件系统"嫁接"到目录树的某个节点（:strong:`挂载点` ），使该目录下的内容来自该设备。

.. code-block:: bash

   mount /dev/sda2 /mnt
   ls /mnt    # 看到 sda2 上的文件

根文件系统 ``/`` 在启动时由内核挂载，其他文件系统（`` /home``、``/boot``、tmpfs 的 ``/tmp``）由 init 或用户后续挂载。``/etc/fstab`` 定义开机自动挂载项。

.. code-block:: bash

   cat /etc/fstab
   findmnt              # 查看当前挂载

虚拟文件系统
========================

并非所有"文件"都对应磁盘块：

.. list-table::
   :header-rows: 1
   :widths: 18 42

   * - 类型
     - 说明
   * - proc
     - 进程和内核信息，/proc
   * - sysfs
     - 设备与驱动信息，/sys
   * - tmpfs
     - 内存中的临时文件
   * - devtmpfs/devfs
     - 设备节点，/dev
   * - pipe/socket
     - 进程间通信端点

这些由内核在内存中实现，提供统一的文件接口，是"一切皆文件"的体现。

权限与访问控制
========================

每个 inode 的 mode 包含:strong:`文件类型` （普通文件、目录、链接、设备等）和:strong:`权限位` （owner/group/other 的 rwx）。目录的 ``x`` 表示进入目录的权限。

.. code-block:: bash

   ls -l /etc/passwd
   # -rw-r--r-- 1 root root 1234 ...

第 9 章将详述 DAC、ACL、Capabilities。此处只需知道：打开文件时内核检查进程 UID/GID 与 inode 权限是否匹配。

文件系统是操作系统与存储的桥梁。Linux 如何用统一接口支持 ext4、Btrfs、proc 等各异实现？下一节介绍 VFS 虚拟文件系统层。
