==========================
参考资料与补充阅读
==========================

本章内容索引
==========================

.. list-table::
   :header-rows: 1
   :widths: 28 52

   * - 节
     - 主题
   * - :doc:`01_history`
     - 网络协议栈发展史
   * - :doc:`02_net_stack`
     - 分层模型、``sk_buff``、收发包路径
   * - :doc:`03_socket`
     - socket API、TCP 状态机
   * - :doc:`04_netfilter`
     - iptables/nftables、NAT、Netfilter 钩子
   * - :doc:`05_ebpf`
     - eBPF、XDP、bpftrace
   * - :doc:`lab_network_tools`
     - ``ip``、``ss``、``tcpdump``、conntrack
   * - :doc:`lab_socket_program`
     - TCP echo 客户端/服务端
   * - :doc:`lab_netfilter`
     - ``hello_netfilter.ko`` 内核模块
   * - :doc:`outlook`
     - Cilium、XDP、kTLS

示例代码
==========================

- ``source/code/chap07/echo_server``、``echo_client`` —— :doc:`lab_socket_program`
- ``source/code/chap07/hello_netfilter.ko`` —— 见 :doc:`lab_netfilter`；需 ``make kernel``

手册页与内核文档
==========================

- ``man 2 socket``、``man 2 bind``、``man 2 listen`` —— :doc:`lab_socket_program`、:doc:`03_socket`
- ``man 8 iptables``、``man 8 nft`` —— :doc:`04_netfilter`、:doc:`lab_network_tools`
- ``man 8 ip``、``man 8 ss``、``man 1 tcpdump`` —— :doc:`lab_network_tools`
- 内核 ``net/socket.c``、``net/core/dev.c``、``net/netfilter/`` —— 附录 :doc:`/appendix/a3_references` 第 7 章路线

书籍与在线资料
==========================

- 《Unix 网络编程》卷 1 —— Socket 编程经典
- 内核 ``Documentation/networking/``、``Documentation/bpf/``
- 附录 :doc:`/appendix/a1_debug_tools` —— :doc:`lab_network_tools` 抓包与 ``strace`` 网络跟踪

相关章节
==========================

- :doc:`/chapter08_syscall/index` —— socket 相关系统调用
- :doc:`/chapter10_virt/index` —— 容器网络 veth/CNI（:doc:`outlook`）
- :doc:`/chapter03_process/outlook` —— eBPF 调度与网络观测交叉
