======================
初始化系统
======================

内核完成了硬件初始化、驱动加载和根文件系统挂载，但它不会自己去启动 SSH 服务、挂载用户家目录或显示登录界面。这些工作属于用户空间——而用户空间的第一个进程 **init** （PID 1）负责协调一切。本节介绍 init 系统的演进，重点讲解现代 Linux 中 systemd 的工作原理。

为什么需要 init
========================

内核启动 ``kernel_init`` 线程最终通过 ``execve()`` 执行 init 程序。init 作为 PID 1，承担以下不可替代的职责：

- **启动系统服务** ：按依赖顺序启动网络、存储、日志、图形界面等服务
- **回收孤儿进程** ：父进程先于子进程退出时，子进程被 init 收养；init 必须调用 ``wait()`` 回收这些僵尸进程，否则 PID 资源会耗尽
- **处理信号** ：init 对 ``SIGCHLD`` 等信号有特殊处理逻辑
- **切换运行级别** ：在 SysVinit 时代，通过运行级别控制系统的整体状态

如果 init 进程崩溃，内核会触发 panic——因为没有 PID 1，整个用户空间失去管理。这就是为什么 init 必须是健壮、可靠、不可被随意终止的进程。

SysVinit：运行级别模型
========================

**SysVinit** 是 Unix 传统的 init 实现，从 1980 年代沿用至 2010 年代。它的核心概念是 **运行级别** （runlevel）：

.. list-table::
   :header-rows: 1
   :widths: 15 55

   * - 级别
     - 含义
   * - 0
     - 关机
   * - 1
     - 单用户模式（维护用）
   * - 2
     - 多用户，无网络（Debian/Ubuntu）
   * - 3
     - 多用户，有网络（RHEL/CentOS 默认）
   * - 5
     - 多用户 + 图形界面
   * - 6
     - 重启

每个运行级别对应 ``/etc/rcN.d/`` 目录下的一组符号链接，指向 ``/etc/init.d/`` 中的启动脚本：

.. code-block:: text

   /etc/rc3.d/
   ├── S20network    → ../init.d/network    # S = Start，数字越小越先执行
   ├── S50sshd       → ../init.d/sshd
   ├── K20httpd      → ../init.d/httpd      # K = Kill（停止）
   └── ...

SysVinit 的启动是 **严格串行** 的——一个脚本执行完才执行下一个。这导致启动缓慢：即使网络和存储互不依赖，也必须排队等待。

.. note::

   今天仍可在部分嵌入式系统或极简 Linux 环境中遇到 SysVinit。Debian 直到 2012 年才默认切换到 systemd。

systemd：依赖图与并行启动
==============================

**systemd** 由 Lennart Poettering 等人开发，2010 年首次发布，现已成为事实上的 Linux init 标准。systemd 的设计哲学与 SysVinit 截然不同：

**单元** （unit）模型
  systemd 将所有管理对象抽象为 **单元** ，每种类型有明确的职责：

  .. list-table::
     :header-rows: 1
     :widths: 20 55

     * - 单元类型
       - 用途
     * - ``.service``
       - 系统服务（如 sshd、nginx）
     * - ``.target``
       - 目标组（类似运行级别，如 multi-user.target）
     * - ``.mount``
       - 文件系统挂载点
     * - ``.timer``
       - 定时任务（替代 cron）
     * - ``.socket``
       - 套接字激活（按需启动服务）

**依赖解析**
  每个单元在 ``[Unit]`` 段声明依赖关系：

  .. code-block:: ini

     [Unit]
     Description=OpenSSH server
     After=network.target
     Requires=sshd.socket

  systemd 根据所有单元的依赖关系构建 **有向无环图** （DAG），在同一层级的服务 **并行启动** 。网络和存储如果互不依赖，可以同时初始化——这是 systemd 启动速度远超 SysVinit 的核心原因。

**target** 替代 runlevel
  systemd 用 target 单元替代运行级别：

  .. list-table::
     :header-rows: 1
     :widths: 25 45

     * - target
       - 等价 runlevel
     * - ``poweroff.target``
       - 0（关机）
     * - ``rescue.target``
       - 1（单用户）
     * - ``multi-user.target``
       - 3（多用户）
     * - ``graphical.target``
       - 5（图形界面）
     * - ``reboot.target``
       - 6（重启）

  查看当前 target：

  .. code-block:: bash

     systemctl get-default

systemd 启动流程
========================

systemd 作为 PID 1 启动后，自身的初始化路径大致如下：

.. code-block:: text

   systemd (PID 1)
   ├── 挂载必要的虚拟文件系统（/proc、/sys、/dev）
   ├── 加载 systemd 单元文件（/usr/lib/systemd/、/etc/systemd/）
   ├── 启动 default.target（通常是 graphical.target）
   │   ├── basic.target
   │   ├── sysinit.target（早期系统初始化）
   │   ├── sockets.target（套接字激活）
   │   ├── multi-user.target（多用户服务）
   │   └── display-manager.service（图形登录）
   └── 进入事件循环，等待服务状态变化

**套接字激活** （socket activation）是 systemd 的一项巧妙设计：服务不必在启动时全部运行，systemd 先监听端口，当有连接到来时才启动对应服务。这进一步加快了启动速度。

常用 systemd 命令
========================

.. code-block:: bash

   # 查看服务状态
   systemctl status sshd

   # 启动/停止/重启服务
   systemctl start nginx
   systemctl stop nginx
   systemctl restart nginx

   # 设置开机自启
   systemctl enable sshd

   # 查看所有运行中的服务
   systemctl list-units --type=service --state=running

   # 分析启动耗时
   systemd-analyze
   systemd-analyze blame
   systemd-analyze critical-chain

``systemd-analyze blame`` 列出各服务启动耗时，按从长到短排序——排查启动慢的利器。

systemd 的争议
========================

systemd 并非没有争议。批评者认为：

- **违反 Unix 哲学** ：一个程序承担了 init、日志（journald）、网络（networkd）、时间同步（timesyncd）等过多职责
- **日志不透明** ：journald 使用二进制格式存储日志，不像传统 syslog 那样可以用 ``cat`` 直接查看
- **强制依赖** ：许多软件开始硬依赖 systemd 的特定接口（如 cgroup 管理）

支持者则认为：

- 并行启动显著缩短了开机时间
- 统一的单元管理比分散的 init 脚本更易维护
- cgroup 集成使资源控制（容器基础）更加自然

无论立场如何，systemd 已是现代 Linux 的事实标准。作为学习者，理解它的工作方式比争论它的优劣更有价值。

从固件到 init，启动链的每一个环节都已就位。下一节通过实际操作，把整条启动路径串联起来。
