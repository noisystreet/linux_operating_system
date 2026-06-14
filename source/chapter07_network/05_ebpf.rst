======================
eBPF 基础
======================

:strong:`eBPF` （extended Berkeley Packet Filter）允许在内核中安全运行用户提交的沙箱程序，无需修改内核源码或加载传统内核模块。网络追踪、性能分析、负载均衡等场景广泛应用。本节介绍 eBPF 的概念、架构和典型用途。

从 BPF 到 eBPF
========================

1992 年:strong:`BPF` 最初用于 tcpdump 等工具过滤网络包，在内核解释执行简单过滤指令。2014 年起:strong:`eBPF` 大幅扩展：

- :strong:`图灵完备` ：支持循环、函数调用（受验证器约束）
- :strong:`map` ：内核与用户态共享数据的键值存储
- :strong:`多种程序类型` ：kprobe、tracepoint、XDP、tc（traffic control）等
- :strong:`JIT` ：编译为本地机器码，性能接近原生

eBPF 程序由:strong:`验证器` （verifier）检查：无无限循环、无非法内存访问、栈深度有限，保证内核安全。

eBPF 程序类型（网络相关）
==================================

.. list-table::
   :header-rows: 1
   :widths: 18 42

   * - 类型
     - 挂载点
   * - XDP
     - 网卡驱动最早入口，可 DROP/REDIRECT 包
   * - tc (cls_bpf)
     - 流量控制层，ingress/egress
   * - socket filter
     - 传统 BPF 兼容，套接字过滤
   * - cgroup skb
     - 容器网络策略

XDP 可在驱动层直接丢弃 DDoS 流量，延迟极低。Cilium 等容器网络方案用 eBPF 替代 iptables 实现服务发现和策略。

工具链
========================

.. code-block:: bash

   # 查看已加载的 eBPF 程序
   sudo bpftool prog list
   sudo bpftool map list

:strong:`bcc` （BPF Compiler Collection）：Python/Lua 前端，便于编写追踪脚本。:strong:`bpftrace` ：类似 awk 的单行追踪语言。:strong:`libbpf` ：CO-RE（Compile Once, Run Everywhere）加载预编译的 eBPF 对象。

.. code-block:: bash

   # 示例：跟踪 connect 系统调用（需安装 bcc-tools）
   sudo biolatency-bpfcc

map 与用户态通信
========================

eBPF 程序通过:strong:`map` 向用户态导出数据或接收配置：

- ``BPF_MAP_TYPE_HASH`` ：哈希表
- ``BPF_MAP_TYPE_ARRAY`` ：数组
- ``BPF_MAP_TYPE_PERF_EVENT_ARRAY`` ：性能事件

用户态程序 ``bpf()`` 系统调用加载 eBPF 字节码，通过 map 的 fd 读写数据。

典型应用场景
========================

- :strong:`可观测性` ：跟踪延迟、丢包、TCP 重传（tcptracer、tcplife）
- :strong:`安全` ：运行时检测异常行为（Falco）
- :strong:`负载均衡` ：Katran、Cilium 的 DSR
- :strong:`网络策略` ：Kubernetes NetworkPolicy 的 eBPF 实现

.. note::

   eBPF 学习曲线较陡，涉及内核网络路径、验证器约束和 CO-RE 移植。入门可从 ``bpftrace`` 一行工具和 ``bpftool`` 观察现有程序开始。

与内核模块的对比
========================

.. list-table::
   :header-rows: 1
   :widths: 18 38 38

   * - 维度
     - 内核模块
     - eBPF
   * - 加载
     - insmod，需匹配内核版本
     - bpf()，验证器保证安全
   * - 崩溃风险
     - 错误可导致内核 panic
     - 验证器拒绝危险代码
   * - 热更新
     - 需卸载重载
     - 可替换程序

eBPF 不是内核模块的完全替代，复杂驱动和状态机仍用模块实现。但在网络观测和策略方面，eBPF 已成为主流选择。

网络子系统从 socket 到 Netfilter 到 eBPF，构成了 Linux 网络能力的完整图景。下一节通过命令和 C++ 程序实践网络诊断与 TCP 通信。
