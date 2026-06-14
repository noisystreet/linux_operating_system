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

网络命名空间与 bridge
==========================

.. code-block:: bash

   sudo ip netns add testns
   sudo ip link add br0 type bridge
   sudo ip link set br0 up
   # 配合 setup_veth_netns.sh 或 lab_namespaces 中的 veth 实验

``ip netns exec testns ss -tlnp`` 在隔离环境中查看监听端口，与宿主机互不可见（除非通过 veth/bridge 连接）。

连接跟踪表
==========================

.. code-block:: bash

   cat /proc/net/nf_conntrack | head
   conntrack -L 2>/dev/null | head

``nf_conntrack`` 记录 TCP 状态（``ESTABLISHED``、``TIME_WAIT`` 等），iptables ``-m state --state ESTABLISHED,RELATED`` 依赖此表。容器大量短连接时，conntrack 表满会导致新连接失败，需调 ``net.netfilter.nf_conntrack_max``。

与 Netfilter 钩子对照
==========================

抓取经过本机 8080 的包，对照 ``lab_socket_program`` 的 echo 服务：

.. code-block:: bash

   # 终端 1：启动 echo_server
   cd source/code/chap07 && ./echo_server

   # 终端 2：抓包
   sudo tcpdump -i lo port 8080 -nn

   # 终端 3：客户端
   ./echo_client 127.0.0.1 "test"

观察 TCP 三次握手、回显数据、四次挥手。``ss -tan`` 中可看到 ``TIME-WAIT`` 状态，与 conntrack 条目对应。

下一节编写 TCP echo 服务器和客户端，亲手完成 socket 编程全流程。
