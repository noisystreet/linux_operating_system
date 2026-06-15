==========================
参考资料与补充阅读
==========================

本章内容索引
==========================

.. list-table::
   :header-rows: 1
   :widths: 28 52

   * - 节
     - 主题
   * - :doc:`01_history`
     - 安全模型演进
   * - :doc:`02_permission`
     - DAC、``chmod``、setuid
   * - :doc:`03_capabilities`
     - capability 位、``setcap``
   * - :doc:`04_acls`
     - POSIX ACL、xattr
   * - :doc:`05_lsm`
     - SELinux、AppArmor、seccomp、Landlock
   * - :doc:`lab_security_tools`
     - 权限、ACL、SELinux、seccomp 实验
   * - :doc:`lab_security_program`
     - ``setuid``、``prctl``、安全文件权限
   * - :doc:`outlook`
     - BPF LSM、机密计算

示例代码
==========================

- ``source/code/chap09/`` —— 见 :doc:`lab_security_program` 与 :doc:`lab_security_tools`

手册页与内核文档
==========================

- ``man 2 chmod``、``man 2 setuid``、``man 2 getuid`` —— :doc:`02_permission`、:doc:`lab_security_program`
- ``man 5 capabilities``、``man 8 setcap`` —— :doc:`03_capabilities`、:doc:`lab_security_tools`
- ``man 1 getfacl``、``man 1 setfacl`` —— :doc:`04_acls`、:doc:`lab_security_tools`
- ``man 8 sestatus``、``man 1 aa-status`` —— :doc:`05_lsm`、:doc:`lab_security_tools`
- 内核 ``security/``、``kernel/seccomp.c`` —— 附录 :doc:`/appendix/a3_references` 第 9 章路线

书籍与在线资料
==========================

- 内核 ``Documentation/security/``
- SELinux / AppArmor 官方文档
- 附录 :doc:`/appendix/a1_debug_tools` —— :doc:`lab_security_tools` 权限拒绝 ``strace`` 场景

相关章节
==========================

- :doc:`/chapter08_syscall/index` —— LSM/seccomp 在 syscall 路径上的作用
- :doc:`/chapter10_virt/index` —— 容器 capability drop、seccomp profile
- :doc:`/chapter07_network/outlook` —— BPF LSM 与 eBPF
