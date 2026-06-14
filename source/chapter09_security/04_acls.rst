======================
访问控制列表（ACL）
======================

传统 Unix 权限只有 owner/group/other 三组。:strong:`ACL` （Access Control List）为单个文件添加任意用户或组的独立权限，适合协作目录、多租户共享等场景。

ACL 扩展
========================

POSIX ACL 扩展标准权限，增加:

- :strong:`ACL_USER` ：指定用户的权限
- :strong:`ACL_GROUP` ：指定组的权限
- :strong:`ACL_MASK` ：上限掩码，限制 named user/group 的最大权限
- :strong:`default ACL` ：目录的默认 ACL，新建文件继承

查看与设置
==========================

.. code-block:: bash

   getfacl file.txt
   getfacl -R project_dir/

输出示例：

.. code-block:: text

   # file: shared.txt
   # owner: alice
   # group: dev
   user::rw-
   user:bob:r--
   group::r--
   mask::rw-
   other::---

``user:bob:r--`` 表示 bob 有读权限，与 owner/group/other 独立。

.. code-block:: bash

   setfacl -m u:bob:rw shared.txt
   setfacl -m g:contractors:r-x shared_dir/
   setfacl -d -m u:bob:rw shared_dir/    # 默认 ACL

``-m`` 修改，``-d`` 设置默认 ACL（仅目录），``-x`` 删除条目，``-b`` 清除全部 ACL。

与 chmod 的关系
========================

设置 ACL 后，``ls -l`` 可能显示 ``+`` 表示有扩展 ACL：

.. code-block:: bash

   ls -l shared.txt
   # -rw-rw-r--+ 1 alice dev ...

``chmod`` 修改 group 权限时，实际可能更新 ``ACL_MASK`` 而非 ``group::``。``getfacl`` 可看清实际生效权限。

何时使用 ACL
========================

- 项目组共享目录，不同成员不同权限
- 无法通过单一组涵盖所有协作者
- Web 服务器用户对多个用户目录只读访问

简单场景用 owner/group/other 和补充组即可，ACL 增加管理复杂度，按需使用。

文件系统支持
========================

ext4、XFS、Btrfs 等支持 POSIX ACL，挂载时可能需要 ``acl`` 选项：

.. code-block:: bash

   mount | grep acl
   # /etc/fstab 中可加 acl 选项

ACL 仍属 DAC——用户若有文件写权限，可修改 ACL 授予他人访问。要防止所有者随意授权，需:strong:`强制访问控制` （LSM）。下一节介绍 LSM 框架及 SELinux、AppArmor。
