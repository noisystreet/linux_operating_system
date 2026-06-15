======================
Cgroups 资源控制
======================

:strong:`Cgroups` （Control Groups）限制、记录和隔离进程组的资源使用：CPU、内存、I/O、设备等。与 Namespace 结合，构成 Linux 容器的资源管理基础。

Cgroup 将进程组织为树形层次：每个进程恰属于一个 cgroup，子进程默认继承父进程的 cgroup。systemd、Docker、Kubernetes 都在这棵树上为服务、容器或 Pod 分配子树，并写入资源限制。

Cgroup 版本
========================

- :strong:`cgroup v1` ：每种资源独立层级，多挂载点，配置分散
- :strong:`cgroup v2` ：统一层级，单挂载点 ``/sys/fs/cgroup``，RHEL 9、Ubuntu 22.04+ 默认

v1 时代，memory、cpu、blkio 等控制器各自挂载到不同目录，同一进程在不同层级各有一条路径，配置容易遗漏。v2 将所有控制器统一到单棵树，接口更一致，也是新特性的主要承载版本。

.. mermaid::

   flowchart LR
       subgraph v1 [cgroup v1 示意]
           M["/sys/fs/cgroup/memory"]
           C["/sys/fs/cgroup/cpu"]
           P["进程 P"] --> M
           P --> C
       end
       subgraph v2 [cgroup v2 示意]
           R["/sys/fs/cgroup"]
           D["demo/"]
           R --> D
           P2["进程 P"] --> D
       end

:strong:`图` ：v1 多挂载点 vs v2 统一层级（概念示意）

.. code-block:: bash

   mount | grep cgroup
   ls /sys/fs/cgroup/

systemd 将每个服务、用户会话、容器放入 cgroup，自动管理资源。在 v2 系统上，``systemd-cgls`` 展示的路径通常以 ``/sys/fs/cgroup/`` 为根，不再出现 ``/memory/docker/...`` 这类 v1 风格的多根路径。

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
   * - devices
     - 允许访问的字符/块设备节点
   * - freezer
     - 暂停/恢复 cgroup 内所有进程

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

超出 ``memory.max`` 时，进程可能被 OOM killer 杀死（cgroup 级）。与系统级 OOM 不同，cgroup OOM 只回收该组内的进程，不影响其他服务。

除硬上限 ``memory.max`` 外，v2 还提供 ``memory.high`` ：超过阈值时内核开始节流分配（回收 page cache、触发直接回收），进程变慢但不一定被杀。适合作为"软限制"预警。

.. code-block:: bash

   # 观察 cgroup 内存事件（OOM、高水位等）
   cat /sys/fs/cgroup/demo/memory.events
   # 典型字段：low, high, max, oom, oom_kill

CPU 限制
========================

.. code-block:: bash

   # 限制为 50% 单核（cgroup v2 cpu.max 格式）
   echo "50000 100000" | sudo tee /sys/fs/cgroup/demo/cpu.max

Docker 的 ``--cpus=0.5``、``--memory=512m`` 即设置相应 cgroup 参数。

I/O 与进程数限制
========================

cgroup v2 的 ``io`` 控制器可对块设备限速，格式为 ``MAJ:MIN rbps=... wbps=...`` （读/写每秒字节数）：

.. code-block:: bash

   # 将 /dev/sda 主设备号:次设备号 写入 io.max（示例，设备号因机器而异）
   echo "8:0 rbps=1048576 wbps=1048576" | sudo tee /sys/fs/cgroup/demo/io.max

``pids.max`` 限制 cgroup 内进程/线程总数，防止 fork 炸弹拖垮节点：

.. code-block:: bash

   echo 64 | sudo tee /sys/fs/cgroup/demo/pids.max

delegation 与 rootless 容器
========================

非 root 用户要在自己的 cgroup 子树中创建容器，需要:strong:`delegation` ：父 cgroup 的拥有者将子树控制权交给用户。v2 通过 ``cgroup.subtree_control`` 和 ``cgroup.controllers`` 实现：

.. code-block:: bash

   # 父 cgroup 允许子树启用 memory、cpu 控制器（需管理员配置）
   echo "+memory +cpu" | sudo tee /sys/fs/cgroup/user.slice/cgroup.subtree_control

rootless Podman/Docker 依赖 systemd 为用户会话预先配置可写 cgroup 子树。若 ``/sys/fs/cgroup`` 下无写权限，容器运行时无法设置 ``memory.max``，会报错退出。

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

典型映射关系：

.. list-table::
   :header-rows: 1
   :widths: 28 42

   * - 容器/K8s 参数
     - cgroup v2 接口
   * - ``docker run -m 512m``
     - ``memory.max=536870912``
   * - ``--cpus=2``
     - ``cpu.max`` 配额
   * - K8s ``resources.limits.memory``
     - kubelet 写入 Pod cgroup
   * - K8s ``resources.limits.cpu``
     - ``cpu.max`` 或 cpuset（取决于 QoS 类）

排错时，在容器内 ``cat /proc/self/cgroup`` 可看到当前 cgroup 路径，再到宿主机对应目录核对 ``memory.current``、``cpu.stat`` 是否与预期一致。

下一节介绍容器如何组合 Namespace、Cgroup、镜像和运行时，并动手实践。
