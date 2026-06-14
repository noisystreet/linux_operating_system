======================
Netfilter 与防火墙
======================

数据包在内核协议栈中流动时，需要经过检查、过滤、地址转换等处理。:strong:`Netfilter` 是 Linux 内核中的框架，在关键钩子点插入处理逻辑，iptables/nftables 是用户态配置工具。本节介绍 Netfilter 钩子、iptables 基础和 nftables 趋势。

Netfilter 钩子点
========================

Netfilter 在协议栈的五个钩子点提供回调：

.. list-table::
   :header-rows: 1
   :widths: 22 48

   * - 钩子
     - 时机
   * - NF_INET_PRE_ROUTING
     - 入站包，路由决策前
   * - NF_INET_LOCAL_IN
     - 发往本机的包
   * - NF_INET_FORWARD
     - 转发的包（本机作路由器）
   * - NF_INET_LOCAL_OUT
     - 本机发出的包
   * - NF_INET_POST_ROUTING
     - 出站包，离开前

每个钩子可返回 ``NF_ACCEPT`` （继续）、``NF_DROP`` （丢弃）、``NF_STOLEN`` （包已被取走）等。防火墙规则按链（chain）组织，匹配包的源/目的地址、端口、协议等。

iptables 四表五链
========================

:strong:`iptables` 是传统的 Netfilter 配置工具，规则组织为:strong:`表` （table）和:strong:`链` （chain）：

.. list-table::
   :header-rows: 1
   :widths: 18 42

   * - 表
     - 用途
   * - filter
     - 过滤（允许/拒绝），最常用
   * - nat
     - 网络地址转换（SNAT/DNAT）
   * - mangle
     - 修改包头（TTL、TOS 等）
   * - raw
     - 连接跟踪前处理

:strong:`filter` 表含 INPUT、OUTPUT、FORWARD 链。默认策略 ``ACCEPT`` 或 ``DROP``，规则按顺序匹配。

.. code-block:: bash

   sudo iptables -L -n -v
   sudo iptables -A INPUT -p tcp --dport 22 -j ACCEPT
   sudo iptables -A INPUT -j DROP

``-n`` 数字显示地址，``-v`` 显示计数。生产环境建议配合 ``iptables-save``/``iptables-restore`` 持久化。

连接跟踪（conntrack）
========================

:strong:`连接跟踪` 记录活跃连接状态，使防火墙可识别"已建立连接的回包"并放行，无需为每个回包单独写规则。``RELATED``、``ESTABLISHED`` 状态即基于此。

.. code-block:: bash

   sudo iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT

NAT
========================

:strong:`SNAT` （源 NAT）：内网主机访问外网时，将源 IP 替换为网关公网 IP。:strong:`DNAT` （目标 NAT）：将访问网关某端口的流量转发到内网服务器（端口转发）。

.. code-block:: bash

   # 端口转发示例：将 80 转到内网 192.168.1.10:8080
   sudo iptables -t nat -A PREROUTING -p tcp --dport 80 \
       -j DNAT --to-destination 192.168.1.10:8080

Docker 等容器网络大量使用 NAT 和 iptables 规则。

nftables
========================

:strong:`nftables` 是 iptables 的继任者，单一框架替代 iptables、ebtables、arptables，语法更统一，内核接口更高效。近年发行版逐步默认 nftables。

.. code-block:: bash

   sudo nft list ruleset
   sudo nft add table inet filter
   sudo nft add chain inet filter input { type filter hook input priority 0 \; policy accept \; }

iptables 规则可通过 ``iptables-nft`` 兼容层迁移。新项目建议直接学习 nftables。

与网络命名空间
========================

容器每个拥有独立网络命名空间，可配置独立的 iptables 规则和路由。``docker0`` 桥接、veth 对、MASQUERADE 等是容器网络的典型配置（第 10 章）。

数据包穿越钩子点
==========================

入站 TCP 包经过 Netfilter 钩子的顺序：

.. code-block:: text

   网卡 → PRE_ROUTING → 路由决策 → LOCAL_IN → 传输层(TCP)
                              ↘ FORWARD（转发给其他主机）

出站路径：

.. code-block:: text

   应用 → LOCAL_OUT → POST_ROUTING → 网卡

NAT 规则通常挂在 ``PREROUTING`` （DNAT）和 ``POST_ROUTING`` （SNAT/MASQUERADE）。``iptables -t nat -L -n -v`` 可查看各链计数。

最小 Netfilter 内核模块（概念）
====================================

在内核模块中注册钩子可观察或过滤流量（:strong:`实验环境` ）：

.. code-block:: c

   #include <linux/netfilter.h>
   #include <linux/netfilter_ipv4.h>

   static unsigned int hello_hook(void *priv, struct sk_buff *skb,
                                  const struct nf_hook_state *state)
   {
       pr_info("hello_netfilter: hook %d\n", state->hook);
       return NF_ACCEPT;   // 放行；NF_DROP 丢弃
   }

   static struct nf_hook_ops nfho = {
       .hook     = hello_hook,
       .pf       = NFPROTO_IPV4,
       .hooknum  = NF_INET_LOCAL_IN,
       .priority = NF_IP_PRI_FIRST,
   };

   static int __init hello_init(void)
   {
       return nf_register_net_hook(&init_net, &nfho);
   }

``nf_register_net_hook`` 将函数挂到指定钩子；``init_net`` 为初始网络命名空间。容器网络在各自 netns 中可有独立钩子实例。卸载时须 ``nf_unregister_net_hook``。

内核路径：``net/netfilter/``、``include/linux/netfilter.h``。动手实验见 :doc:`lab_netfilter` 与 ``source/code/chap07/hello_netfilter.c``。

Netfilter 提供静态规则过滤，:strong:`eBPF` 则允许在内核中运行自定义程序，实现更灵活的观测和过滤——下一节介绍 eBPF 基础。
