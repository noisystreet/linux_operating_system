======================
引导加载程序
======================

固件完成了硬件初始化，但它并不知道内核文件放在哪个目录、用什么文件系统格式存储。把内核从磁盘读到内存并跳转执行——这是 :strong:`引导加载程序` （bootloader）的职责。本节以 Linux 世界最主流的 GRUB 2 为例，拆解 bootloader 的工作原理和配置方式。

Bootloader 的两阶段设计
========================

由于历史原因，bootloader 通常分为两个阶段：

:strong:`第一阶段` （Stage 1）
  代码体积极小（MBR 时代仅 512 字节），由固件直接加载。它的任务单一：找到并加载第二阶段的代码。在 UEFI 模式下，第一阶段的 ``.efi`` 文件由固件从 ESP 加载，空间限制宽松得多。

:strong:`第二阶段` （Stage 2）
  功能完整的引导程序，能够理解文件系统、解析配置文件、显示启动菜单、加载内核。GRUB 2 的第二阶段就是 ``grubx64.efi`` 加上一系列模块（``*.mod``）。

这种两阶段设计是实模式时代 512 字节限制的产物。UEFI 虽然摆脱了空间约束，但两阶段的逻辑划分仍然保留——第一阶段负责"被固件找到"，第二阶段负责"真正干活"。

GRUB 2 架构
========================

GRUB 2（GRand Unified Bootloader version 2）是当前绝大多数 Linux 发行版的默认引导程序。与 GRUB Legacy 相比，GRUB 2 采用模块化架构：

- :strong:`grubx64.efi` ：UEFI 环境下的主程序
- :strong:`*.mod` ：功能模块——文件系统驱动（``ext2.mod``、``btrfs.mod``）、加密支持（``cryptodisk.mod``）、终端渲染（``gfxterm.mod``）等
- :strong:`grub.cfg` ：启动配置文件，定义内核位置、启动参数和菜单项

GRUB 2 的配置不直接手写 ``grub.cfg``，而是通过脚本生成。在 Debian/Ubuntu 上：

.. code-block:: bash

   # 配置脚本目录
   ls /etc/grub.d/

   # 生成的配置文件
   cat /boot/grub/grub.cfg

``/etc/grub.d/`` 中的编号脚本（``10_linux``、``20_memtest86+`` 等）按顺序执行，输出合并为 ``/boot/grub/grub.cfg``。修改启动配置应编辑 ``/etc/default/grub``，然后运行 ``update-grub`` 重新生成。

启动菜单与内核参数
========================

``grub.cfg`` 中每个 :strong:`menuentry` 定义一个启动选项。一个典型的 GRUB 菜单项结构如下：

.. code-block:: text

   menuentry 'Ubuntu 24.04' {
       linux   /vmlinuz-6.8.0-45-generic root=UUID=xxxx ro quiet splash
       initrd  /initrd.img-6.8.0-45-generic
   }

两行关键指令：

- ``linux`` ：指定内核镜像路径和 :strong:`内核参数` （kernel command line）
- ``initrd`` ：指定初始 RAM 磁盘（initramfs）镜像路径

:strong:`内核参数` 是 bootloader 传递给内核的字符串，内核启动后可通过 ``/proc/cmdline`` 查看。常见参数：

.. list-table::
   :header-rows: 1
   :widths: 25 55

   * - 参数
     - 含义
   * - ``root=UUID=...`` 或 ``root=/dev/sda2``
     - 指定根文件系统的位置
   * - ``ro``
     - 以只读方式挂载根文件系统（挂载完成后再 remount 为读写）
   * - ``quiet``
     - 抑制大部分内核启动日志输出
   * - ``splash``
     - 显示发行版启动画面（Plymouth）
   * - ``init=/bin/bash``
     - 指定 PID 1 进程（调试用，跳过正常 init）
   * - ``single`` 或 ``1``
     - 进入单用户模式

.. warning::

   编辑 ``/etc/default/grub`` 后必须运行 ``update-grub`` （Debian/Ubuntu）或 ``grub2-mkconfig -o /boot/grub2/grub.cfg`` （RHEL/Fedora），否则修改不会生效。切勿直接编辑 ``grub.cfg``——下次更新时会被覆盖。

GRUB 命令行
========================

GRUB 提供交互式命令行环境，在启动菜单界面按 ``c`` 键进入。这在系统无法正常启动时非常有用：

.. code-block:: text

   # GRUB 命令行示例
   grub> ls
   grub> ls (hd0,gpt2)/
   grub> set root=(hd0,gpt2)
   grub> linux /vmlinuz-6.8.0-45-generic root=UUID=xxxx ro
   grub> initrd /initrd.img-6.8.0-45-generic
   grub> boot

``ls`` 列出设备和分区；``linux`` 和 ``initrd`` 手动加载内核与 initramfs；``boot`` 跳转执行。掌握这些命令可以在引导配置损坏时手动引导系统。

initramfs 的角色
========================

注意到 ``grub.cfg`` 中除了 ``linux`` 行，还有 ``initrd`` 行。:strong:`initramfs` （initial RAM filesystem）是一个临时的根文件系统镜像，在真正的根分区挂载之前由内核加载到内存中。

为什么需要它？因为内核镜像本身不包含所有文件系统驱动和磁盘控制器的驱动模块。如果你的根分区在 LVM 逻辑卷上、使用了 RAID、或者文件系统是 Btrfs，内核在启动初期根本无法访问它。initramfs 中包含了必要的驱动模块和挂载脚本，在内核早期用户态完成以下工作：

1. 加载存储驱动模块（如 ``dm-mod``、``btrfs``）
2. 组装 RAID/LVM 设备
3. 挂载真正的根文件系统
4. 执行 ``switch_root`` 切换到真正的根分区

initramfs 是 :strong:`cpio` 格式的压缩归档，可以用以下命令检查其内容：

.. code-block:: bash

   lsinitramfs /boot/initrd.img-$(uname -r) | head -20

其他引导加载程序
========================

GRUB 2 并非唯一选择：

:strong:`systemd-boot`
  专为 UEFI + GPT 设计的轻量级引导器。配置以文件形式存放在 ESP 中，每个启动项一个 ``.conf`` 文件。不支持 BIOS 模式，也不提供交互式命令行。Arch Linux 和部分 Fedora 安装使用它。

:strong:`rEFInd`
  图形化的 UEFI 引导管理器，自动检测各操作系统的引导项，适合多系统启动场景。

:strong:`ELILO`
  EFI 版的 LILO，已基本淘汰。

对于本教程，我们聚焦 GRUB 2——它是你遇到 Linux 系统时最可能面对的 bootloader。

Bootloader 加载内核并跳转后，控制权就交给了 Linux 内核。下一节跟踪内核从被加载到启动第一个用户进程的完整路径。
