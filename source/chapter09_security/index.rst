==================
安全与权限
==================

安全是多用户操作系统的基石。本章从传统的 Unix 权限模型讲起，
逐步深入到 Capabilities、LSM 框架等 Linux 安全机制。

学习目标
========

- 运用 ``chmod``/``chown``、ACL、``setcap`` 配置最小权限
- 对比 DAC、capability、SELinux/AppArmor、seccomp 的适用场景
- 运行 ``seccomp_demo`` 与 setuid/capability 示例（``lab_security_*``）

先修要求：第 8 章 syscall 边界；第 10 章容器安全与本章内容互补。

.. toctree::
   :maxdepth: 2

   01_history
   02_permission
   03_capabilities
   04_acls
   05_lsm
   lab_security_tools
   lab_security_program
   outlook
   references