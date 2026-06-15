==============
文件系统
==============

文件系统负责数据的组织、存储和检索。本章介绍 Linux 文件系统的
层次化架构——从 VFS 抽象层到具体的 ext4 实现。

学习目标
========

- 说明 VFS 四大对象（super_block、inode、dentry、file）与路径解析
- 描述 ext4 布局、日志、extent 与 page cache 写路径
- 实践 loop 挂载、bind mount、``strace`` 跟踪文件 I/O（``lab_fs_*``）

先修要求：第 8 章有助于理解 ``open/read/write`` 系统调用；部分实验需 root。

.. toctree::
   :maxdepth: 2

   01_history
   02_fs_concept
   03_vfs
   04_ext4
   05_other_fs
   06_io_uring
   lab_fs_tools
   lab_fs_program
   outlook
   references