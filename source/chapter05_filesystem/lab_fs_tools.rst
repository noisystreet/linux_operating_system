================================
实践：文件系统命令操作
================================

用命令观察 inode、挂载点和文件 I/O 系统调用，把前几节的抽象概念对应到真实系统输出。

inode 与元数据
==========================

.. code-block:: bash

   ls -li /etc/passwd /etc/group
   stat /etc/passwd

``ls -li`` 第一列为 inode 号。同一文件系统的不同文件 inode 号唯一。``stat`` 显示大小、块数、权限、三个时间戳。

.. code-block:: bash

   # inode 使用情况
   df -i
   df -ih /home

``df -i`` 显示各文件系统的 inode 总数与已用数。小文件极多的场景可能 inode 耗尽而空间仍有剩余。

硬链接与符号链接
==========================

.. code-block:: bash

   cd /tmp
   echo hello > orig.txt
   ln orig.txt hard.txt
   ln -s orig.txt sym.txt
   ls -li orig.txt hard.txt sym.txt
   stat sym.txt

``orig.txt`` 与 ``hard.txt`` inode 相同；``sym.txt`` 为另一 inode，类型为符号链接。

创建并挂载 ext4 镜像
==========================

.. warning::

   以下操作需要 root，建议在虚拟机或测试环境进行。

.. code-block:: bash

   # 创建 100 MB 空文件
   dd if=/dev/zero of=/tmp/fs.img bs=1M count=100

   # 格式化为 ext4
   mkfs.ext4 /tmp/fs.img

   # 关联到 loop 设备并挂载
   sudo losetup -f /tmp/fs.img
   sudo losetup -a    # 查看 loop 设备名，如 /dev/loop0
   sudo mkdir -p /mnt/testfs
   sudo mount /dev/loop0 /mnt/testfs

   # 测试读写
   echo test | sudo tee /mnt/testfs/hello.txt
   cat /mnt/testfs/hello.txt

   # 卸载与清理
   sudo umount /mnt/testfs
   sudo losetup -d /dev/loop0

bind mount 与 tmpfs
==========================

.. code-block:: bash

   # 将目录绑定到另一路径
   mkdir -p /tmp/bind_src /tmp/bind_dst
   echo content > /tmp/bind_src/file
   sudo mount --bind /tmp/bind_src /tmp/bind_dst
   cat /tmp/bind_dst/file

   # tmpfs：内存中的临时文件系统
   sudo mkdir -p /mnt/ramfs
   sudo mount -t tmpfs -o size=32M tmpfs /mnt/ramfs
   df -h /mnt/ramfs

tmpfs 适合临时数据，重启或卸载后内容消失。

跟踪文件 I/O 系统调用
==========================

.. code-block:: bash

   strace -e trace=openat,read,write,close cat /etc/hostname

观察 ``openat`` 打开文件、``read`` 读取、``write`` 输出到 stdout、``close`` 关闭。``-e trace=file`` 可跟踪所有文件相关调用。

.. code-block:: bash

   # 统计某命令的文件 I/O 模式
   strace -c -e trace=openat,read,write cp /etc/passwd /tmp/passwd.copy

挂载与 fstab
==========================

.. code-block:: bash

   findmnt
   cat /proc/mounts | column -t | head -15
   cat /etc/fstab

``findmnt`` 以树形显示挂载关系，便于理解 bind mount 和子挂载。

下一节用 C++ 编写程序，实践 ``open/read/write`` 和 ``O_DIRECT`` 标志。
