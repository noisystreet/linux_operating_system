======================
Linux 设备驱动模型
======================

Linux 用:strong:`总线-设备-驱动` 三层模型组织硬件和驱动程序，通过 sysfs 导出到用户空间。理解该模型，是阅读 ``/sys``、编写驱动和排查设备问题的基础。

三层结构
========================

.. code-block:: text

   bus（总线）     PCI、USB、platform、i2c、spi...
      │
      ├── device（设备）   物理或逻辑硬件实例
      │
      └── driver（驱动）   与设备匹配的软件模块

:strong:`总线` （bus）是设备和驱动的纽带。每种总线定义匹配规则：当设备与驱动的 ID 或名称匹配时，内核调用驱动的 ``probe`` 函数绑定设备。

:strong:`设备` （device）代表一个硬件实例，有唯一标识（如 PCI 的 vendor:device、USB 的 idVendor:idProduct）。

:strong:`驱动` （driver）实现该设备的具体操作，注册到总线，等待匹配的设备出现。

sysfs 中的体现
========================

内核将设备模型映射到 ``/sys`` 虚拟文件系统：

.. code-block:: bash

   ls /sys/bus/          # 各总线
   ls /sys/devices/      # 设备树
   ls /sys/class/        # 按功能分类（block、net、tty...）

:strong:`class` 是按功能对设备的抽象，与总线正交。同一块 NVMe 磁盘可能同时出现在 ``/sys/block/nvme0n1`` 和 ``/sys/class/block/nvme0n1``。

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

ARM 等嵌入式架构常用:strong:`设备树` （DT）描述硬件拓扑，替代 x86 的 ACPI 表。设备树源文件（``.dts``）编译为 ``.dtb``，由 bootloader 传给内核。内核解析 DT 创建 platform 设备，驱动通过 ``of_match_table`` 匹配兼容字符串（``compatible``）。

x86 PC 主要使用 ACPI 枚举硬件，设备树多见于树莓派、手机 SoC 等。

引用计数与电源管理
========================

设备模型维护:strong:`引用计数` ，防止使用中的设备被意外移除。:strong:`电源管理` （PM）框架支持休眠、唤醒：驱动实现 ``suspend``/``resume`` 回调，系统休眠时保存设备状态。

``cat /sys/devices/.../power/control`` 可查看设备的运行时电源策略（``auto``/``on``）。

模块与内置
========================

驱动可:strong:`编译进内核` （built-in）或:strong:`编译为模块` （module）：

.. code-block:: bash

   lsmod              # 已加载模块
   modinfo nvme       # 模块信息
   sudo modprobe ext4 # 加载模块（含依赖）

``/lib/modules/$(uname -r)/`` 存放模块文件，``modules.dep`` 描述依赖关系。

设备类型决定 I/O 方式——字符设备、块设备、网络设备各有不同的接口和子系统。下一节分别介绍。
