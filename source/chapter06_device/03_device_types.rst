======================
设备类型
======================

Linux 将设备分为字符设备、块设备和网络设备三大类，每类有不同的访问接口和内核子系统。本节介绍各类特点及对应的 ``/dev`` 节点和驱动接口。

字符设备
========================

:strong:`字符设备` 按字节流顺序访问，不支持随机寻址（或寻址通过 ioctl 实现）。典型设备：

.. list-table::
   :header-rows: 1
   :widths: 25 45

   * - 设备
     - 说明
   * - /dev/tty、/dev/console
     - 终端
   * - /dev/null、/dev/zero
     - 丢弃写入 / 无限零字节
   * - /dev/random、/dev/urandom
     - 随机数
   * - /dev/input/*
     - 输入设备

字符设备驱动实现 ``file_operations``：``open``、``read``、``write``、``ioctl``、``release`` 等。内核用:strong:`主次设备号` （major:minor）标识设备类型和实例。``/dev/null`` 的主设备号为 1，次设备号为 3。

.. code-block:: bash

   ls -l /dev/null
   # crw-rw-rw- 1 root root 1, 3 ...

``c`` 表示字符设备，``1, 3`` 为主次设备号。

块设备
========================

:strong:`块设备` 以固定大小块（通常 512 或 4096 字节）随机访问，支持缓存和 I/O 调度。磁盘、SSD、U 盘、loop 设备均为块设备。

块设备驱动向内核注册 ``gendisk`` 和 ``request_queue``，处理读写请求。文件系统挂载在块设备之上（如 ``/dev/sda1``、``/dev/nvme0n1p2``）。

.. code-block:: bash

   ls -l /dev/sda
   lsblk
   cat /proc/partitions

``b`` 表示块设备。块设备也可通过 ``dd`` 直接读写扇区，绕过文件系统。

网络设备
========================

:strong:`网络设备` 不以 ``/dev`` 节点暴露（传统方式），而是通过 socket 接口访问。接口名如 ``eth0``、``wlan0``、``lo``，在 ``/sys/class/net/`` 和 ``ip link`` 中可见。

网络驱动实现 ``net_device`` 和 ``net_device_ops`` ：``open``、``stop``、``start_xmit`` （发送）、中断接收等。数据包经过网络协议栈（第 7 章）处理。

.. code-block:: bash

   ip link show
   ls /sys/class/net/

其他设备类型
========================

:strong:`misc 设备` ：共享主设备号 10，用于简单字符设备，简化注册。

:strong:`input 子系统` ：统一键盘、鼠标、触摸屏等输入设备，通过 ``/dev/input/event*`` 上报事件。

:strong:`DRM/KMS` ：图形显示设备，用户态通过 Mesa/Vulkan 访问。

:strong:`V4L2` ：视频采集与输出。

misc 与 input 子系统
========================

:strong:`misc 设备` 共享主设备号 10，适合功能简单、无需独立主设备号的字符设备。注册 ``misc_register()`` 比完整 ``alloc_chrdev_region`` 更简便，典型如 ``/dev/kmsg``、``/dev/fuse``。

:strong:`input 子系统` 将键盘、鼠标、触摸屏等统一为 ``input_event`` 结构（type、code、value），经 ``/dev/input/event*`` 上报用户态。Wayland/X11 读取这些事件完成输入路由，无需各驱动各自定义 ioctl。

.. code-block:: bash

   sudo evtest
   # 选择设备编号，按键/移动鼠标观察事件流

DRM 与 V4L2 简述
========================

:strong:`DRM/KMS` （Direct Rendering Manager）管理 GPU 显示与渲染。内核侧负责 modeset（分辨率、刷新率）和显存管理；用户态 Mesa/Vulkan 通过 ioctl 提交命令缓冲区。笔记本双显卡切换、多显示器配置都经过 DRM 子系统。

:strong:`V4L2` （Video4Linux2）用于摄像头和采集卡。应用通过 ``VIDIOC_REQBUFS``、``VIDIOC_QBUF`` 管理缓冲区队列，``mmap`` 或 ``read`` 获取帧数据。视频会议、监控软件底层常基于 V4L2。

块设备与字符设备的打开路径差异
========================

应用 ``open("/dev/sda")`` 走块设备路径，内核创建 ``struct block_device`` 引用，后续 ``read`` 需通过文件系统挂载后访问——直接 ``dd`` 读写扇区则绕过 FS。``open("/dev/tty")`` 走字符设备路径，``read``/``write`` 直接调用驱动 ``file_operations``。

.. code-block:: bash

   # 块设备：查看分区与文件系统
   lsblk -f /dev/nvme0n1
   # 字符设备：查看主次设备号
   ls -l /dev/nvme0n1

``nvme0n1`` 是块设备（``b``），其分区 ``nvme0n1p1`` 同样为块设备；对应的字符设备节点（如某些 ioctl 接口）在 ``/dev`` 中单独存在。

主次设备号
========================

:strong:`主设备号` 标识驱动类型，:strong:`次设备号` 标识该类型下的具体实例。内核根据 open 的 ``/dev/xxx`` 节点查找对应驱动。

.. code-block:: bash

   cat /proc/devices    # 已注册的主设备号及名称

动态分配：现代驱动多使用 ``alloc_chrdev_region()`` 由内核分配主设备号，避免冲突。

设备节点与 devtmpfs
========================

:strong:`devtmpfs` 在内核启动早期挂载 ``/dev``，根据注册的设备自动创建基本节点。udev 在此基础上创建符号链接、设置权限、处理热插拔规则。

字符设备适合流式、低延迟的 I/O；块设备承载文件系统；网络设备处理数据包。无论哪种类型，都依赖中断和 DMA 与硬件交互——下一节介绍 I/O 子系统。
