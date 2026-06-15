======================
访问控制列表（ACL）
======================

传统 Unix 权限只有 owner/group/other 三组。**ACL** （Access Control List）为单个文件添加任意用户或组的独立权限，适合协作目录、多租户共享等场景。

ACL 扩展
========================

POSIX ACL 扩展标准权限，增加:

- **ACL_USER** ：指定用户的权限
- **ACL_GROUP** ：指定组的权限
- **ACL_MASK** ：上限掩码，限制 named user/group 的最大权限
- **default ACL** ：目录的默认 ACL，新建文件继承

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

ACL 仍属 DAC——用户若有文件写权限，可修改 ACL 授予他人访问。要防止所有者随意授权，需 **强制访问控制** （LSM）。下一节介绍 LSM 框架及 SELinux、AppArmor。

内核中的 ACL 实现
==========================

VFS 层在常规 ``inode_permission()`` 检查之后，调用文件系统特定的 ACL 钩子。ext4 将 POSIX ACL 存储在 **扩展属性** （xattr）中：

.. code-block:: bash

   getfattr -d -m - file_with_acl.txt
   # 可见 security.system.posix_acl_access

相关内核路径：

- ``fs/posix_acl.c`` —— POSIX ACL 通用逻辑
- ``fs/ext4/xattr.c`` —— ext4 扩展属性存储
- ``fs/namei.c`` —— 路径查找与权限检查入口

用户态 ``setfacl``/``getfacl`` 通过 ``setxattr``/``getxattr`` 系统调用操作上述属性。``ls -l`` 显示的 ``+`` 即提示存在扩展 ACL。

C 程序设置 ACL（了解即可）
==========================

.. code-block:: c

   #include <acl/libacl.h>
   acl_t acl = acl_get_file("shared.txt", ACL_TYPE_ACCESS);
   acl_entry_t entry;
   acl_create_entry(&acl, &entry);
   acl_set_tag_type(entry, ACL_USER);
   uid_t bob = 1001;
   acl_set_qualifier(entry, &bob);
   acl_permset_t perm;
   acl_get_permset(entry, &perm);
   acl_add_perm(perm, ACL_READ);
   acl_set_file("shared.txt", ACL_TYPE_ACCESS, acl);
   acl_free(acl);

生产环境优先使用 ``setfacl`` 命令或配置管理工具；直接调用 libacl 适合需要动态授权的应用。
