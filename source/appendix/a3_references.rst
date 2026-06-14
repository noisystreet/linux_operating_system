======================
推荐阅读与参考资料
======================

本教程覆盖操作系统核心主题的入门路径。以下资料适合在掌握本教程内容后深入各子系统。

官方文档
========================

- `Linux 内核文档 <https://docs.kernel.org/>`_ —— 权威技术参考
- `man pages <https://man7.org/linux/man-pages/>`_ —— 系统调用、命令、文件格式
- `POSIX 标准 <https://pubs.opengroup.org/onlinepubs/9699919799/>`_ —— API 规范
- `systemd 文档 <https://www.freedesktop.org/wiki/Software/systemd/>`_

书籍
========================

.. list-table::
   :header-rows: 1
   :widths: 35 45

   * - 书名
     - 侧重
   * - 《Unix 环境高级编程》（APUE）
     - 系统调用、进程、I/O
   * - 《Unix 网络编程》卷 1（UNP）
     - Socket 网络编程
   * - 《Linux 内核设计与实现》（LKD）
     - 内核原理，适中深度
   * - 《深入理解 Linux 内核》
     - 更全面的内核细节
   * - 《Operating Systems: Three Easy Pieces》
     - OS 概念，免费在线
   * - 《The Linux Programming Interface》（TLPI）
     - Linux 编程百科全书

在线资源
========================

- `Bootlin Linux 内核图谱 <https://bootlin.com/doc/training/linux-kernel/>`_ —— 培训材料
- `0xAX/linux-insides <https://github.com/0xAX/linux-insides>`_ —— 内核启动等（英文）
- `Linux 源码浏览器 <https://elixir.bootlin.com/linux/latest/source>`_

按章节的内核源码阅读路线
==========================

每章建议 3–5 个关键文件，按序阅读可建立「用户态现象 → 内核实现」的映射。

第 1–2 章：概述与启动
------------------------

.. list-table::
   :header-rows: 1
   :widths: 28 52

   * - 文件
     - 内容
   * - ``init/main.c``
     - ``start_kernel()`` 入口
   * - ``arch/x86/kernel/head64.c``
     - x86-64 早期启动
   * - ``init/do_mounts.c``
     - 根文件系统挂载
   * - ``kernel/sys.c``
     - ``sysinfo``、``uname`` 等实现

第 3 章：进程与线程
------------------------

.. list-table::
   :header-rows: 1
   :widths: 28 52

   * - 文件
     - 内容
   * - ``kernel/fork.c``
     - ``copy_process()``、``fork`` 核心
   * - ``kernel/sched/core.c``
     - 调度器主逻辑
   * - ``kernel/sched/fair.c``
     - CFS 公平调度
   * - ``kernel/rcu/tree.c``
     - RCU 宽限期实现
   * - ``kernel/exit.c``
     - 进程退出与僵尸回收

第 4 章：内存管理
------------------------

.. list-table::
   :header-rows: 1
   :widths: 28 52

   * - 文件
     - 内容
   * - ``mm/page_alloc.c``
     - 伙伴系统页分配
   * - ``mm/memory.c``
     - 缺页异常、页表操作
   * - ``mm/mmap.c``
     - ``mmap`` 系统调用
   * - ``mm/swap.c``
     - swap 换入换出

第 5 章：文件系统
------------------------

.. list-table::
   :header-rows: 1
   :widths: 28 52

   * - 文件
     - 内容
   * - ``fs/open.c``、``fs/read_write.c``
     - VFS 打开与读写
   * - ``fs/namei.c``
     - 路径解析
   * - ``fs/ext4/``、``fs/xfs/``、``fs/btrfs/``
     - 具体文件系统实现
   * - ``mm/shmem.c``
     - tmpfs 后端
   * - ``mm/filemap.c``
     - page cache

第 6 章：设备驱动
------------------------

.. list-table::
   :header-rows: 1
   :widths: 28 52

   * - 文件
     - 内容
   * - ``drivers/base/core.c``
     - 设备模型
   * - ``drivers/base/dd.c``
     - probe 绑定
   * - ``drivers/char/mem.c``
     - 简单字符设备参考
   * - ``block/blk-core.c``
     - 块设备层

第 7 章：网络
------------------------

.. list-table::
   :header-rows: 1
   :widths: 28 52

   * - 文件
     - 内容
   * - ``net/socket.c``
     - socket 系统调用
   * - ``net/core/dev.c``
     - 收发包入口
   * - ``net/ipv4/tcp_ipv4.c``
     - TCP over IPv4
   * - ``net/netfilter/``
     - 防火墙钩子

第 8 章：系统调用
------------------------

.. list-table::
   :header-rows: 1
   :widths: 28 52

   * - 文件
     - 内容
   * - ``arch/x86/entry/entry_64.S``
     - syscall 入口汇编
   * - ``arch/x86/entry/syscall_64.c``
     - 系统调用分发
   * - ``arch/x86/entry/syscalls/syscall_64.tbl``
     - 系统调用号表
   * - ``kernel/seccomp.c``
     - seccomp 过滤

第 9–10 章：安全与虚拟化
---------------------------

.. list-table::
   :header-rows: 1
   :widths: 28 52

   * - 文件
     - 内容
   * - ``security/selinux/``
     - SELinux 实现
   * - ``security/apparmor/``
     - AppArmor 实现
   * - ``kernel/nsproxy.c``
     - namespace 管理
   * - ``kernel/cgroup/``
     - cgroup v2
   * - ``virt/kvm/kvm_main.c``
     - KVM 核心

实践建议
========================

1. 结合 ``strace`` 阅读 APUE 示例，对照 ``kernel/`` 中对应 ``SYSCALL_DEFINE``
2. 用 ``perf``/``bpftrace`` 观察本教程 ``source/code/`` 各章实验程序
3. 在 QEMU 虚拟机中编译、安装自定义内核（见 :doc:`a2_build_kernel`）
4. 从与你工作最相关的子系统选一条上表路线，每次只读一个文件的一个函数

操作系统与 Linux 内核博大精深，本教程是起点而非终点。祝学习顺利。
