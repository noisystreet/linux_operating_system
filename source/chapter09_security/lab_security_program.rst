================================
实践：权限与 Capability 编程
================================

通过 C++ 程序理解 UID 切换、setuid 程序行为，以及 capability 与 ``prctl`` 的配合。

.. note::

   本节全部示例（``print_ids``、``setuid_demo``、``keepcaps_demo``、``secure_file``、``seccomp_demo``）源码见 ``source/code/chap09/`` 。在 ``source/code`` 目录执行 ``make user`` 可一键编译。

获取与打印 UID
==========================

.. code-block:: cpp

   #include <iostream>
   #include <unistd.h>

   int main() {
       std::cout << "真实 UID (uid):  " << getuid() << "\n"
                 << "有效 UID (euid): " << geteuid() << "\n"
                 << "真实 GID (gid):  " << getgid() << "\n"
                 << "有效 GID (egid): " << getegid() << "\n";
       return 0;
   }

``getuid()`` 为真实 UID，``geteuid()`` 为有效 UID（setuid 程序中二者可能不同）。

setuid 演示（需 root 编译安装）
======================================

以下程序演示 setuid 后有效 UID 变化。:strong:`仅用于学习` ，勿在生产环境部署不可信 setuid 程序。

.. code-block:: cpp

   #include <iostream>
   #include <unistd.h>
   #include <sys/types.h>

   int main() {
       std::cout << "切换前 euid: " << geteuid() << "\n";

       if (setuid(65534) != 0) {   // nobody 的 UID，因系统而异
           perror("setuid");
           return 1;
       }

       std::cout << "切换后 euid: " << geteuid() << "\n";
       // 此时已无 root 权力（若原为 root）
       return 0;
   }

若以 root 运行，``setuid(nobody)`` 永久降权，无法恢复 root（无 CAP_SETUID 时）。

prctl 与 KEEPCAPS
==========================

setuid 通常会清空 capability。``PR_SET_KEEPCAPS`` 可在 setuid 后保留 permitted 集合：

.. code-block:: cpp

   #include <iostream>
   #include <unistd.h>
   #include <sys/prctl.h>

   int main() {
       if (prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0) < 0) {
           perror("prctl");
           return 1;
       }

       // 假设以 root 启动，setuid 降权
       if (setuid(1000) < 0) {
           perror("setuid");
           return 1;
       }

       std::cout << "setuid 后 euid=" << geteuid() << "\n";
       // 需 cap_set_proc 等将 permitted 中的 cap 转入 effective
       return 0;
   }

完整 capability 管理需 ``libcap`` （``cap_set_proc``、``cap_get_proc``）。生产服务常用：root 启动 → 绑定端口 → setuid 降权 → 保留 ``CAP_NET_BIND_SERVICE`` 等。

文件权限创建
==========================

.. code-block:: cpp

   #include <iostream>
   #include <fcntl.h>
   #include <unistd.h>
   #include <sys/stat.h>

   int main() {
       const char* path = "/tmp/secure_file.txt";
       int fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0600);
       if (fd < 0) { perror("open"); return 1; }

       write(fd, "secret\n", 7);
       close(fd);

       struct stat st;
       stat(path, &st);
       std::cout << "mode: " << std::oct << (st.st_mode & 0777) << std::dec << "\n";
       return 0;
   }

``0600`` 即仅所有者可读写。进程的 ``umask`` 会与此做与运算。

安全编程原则
==========================

- 最小权限：能不用 root 就不用
- 输入验证：防止路径遍历、注入
- 避免 setuid：优先 capability 或独立特权进程
- 使用 LSM 和 seccomp 加固服务

安全机制保护单机和多用户环境。下一章介绍:strong:`虚拟化与容器` —— 在共享内核上实现更强隔离的另一条路径。

拓展阅读
==========

- ``man 7 credentials``、``man 7 capabilities``、``man 5 acl``
- SELinux 文档：Red Hat SELinux User's and Administrator's Guide
