==========================
参考资料与补充阅读
==========================

本章内容索引
==========================

.. list-table::
   :header-rows: 1
   :widths: 28 52

   * - 节
     - 主题
   * - :doc:`01_history`
     - 文件系统发展史
   * - :doc:`02_fs_concept`
     - inode、dentry、super_block
   * - :doc:`03_vfs`
     - VFS 架构、文件描述符与 inode 关系
   * - :doc:`04_ext4`
     - ext4 布局、日志、extent
   * - :doc:`05_other_fs`
     - XFS、Btrfs、tmpfs
   * - :doc:`06_io_uring`
     - page cache、``O_DIRECT``、io_uring
   * - :doc:`lab_fs_tools`
     - ``stat``、``losetup``、挂载实验
   * - :doc:`lab_fs_program`
     - ``open``、``read``、``write``、``fsync``、``O_DIRECT``
   * - :doc:`outlook`
     - io_uring、fscrypt 等趋势

示例代码
==========================

- ``source/code/chap05/`` —— 见 :doc:`lab_fs_program`

手册页与内核文档
==========================

- ``man 2 open``、``man 2 read``、``man 2 write``、``man 2 fsync`` —— :doc:`lab_fs_program`
- ``man 2 io_uring_setup`` —— :doc:`06_io_uring`
- ``man 1 stat``、``man 8 mount``、``man 8 mkfs.ext4`` —— :doc:`lab_fs_tools`、:doc:`04_ext4`
- ``man 5 ext4``、``man 8 xfs_growfs``、``man 8 btrfs`` —— :doc:`05_other_fs`
- 内核 ``fs/open.c``、``fs/read_write.c``、``fs/ext4/`` —— 附录 :doc:`/appendix/a3_references` 第 5 章路线

书籍与在线资料
==========================

- 《TLPI》第 4–5、14 章 —— 文件 I/O 与属性
- 内核 ``Documentation/filesystems/``、``Documentation/io_uring/``
- 附录 :doc:`/appendix/a1_debug_tools` —— :doc:`lab_fs_tools` 中 ``strace`` 跟踪 I/O

相关章节
==========================

- :doc:`/chapter04_memory/index` —— page cache（:doc:`06_io_uring`）
- :doc:`/chapter06_device/index` —— 块设备与磁盘 I/O 路径
- :doc:`/chapter08_syscall/index` —— ``open`` 等文件相关 syscall
