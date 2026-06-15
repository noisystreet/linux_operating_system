======================
延伸与趋势
======================

总线-设备-驱动模型、字符/块/网络设备划分、中断与 DMA 框架长期稳定。创新集中在:strong:`用户态高性能 I/O` 、:strong:`Rust 驱动` 与:strong:`设备直通虚拟化` 。

工业界现状
==========================

PCIe NVMe、virtio 网卡、USB-C 由内核驱动 + udev 管理；模块自动加载与 sysfs 排障仍是运维日常（``lab_device_tools``、``lab_char_driver``）。

值得关注的变化
==========================

- :strong:`Rust 内核驱动` ：pin-init、``kernel::device`` 等抽象进入主线，降低 UAF/越界风险。与第 6 章 C 版 ``file_operations`` 对照阅读，理解同一设备模型下的不同语言绑定。
- :strong:`用户态驱动栈` ：DPDK、SPDK 通过 UIO/VFIO 绕过内核数据路径处理包与 NVMe I/O，内核仍负责枚举与中断（与第 7 章 XDP 内核快速路径形成光谱两端）。
- :strong:`VFIO 与 GPU 直通` ：将物理 GPU、SmartNIC 分配给 VM（第 10 章 KVM），云游戏与 AI 训练常见；依赖 IOMMU 隔离 DMA。
- :strong:`I/O 调度器演进` ：NVMe 多队列使传统 CFQ 退出，``none`` / ``mq-deadline`` 与 ``blk-mq`` 为默认；``iostat -x`` 观察 ``await`` 仍适用（``04_io_subsystem``）。

与本教程的衔接
==========================

``hello_chardev`` 演示最小字符设备；真实网卡/NVMe 走 PCI probe 链（``02_driver_model`` sysfs 导览）。eBPF 亦可在 XDP 层处理网卡流量（第 7 章）。

动手延伸
========================

#. 插入 USB 设备时运行 ``udevadm monitor``，记录 ``ID_SERIAL``、``DEVNAME``，编写一条 ``SYMLINK`` 规则并用 ``udevadm test`` 验证。
#. 对比 ``ls -l /dev/nvme0n1`` 与 ``lsblk -f``，标出块设备与分区的对应关系（``03_device_types``）。
#. 加载 ``hello_chardev`` 后，用 ``cat /proc/devices`` 与 ``dmesg`` 对照主次设备号分配。

进一步了解
==========================

- 内核 ``Documentation/driver-api/``、``Documentation/rust/``
- 第 5 章块设备与文件系统落盘路径
- SPDK/DPDK 官方文档（用户态栈）
- 本章 :doc:`references` —— 驱动模型、udev 与字符设备实验索引

.. note::

   本节约 2025–2026 年方向撰写。
