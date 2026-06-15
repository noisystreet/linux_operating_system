======================
Capabilities
======================

传统上 root 拥有全部权力。**Capabilities** 将 root 特权拆分为独立单元，进程可只保留所需部分，实现最小权限。本节介绍 capability 集合、常用 capability 和 setcap 工具。

能力位图
========================

Linux 将特权分为约 40 个 **capability** ，每个为一位。进程的 capability 分集合管理：

.. list-table::
   :header-rows: 1
   :widths: 22 48

   * - 集合
     - 含义
   * - Permitted
     - 进程可能获得的能力上限
   * - Effective
     - 当前生效的能力
   * - Inheritable
     - exec 时可继承给子进程
   * - Bounding
     - 系统允许的最大集合（可收缩）

非 root 进程默认 capability 为空。root 启动的进程拥有全部 capability，可通过 ``capset`` 或 ``prctl`` 丢弃不需要的。

常用 Capabilities
========================

.. list-table::
   :header-rows: 1
   :widths: 28 42

   * - Capability
     - 作用
   * - CAP_NET_BIND_SERVICE
     - 绑定 1024 以下端口
   * - CAP_NET_RAW
     - 原始套接字（ping、tcpdump）
   * - CAP_SYS_PTRACE
     - 调试其他进程
   * - CAP_DAC_OVERRIDE
     - 绕过文件读写的 DAC 检查
   * - CAP_SETUID / CAP_SETGID
     - 修改进程 UID/GID
   * - CAP_SYS_ADMIN
     - 大量管理操作（挂载等）

完整列表见 ``man 7 capabilities``。

文件 capabilities（setcap）
==================================

可给可执行文件设置 **文件 capability** ，运行该程序时自动获得指定能力，无需完整 root：

.. code-block:: bash

   # 允许 ping 绑定 ICMP 套接字（示例，现代系统可能用不同方式）
   sudo setcap cap_net_raw+ep /usr/bin/ping

   getcap /usr/bin/ping
   # /usr/bin/ping cap_net_raw=ep

``+ep`` 表示加入 effective 和 permitted 集合。``capsh --print`` 查看当前 shell 的 capability。

.. code-block:: bash

   capsh --print

Docker 等容器运行时常丢弃大部分 capability，仅保留必要项。

编程接口
========================

.. code-block:: cpp

   #include <sys/capability.h>   // libcap
   // 或 prctl
   #include <sys/prctl.h>

   // 丢弃除 CAP_NET_BIND_SERVICE 外的所有 effective capability
   // cap_set_proc() 等

   // 保持 capability 跨越 setuid
   prctl(PR_SET_KEEPCAPS, 1);

``PR_SET_KEEPCAPS`` 在 setuid 后保留 permitted capability，便于降权后仍使用部分能力。实践环节将有 setuid 与 capability 示例。

与 root 的对比
========================

.. code-block:: text

   传统 root：全部权力，攻破即全盘沦陷
   Capabilities：nginx 只需 CAP_NET_BIND_SERVICE，无需 CAP_SYS_MODULE

systemd 服务单元可用 ``CapabilityBoundingSet``、``AmbientCapabilities`` 配置。容器用 ``--cap-drop``、``--cap-add`` 精细控制。

Capabilities 细化进程权力，但不改变 DAC 的"所有者决定授权"模型。需要强制策略时，使用 ACL 或 LSM——下两节介绍。
