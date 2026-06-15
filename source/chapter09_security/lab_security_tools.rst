================================
实践：安全与权限命令
================================

动手实验 DAC、ACL、Capabilities 和 SELinux/AppArmor 状态查看，理解各层安全机制的实际效果。

权限实验
==========================

.. code-block:: bash

   touch /tmp/myfile
   ls -l /tmp/myfile
   chmod 600 /tmp/myfile
   chmod u+x /tmp/myfile

   # 以其他用户测试（若有测试账号）
   # su - otheruser -c "cat /tmp/myfile"   # 应拒绝

.. code-block:: bash

   umask
   umask 077
   touch /tmp/private.txt
   ls -l /tmp/private.txt

setuid 观察
==========================

.. code-block:: bash

   ls -l /usr/bin/passwd
   ls -l /usr/bin/sudo

观察 ``s`` 位。**不要** 对不可信程序设置 setuid。

ACL 实验
==========================

.. code-block:: bash

   mkdir -p /tmp/acl_test
   touch /tmp/acl_test/file1
   getfacl /tmp/acl_test/file1

   setfacl -m u:$(whoami):rw /tmp/acl_test/file1
   getfacl /tmp/acl_test/file1

若有第二用户，可 ``setfacl -m u:otheruser:r`` 并验证访问。

Capabilities
==========================

.. code-block:: bash

   getcap -r /usr/bin 2>/dev/null | head -10
   capsh --print

查看 ping、tcpdump 等是否带文件 capability。``capsh --drop=cap_net_raw -- -c 'ping -c1 127.0.0.1'`` 可演示丢弃能力后的失败（若 ping 依赖该 cap）。

SELinux / AppArmor
==========================

.. code-block:: bash

   # SELinux（若安装）
   getenforce 2>/dev/null || echo "SELinux 未启用"
   sestatus 2>/dev/null

   # AppArmor
   sudo aa-status 2>/dev/null || echo "AppArmor 未启用"

Ubuntu 默认 AppArmor，RHEL 系默认 SELinux。虚拟机镜像可能简化配置。

进程安全信息
==========================

.. code-block:: bash

   grep -E 'Uid|Gid|CapInh|CapPrm|CapEff|Seccomp' /proc/self/status

``CapPrm``/``CapEff`` 为 capability 位图十六进制。``Seccomp`` 为 0 表示未过滤。

协作目录 ACL 实验
==========================

模拟项目组共享目录：owner 完全控制，特定协作者只读，新建文件自动继承权限。

.. code-block:: bash

   mkdir -p /tmp/project/docs
   chmod 2770 /tmp/project/docs          # setgid，新建文件继承组
   chgrp $(id -gn) /tmp/project/docs

   touch /tmp/project/docs/readme.txt
   setfacl -m u:$(whoami):rw /tmp/project/docs/readme.txt

   # 默认 ACL：新建文件自动给协作者读权限
   setfacl -d -m u:$(whoami):r /tmp/project/docs
   setfacl -d -m g:$(id -gn):rx /tmp/project/docs

   touch /tmp/project/docs/newfile.txt
   getfacl /tmp/project/docs/newfile.txt   # 应看到 default 继承项

若有第二用户 ``bob``，可 ``setfacl -m u:bob:r /tmp/project/docs`` 并 ``su - bob -c 'cat ...'`` 验证。ACL 解决了"单一组无法涵盖所有协作者"的 DAC 局限。

SELinux 排错演练（Fedora/RHEL 或启用 SELinux 的系统）
========================================================

.. code-block:: bash

   getenforce                    # Enforcing / Permissive / Disabled
   ls -Z /etc/passwd             # 查看 SELinux 上下文 user_u:object_r:passwd_file_t

在 **Permissive** 模式下，违反策略的操作会记录但不拒绝，适合学习：

.. code-block:: bash

   sudo setenforce 0               # 临时 Permissive（实验用）
   # 触发某服务访问受限文件...
   sudo ausearch -m avc -ts recent
   sudo audit2allow -a -M myfix    # 根据 AVC 生成策略模块建议
   sudo semodule -i myfix.pp       # 加载策略（理解含义后再做）

**audit2allow** 输出的是建议而非真理——盲目加载可能扩大攻击面。正确做法是理解 AVC 日志中的 ``scontext``、``tcontext``、``tclass``，在策略中精确授权。

AppArmor profile 浏览
==========================

.. code-block:: bash

   sudo aa-status
   sudo aa-complain /usr/sbin/tcpdump   # 切换为抱怨模式（仅记录）
   cat /etc/apparmor.d/usr.sbin.tcpdump

profile 按 **路径** 限制程序行为，比 SELinux 类型标签直观。容器可通过 ``--security-opt apparmor=profile.json`` 附加自定义 profile。

seccomp 动手实验
==========================

**方式一：C++ 程序** （``source/code/chap09/seccomp_demo``）

.. code-block:: bash

   cd source/code/chap09 && make seccomp_demo
   ./seccomp_demo    # 安装 BPF 过滤器后调用 socket 会被 SIGSYS 终止

**方式二：systemd 限制**

.. code-block:: bash

   systemd-run --scope -p SystemCallFilter=@system-service \
       -p SystemCallLog=exec ls

**方式三：Docker 默认 profile**

.. code-block:: bash

   docker run --rm alpine grep Seccomp /proc/1/status
   # Seccomp: 2 表示 FILTER 模式

seccomp 与 LSM 互补：LSM 控制"谁可以访问什么资源"，seccomp 控制"进程可以调用哪些 syscall"。详见 :doc:`05_lsm` 与第 8 章系统调用。

下一节用 C++ 演示 setuid 与 ``prctl(PR_SET_KEEPCAPS)``。
