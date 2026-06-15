==============
网络协议栈
==============

网络是现代操作系统不可或缺的功能。本章分析 Linux 内核网络子系统的
分层架构，从 socket 层到设备驱动层的完整数据流。

学习目标
========

- 描述 sk_buff 流经协议栈的路径与 socket API 语义
- 理解 Netfilter 钩子与 iptables/nftables 的关系
- 编写 TCP echo 程序并实验 Netfilter 模块（``lab_socket_program``、``lab_netfilter``）

先修要求：基本 C 套接字概念；第 10 章 namespace 与容器网络相关。

.. toctree::
   :maxdepth: 2

   01_history
   02_net_stack
   03_socket
   04_netfilter
   05_ebpf
   lab_network_tools
   lab_socket_program
   lab_netfilter
   outlook
   references