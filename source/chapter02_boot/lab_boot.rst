==========================
实践：启动流程探索
==========================

前面几节讲了从固件到 init 的完整理论链路，现在打开终端，用命令亲眼验证每一个环节。本节不需要 root 权限即可完成大部分操作。

.. note::

   本节 ``mini_init.cpp`` 的完整源码见 ``source/code/chap02/`` ，initramfs 打包脚本为 ``build_initramfs.sh`` 。在 ``source/code`` 目录执行 ``make user`` 可一键编译全部用户态示例。

查看启动日志
==========================

内核启动时产生的日志是排查启动问题的第一手资料。

.. code-block:: bash

   # 查看内核环形缓冲区（本次启动以来的全部日志）
   dmesg

   # 人类可读的时间戳
   dmesg -T | head -30

   # 仅查看启动以来的日志
   dmesg --since boot 2>/dev/null || dmesg

   # 过滤关键启动事件
   dmesg | grep -E "Linux version|CPU|Memory:|mounted filesystem"

``dmesg`` 输出的时间戳 ``[    X.XXXXXX]`` 表示内核启动后经过的秒数。找到 ``Linux version`` 行确认内核版本，找到 ``mounted filesystem`` 行确认根文件系统挂载成功。

.. hint::

   如果 ``dmesg`` 提示权限不足，用 ``sudo dmesg`` 即可。部分系统配置了 ``kernel.dmesg_restrict``，限制非 root 用户查看内核日志。

探索 /boot 目录
==========================

``/boot`` 存放内核镜像、initramfs 和引导加载程序配置，是启动链在磁盘上的"物证"。

.. code-block:: bash

   ls -lh /boot/

   # 查看当前运行的内核对应的文件
   ls -lh /boot/vmlinuz-$(uname -r)
   ls -lh /boot/initrd.img-$(uname -r)

   # 查看 GRUB 配置
   cat /boot/grub/grub.cfg | head -40

``/boot`` 目录中常见的文件：

.. list-table::
   :header-rows: 1
   :widths: 30 50

   * - 文件
     - 说明
   * - ``vmlinuz-X.Y.Z``
     - 压缩的内核镜像
   * - ``initrd.img-X.Y.Z``
     - 初始 RAM 磁盘（initramfs）
   * - ``config-X.Y.Z``
     - 该内核的编译配置
   * - ``System.map-X.Y.Z``
     - 内核符号表（调试用）
   * - ``grub/``
     - GRUB 引导加载程序文件

如果系统使用 UEFI，还可以查看 EFI 系统分区：

.. code-block:: bash

   # 查看 ESP 挂载点
   findmnt /boot/efi 2>/dev/null || findmnt | grep -i efi

   # 列出 ESP 中的引导程序
   ls -R /boot/efi/EFI/ 2>/dev/null

查看内核启动参数
==========================

Bootloader 传递给内核的参数保存在 ``/proc/cmdline`` 中：

.. code-block:: bash

   cat /proc/cmdline

输出类似：

.. code-block:: text

   BOOT_IMAGE=/vmlinuz-6.8.0-45-generic root=UUID=a1b2c3d4-... ro quiet splash

逐字段理解：``BOOT_IMAGE`` 是 GRUB 加载的内核路径，``root=UUID=...`` 指定根文件系统，``ro`` 表示初始只读挂载，``quiet splash`` 抑制日志并显示启动画面。

分析 systemd 启动耗时
==========================

.. code-block:: bash

   # 总启动时间
   systemd-analyze

   # 各服务启动耗时排名
   systemd-analyze blame

   # 关键路径分析（最慢的依赖链）
   systemd-analyze critical-chain

``systemd-analyze blame`` 的输出按耗时从长到短排列。通常 ``NetworkManager-wait-online.service`` 和图形相关服务排在前列。如果某个服务耗时异常，可以用 ``systemctl status <服务名>`` 进一步排查。

检查 init 进程
==========================

.. code-block:: bash

   # 确认 PID 1 是 systemd
   ps -p 1 -o pid,comm,args

   # 查看 systemd 管理的所有服务
   systemctl list-units --type=service --state=running | head -20

   # 查看当前默认 target
   systemctl get-default

在现代 Linux 上，PID 1 几乎一定是 ``systemd``。如果你看到其他进程名（如 ``init``），说明系统可能使用了不同的 init 实现。

用 C++ 编写最小 init 程序
==========================

理解 init 职责的最好方式是亲手写一个。下面是一个 :strong:`极简 init` 程序，演示 PID 1 的核心行为：启动一个 shell 并回收子进程。

