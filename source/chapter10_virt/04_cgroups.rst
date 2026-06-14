======================
Cgroups 资源控制
======================

:strong:`Cgroups` （Control Groups）限制、记录和隔离进程组的资源使用：CPU、内存、I/O、设备等。与 Namespace 结合，构成 Linux 容器的资源管理基础。

Cgroup 版本
========================

- :strong:`cgroup v1` ：每种资源独立层级，多挂载点，配置分散
- :strong:`cgroup v2` ：统一层级，单挂载点 ``/sys/fs/cgroup``，RHEL 9、Ubuntu 22.04+ 默认

.. code-block:: bash

   mount | grep cgroup
   ls /sys/fs/cgroup/

systemd 将每个服务、用户会话、容器放入 cgroup，自动管理资源。

主要控制器
========================

.. list-table::
   :header-rows: 1
   :widths: 18 42

   * - 控制器
     - 作用
   * - cpu
     - CPU 时间片、带宽限制
   * - cpuset
     - 绑定特定 CPU 核心
   * - memory
     - 内存上限、swap 限制
   * - io
     - 块设备 I/O 权重和限速
   * - pids
     - 进程数上限

查看进程所属 cgroup：

.. code-block:: bash

   cat /proc/self/cgroup
   systemd-cgls

memory 限制示例
========================

cgroup v2 下（路径因系统而异）：

.. code-block:: bash

   # 创建测试 cgroup（需 root，路径示例）
   sudo mkdir -p /sys/fs/cgroup/demo
   echo "+memory" | sudo tee /sys/fs/cgroup/cgroup.subtree_control

   echo "100M" | sudo tee /sys/fs/cgroup/demo/memory.max
   echo $$ | sudo tee /sys/fs/cgroup/demo/cgroup.procs

   # 该 shell 及其子进程内存受限 100MB

超出 ``memory.max`` 时，进程可能被 OOM killer 杀死（cgroup 级）。

CPU 限制
========================

.. code-block:: bash

   # 限制为 50% 单核（cgroup v2 cpu.max 格式）
   echo "50000 100000" | sudo tee /sys/fs/cgroup/demo/cpu.max

Docker 的 ``--cpus=0.5``、``--memory=512m`` 即设置相应 cgroup 参数。

systemd 资源控制
========================

.. code-block:: ini

   [Service]
   MemoryMax=512M
   CPUQuota=50%

.. code-block:: bash

   systemctl set-property user-1000.slice MemoryMax=4G

与容器的关系
========================

每个容器通常对应一个 cgroup 子树，包含容器内所有进程。Kubernetes 的 requests/limits 最终映射为 cgroup 配置。Namespace 隔离视图，Cgroup 限制资源——二者正交、互补。

下一节介绍容器如何组合 Namespace、Cgroup、镜像和运行时，并动手实践。
