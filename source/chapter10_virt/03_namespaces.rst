======================
Linux Namespaces
======================

**Namespace** 将内核全局资源包装为独立实例，使进程组看到隔离的 PID、网络、挂载等视图。Namespace 是容器的核心隔离机制之一。本节介绍七种 namespace 及其用途。

七种 Namespace
========================

.. list-table::
   :header-rows: 1
   :widths: 18 42

   * - Namespace
     - 隔离内容
   * - PID
     - 进程 ID 空间，子命名空间 PID 1 独立
   * - Network
     - 网络设备、IP、路由、端口
   * - Mount
     - 挂载点列表
   * - UTS
     - 主机名、域名
   * - IPC
     - System V IPC、POSIX 消息队列
   * - User
     - UID/GID 映射，非 root 可"假 root"
   * - Cgroup
     - cgroup 根目录视图（cgroup ns）

进程创建时继承父进程的 namespace。``clone()`` 或 ``unshare()`` 可创建新 namespace。

clone 与 unshare 标志
==========================

.. list-table::
   :header-rows: 1
   :widths: 22 48

   * - 标志
     - 含义
   * - ``CLONE_NEWPID``
     - 新 PID namespace
   * - ``CLONE_NEWNET``
     - 新网络 namespace
   * - ``CLONE_NEWNS``
     - 新 mount namespace
   * - ``CLONE_NEWUTS``
     - 新 UTS（主机名）
   * - ``CLONE_NEWIPC``
     - 新 IPC namespace
   * - ``CLONE_NEWUSER``
     - 新 user namespace
   * - ``CLONE_NEWCGROUP``
     - 新 cgroup namespace 视图

``unshare(1)`` 对应当前进程调用 ``unshare()``；``docker run`` 底层由 runc 组合上述标志创建隔离环境。内核实现见 ``kernel/nsproxy.c``、``kernel/pid_namespace.c``、``net/core/net_namespace.c`` 等。

veth 与容器网络
========================

容器默认网络模型：

.. code-block:: text

   容器 netns: eth0 ── veth 对 ── vethXXX ── docker0（桥接）
                                              │
                                         宿主机路由/NAT

创建容器时，containerd 在宿主机创建 veth 对，将一端移入容器 netns 并命名为 ``eth0``，另一端接入 ``docker0`` 或自定义 bridge。因此容器内 ``ip addr`` 可见独立 IP，而宿主机 ``ip link`` 可见 ``veth*`` 接口。

unshare 实验
========================

.. code-block:: bash

   # 新 UTS namespace，修改主机名不影响宿主机
   sudo unshare -u hostname mycontainer

   # 新 PID namespace + 新 mount（proc）+ fork
   sudo unshare --pid --fork --mount-proc bash
   ps aux    # 仅见本 namespace 内进程

``--mount-proc`` 在新 mount ns 中挂载 ``/proc``，否则 ``ps`` 仍显示宿主机全部进程。

nsenter
========================

进入已存在进程的 namespace：

.. code-block:: bash

   # 查看某进程（如 Docker 容器主进程）的 namespace
   ls -l /proc/<pid>/ns/

   sudo nsenter -t <pid> -n bash    # 进入网络 namespace
   ip addr                          # 看到容器网络

``/proc/<pid>/ns/*`` 是指向 namespace inode 的符号链接，相同 inode 表示同一 namespace。

Docker 与 Namespace
========================

启动容器时，Docker/containerd 创建新的 PID、NET、MNT、UTS、IPC namespace（通常还有 USER）：

.. code-block:: bash

   docker run -d --name test nginx
   PID=$(docker inspect -f '{{.State.Pid}}' test)
   sudo ls -l /proc/$PID/ns/

容器内 PID 1 为应用进程，与宿主机 PID 无关。网络 namespace 有独立 ``lo``、``eth0`` （veth 对一端）。

.. mermaid::

   flowchart LR
       subgraph host [宿主机 netns]
           BR["docker0 桥接"]
           VH["veth_host"]
       end
       subgraph ctr [容器 netns]
           VE["eth0"]
           APP["应用"]
       end
       VH --- VE
       BR --- VH
       VE --> APP

**图** ：容器 veth 网络模型

User Namespace
========================

**User namespace** 允许非 root 用户在命名空间内映射为 UID 0，在命名空间外仍为普通用户，是 rootless 容器的基础：

.. code-block:: bash

   docker run --userns=keep-id ...

安全边界较其他 namespace 弱，需配合 seccomp、capability 限制。

Namespace 提供"视图隔离"，不限制资源用量。CPU、内存限制由 **Cgroups** 实现——下一节介绍。
