======================
网络协议栈发展史
======================

计算机网络将孤立的机器连接起来，操作系统需要提供统一的编程接口和高效的内核实现。网络子系统的演进，从 ARPANET 实验网到今日的 TCP/IP 互联网，深刻影响了 Linux 内核的设计。

ARPANET 与 TCP/IP
========================

1969 年 **ARPANET** 首次连通四台主机，采用 NCP 协议。1970 年代，TCP/IP 协议族逐步取代 NCP，1983 年 ARPANET 全面切换到 TCP/IP，这被视为互联网的诞生。

**TCP/IP** 采用分层设计：

.. code-block:: text

   应用层    HTTP、DNS、SSH...
   传输层    TCP、UDP
   网络层    IP、ICMP
   链路层    以太网、Wi-Fi...

**端到端原则** ：智能在端系统，网络核心尽量简单。这一哲学影响了 Unix socket 抽象——内核提供传输能力，应用决定语义。

BSD Socket 与 Unix
========================

1983 年 Berkeley BSD 引入 **socket**  API，成为网络编程的事实标准。socket 将网络连接抽象为文件描述符，与 Unix"一切皆文件"一脉相承。``read``/``write`` 可用于 TCP 流，``sendto``/``recvfrom`` 用于 UDP。

Linux 从早期就实现 BSD socket，POSIX 标准（``socket``、``bind``、``listen``、``accept``、``connect``）成为应用与内核的契约。

Linux 网络栈的演进
========================

Linux 1.x 的网络栈简陋，仅支持基本 TCP/IP。2.2 版本引入 **软中断** （softirq）处理网络包，提升多 CPU 性能。2.4 增加 Netfilter 框架，支持防火墙和 NAT。2.6 大规模重构：

- **NAPI** ：中断 + 轮询混合，高负载下减少中断风暴
- **SKB** （socket buffer）：统一的数据包表示
- 多队列网卡、RPS/RFS 等多核优化

3.x 起，**BPF** 可编程性增强，**eBPF** 使内核网络路径可动态扩展，无需修改内核源码。

万兆与云时代
========================

数据中心需求推动网络栈持续优化：

.. list-table::
   :header-rows: 1
   :widths: 18 42

   * - 技术
     - 作用
   * - 多队列网卡
     - 每 CPU 独立 RX/TX 队列
   * - XDP
     - 网卡驱动层最早处理包，bypass 部分协议栈
   * - DPDK
     - 用户态轮询，绕过内核（非内核主题）
   * - io_uring
     - 异步网络 I/O 新接口

容器和 Kubernetes 使 **虚拟网络** （veth、bridge、overlay）成为标配，网络命名空间（第 10 章）与 Netfilter 规则紧密配合。

从协议分层到 socket 抽象，Linux 构建了完整的网络子系统。下一节剖析内核网络栈的分层架构和数据包流向。
