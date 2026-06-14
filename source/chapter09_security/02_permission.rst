======================
DAC 与文件权限
======================

Linux 默认采用:strong:`自主访问控制` （DAC）：每个文件有所有者，所有者（和 root）可自主决定访问权限。本节介绍用户/组、权限位、umask 和 setuid 机制。

用户、组与其他
========================

每个进程有:strong:`有效 UID` （EUID）和:strong:`有效 GID` （EGID），决定访问文件时的身份。每个文件 inode 记录:strong:`所有者 UID`、:strong:`所属组 GID` 和:strong:`权限位` 。

访问检查时，内核按顺序匹配：

1. 若 EUID 等于文件 UID → 使用 owner 权限位
2. 否则若 EGID 或补充组匹配文件 GID → 使用 group 权限位
3. 否则 → 使用 other 权限位

.. code-block:: bash

   id
   id username
   groups

权限位 rwx
========================

三组各三位：:strong:`r` （读）、:strong:`w` （写）、:strong:`x` （执行）。目录的 ``x`` 表示:strong:`进入目录` 的权限（cd、访问其中文件），而非列出目录（列出需 ``r``）。

.. code-block:: bash

   ls -l /etc/passwd
   # -rw-r--r-- 1 root root 1234 ...

数字表示：r=4，w=2，x=1。``755`` = rwxr-xr-x，``644`` = rw-r--r--。

.. code-block:: bash

   chmod 644 file.txt
   chmod u+x script.sh
   chmod go-w secret.dat

chown 与特权
========================

.. code-block:: bash

   sudo chown user:group file
   sudo chown user file

修改所有者通常需 root。普通用户只能修改自己拥有的文件的组（且须属于目标组）。

umask
========================

:strong:`umask` 决定:strong:`新建文件/目录` 的默认权限掩码。创建时权限为 ``mode & ~umask``。

.. code-block:: bash

   umask
   umask 022    # 文件默认 666&~022=644，目录 777&~022=755

``022`` 是常见默认值：禁止 group/other 写。过于宽松的 ``000`` 会导致新建文件全局可写。

setuid、setgid 与 sticky
========================

特殊权限位：

.. list-table::
   :header-rows: 1
   :widths: 12 48

   * - 位
     - 含义
   * - setuid (s)
     - 执行时 EUID 变为文件所有者 UID
   * - setgid (s)
     - 执行时 EGID 变为文件组 GID；目录下新建文件继承组
   * - sticky (t)
     - 目录中只有文件所有者可删除自己的文件（如 /tmp）

.. code-block:: bash

   ls -l /usr/bin/passwd    # -rwsr-xr-x，setuid root
   ls -ld /tmp              # drwxrwxrwt，sticky

``passwd`` 需以 root 身份写 ``/etc/shadow``，故设 setuid。:strong:`setuid root` 程序是重大安全风险，应尽量减少。

进程权限检查流程
========================

``open()`` 访问文件时，内核（简化）：

1. 解析路径，找到 inode
2. 取进程 EUID/EGID 和补充组
3. 按 DAC 规则检查 rwx
4. 若启用 LSM，再交 SELinux/AppArmor 等二次检查
5. 通过则允许，否则 ``EACCES``

.. code-block:: bash

   # 以其他用户身份运行命令
   sudo -u nobody cat /etc/shadow   # 应失败

DAC 简单有效，但无法表达"Alice 可读、Bob 可写、Charlie 无权限"等细粒度需求。下一节介绍 Capabilities，再下一节介绍 ACL 和 LSM。
