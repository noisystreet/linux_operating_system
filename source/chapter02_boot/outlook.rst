======================
延伸与趋势
======================

从 BIOS/UEFI 到 GRUB 再到 ``start_kernel`` 的启动链路数十年来稳定存在。变化集中在:strong:`固件标准` 、:strong:`可信启动` 与:strong:`init 系统` 如何编排现代服务器与桌面。

工业界现状
==========================

x86 服务器与 PC 仍以 UEFI + GRUB/systemd-boot + systemd 为主流；嵌入式可能使用 U-Boot 与 BusyBox init。本章 ``dmesg``、``/boot`` 实践仍适用于绝大多数 Linux 环境。

值得关注的变化
==========================

- :strong:`UKI（Unified Kernel Image）` ：将内核、initramfs、cmdline 等打包为 UEFI 可执行文件，简化安全启动与部署，Fedora、systemd 生态推动中。与第 2 章 ``vmlinuz`` + ``initrd`` 分离模式并存。
- :strong:`Measured Boot / TPM` ：固件与引导链测量各阶段哈希，支持远程证明（机密计算、零信任接入）。理解启动链有助于读懂 ``/sys/kernel/security/tpm0`` 等接口。
- :strong:`Immutable OS` ：Silverblue、openSUSE MicroOS 等以只读根文件系统 + 原子更新为特点，启动时挂载策略与经典 ``ext4`` 根分区不同，但底层仍经 GRUB 加载内核。
- :strong:`快速启动与休眠` ：笔记本 S3/S0ix 休眠恢复路径与冷启动不同，排障需结合 ``journalctl -b`` 与 ``dmesg``，呼应本章启动日志分析。

与本教程的衔接
==========================

自定义 init 实验（``source/code/chap02/mini_init``、``build_initramfs.sh``）对应 ``kernel_init`` 最终 ``exec`` init 的路径。附录 :doc:`/appendix/a2_build_kernel` 说明如何构建自有 ``bzImage`` 并在 QEMU 中验证。

动手延伸
========================

#. 用 ``journalctl -b 0`` 与 ``journalctl -b -1`` 对比本次与上次启动日志，定位失败启动时的内核 panic 行。
#. 在 ``/boot`` 中列出 ``vmlinuz``、``initrd``、``config``，用 ``lsinitrd`` 查看 initramfs 内含模块（若已安装）。
#. 修改 GRUB 内核参数临时加入 ``init=/bin/bash``（仅虚拟机测试），观察跳过 systemd 后的最小环境。

进一步了解
==========================

- systemd 文档：``systemd-boot``、UKI
- 内核 ``Documentation/admin-guide/acpi/``、``Documentation/efi-stub.txt``
- 第 10 章容器启动可视为「用户态构造的另类 init 环境」
- 本章 :doc:`references` —— 启动相关手册页、内核路径与交叉索引

.. note::

   前沿条目随固件与发行版策略变化，本节约 2025–2026 年方向撰写。
