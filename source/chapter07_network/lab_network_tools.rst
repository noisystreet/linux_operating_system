================================
实践：网络诊断命令
================================

用命令查看接口、路由、连接状态和抓包，把协议栈理论对应到系统实际行为。

网络接口与地址
==========================

.. code-block:: bash

   ip link show
   ip addr show
   ip -4 addr show dev eth0

``ip addr`` 显示 IPv4/IPv6 地址、前缀长度。``lo`` 为回环，``eth0``/``ens33`` 等为物理或虚拟网卡。

路由表
==========================

.. code-block:: bash

   ip route show
   ip route get 8.8.8.8

``default via`` 为默认网关。``ip route get`` 显示到指定目标的路由决策。

Socket 与连接
==========================

.. code-block:: bash

   ss -tlnp
   ss -tan
   ss -ulnp

``-t`` TCP，``-u`` UDP，``-l`` 仅监听，``-n`` 数字显示，``-p`` 显示进程。``ESTAB``、``LISTEN``、``TIME-WAIT`` 等为 TCP 状态。

.. code-block:: bash

   # 查看某端口占用
   ss -tlnp | grep 8080

抓包分析
==========================

.. code-block:: bash

   # 抓取 eth0 上 80 端口的包（需 root）
   sudo tcpdump -i any port 80 -nn

   # 抓取 localhost 流量
   sudo tcpdump -i lo -nn

``-nn`` 不解析主机名和端口名，``-c 10`` 限制包数。配合 Wireshark 可图形分析 ``tcpdump -w cap.pcap`` 保存的文件。

跟踪网络系统调用
==========================

.. code-block:: bash

   strace -e trace=network curl -s http://example.com 2>&1 | head -30

观察 ``socket``、``connect``、``sendto``/``recvfrom`` 等调用。DNS 解析可能先出现 ``connect`` 到 53 端口（UDP）。

防火墙规则
==========================

.. code-block:: bash

   sudo iptables -L -n -v
   sudo nft list ruleset 2>/dev/null

查看 filter 表 INPUT 链规则及匹配计数。无规则时可能显示 policy ACCEPT 且链为空。

连通性测试
==========================

.. code-block:: bash

   ping -c 3 127.0.0.1
   ping -c 3 8.8.8.8
   traceroute 8.8.8.8

``ping`` 使用 ICMP，``traceroute`` 显示路径上各跳。防火墙可能阻止 ICMP，ping 失败不一定表示网络不通。

下一节编写 TCP echo 服务器和客户端，亲手完成 socket 编程全流程。
