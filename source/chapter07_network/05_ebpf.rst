======================
eBPF 基础
======================

**eBPF** （extended Berkeley Packet Filter）允许在内核中安全运行用户提交的沙箱程序，无需修改内核源码或加载传统内核模块。网络追踪、性能分析、负载均衡等场景广泛应用。本节介绍 eBPF 的概念、架构和典型用途。

从 BPF 到 eBPF
========================

1992 年 **BPF** 最初用于 tcpdump 等工具过滤网络包，在内核解释执行简单过滤指令。2014 年起 **eBPF** 大幅扩展：

- **图灵完备** ：支持循环、函数调用（受验证器约束）
- **map** ：内核与用户态共享数据的键值存储
- **多种程序类型** ：kprobe、tracepoint、XDP、tc（traffic control）等
- **JIT** ：编译为本地机器码，性能接近原生

eBPF 程序由 **验证器** （verifier）检查：无无限循环、无非法内存访问、栈深度有限，保证内核安全。

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

**bcc** （BPF Compiler Collection）：Python/Lua 前端，便于编写追踪脚本。**bpftrace** ：类似 awk 的单行追踪语言。**libbpf** ：CO-RE（Compile Once, Run Everywhere）加载预编译的 eBPF 对象。

.. code-block:: bash

   # 示例：跟踪 connect 系统调用（需安装 bcc-tools）
   sudo biolatency-bpfcc

bpftrace 一行实验
==========================

安装 ``bpftrace`` 后，无需编写 C 即可探测内核：

.. code-block:: bash

   # 统计各进程 connect() 调用次数
   sudo bpftrace -e 'tracepoint:syscalls:sys_enter_connect { @[comm] = count(); }'

   # 跟踪本机 TCP 连接建立（按 PID）
   sudo bpftrace -e 'kprobe:tcp_v4_connect /pid == $1/ { printf("%s\n", comm); }' -p $(pgrep echo_server)

按 ``Ctrl-C`` 结束并打印聚合结果。bpftrace 脚本在启动时由 LLVM 编译为 eBPF 字节码，经 ``bpf()`` 加载。

验证器常见拒绝原因
==========================

eBPF 验证器（``kernel/bpf/verifier.c``）在加载前静态分析程序，典型拒绝原因：

- 未初始化的栈变量被读取
- 指针运算越界或类型不匹配
- 循环无界（无法证明终止）
- 访问未授权的 map 或 helper

初学者可先用 ``bpftrace`` 和 ``bpftool prog dump xlated`` 观察已加载程序的指令，再过渡到 libbpf CO-RE 开发。

map 与用户态通信
========================

eBPF 程序通过 **map** 向用户态导出数据或接收配置：

- ``BPF_MAP_TYPE_HASH`` ：哈希表
- ``BPF_MAP_TYPE_ARRAY`` ：数组
- ``BPF_MAP_TYPE_PERF_EVENT_ARRAY`` ：性能事件

用户态程序 ``bpf()`` 系统调用加载 eBPF 字节码，通过 map 的 fd 读写数据。

典型应用场景
========================

- **可观测性** ：跟踪延迟、丢包、TCP 重传（tcptracer、tcplife）
- **安全** ：运行时检测异常行为（Falco）
- **负载均衡** ：Katran、Cilium 的 DSR
- **网络策略** ：Kubernetes NetworkPolicy 的 eBPF 实现

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
