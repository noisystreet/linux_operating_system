=================
虚拟化与容器
=================

虚拟化和容器技术改变了现代基础设施的面貌。本章介绍 Linux 上的
硬件虚拟化（KVM）和操作系统级虚拟化（容器）的实现原理。

学习目标
========

- 对比 VM 与容器隔离模型；说明 namespace 与 cgroup 的分工
- 手工或借助工具体验 ``unshare``、cgroup ``memory.max``（``lab_*``）
- 理解 OCI 镜像、Docker 分层与 Kubernetes Pod 的基本架构

先修要求：第 3、4、7、9 章；**建议先完成 ``lab_namespaces`` 再作 ``lab_cgroups``**。

.. toctree::
   :maxdepth: 2

   01_history
   02_kvm
   03_namespaces
   04_cgroups
   05_container
   lab_namespaces
   lab_cgroups
   outlook
   references