第 10 章实践说明
================

第 10 章「虚拟化与容器」的实践以系统命令为主（``unshare``、``nsenter``、``docker``、
``systemd-cgls`` 等），无需独立 C++ 示例程序。

辅助脚本
========

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 脚本
     - 说明
   * - ``check_kvm.sh``
     - 检测 KVM 与 QEMU 环境
   * - ``limit_cpu.sh``
     - cgroup v2 CPU 限制演示（需 root）
   * - ``setup_veth_netns.sh``
     - veth + 网络命名空间组网（需 root）

.. code-block:: bash

   ./check_kvm.sh
   sudo ./limit_cpu.sh 3
   sudo ./setup_veth_netns.sh ns1

请参阅教程中的 ``lab_namespaces.rst`` 与 ``lab_cgroups.rst``。
