======================
各章延伸与趋势索引
======================

本附录汇总教程各章末尾 ``outlook.rst`` 的入口，便于按主题跳转到前沿讨论。各节篇幅控制在 1 页以内，侧重「与本章原理的关系」与延伸阅读，不替代主线正文。

.. note::

   前沿内容随内核与生态演进需不定期更新；阅读时请以各章 ``outlook`` 中的内核文档与 LWN 链接为准。

按章节索引
==========================

.. list-table::
   :header-rows: 1
   :widths: 22 58

   * - 章
     - 趋势主题概要
   * - :doc:`/chapter01_intro/outlook`
     - RISC-V、Rust 内核、云原生抽象
   * - :doc:`/chapter02_boot/outlook`
     - UKI、Measured Boot、Immutable OS
   * - :doc:`/chapter03_process/outlook`
     - sched_ext、eBPF 调度观测、io_uring 与并发模型
   * - :doc:`/chapter04_memory/outlook`
     - MGLRU、CXL、DAMON、内存分层
   * - :doc:`/chapter05_filesystem/outlook`
     - io_uring、fscrypt、只读根文件系统
   * - :doc:`/chapter06_device/outlook`
     - Rust 驱动、DPDK/SPDK、VFIO 直通
   * - :doc:`/chapter07_network/outlook`
     - eBPF/Cilium、XDP、kTLS、QUIC
   * - :doc:`/chapter08_syscall/outlook`
     - seccomp、io_uring 批处理、vDSO、SUD
   * - :doc:`/chapter09_security/outlook`
     - Landlock、BPF LSM、机密计算
   * - :doc:`/chapter10_virt/outlook`
     - Kubernetes、安全容器、Wasm、Cgroup v2

跨章主题路线图
==========================

若关心某一技术方向，可跨章对照阅读：

.. list-table::
   :header-rows: 1
   :widths: 20 50

   * - 主题
     - 相关章节 outlook
   * - eBPF
     - 第 3、7、8、9 章
   * - 容器与隔离
     - 第 3、9、10 章
   * - 存储与 I/O 性能
     - 第 4、5、6 章
   * - 信任与启动
     - 第 2、9、10 章

写作与维护说明
==========================

各章 ``outlook.rst`` 采用统一结构：工业界现状 → 值得关注的变化 → 与本教程衔接 → 进一步了解。贡献者更新时请保持简短，并标注大致时效（如「约 2025–2026 年」）。详见根目录 ``CONTRIBUTING.rst``。
