================================
实践：Namespace 实验
================================

用 ``unshare`` 和 ``nsenter`` 亲手创建和进入隔离环境，理解容器的基础隔离机制。

UTS Namespace：独立主机名
==========================

.. code-block:: bash

   hostname
   sudo unshare -u bash -c 'hostname isolated; hostname; exec bash'
   # 在新 shell 中 hostname 为 isolated
   # 另开终端，宿主机 hostname 未变

PID Namespace：独立进程树
==========================

.. code-block:: bash

   sudo unshare --pid --fork --mount-proc bash
   ps aux
   # 仅见 PID 1（bash）和子进程，无宿主机其他进程

退出该 shell 即回到宿主机 namespace。

Mount Namespace 与 /proc
==========================

``--mount-proc`` 在新 mount ns 挂载 ``/proc``，使 ``ps`` 反映 PID ns 内容。否则 ``/proc`` 仍显示宿主机进程。

网络 Namespace：veth 组网实验
================================

单独 ``unshare -n`` 只能看到 ``lo``。要让隔离环境与外界通信，需创建 **veth 对** （虚拟以太网对）：一端留在宿主机，另一端移入目标网络命名空间，再配置 IP 与路由。

.. code-block:: text

   宿主机 namespace                    容器/实验 namespace (ns1)
   ┌─────────────┐                    ┌─────────────┐
   │  veth0      │◄──── veth 对 ────►│  veth1      │
   │ 192.168.100.1                   │ 192.168.100.2
   └─────────────┘                    └─────────────┘

**方式一：使用 ``ip netns`` （推荐）**

.. code-block:: bash

   # 仓库提供一键脚本
   sudo source/code/chap10/setup_veth_netns.sh ns1

   # 或手工分步
   sudo ip netns add ns1
   sudo ip link add veth0 type veth peer name veth1
   sudo ip link set veth1 netns ns1
   sudo ip addr add 192.168.100.1/24 dev veth0
   sudo ip link set veth0 up
   sudo ip netns exec ns1 ip addr add 192.168.100.2/24 dev veth1
   sudo ip netns exec ns1 ip link set veth1 up
   sudo ip netns exec ns1 ip link set lo up

   # 从命名空间 ping 宿主机
   sudo ip netns exec ns1 ping -c 2 192.168.100.1

   # 清理
   sudo ip netns del ns1
   sudo ip link del veth0

**方式二：配合 ``unshare -n``**

在 ``unshare -n`` 创建的 shell 中，进程已处于新 netns，但接口需由 **另一终端** 创建 veth 并 ``ip link set veth1 netns <pid>`` 移入。Docker 的 ``docker0`` 桥接 + veth 正是此模式的自动化版本。

.. code-block:: bash

   sudo unshare -n bash
   # 新 shell 中: echo $$ 记下 PID，在宿主机另一终端配置 veth

User Namespace 体验
==========================

User namespace 允许非 root 在命名空间内表现为 UID 0，是 rootless 容器的基础：

.. code-block:: bash

   # 映射当前用户为 ns 内 root（无需 sudo 创建 user ns 时）
   unshare --user --map-root-user bash
   id    # 显示 uid=0，但仅在该 ns 内有效

与 Docker 对比：``docker run --userns=keep-id`` 保留宿主机 UID 映射，避免容器内 root 文件落盘为宿主 root 属主。

进入 Docker 容器 Namespace
==========================

.. code-block:: bash

   docker run -d --name nstest alpine sleep 3600
   PID=$(docker inspect -f '{{.State.Pid}}' nstest)
   sudo nsenter -t $PID -m -u -i -n -p bash
   # 若容器无 bash，用 sh
   hostname
   ip addr

``-m -u -i -n -p`` 分别进入 mount、uts、ipc、net、pid namespace（与目标进程共享的）。

查看 Namespace 链接
==========================

.. code-block:: bash

   ls -l /proc/self/ns/
   ls -l /proc/$PID/ns/

相同 inode 编号的 ``net``、``pid`` 等表示同一 namespace。

与第 3 章的联系
==========================

Namespace 隔离进程视图，但同一内核的容器间仍共享内核攻击面。第 9 章 seccomp、LSM 与 namespace 叠加，构成纵深防御。

下一节实验 cgroup 资源限制。
