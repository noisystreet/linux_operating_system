======================
Linux 内核启动
======================

引导加载程序把内核镜像和 initramfs 加载到内存并跳转执行后，Linux 内核接管控制权。从这一刻到屏幕上出现登录提示符，内核要完成一系列精密的初始化工作。本节跟踪内核启动的关键阶段，从汇编入口到 ``start_kernel``，再到挂载根文件系统和启动 init 进程。

.. mermaid::

   flowchart LR
       FW["UEFI / BIOS"] --> BL["GRUB Bootloader"]
       BL --> KZ["vmlinuz 解压"]
       KZ --> SK["start_kernel()"]
       SK --> RD["挂载 rootfs"]
       RD --> INIT["init / systemd<br/>PID 1"]

:strong:`图` ：从固件到 init 的启动链路概览

内核镜像的组成
========================

GRUB 加载的 ``vmlinuz`` 并非单一的二进制文件，而是一个 :strong:`压缩的内核镜像` ：

.. code-block:: text

   vmlinuz = vmlin(u)n(z) —— "z" 表示 gzip 压缩

内核镜像的典型结构：

.. code-block:: text

   ┌─────────────────────────────┐
   │  引导扇区/头部（汇编代码）    │  ← 16 位实模式入口
   ├─────────────────────────────┤
   │  解压器（decompressor）      │  ← 解压 gzip 压缩的内核
   ├─────────────────────────────┤
   │  压缩的内核代码（piggy.o）    │
   └─────────────────────────────┘

可以用 ``file`` 命令查看内核镜像信息：

.. code-block:: bash

   file /boot/vmlinuz-$(uname -r)

启动的第一阶段：汇编入口
========================

x86-64 平台上，内核入口在 ``arch/x86/kernel/head_64.S`` 中定义。GRUB 跳转后，CPU 仍处于 GRUB 设置好的状态（通常是 32 位保护模式或 64 位长模式），内核汇编代码需要：

1. 确认 CPU 能力（长模式、SSE 等特性）
2. 建立临时页表，开启分页
3. 设置栈指针
4. 调用解压器解压内核
5. 跳转到解压后的内核入口 ``startup_64``

.. note::

   如果你在内核源码树中搜索，入口标签通常是 ``startup_32`` （32 位阶段）和 ``startup_64`` （64 位阶段）。不同内核版本的具体文件名可能略有差异，但启动逻辑一致。

start_kernel：内核的"C 语言起点"
==================================

解压完成后，执行流进入 ``init/main.c`` 中的 ``start_kernel()`` 函数——这是内核 C 代码的真正入口。``start_kernel()`` 是一个庞大的初始化函数，按顺序完成：

.. code-block:: text

   start_kernel()
   ├── setup_arch()          # 架构相关初始化（内存布局、ACPI 等）
   ├── mm_init()             # 内存管理子系统
   ├── sched_init()          # 调度器
   ├── rcu_init()            # RCU 同步机制
   ├── init_IRQ()            # 中断子系统
   ├── time_init()           # 时钟和定时器
   ├── console_init()        # 控制台（此时 dmesg 开始有输出）
   ├── vfs_caches_init()     # 虚拟文件系统缓存
   ├── signals_init()        # 信号机制
   ├── rest_init()           # 创建内核线程，启动 init 进程
   └── ...

``start_kernel()`` 运行在 :strong:`内核线程上下文` 中，此时还没有任何用户态进程。所有初始化都在内核态完成。

内核线程与 PID 1
========================

``start_kernel()`` 末尾调用 ``rest_init()``，它做三件关键的事：

1. 创建 :strong:`idle 线程` （PID 0）—— 每个 CPU 核心一个，在没有可运行任务时执行
2. 创建 :strong:`kernel_init 线程` —— 负责挂载根文件系统和启动 init 进程
3. 当前线程变为 idle 线程，进入调度循环

``kernel_init`` 线程的执行路径（简化）：

.. code-block:: text

   kernel_init()
   ├── kernel_init_freeable()
   │   ├── do_basic_setup()     # 驱动初始化、设备探测
   │   ├── wait_for_initramfs() # 等待 initramfs 就绪
   │   └── ...
   ├── initramfs_execute()      # 执行 initramfs 中的 /init（如果存在）
   ├── prepare_namespace()      # 挂载真正的根文件系统
   └── run_init_process()       # 执行 init 进程（/sbin/init 或 systemd）

``run_init_process()`` 通过 ``execve()`` 系统调用启动用户空间的第一个进程。内核按以下顺序查找 init 程序：

1. 内核参数 ``init=`` 指定的路径
2. ``/sbin/init``
3. ``/etc/init``
4. ``/bin/init``
5. 如果都找不到，内核 panic

initramfs 阶段
========================

在挂载真正的根文件系统之前，内核先挂载 initramfs 作为临时根。initramfs 中的 ``/init`` 是一个可执行文件（通常是 shell 脚本或轻量级二进制），负责：

.. code-block:: bash

   # 典型的 initramfs 初始化脚本逻辑（简化）
   # 1. 加载必要的内核模块
   modprobe dm-mod
   modprobe ext4

   # 2. 等待设备就绪
   wait_for_device /dev/mapper/vg0-root

   # 3. 挂载真正的根文件系统
   mount /dev/mapper/vg0-root /sysroot

   # 4. 切换根目录
   exec switch_root /sysroot /sbin/init

``switch_root`` 是关键的切换操作：卸载 initramfs，将 ``/sysroot`` 变为新的根目录 ``/``，然后 ``exec`` 执行真正的 init 程序。从此，initramfs 占用的内存被释放。

挂载根文件系统
========================

``prepare_namespace()`` 函数（位于 ``init/do_mounts.c``）负责解析内核参数中的 ``root=`` 选项，挂载根文件系统：

- ``root=UUID=xxxx`` ：通过 UUID 查找块设备
- ``root=/dev/sda2`` ：直接指定设备节点
- ``root=LABEL=rootfs`` ：通过文件系统标签查找

挂载时先以 ``ro`` （只读）模式挂载，确保文件系统一致性。init 进程启动后会执行 ``mount -o remount,rw /`` 切换为读写模式。

如果根文件系统需要检查（如非正常关机），内核会触发 fsck 流程——这通常由 initramfs 中的脚本或 systemd 的 ``systemd-fsck@`` 服务完成。

启动日志：dmesg
========================

内核初始化过程中产生的日志写入 :strong:`内核环形缓冲区` （kernel ring buffer）。这些日志记录了设备探测、驱动加载、内存分配等关键事件，是诊断启动问题的重要线索。

.. code-block:: bash

   # 查看全部内核日志
   dmesg

   # 仅查看启动阶段的日志（本次启动以来）
   dmesg --since boot

   # 人类可读的时间戳
   dmesg -T

   # 过滤存储相关日志
   dmesg | grep -i "sd\|nvme\|ata"

典型的启动日志片段：

.. code-block:: text

   [    0.000000] Linux version 6.8.0-45-generic ...
   [    0.123456] x86/fpu: Supporting XSAVE feature 0x001: 'x87 ...
   [    1.234567] SCSI subsystem initialized
   [    2.345678] EXT4-fs (dm-0): mounted filesystem with ordered data mode
   [    3.456789] systemd[1]: systemd 255.4-1ubuntu8 running in system mode

时间戳 ``[    X.XXXXXX]`` 表示内核启动后经过的秒数。通过分析这些时间戳，可以定位启动过程中的瓶颈。

内核启动完成后，第一个用户态进程——init——开始运行。下一节介绍 init 系统的演进，以及 systemd 如何管理整个系统的启动。
