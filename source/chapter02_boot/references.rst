==========================
参考资料与补充阅读
==========================

本章内容索引
==========================

.. list-table::
   :header-rows: 1
   :widths: 28 52

   * - 节
     - 主题
   * - :doc:`01_history`
     - 启动技术演进：BIOS → UEFI → Bootloader
   * - :doc:`02_bios_uefi`
     - 固件、GPT、UEFI 启动管理器
   * - :doc:`03_bootloader`
     - GRUB、内核参数、initramfs 加载
   * - :doc:`04_linux_boot`
     - ``start_kernel``、挂载根文件系统、``dmesg``
   * - :doc:`05_init_system`
     - SysVinit 与 systemd、PID 1
   * - :doc:`lab_boot`
     - 启动日志、``/boot``、``mini_init``、initramfs 脚本
   * - :doc:`outlook`
     - UKI、Measured Boot 等趋势

示例代码
==========================

- ``source/code/chap02/mini_init.cpp`` —— :doc:`lab_boot` 最小 init
- ``source/code/chap02/build_initramfs.sh`` —— 打包 initramfs 供 QEMU 实验

手册页与内核文档
==========================

- ``man 7 boot`` —— Linux 启动流程概述
- ``man 8 systemd``、``man 1 systemd-analyze`` —— :doc:`05_init_system`、:doc:`lab_boot`
- ``man 8 dmesg`` —— :doc:`04_linux_boot` 内核环形缓冲区
- 内核 ``init/main.c`` （``start_kernel``）、``init/do_mounts.c`` —— 附录 :doc:`/appendix/a3_references`

书籍与在线资料
==========================

- `linux-insides <https://github.com/0xAX/linux-insides>`_ —— 启动与内核初始化（英文）
- 《Linux 内核设计与实现》第 1–2 章 —— 内核启动概览
- 附录 :doc:`/appendix/a2_build_kernel` —— 编译安装内核与 :doc:`lab_boot` QEMU 实验衔接

相关章节
==========================

- :doc:`/chapter01_intro/index` —— 操作系统整体模型
- :doc:`/chapter06_device/index` —— 启动后设备探测与驱动加载
- :doc:`/chapter10_virt/index` —— QEMU/KVM 实验环境（:doc:`outlook`）
