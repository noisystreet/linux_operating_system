======================
安全机制发展史
======================

多用户操作系统必须防止恶意或故障程序破坏系统和其他用户的数据。安全模型的演进，从简单的 root/普通用户二分，到细粒度的 Capabilities 和强制访问控制，反映了威胁模型与使用场景的复杂化。

早期：无保护
========================

早期单用户系统几乎没有安全概念。任何程序可访问全部内存和文件。Multics 和 Unix 引入 **用户 ID** 和 **权限位** ，实现 **自主访问控制** （DAC，Discretionary Access Control）——文件所有者决定谁可访问。

DAC 至今仍是 Linux 的基础，但仅靠 DAC 难以满足现代需求。

超级用户问题
========================

Unix 的 **root** （UID 0）拥有无限权力：可读写任意文件、杀死任意进程、加载内核模块。一旦服务以 root 运行被攻破，攻击者获得系统完全控制权。

1990 年代起，业界推动 **最小权限原则** ：进程只应拥有完成功能所需的最小权限。这催生了 Capabilities 和后续 LSM。

Capabilities 的诞生
========================

2000 年前后，Linux 将传统 root 权力拆分为 **Capabilities** 独立单元，如 ``CAP_NET_BIND_SERVICE`` （绑定 1024 以下端口）、``CAP_SYS_ADMIN`` （大量管理操作）。进程可只保留所需 capability，降低 root 全有或全无的风险。

ACL 与细粒度权限
========================

传统 Unix 权限仅 owner/group/other 三组。**ACL** （Access Control List）允许为任意用户或组设置独立权限，满足共享目录、协作场景需求。POSIX ACL 在 Linux 通过 ``setfacl``/``getfacl`` 支持。

强制访问控制
========================

DAC 的弱点：文件所有者可将权限授予任何人，无法阻止信息泄露。**MAC** （Mandatory Access Control，强制访问控制）由系统策略统一规定，用户不能绕过。

Linux 通过 **LSM** （Linux Security Module）框架支持可插拔 MAC 实现：

- **SELinux** ：美国 NSA 开发，RHEL/Fedora 默认，策略复杂强大
- **AppArmor** ：Ubuntu/SUSE 常用，基于路径的 profile，相对易用
- **Smack**、**TOMOYO** ：其他实现

容器与隔离
========================

2010 年代，**容器** 和 **命名空间** 提供进程级隔离，**seccomp** 限制系统调用，**cgroups** 限制资源。安全从"单系统多用户"扩展到"多租户共享内核"的新维度。

现代威胁与纵深防御
========================

今日 Linux 安全依赖多层机制叠加：

.. code-block:: text

   用户认证 → DAC 权限位 → ACL → Capabilities
   → LSM（SELinux/AppArmor）→ seccomp → 命名空间隔离

没有单一银弹，纵深防御是实践准则。下一节从最基础的 DAC 权限模型讲起。
