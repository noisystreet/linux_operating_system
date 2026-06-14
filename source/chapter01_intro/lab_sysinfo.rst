==========================
实践：系统信息查看
==========================

理论讲完了，现在打开终端，亲自看看你的 Linux 系统长什么样。这一节的命令不需要 root 权限，放心操作。

.. note::

   本节 C++ 示例 ``sysinfo.cpp`` 的完整源码见 ``source/code/chap01/`` 。在 ``source/code`` 目录执行 ``make user`` 可一键编译全部用户态示例。

查看内核与发行版信息
======================

.. code-block:: bash

   # 内核版本
   uname -a

   # 发行版信息
   cat /etc/os-release

   # 主机名与内核版本
   hostnamectl

`uname -a` 的输出包含内核版本、主机名、硬件架构等信息。试着看看你的内核是什么版本——对照前一节的版本号规则，理解主版本、次版本和补丁号的含义。

探索 /proc 文件系统
====================

/proc 是一个:strong:`虚拟文件系统`，内核运行时通过它暴露系统状态。目录下的每个数字文件夹对应一个运行的进程。

.. code-block:: bash

   # 查看 CPU 信息
   cat /proc/cpuinfo

   # 查看内存信息
   cat /proc/meminfo

   # 查看系统运行时间
   cat /proc/uptime

   # 查看内核版本（另一种方式）
   cat /proc/version

`/proc/meminfo` 的输出很有意思——对比一下 `MemTotal` 和你的物理内存总量，通常会略少一些，因为内核和启动时保留的内存不计入其中。

观察系统实时状态
==================

.. code-block:: bash

   # 实时观察系统运行状态（每 1 秒刷新一次）
   vmstat 1

   # 对 output 的解释
   vmstat 1 5   # 每 1 秒采样一次，共 5 次

``vmstat`` 的输出列含义：

- ``procs > r`` ：等待 CPU 的进程数（不包含正在运行的）
- ``memory > free`` ：空闲内存（KB）
- ``swap > si / so`` ：换入/换出磁盘的数据量（非零说明内存紧张）
- ``system > in / cs`` ：中断数（interrupts）和上下文切换数（context switches）
- ``cpu > us / sy / id / wa`` ：用户态 CPU / 内核态 CPU / 空闲 / 等待 I/O 的占比

在另一个终端运行一些操作（比如 `ls -R /` 遍历文件系统），观察 `cs` （context switch）和 `wa` （I/O wait）的变化。

用 C++ 程序获取系统信息
==========================

理论联系实际，写一个小程序调用系统接口获取硬件信息。

.. code-block:: cpp

   #include <iostream>
   #include <sys/utsname.h>
   #include <sys/sysinfo.h>

   int main() {
       // uname() 系统调用
       struct utsname buf;
       if (uname(&buf) == 0) {
           std::cout << "系统: " << buf.sysname << "\n"
                     << "主机名: " << buf.nodename << "\n"
                     << "内核版本: " << buf.release << "\n"
                     << "架构: " << buf.machine << "\n";
       }

       // sysinfo() 系统调用
       struct sysinfo info;
       if (sysinfo(&info) == 0) {
           std::cout << "已运行: " << info.uptime << " 秒\n"
                     << "总内存: " << info.totalram / (1024*1024) << " MB\n"
                     << "空闲内存: " << info.freeram  / (1024*1024) << " MB\n"
                     << "进程数: " << info.procs << "\n";
       }

       return 0;
   }

保存为 `sysinfo.cpp`，编译运行：

.. code-block:: bash

   g++ -std=c++17 -o sysinfo sysinfo.cpp
   ./sysinfo

输出的信息和你用 `uname -a`、`cat /proc/meminfo` 看到的应该一致——因为底层调用的是同一个内核接口。

拓展阅读
==========

- `man 2 uname` —— uname 系统调用的手册页
- `man 2 sysinfo` —— sysinfo 系统调用的手册页
- `man proc` —— /proc 文件系统的完整文档
- 浏览 `/proc/self/` —— 当前进程自己的 proc 条目

.. todo:: 增加更多 /proc 文件的实验操作，比如修改 /proc/sys/ 下的运行时参数。