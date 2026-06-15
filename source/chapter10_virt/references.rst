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
     - 虚拟化技术演进
   * - :doc:`02_kvm`
     - KVM、QEMU、virtio、EPT
   * - :doc:`03_namespaces`
     - 七种 namespace、veth 网络
   * - :doc:`04_cgroups`
     - cgroup v2、资源限制
   * - :doc:`05_container`
     - OCI、Docker 架构、overlayfs
   * - :doc:`lab_namespaces`
     - ``unshare``、``nsenter``、veth 组网
   * - :doc:`lab_cgroups`
     - Docker 限制、cgroup 手工实验、QEMU
   * - :doc:`outlook`
     - Kubernetes、安全容器、Wasm

示例代码与脚本
==========================

- ``source/code/chap10/check_kvm.sh``、``limit_cpu.sh``、``setup_veth_netns.sh`` —— :doc:`lab_cgroups`、:doc:`lab_namespaces`

手册页与内核文档
==========================

- ``man 1 unshare``、``man 1 nsenter``、``man 8 ip-netns`` —— :doc:`lab_namespaces`
- ``man 7 cgroups`` —— :doc:`04_cgroups`、:doc:`lab_cgroups`
- ``man 1 systemd-cgls`` —— :doc:`lab_cgroups`
- 内核 ``kernel/nsproxy.c``、``kernel/cgroup/``、``virt/kvm/`` —— 附录 :doc:`/appendix/a3_references` 第 10 章路线

书籍与在线资料
==========================

- `OCI runtime-spec <https://github.com/opencontainers/runtime-spec>`_
- Kubernetes 官方文档 —— Pod、CRI、CNI
- 内核 ``Documentation/admin-guide/cgroup-v2.rst``

相关章节
==========================

- :doc:`/chapter03_process/index` —— 进程与 PID namespace
- :doc:`/chapter04_memory/index` —— cgroup 内存限制
- :doc:`/chapter07_network/index` —— 容器网络与 :doc:`03_namespaces` veth
- :doc:`/chapter09_security/index` —— 容器安全加固
- 附录 :doc:`/appendix/a2_build_kernel` —— :doc:`02_kvm` QEMU 自定义内核实验
