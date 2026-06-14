======================
编译与安装内核
======================

从源码编译 Linux 内核是深入理解系统的高级实践。本附录概述获取源码、配置、编译和安装的流程，供实验环境参考。

.. warning::

   自行编译内核可能导致系统无法启动。请在虚拟机或备用机器上操作，保留可启动的旧内核。

获取源码
==========================

.. code-block:: bash

   apt install build-essential libssl-dev libelf-dev bc flex bison
   wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.8.tar.xz
   tar xf linux-6.8.tar.xz
   cd linux-6.8

或使用发行版源码包：

.. code-block:: bash

   apt source linux-image-$(uname -r)    # Debian/Ubuntu

配置
==========================

复制当前运行内核配置为起点：

.. code-block:: bash

   cp /boot/config-$(uname -r) .config
   make olddefconfig

交互式调整：

.. code-block:: bash

   make menuconfig    # 需 ncurses-dev
   # 或 make nconfig / xconfig

常见选项：启用调试符号（``CONFIG_DEBUG_INFO``）、特定驱动、关闭不需要的模块以加快编译。

编译
==========================

.. code-block:: bash

   # 并行编译，-j 为 CPU 核心数
   make -j$(nproc)

耗时视机器性能，十余分钟到数小时。输出：``arch/x86/boot/bzImage``、各 ``.ko`` 模块。

安装模块与内核
==========================

.. code-block:: bash

   sudo make modules_install
   sudo make install

``make install`` 通常更新 ``/boot``、``grub.cfg`` 或 ``systemd-boot`` 条目。确认引导加载程序包含新内核后重启：

.. code-block:: bash

   sudo reboot
   uname -r

启动失败时，在 GRUB 菜单选择旧内核进入系统。

仅编译模块
==========================

编写内核模块时，无需重编整个内核：

.. code-block:: bash

   make -C /lib/modules/$(uname -r)/build M=$PWD modules

需安装 ``linux-headers-$(uname -r)``。第 6 章字符设备实验即用此方式。

调试内核
========================

- 编译时启用 ``CONFIG_DEBUG_INFO``，配合 ``crash`` 分析 vmcore
- ``printk`` / ``pr_debug`` 打印
- ``ftrace``、``kprobes`` 动态追踪
- QEMU 运行自定义 ``bzImage`` 避免搞坏物理机

附录 C 列出内核文档与书籍，供进一步学习。
