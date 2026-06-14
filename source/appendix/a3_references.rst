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

本教程章节与源码路径对照
========================

.. list-table::
   :header-rows: 1
   :widths: 20 50

   * - 主题
     - 内核路径示例
   * - 调度
     - ``kernel/sched/fair.c``
   * - 内存
     - ``mm/page_alloc.c``、``mm/memory.c``
   * - VFS
     - ``fs/read_write.c``、``fs/open.c``
   * - 网络
     - ``net/socket.c``、``net/ipv4/tcp.c``
   * - 系统调用
     - ``arch/x86/entry/entry_64.S``

实践建议
========================

1. 结合 ``strace`` 阅读 APUE 示例
2. 用 ``perf``/``bpftrace`` 观察本教程各章实验程序
3. 在 QEMU 虚拟机中编译、安装自定义内核
4. 阅读与你工作最相关的子系统源码（如仅网络或仅存储）

操作系统与 Linux 内核博大精深，本教程是起点而非终点。祝学习顺利。
