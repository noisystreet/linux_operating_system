======================
KVM 硬件虚拟化
======================

:strong:`KVM` （Kernel-based Virtual Machine）是 Linux 内核的虚拟化模块，将内核变为 Hypervisor，配合 QEMU 提供完整虚拟机。本节介绍 KVM 架构、/dev/kvm 和与 QEMU 的协作。

KVM 架构
========================

.. code-block:: text

   客户机 OS（Guest）
   → 客户机内核/应用
   → KVM 虚拟 CPU（vCPU）
   → /dev/kvm ioctl
   → KVM 内核模块
   → 硬件 VT-x/AMD-V

每个虚拟机是一个:strong:`KVM 虚拟机` 对象，包含一个或多个:strong:`vCPU` 。QEMU 负责设备模拟（磁盘、网卡、显卡），KVM 负责 CPU 和内存虚拟化。

/dev/kvm
========================

KVM 通过字符设备 ``/dev/kvm`` 暴露接口：

.. code-block:: bash

   ls -l /dev/kvm
   lsmod | grep kvm

若文件存在且当前用户在 ``kvm`` 组，可用 QEMU/KVM 创建虚拟机。BIOS/UEFI 需启用虚拟化（Intel VT-x、AMD SVM）。

QEMU 的角色
========================

:strong:`QEMU` 是用户态虚拟机监视器，可纯软件模拟，也可配合 KVM 加速：

.. code-block:: bash

   qemu-system-x86_64 -enable-kvm -m 2048 -hda disk.img -cdrom ubuntu.iso

``-enable-kvm`` 使用硬件加速。无 KVM 时 QEMU 纯模拟，极慢。

virt-manager、GNOME Boxes、cloud 平台的 VM 多基于 libvirt + QEMU + KVM。

内存虚拟化
========================

客户机使用:strong:`Guest Physical Address` （GPA），KVM 通过:strong:`EPT` （Intel）或:strong:`NPT` （AMD）映射到:strong:`Host Physical Address` （HPA）。二级页表由硬件_walk，开销低于软件影子页表。

设备虚拟化
========================

- :strong:`全模拟` ：QEMU 模拟 IDE、e1000 网卡等，兼容性好
- :strong:`半虚拟化` （virtio）：客户机安装 virtio 驱动，与宿主机高效通信，性能优
- :strong:`设备直通` （VFIO）：将物理 PCI 设备直接分配给 VM，低延迟，用于 GPU、NVMe

.. code-block:: bash

   # 查看 VM 是否使用 virtio
   # 客户机内 lspci | grep Virtio

嵌套虚拟化
========================

在 VM 内再运行 KVM（如在云主机上跑 KVM）需:strong:`嵌套虚拟化` 支持，宿主机 CPU 和内核需开启相应选项。

KVM vs 容器
========================

.. list-table::
   :header-rows: 1
   :widths: 15 40 40

   * - 维度
     - KVM 虚拟机
     - 容器
   * - 内核
     - 每 VM 独立客户内核
     - 共享宿主机内核
   * - 隔离
     - 硬件级，强
     - 命名空间，较弱
   * - 启动
     - 秒级到分钟级
     - 毫秒到秒级
   * - 镜像
     - GB 级完整 OS
     - MB 级应用层

需要运行不同内核或强隔离时用 KVM；微服务、CI/CD 等场景多用容器。容器隔离依赖 Namespaces 和 Cgroups——下节介绍。

EPT/NPT 二级地址转换
==========================

客户机操作系统使用:strong:`GPA` （Guest Physical Address）访问"物理"内存，KVM 借助硬件:strong:`二级页表` 将其翻译为宿主机:strong:`HPA` （Host Physical Address）：

.. code-block:: text

   客户机虚拟地址 (GVA)
        → 客户机页表 → GPA
        → EPT/NPT（硬件）→ HPA
        → 宿主机物理内存

Intel 称:strong:`EPT` （Extended Page Tables），AMD 称:strong:`NPT` （Nested Page Tables）。相比软件:strong:`影子页表` ，硬件 walk 大幅降低 VM 内存访问开销，是 KVM 实用化的关键。

QEMU 启动流程详解
==========================

典型启动参数含义：

.. code-block:: bash

   qemu-system-x86_64 \
       -enable-kvm \          # 使用 /dev/kvm 硬件加速
       -cpu host \            # 将宿主机 CPU 特性暴露给客户机
       -m 2048 \              # 内存 2 GiB
       -smp 2 \               # 2 个 vCPU
       -drive file=disk.qcow2,format=qcow2,if=virtio \  # virtio 磁盘
       -cdrom installer.iso \ # 从 ISO 安装
       -boot d                # 从光驱启动

:strong:`qcow2` 是 QEMU 的稀疏磁盘格式，支持快照。安装完成后去掉 ``-cdrom`` 从硬盘启动。``libvirt`` 与 ``virt-manager`` 封装了上述参数，底层仍是 QEMU + KVM。

virtio 半虚拟化
==========================

.. list-table::
   :header-rows: 1
   :widths: 18 38 38

   * - 设备
     - 全模拟（如 e1000）
     - virtio
   * - 磁盘
     - IDE/SATA 模拟，兼容性好
     - virtio-blk，需驱动，IOPS 高
   * - 网卡
     - e1000 模拟
     - virtio-net，批量收发，低 CPU 占用
   * - 实现
     - QEMU 纯软件
     - 客户机驱动与宿主机共享队列（virtqueue）

现代 Linux 发行版安装镜像通常内置 virtio 驱动。云厂商的 VM 几乎一律使用 virtio 以获得接近裸金属的 I/O 性能。
