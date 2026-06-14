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

网络 Namespace（需 root）
==========================

.. code-block:: bash

   sudo unshare -n bash
   ip link
   # 仅有 lo，无 eth0

在另一终端配置 veth 对连接该 net ns 需额外步骤；Docker 自动完成 veth 创建。

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
