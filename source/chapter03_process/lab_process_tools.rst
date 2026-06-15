================================
实践：进程与线程命令操作
================================

前面几节讲了进程、调度、线程和 IPC 的理论，现在用系统命令观察真实 Linux 系统中的进程行为。本节操作大多不需要 root 权限。

查看进程列表
==========================

.. code-block:: bash

   # 所有进程，树形显示
   ps auxf

   # 显示线程（LWP 列）
   ps -eLf | head -20

   # 进程树，含 PID
   pstree -p

``ps auxf`` 中：``USER`` 为所有者，``PID`` 为进程 ID，``%CPU``/``%MEM`` 为资源占用，``STAT`` 为状态（R/S/D/Z 等），``COMMAND`` 为启动命令。``f`` 选项以树形缩进显示父子关系。

实时监视进程
==========================

.. code-block:: bash

   top
   # 或更现代的 htop（需安装）

``top`` 默认按 CPU 使用率排序。常用交互键：

- ``H`` ：切换是否显示线程
- ``M`` ：按内存排序
- ``P`` ：按 CPU 排序
- ``k`` ：向进程发送信号（输入 PID）
- ``q`` ：退出

``top -H -p <pid>`` 仅显示指定进程的线程。

探索 /proc 中的进程
==========================

.. code-block:: bash

   # 当前 shell 的进程信息
   ls /proc/self/
   cat /proc/self/status

   # 查看任意进程（替换 <pid>）
   ls /proc/<pid>/
   cat /proc/<pid>/cmdline | tr '\0' ' '
   cat /proc/<pid>/environ | tr '\0' '\n' | head -5

``status`` 文件中的关键字段：

.. list-table::
   :header-rows: 1
   :widths: 20 50

   * - 字段
     - 含义
   * - ``Name``
     - 进程名
   * - ``State``
     - 运行状态
   * - ``Pid`` / ``PPid``
     - 进程 ID / 父进程 ID
   * - ``VmRSS``
     - 常驻内存（KB）
   * - ``Threads``
     - 线程数

跟踪系统调用
==========================

**strace** 可跟踪进程的系统调用，是理解进程行为的利器：

.. code-block:: bash

   # 跟踪新启动的命令
   strace -f ls /tmp

   # 跟踪已运行进程（需权限）
   strace -p <pid>

   # 统计各系统调用次数
   strace -c sleep 1

``-f`` 跟踪 ``fork()`` 产生的子进程。观察 ``clone``、``mmap``、``openat`` 等调用，可验证前面章节所学。

CPU 亲和性与调度
==========================

.. code-block:: bash

   # 查看进程的 CPU 亲和性
   taskset -p <pid>

   # 将进程绑定到 CPU 0
   taskset -cp 0 <pid>

   # 查看调度策略和优先级
   chrt -p <pid>

绑定到单一 CPU 可降低缓存失效，但可能降低整体吞吐。通常仅在性能调优或测试时使用。

信号实验
==========================

.. code-block:: bash

   # 终端 1：启动睡眠进程
   sleep 300 &
   echo $!    # 记下 PID

   # 终端 2：发送信号
   kill -TERM <pid>    # 请求终止
   kill -9 <pid>       # 强制杀死（若 TERM 无效）

观察 ``ps`` 中进程状态变化。``SIGTERM`` 可被捕获，进程可清理后退出；``SIGKILL`` 立即终止，无法捕获。

IPC 对象查看
==========================

.. code-block:: bash

   ipcs -a    # 查看消息队列、信号量、共享内存

若系统上有 System V IPC 对象，会列出其 key、id、权限等。多数现代应用使用管道、套接字或 POSIX IPC，此命令可能输出较少。

启动流程回顾
==========================

结合第 2 章，从进程角度回顾启动链：

.. code-block:: bash

   ps -p 1 -o pid,comm,args          # PID 1 是 systemd
   pstree -p | head -20              # 进程树从 systemd 展开

每个用户登录会话、每个服务，都是 systemd 通过 ``fork()``/``exec()`` 创建的进程树的一部分。

下一节通过 C++ 程序亲手创建进程和线程，验证 fork、管道和互斥锁的行为。
