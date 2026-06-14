======================
LSM 与安全模块
======================

:strong:`LSM` （Linux Security Module）在内核关键操作点插入钩子，由可加载的安全模块实施:strong:`强制访问控制` （MAC）。用户无法绕过系统策略，即使文件属主也不能随意授权。本节介绍 LSM 框架、SELinux 和 AppArmor。

LSM 框架
========================

LSM 在 DAC 检查:strong:`之后` 进行额外验证。钩子覆盖：

- 文件 inode 的 permission、alloc
- 进程的 execve、ptrace
- socket 的 bind、connect
- capability 的 capable

每个安全模块注册自己的钩子函数。返回值 ``0`` 允许，负值拒绝。多个 LSM 可堆叠（内核 5.4+ 支持）。

.. code-block:: text

   访问请求 → DAC 检查 → LSM 钩子 → 允许/拒绝

SELinux
========================

:strong:`SELinux` （Security-Enhanced Linux）由 NSA 开发，RHEL、Fedora、CentOS 默认启用。基于:strong:`类型强制` （Type Enforcement）：每个进程有域（domain），每个文件有类型（type），策略规定哪些域可访问哪些类型。

.. code-block:: bash

   getenforce
   # Enforcing / Permissive / Disabled

   sestatus
   ls -Z /etc/passwd    # 查看 SELinux 上下文

:strong:`Enforcing` ：违反策略拒绝并记录。:strong:`Permissive` ：仅记录不拒绝，用于调试。:strong:`Disabled` ：关闭 SELinux。

常见排错：

.. code-block:: bash

   sudo ausearch -m avc -ts recent
   sudo audit2allow -a    # 根据拒绝日志生成策略建议

SELinux 策略复杂，学习曲线陡，但提供细粒度强制隔离，容器（sVirt）也依赖它。

AppArmor
========================

:strong:`AppArmor` 由 Ubuntu/SUSE 等采用，基于:strong:`路径` 的 profile：规定某程序可访问哪些路径、可执行哪些能力。

.. code-block:: bash

   sudo aa-status
   cat /etc/apparmor.d/usr.sbin.tcpdump   # 示例 profile

profile 比 SELinux 类型标签直观，适合桌面和通用服务器。Docker 可选用 AppArmor profile 限制容器。

对比
========================

.. list-table::
   :header-rows: 1
   :widths: 15 40 40

   * - 维度
     - SELinux
     - AppArmor
   * - 模型
     - 类型/角色强制
     - 路径 profile
   * - 默认发行版
     - RHEL/Fedora
     - Ubuntu
   * - 学习曲线
     - 较陡
     - 相对平缓
   * - 标签
     - 扩展属性，难篡改
     - 路径，重命名需更新

seccomp 与沙箱
========================

:strong:`seccomp` 限制进程可用的:strong:`系统调用` 集合，与 LSM 互补。Chrome、systemd、Docker 广泛使用。``SECCOMP_MODE_FILTER`` 使用 BPF 程序过滤 syscall。

.. code-block:: bash

   grep Seccomp /proc/self/status

容器默认 seccomp profile 禁止危险 syscall（如 ``reboot``、``kexec_load``）。

LSM 堆叠与 BPF LSM
==========================

Linux 5.4+ 支持多个 LSM 同时启用（:strong:`LSM stacking` ）。查看当前顺序：

.. code-block:: bash

   cat /sys/kernel/security/lsm
   # 例如 lockdown,capability,landlock,yama,apparmor

:strong:`Landlock` （较新 LSM）允许:strong:`非特权进程` 自 sandbox，与 seccomp 类似但可限制文件系统路径。:strong:`BPF LSM` 允许用 eBPF 程序实现自定义 MAC 钩子，适合云原生策略引擎。

seccomp 过滤器原理
==========================

``SECCOMP_MODE_FILTER`` 使用经典 BPF 指令检查每次 syscall 的编号与参数：

.. code-block:: text

   syscall 进入内核
        → seccomp BPF 程序评估
        → ALLOW / ERRNO / TRACE / KILL

Docker 的 ``default.json`` profile 禁止约 40+ 个 syscall（``keyctl``、``bpf``、``mount`` 等）。自定义 profile 可用 ``docker run --security-opt seccomp=profile.json`` 加载。

与第 8 章的联系
==========================

seccomp 过滤器在内核 ``seccomp_run_filters()`` 中执行，位于 syscall 进入路径的早期。理解第 8 章 x86-64 syscall 约定，有助于阅读 seccomp-BPF 如何访问 ``struct seccomp_data`` 中的 ``nr`` 和参数。

安全实践小结
========================

- 不以 root 运行服务；用专用用户 + capability
- 启用发行版默认 LSM（勿随意 Disabled）
- 容器使用 seccomp、cap-drop、只读根文件系统
- 定期更新，最小化攻击面

下一节通过命令和 C++ 程序实践权限、ACL 和 capability 操作。
