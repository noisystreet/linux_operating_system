================================
实践：Netfilter 内核模块
================================

在内核协议栈的 Netfilter 钩子上加载模块，观察 TCP 包经过 ``LOCAL_IN`` 时的日志输出。

.. warning::

   内核模块实验需在虚拟机中进行。错误的模块可能导致网络异常，实验后务必 ``rmmod`` 卸载。

源码与编译
==========================

模块源码 ``hello_netfilter.c`` 位于 ``source/code/chap07/`` ：

.. code-block:: bash

   cd source/code/chap07
   make modules    # 需 linux-headers-$(uname -r)
   ls hello_netfilter.ko

模块在 ``NF_INET_LOCAL_IN`` 钩子注册回调，对入站 TCP 包打印源/目的 IP 与端口，返回 ``NF_ACCEPT`` 放行。

加载与观察
==========================

.. code-block:: bash

   sudo insmod hello_netfilter.ko
   sudo dmesg | tail -3

在另一终端触发本机 TCP 流量：

.. code-block:: bash

   # 启动 echo 服务（若未运行）
   cd source/code/chap07 && ./echo_server &
   ./echo_client 127.0.0.1 "netfilter test"

   # 或简单 HTTP 请求
   curl -s http://127.0.0.1:8080/ 2>/dev/null || true

观察 ``dmesg`` 中的 ``hello_netfilter: TCP ...`` 行。回环流量同样经过网络栈与 Netfilter 钩子。

修改钩子行为（实验）
==========================

将源码中 ``return NF_ACCEPT`` 改为 ``return NF_DROP`` 可对匹配的 TCP 包 **静默丢弃** （**勿在生产环境尝试** ）。重新 ``make modules``、``rmmod``、``insmod`` 后，``echo_client`` 应无法连接。

卸载
==========================

.. code-block:: bash

   sudo rmmod hello_netfilter
   sudo dmesg | tail -1

与 iptables 的关系
==========================

``iptables``/``nftables`` 规则同样通过 Netfilter 框架注册钩子。本模块演示的是 **内核模块直接注册钩子** 的方式，等价于在链上插入自定义逻辑。生产环境更常用 nftables 或 eBPF（见 :doc:`05_ebpf`）配置策略。

内核路径：``net/netfilter/nf_hook.c``、``include/linux/netfilter.h``。

拓展阅读
==========

- ``man 8 iptables``、``man 8 nft``
- 内核文档 ``Documentation/networking/netfilter.rst``
- 教程 :doc:`04_netfilter` 钩子点说明

本章完整参考资料与各节交叉索引见 :doc:`references`。
