================================
实践：设备与 I/O 命令操作
================================

用命令探索内核模块、sysfs 设备树、中断分配和磁盘 I/O 统计，建立对设备子系统的直观认识。

内核模块
==========================

.. code-block:: bash

   lsmod
   lsmod | head -10

   modinfo ext4
   modinfo -F description nvme

``lsmod`` 列出已加载模块及大小、引用计数、依赖。``modinfo`` 显示模块路径、作者、许可证、依赖等。

.. code-block:: bash

   # 加载/卸载模块（需 root，示例模块名请替换为系统存在的）
   sudo modprobe loop
   lsmod | grep loop
   sudo modprobe -r loop

浏览 /sys 设备树
==========================

.. code-block:: bash

   ls /sys/bus/
   ls /sys/class/
   ls /sys/devices/ | head

按功能查看设备：

.. code-block:: bash

   ls /sys/class/block/
   ls /sys/class/net/
   ls -l /sys/class/tty/tty0

.. code-block:: bash

   # 块设备详情
   cat /sys/block/sda/size    # 扇区数
   cat /sys/block/sda/queue/scheduler

``size`` 单位为 512 字节扇区。NVMe 设备名为 ``nvme0n1`` 等。

中断分配
==========================

.. code-block:: bash

   cat /proc/interrupts | head -25

观察各 IRQ 在各 CPU 上的处理次数。APIC、MSI、MSI-X 使现代 PCI 设备多用消息 signaled 中断，减少共享 IRQ 冲突。

磁盘 I/O 统计
==========================

.. code-block:: bash

   iostat -x 1 3

在另一个终端运行 ``dd if=/dev/zero of=/tmp/test bs=1M count=100`` 制造 I/O，观察 ``%util``、``await`` 变化。

.. code-block:: bash

   lsblk -f
   fdisk -l    # 需 root，列出分区

udev 热插拔监控
==========================

.. code-block:: bash

   sudo udevadm monitor --udev --property

插入 U 盘或 USB 设备，观察 ``ACTION=add`` 及 ``DEVNAME``、``ID_BUS``、``ID_FS_UUID`` 等属性。拔出时可见 ``ACTION=remove``。

设备节点
==========================

.. code-block:: bash

   ls -l /dev/sd* /dev/nvme* 2>/dev/null | head
   ls -l /dev/disk/by-id/ | head
   file /dev/null /dev/zero /dev/random

字符设备 ``c``，块设备 ``b``，后面的数字为主:次设备号。

主设备号列表
==========================

.. code-block:: bash

   cat /proc/devices

``Character devices`` 和 ``Block devices`` 分段列出已注册主设备号。编写驱动时需避免与现有号冲突，通常使用动态分配。

解读命令输出
==========================

``lsmod`` 典型一行：

.. code-block:: text

   nvme           45056  0

列含义：模块名、占用字节数、引用计数（0 表示可 ``modprobe -r``）、依赖模块列表。

``iostat -x`` 关键列：

.. list-table::
   :header-rows: 1
   :widths: 12 48

   * - 列
     - 含义
   * - %util
     - 设备忙碌时间占比；接近 100% 表示饱和
   * - await
     - 平均 I/O 等待时间（ms）；HDD 高、NVMe 通常低
   * - r/s, w/s
     - 每秒读写次数

``/proc/interrupts`` 中，同一 IRQ 行末多列数字对应各 CPU 处理次数。若全部集中在 CPU0，可能存在中断亲和性未分散问题。

热插拔端到端观察
==========================

在运行 ``udevadm monitor`` 的终端外，插入 U 盘，典型事件序列：

.. code-block:: text

   ACTION=add SUBSYSTEM=block DEVNAME=sdb
   ACTION=add SUBSYSTEM=block DEVNAME=sdb1 ID_FS_UUID=...

随后在另一终端：

.. code-block:: bash

   ls -l /dev/disk/by-uuid/
   dmesg | tail -5

将 ``DEVNAME``、``ID_FS_UUID`` 与 ``05_udev`` 规则编写、`/etc/fstab`` UUID 挂载联系起来。

动手练习
==========================

#. 将某块设备的 I/O 调度器从 ``mq-deadline`` 改为 ``none``（NVMe 常见），对比 ``dd`` 时 ``iostat`` 的 ``await`` （仅测试环境）。
#. 对 ``hello_chardev`` 加载前后各执行一次 ``cat /proc/devices``，确认新主设备号出现。
#. 用 ``find /sys/devices -name 'modalias' | head`` 理解驱动匹配字符串来源。

下一节编写一个简单的字符设备内核模块，实现最基本的 read/write 接口。
