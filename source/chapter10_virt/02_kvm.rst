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
