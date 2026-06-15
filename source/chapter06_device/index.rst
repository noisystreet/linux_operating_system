==================
设备与 I/O 管理
==================

操作系统需要管理种类繁多的硬件设备。本章介绍 Linux 设备驱动模型、
I/O 子系统的分层设计，以及中断与 DMA 机制。

学习目标
========

- 理解 bus/device/driver 模型与 sysfs 设备树
- 区分字符设备、块设备、网络设备及其 ``/dev`` 节点
- 编写并加载最小字符设备模块（``lab_char_driver``，需 ``linux-headers``）

先修要求：第 5 章块层、第 8 章 syscall；**内核模块实验请在虚拟机进行**。

.. toctree::
   :maxdepth: 2

   01_history
   02_driver_model
   03_device_types
   04_io_subsystem
   05_udev
   lab_device_tools
   lab_char_driver
   outlook
   references