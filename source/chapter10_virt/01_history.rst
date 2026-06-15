======================
虚拟化发展史
======================

虚拟化让一台物理机器运行多个相互隔离的操作系统实例，提高资源利用率和灵活性。从纯软件模拟到硬件辅助，再到操作系统级容器，虚拟化技术经历了几个重要阶段。

.. mermaid::

   timeline
       title 虚拟化技术演进（简图）
       1970s : IBM LPAR
       1999  : VMware 二进制翻译
       2005  : Intel VT-x / AMD-V
       2007  : KVM 合入 Linux
       2013  : Docker 标准化容器
       2018+ : K8s 编排 + Kata/gVisor

早期：纯软件虚拟化
========================

1970 年代 IBM 大型机即支持逻辑分区（LPAR）。PC 时代，:strong:`VMware` （1999）通过二进制翻译在 x86 上运行多个客户 OS——x86 架构部分指令非特权级执行时行为不同，难以经典 trap-and-emulate。

软件虚拟化开销大，但无需硬件支持，兼容性好。VMware 在敏感指令路径插入二进制翻译块，将"应在特权态执行"的指令序列改写为安全等价实现。这一思路在 VT-x 出现前是 PC 虚拟化的主流方案。

硬件辅助虚拟化
========================

2005 年前后，Intel:strong:`VT-x` 和 AMD:strong:`AMD-V` 引入硬件虚拟化扩展：

- 新增:strong:`Guest` 和:strong:`Host` 模式
- 敏感指令自动陷入 Hypervisor
- 二级地址翻译（EPT/NPT）加速客户机内存访问

:strong:`KVM` （Kernel-based Virtual Machine，2007）将 Linux 内核变为 Type-1 Hypervisor，利用硬件扩展运行虚拟机，性能接近原生。

QEMU 负责设备模拟（磁盘、网卡、显卡），KVM 负责 CPU 调度和内存虚拟化。用户态通过 ``ioctl(KVM_RUN)`` 进入客户机 vCPU 执行，遇敏感指令或中断时返回宿主机处理。这一分工使 KVM 成为云厂商 IaaS 的默认选择。

操作系统级虚拟化
========================

传统虚拟化每个 VM 运行完整客户 OS，开销大。:strong:`容器` 共享宿主机内核，仅隔离进程、网络、文件系统视图：

- :strong:`chroot` （1979）：最早的原型，仅改变根目录
- :strong:`FreeBSD Jails` （2000）
- :strong:`Linux VServer/OpenVZ` ：早期 Linux 容器
- :strong:`Docker` （2013）：标准化镜像和工具链，引爆容器浪潮

Linux 的:strong:`Namespaces` + :strong:`Cgroups` 构成现代容器基础。

与 KVM 虚拟机相比，容器共享内核，启动可在百毫秒级，镜像体积通常更小；但内核漏洞可能影响所有容器，隔离强度弱于硬件虚拟化。生产环境常采用"VM 上跑容器"的多层隔离：云厂商用 KVM 划分租户，租户内再用 Docker/K8s 调度工作负载。

云原生时代
========================

.. list-table::
   :header-rows: 1
   :widths: 18 42

   * - 技术
     - 作用
   * - KVM/QEMU
     - 虚拟机，强隔离
   * - Docker/containerd
     - OCI 容器运行时
   * - Kubernetes
     - 容器编排
   * - Kata/gVisor
     - 轻量 VM 级隔离容器

虚拟机和容器并非互斥：Kubernetes 节点常跑在 VM 上，Pod 内跑容器，多层隔离叠加。

:strong:`Kata Containers` 为每个 Pod 启动轻量 VM，容器进程运行在客户机内核中，兼顾启动速度与 VM 级隔离。:strong:`gVisor` 则在用户态实现 syscall 拦截层（Sentry），不依赖硬件虚拟化，适合不可信多租户场景。二者与原生 runc 容器在性能、兼容性、安全之间取舍不同，云厂商按工作负载选择运行时。

从完整 VM 到共享内核的容器，Linux 提供了全谱系方案。下一节介绍 KVM 硬件虚拟化。
