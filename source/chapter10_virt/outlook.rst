======================
延伸与趋势
======================

KVM 硬件虚拟化 + namespace/cgroup 容器构成当今 Linux 基础设施两极。二者融合（:strong:`安全容器` 、:strong:`Wasm 沙箱` 、:strong:`K8s 编排` ）是主要演进方向。

工业界现状
==========================

公有云大量 KVM 虚拟机；微服务与 CI 以容器为主。本章 ``lab_namespaces``、``lab_cgroups`` 与 ``setup_veth_netns.sh`` 演示隔离与资源限制的底层机制。

值得关注的变化
==========================

- :strong:`Kubernetes 标准化编排` ：Pod（共享 net ns）、CRI（containerd）、CNI（Calico/Cilium）将本章 primitive 产品化；排障仍依赖 ``crictl``、``nsenter``、``cgroup`` 路径。
- :strong:`gVisor / Kata Containers` ：用户态内核或轻量 VM 增强隔离，弥补共享内核容器的攻击面；性能与兼容性权衡于原生 runc。
- :strong:`WebAssembly 容器` ：runwasi 等用 Wasm 作为可移植沙箱单元，适合边缘与多租户 FaaS；底层常仍依赖 namespace/cgroup。
- :strong:`Cgroup v2 统一` ：新发行版默认 v2 单一层次，``memory.max``、``cpu.max`` 语义更清晰（``lab_cgroups`` 手工实验）；systemd 作为 cgroup 控制器集成更深。

与本教程的衔接
==========================

理解 ``unshare`` / ``pivot_root`` / cgroup 分步清单（``05_container``）有助于读懂 OCI/runc 规范，而非仅会 ``docker run``。

进一步了解
==========================

- Kubernetes 官方文档、OCI runtime-spec
- 内核 ``Documentation/virt/kvm/``、``Documentation/admin-guide/cgroup-v2.rst``
- 第 7 章 Cilium 网络、第 9 章容器安全加固

.. note::

   容器与 K8s 生态迭代快，本节约 2025–2026 年方向撰写；以 OCI 与内核 cgroup 文档为稳定参考。
