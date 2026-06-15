======================
延伸与趋势
======================

Socket API + TCP/IP 内核栈 + Netfilter/eBPF 仍是 Linux 网络主干。前沿集中在 **内核可编程性** 、**XDP 快速路径** 与 **服务网格数据面下沉** 。

工业界现状
==========================

``ss``、``tcpdump``、iptables/nftables 与 Cilium 等并存；本章 ``echo_server`` / ``echo_client``、``hello_netfilter.ko`` 覆盖 socket 与钩子两条入门路径。

值得关注的变化
==========================

- **eBPF 替代 iptables** ：Cilium、Katran 等用 BPF 实现负载均衡、NetworkPolicy，降低 per-rule 开销。教程 ``05_ebpf``、``lab_netfilter`` 分别展示 BPF 观测与经典 Netfilter 模块。
- **XDP 与 multi-buffer** ：在网卡驱动最早入口丢包、转发、负载均衡，DDoS 清洗与 CDN 边缘常用；需理解 ``sk_buff`` 生命周期（``02_net_stack``）。
- **内核 TLS（kTLS）** ：对称加密在内核完成，减少用户态拷贝，HTTPS 服务器与存储加密传输评估中。
- **QUIC 用户态栈** ：HTTP/3 多在用户态实现（quiche、msquic），但仍依赖 UDP socket 与内核路由；未取代 TCP 内核栈，而是补充。

与本教程的衔接
==========================

抓包实验（见 ``lab_network_tools``）验证协议栈行为；源码阅读路线见附录 :doc:`/appendix/a3_references` 第 7 章表格。

进一步了解
==========================

- 内核 ``Documentation/networking/``、``Documentation/bpf/``
- Cilium 文档、LWN XDP 系列
- 第 10 章容器网络（veth、CNI）与本章协议栈的衔接
- 本章 :doc:`references` —— socket、Netfilter、eBPF 与示例代码索引

.. note::

   eBPF 程序需与运行内核版本匹配或使用 CO-RE；本节约 2025–2026 年方向撰写。
