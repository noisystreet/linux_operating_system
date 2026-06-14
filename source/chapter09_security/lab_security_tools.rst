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

观察 ``s`` 位。:strong:`不要` 对不可信程序设置 setuid。

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

下一节用 C++ 演示 setuid 与 ``prctl(PR_SET_KEEPCAPS)``。
