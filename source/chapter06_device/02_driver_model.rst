======================
Linux 设备驱动模型
======================

Linux 用 **总线-设备-驱动** 三层模型组织硬件和驱动程序，通过 sysfs 导出到用户空间。理解该模型，是阅读 ``/sys``、编写驱动和排查设备问题的基础。

三层结构
========================

.. code-block:: text

   bus（总线）     PCI、USB、platform、i2c、spi...
      │
      ├── device（设备）   物理或逻辑硬件实例
      │
      └── driver（驱动）   与设备匹配的软件模块

**总线** （bus）是设备和驱动的纽带。每种总线定义匹配规则：当设备与驱动的 ID 或名称匹配时，内核调用驱动的 ``probe`` 函数绑定设备。

**设备** （device）代表一个硬件实例，有唯一标识（如 PCI 的 vendor:device、USB 的 idVendor:idProduct）。

**驱动** （driver）实现该设备的具体操作，注册到总线，等待匹配的设备出现。

sysfs 中的体现
========================

内核将设备模型映射到 ``/sys`` 虚拟文件系统：

.. code-block:: bash

   ls /sys/bus/          # 各总线
   ls /sys/devices/      # 设备树
   ls /sys/class/        # 按功能分类（block、net、tty...）

**class** 是按功能对设备的抽象，与总线正交。同一块 NVMe 磁盘可能同时出现在 ``/sys/block/nvme0n1`` 和 ``/sys/class/block/nvme0n1``。

.. code-block:: bash

   ls -l /dev/disk/by-id/
   readlink /sys/block/sda

驱动注册与 probe
========================

驱动模块加载时向总线注册 ``struct device_driver``，内核遍历该总线上的未绑定设备，若匹配则调用 ``probe``：

.. code-block:: c

   static struct platform_driver my_driver = {
       .probe  = my_probe,
       .remove = my_remove,
       .driver = { .name = "my_device" },
   };
   module_platform_driver(my_driver);

``probe`` 中通常：分配设备私有数据、映射 I/O 内存、注册中断、注册字符/块设备节点等。``remove`` 在设备移除或模块卸载时清理资源。

设备树（Device Tree）
========================

ARM 等嵌入式架构常用 **设备树** （DT）描述硬件拓扑，替代 x86 的 ACPI 表。设备树源文件（``.dts``）编译为 ``.dtb``，由 bootloader 传给内核。内核解析 DT 创建 platform 设备，驱动通过 ``of_match_table`` 匹配兼容字符串（``compatible``）。

x86 PC 主要使用 ACPI 枚举硬件，设备树多见于树莓派、手机 SoC 等。

引用计数与电源管理
========================

设备模型维护 **引用计数** ，防止使用中的设备被意外移除。**电源管理** （PM）框架支持休眠、唤醒：驱动实现 ``suspend``/``resume`` 回调，系统休眠时保存设备状态。

``cat /sys/devices/.../power/control`` 可查看设备的运行时电源策略（``auto``/``on``）。

模块与内置
========================

驱动可 **编译进内核** （built-in）或 **编译为模块** （module）：

.. code-block:: bash

   lsmod              # 已加载模块
   modinfo nvme       # 模块信息
   sudo modprobe ext4 # 加载模块（含依赖）

``/lib/modules/$(uname -r)/`` 存放模块文件，``modules.dep`` 描述依赖关系。

PCI 设备 sysfs 导览
==========================

以 PCI 网卡为例，从用户态命令追踪到 sysfs 路径：

.. code-block:: bash

   lspci | grep -i ethernet
   # 00:03.0 Ethernet controller: Intel Corporation 82540EM

   ls -l /sys/bus/pci/devices/0000:00:03.0/
   cat /sys/bus/pci/devices/0000:00:03.0/vendor
   cat /sys/bus/pci/devices/0000:00:03.0/device
   ls -l /sys/bus/pci/devices/0000:00:03.0/driver    # 绑定的驱动

若 ``driver`` 符号链接存在，表示设备已绑定驱动（如 ``e1000``、``virtio_net``）。``modinfo e1000`` 可查看模块参数；``dmesg | grep e1000`` 可见 probe 日志。

probe 调用链（简化）
==========================

设备被发现或模块加载时，内核执行匹配与绑定：

.. code-block:: text

   设备注册 device_add()
        → bus_probe_device()
        → driver_probe_device()
        → drv->probe(dev)     # 驱动初始化
        → 注册 /dev、中断、sysfs 属性

关键源码（阅读路线）：

- ``drivers/base/core.c`` —— ``device_add()``、``bus_probe_device()``
- ``drivers/base/dd.c`` —— ``driver_probe_device()``
- 具体驱动 ``drivers/net/ethernet/intel/e1000/e1000_main.c`` 等的 ``probe`` 函数

``remove`` 路径在设备热拔或 ``rmmod`` 时调用，须释放 ``probe`` 中申请的资源，否则泄漏或 oops。

uevent 与模块自动加载
==========================

设备插入或注册时，内核向用户空间发送 **uevent** ，``systemd-udevd`` 接收后按规则创建设备节点、设置权限或加载模块：

.. code-block:: text

   kernel: device_add() → kobject_uevent()
        → netlink KOBJECT_UEVENT
        → udevd 匹配规则
        → modprobe / chmod / ln -s

.. code-block:: bash

   udevadm monitor --kernel    # 实时观察内核 uevent
   cat /lib/modules/$(uname -r)/modules.alias | grep pci

``modprobe`` 根据 ``alias`` 表自动加载匹配驱动，无需手工 ``insmod``。这与第 6 章 ``lab_char_driver`` 手动注册形成对比——PC 上多数设备由总线层自动管理。

设备类型决定 I/O 方式——字符设备、块设备、网络设备各有不同的接口和子系统。下一节分别介绍。