.. code-block:: cpp

   #include <iostream>
   #include <unistd.h>
   #include <sys/wait.h>
   #include <signal.h>
   #include <cerrno>
   #include <cstring>

   void sigchld_handler(int) {
       // 回收所有已退出的子进程，防止僵尸进程
       while (waitpid(-1, nullptr, WNOHANG) > 0) {}
   }

   int main() {
       // PID 1 必须处理 SIGCHLD，否则子进程会变成僵尸
       struct sigaction sa{};
       sa.sa_handler = sigchld_handler;
       sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
       sigaction(SIGCHLD, &sa, nullptr);

       std::cout << "mini-init: PID=" << getpid() << " 启动\n";

       pid_t pid = fork();
       if (pid < 0) {
           std::cerr << "fork 失败: " << std::strerror(errno) << "\n";
           return 1;
       }

       if (pid == 0) {
           // 子进程：启动 shell
           execl("/bin/sh", "sh", nullptr);
           std::cerr << "exec 失败: " << std::strerror(errno) << "\n";
           return 1;
       }

       // 父进程（init）：等待子进程退出
       int status;
       waitpid(pid, &status, 0);
       std::cout << "mini-init: shell 退出，状态=" << status << "\n";
       return 0;
   }

保存为 ``mini_init.cpp``，编译：

.. code-block:: bash

   g++ -std=c++17 -Wall -o mini_init mini_init.cpp

.. warning::

   :strong:`不要` 在真实系统上用此程序替换 systemd！在正常的 Linux 上，PID 1 由内核启动时硬编码为 ``/sbin/init`` （指向 systemd）。要实验自定义 init，需要使用虚拟机或 initramfs 环境。下面的 QEMU 实验供学有余力者尝试。

QEMU 实验（进阶）
==========================

要在隔离环境中测试自定义 init，可以用 QEMU 启动一个最小 Linux 环境：

.. code-block:: bash

   # 安装 QEMU（Debian/Ubuntu）
   sudo apt install qemu-system-x86

   # 使用发行版提供的 QEMU 测试镜像（以 Ubuntu 为例）
   # 或下载 Linux 内核 + 自建 initramfs

更完整的 QEMU 实验需要自行编译内核、制作 initramfs 并将 ``mini_init`` 作为 ``/init`` 放入其中。步骤概览：

1. 下载并编译 Linux 内核源码，生成 ``bzImage``
2. 创建 initramfs 目录，放入 ``mini_init`` 并重命名为 ``init``
3. 用 ``cpio`` 打包为 initramfs 镜像
4. 用 QEMU 启动：

   .. code-block:: bash

      qemu-system-x86_64 \
          -kernel arch/x86/boot/bzImage \
          -initrd initramfs.cpio.gz \
          -append "console=ttyS0" \
          -nographic

如果一切正常，QEMU 窗口中会看到 ``mini-init: PID=1 启动`` 的输出，然后进入 shell。

initramfs 制作脚本
==========================

仓库提供辅助脚本，将 ``mini_init`` 打包为 cpio 镜像：

.. code-block:: bash

   cd source/code/chap02
   make
   ./build_initramfs.sh    # 生成 initramfs.cpio.gz

内核编译的完整流程见 :doc:`/appendix/a2_build_kernel`。获得 ``bzImage`` 后，用上一节的 QEMU 命令加载 ``initramfs.cpio.gz`` 即可在隔离环境中观察自定义 init 行为。

启动流程回顾
==========================

把本节命令的输出与前面各节的理论对应起来：

.. code-block:: text

   dmesg                          → 内核启动日志（04 节）
   /boot/vmlinuz + initrd.img     → GRUB 加载的对象（03 节）
   /proc/cmdline                  → GRUB 传递的内核参数（03 节）
   /boot/efi/EFI/                 → UEFI 引导程序（02 节）
   systemd-analyze blame          → init 系统启动耗时（05 节）
   ps -p 1                        → PID 1 进程（05 节）

从按下电源键到登录提示符，整条链路现在应该清晰了。下一章进入操作系统最核心的主题之一——进程与线程管理。

拓展阅读
==========

- ``man 7 boot`` —— Linux 启动流程概述
- ``man systemd`` —— systemd 系统与服务管理器
- ``man 8 systemd-analyze`` —— 启动性能分析工具
- 内核源码 ``init/main.c`` 中的 ``start_kernel()`` 函数
- 内核源码 ``init/do_mounts.c`` 中的根文件系统挂载逻辑

本章完整参考资料与各节交叉索引见 :doc:`references`。
