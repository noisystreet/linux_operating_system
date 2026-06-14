======================
Linux 网络栈架构
======================

应用程序通过 socket 发送数据，内核经过多层协议处理，最终由网卡驱动发送到物理网络。本节介绍 Linux 网络子系统的分层结构、sk_buff 数据包表示，以及收发包的基本路径。

分层模型
========================

Linux 网络栈与 TCP/IP 模型对应，自顶向下大致为：

.. code-block:: text

   用户空间    应用程序（read/write/send/recv）
        ↕
   socket 层   struct socket、struct sock
        ↕
   传输层      TCP、UDP（tcp_input、udp_rcv）
        ↕
   网络层      IP、ICMP（ip_rcv、ip_output）
        ↕
   链路层      ARP、以太网帧（dev_queue_xmit）
        ↕
   设备驱动    网卡硬件（ndo_start_xmit）

每层处理本层首部，向上递交载荷或向下封装。:strong:`Netfilter` 钩子在各层插入，供防火墙、NAT 等处理（下节详述）。

.. mermaid::

   flowchart TB
       App["用户态应用程序<br/>read / write / send / recv"]
       App --> Sock["socket 层<br/>struct socket / sock"]
       Sock --> Trans["传输层 TCP / UDP"]
       Trans --> Net["网络层 IP / ICMP"]
       Net --> Link["链路层 以太网 / ARP"]
       Link --> Drv["设备驱动 ndo_start_xmit"]
       Drv --> NIC["网卡硬件"]

:strong:`图` ：Linux 网络协议栈分层数据流（发送方向）

socket 与 sock
========================

:strong:`struct socket` 是 BSD socket 在用户空间的抽象，包含类型（SOCK_STREAM/SOCK_DGRAM）、协议族（AF_INET）、状态等。:strong:`struct sock` 是协议无关的传输层端点，TCP 对应 ``struct tcp_sock``，UDP 对应 ``struct udp_sock``。

一个 socket 文件描述符在内核中关联 ``struct file`` → ``struct socket`` → ``struct sock``。``accept()`` 为新连接创建新的 socket 和 sock，监听 socket 保持不变。

sk_buff：网络包的数据结构
==================================

:strong:`sk_buff` （socket buffer，简称 skb）是 Linux 表示网络包的核心结构（``include/linux/skbuff.h``）。包含：

- 数据指针（head、data、tail、end）
- 协议首部指针（mac_header、network_header、transport_header）
- 设备信息、路由、时间戳、校验和等元数据

skb 在协议栈各层间传递，每层可能 ``skb_push``/``skb_pull`` 调整指针，添加或剥离首部。避免不必要的拷贝是性能关键。

发送路径（简化）
========================

.. code-block:: text

   write() / send()
   → sock_sendmsg()
   → tcp_sendmsg() / udp_sendmsg()
   → 构建 skb，填充 TCP/UDP 首部
   → ip_queue_xmit()，添加 IP 首部
   → 邻居子系统解析 MAC 地址（ARP）
   → dev_queue_xmit() 进入网卡驱动
   → DMA 发送到网卡

TCP 还涉及拥塞控制、滑动窗口、重传等复杂逻辑（``net/ipv4/tcp_*.c``）。

接收路径（简化）
========================

.. code-block:: text

   网卡中断 / NAPI 轮询
   → 驱动分配 skb，DMA 填入数据
   → netif_receive_skb() 进入协议栈
   → 以太网层剥离 MAC 头
   → ip_rcv() 处理 IP 层
   → tcp_v4_rcv() / udp_rcv() 递交传输层
   → 数据放入 socket 接收队列
   → 唤醒阻塞的 read()/recv()，或就绪 epoll

:strong:`NAPI` （New API）：高负载时驱动一次中断处理多个包，减少中断次数。收包入口函数 ``netif_receive_skb()`` 位于 ``net/core/dev.c``；TCP 层入口 ``tcp_v4_rcv()`` 在 ``net/ipv4/tcp_ipv4.c``。

软中断与 NAPI 调度
==========================

网卡收到数据后，驱动通常:

1. 分配 ``sk_buff``，DMA 填入帧数据
2. 触发硬件中断（硬中断上下文，尽量短）
3. 调度:strong:`软中断` ``NET_RX_SOFTIRQ``，在软中断中批量调用 ``netif_receive_skb()``

.. code-block:: bash

   cat /proc/softirqs | grep NET_RX
   watch -n1 'cat /proc/net/softnet_stat'

高 PPS 场景下，NAPI 的 ``budget`` 限制单次软中断处理的包数，避免长时间占用 CPU 导致饥饿。

网络命名空间
========================

:strong:`网络命名空间` 使不同容器拥有独立的网络栈：独立的接口、路由表、iptables 规则。``ip netns`` 可管理命名空间（第 10 章容器部分）。

.. code-block:: bash

   ip link show
   ip addr show
   ip route show

本地回环与协议族
========================

:strong:`lo` （127.0.0.1）是本地回环接口，数据不离开本机。:strong:`AF_INET` 为 IPv4，:strong:`AF_INET6` 为 IPv6，:strong:`AF_UNIX` 为同一主机进程间通信（第 3 章 IPC 提及）。

.. code-block:: bash

   ping -c 2 127.0.0.1
   ping6 -c 2 ::1

应用通过 socket API 使用上述协议栈。下一节介绍 socket 编程模型和内核实现要点。
