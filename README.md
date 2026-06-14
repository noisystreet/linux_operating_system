# Linux 操作系统教程

> 基于 Linux 的操作系统原理入门教程，使用 Sphinx + reStructuredText 构建。
>
> 在线文档：[https://linux-operating-system.readthedocs.io/zh-cn/latest/](https://linux-operating-system.readthedocs.io/zh-cn/latest/)

## 目标读者

- 具备 C/C++ 语言基础和基本 Linux 使用经验的开发者
- 希望深入理解操作系统原理的学生或工程师
- 对 Linux 内核感兴趣的初学者

## 教程特色

- **原理讲解** —— 以 Linux 为主线，覆盖操作系统核心概念
- **动手实践** —— 每章配套系统命令操作、现代 C++ 示例程序或内核实验
- **源码导向** —— 关键机制引导读者阅读内核源码（`/proc`、`/sys`、内核文档）

## 教程规划

### 第 1 章：操作系统概述

**原理：**
- 什么是操作系统 —— 定义、职责、抽象模型
- 操作系统发展简史 —— 从批处理到现代内核
- 操作系统分类 —— 批处理、分时、实时、分布式
- Linux 简介 —— 历史、发行版、内核版本

**实践：**
- 使用 `uname -a`、`lsb_release -a` 查看系统与内核信息
- 浏览 `/proc/version`、`/proc/cpuinfo`、`/proc/meminfo` 了解内核暴露的系统信息
- 使用 `vmstat 1` 实时观察系统运行状态
- 编写 C++ 程序调用 `uname()`、`sysinfo()` 系统接口获取系统信息

### 第 2 章：系统启动

**原理：**
- BIOS 与 UEFI 固件
- Bootloader 原理 —— GRUB 为例
- Linux 内核启动流程 —— 从 `start_kernel` 到 `init`
- init 系统 —— SysVinit vs systemd

**实践：**
- 使用 `dmesg` 查看内核环形缓冲区中的启动日志
- `systemd-analyze blame` 分析各服务启动耗时
- 查看 `/boot` 目录结构，了解内核镜像与 initramfs
- 编写 C++ 最小 init 程序并用 `qemu` 启动自制内核实验环境

### 第 3 章：进程与线程管理

**原理：**
- 进程概念 —— PCB、进程状态、上下文切换
- 线程模型 —— 用户级 vs 内核级线程、NPTL
- CPU 调度算法 —— O(1)、CFS、实时调度
- 进程间通信 —— 管道、信号、共享内存、消息队列
- 同步与互斥 —— 互斥锁、信号量、RCU

**实践：**
- `ps auxf`、`pstree -p`、`top -H` 查看进程/线程树
- `strace -p <pid>` 跟踪进程系统调用
- `ls /proc/<pid>/` 浏览进程的 procfs 接口
- `taskset` 绑定 CPU 核心观察调度效果
- 编写 C++ 程序：`fork()` 多进程创建与 `wait()` 回收
- 编写 C++ 程序：`pthread_create()` 多线程与 `pthread_mutex_t` 同步
- 编写 C++ 程序：管道通信与 `mmap` 共享内存

### 第 4 章：内存管理

**原理：**
- 内存寻址 —— 分段与分页、x86-64 四级页表
- 虚拟内存 —— COW、`mmap`、缺页异常
- Linux 内存管理 —— 伙伴系统、slab 分配器、KSM
- CMA、ZRAM 与 swap 机制

**实践：**
- `free -h`、`cat /proc/meminfo` 查看内存布局
- `vmstat 1` 观察 page in/out 与 swap 活动
- `cat /proc/zoneinfo` 查看内存区域和水位线
- `dd if=/dev/zero of=/swapfile bs=1M count=1024 && mkswap && swapon` 手工管理 swap
- 编写 C++ 程序：`malloc()` / `mmap()` 分配大块内存，观察 `/proc/self/maps`
- 编写 C++ 程序：访问已释放内存触发 SIGSEGV，用 `gdb` 分析 core dump

### 第 5 章：文件系统

**原理：**
- 文件系统抽象 —— inode、dentry、super_block
- VFS（虚拟文件系统）架构
- ext4 文件系统详解 —— 索引节点、块组、日志
- 其他文件系统 —— Btrfs、XFS、tmpfs
- 文件 I/O 流程 —— page cache、O_DIRECT、io_uring

**实践：**
- `stat`、`ls -li` 查看 inode 号与文件元数据
- `df -i` 查看各文件系统 inode 使用情况
- `dd` + `losetup` 创建并挂载 ext4 文件系统镜像
- `strace -e trace=openat,read,write cat file` 跟踪文件 I/O 系统调用
- `mount --bind`、`tmpfs` 实验
- 编写 C++ 程序：`open()` / `read()` / `write()` 标准 I/O 与 `O_DIRECT` 绕过 page cache

### 第 6 章：设备与 I/O 管理

**原理：**
- Linux 设备驱动模型 —— bus、device、driver
- 字符设备、块设备、网络设备
- I/O 子系统 —— 中断、DMA、I/O 调度器
- udev 与设备文件自动管理

**实践：**
- `lsmod`、`modinfo` 查看内核模块
- 浏览 `/sys/class/`、`/sys/bus/`、`/sys/devices/` 设备树
- `cat /proc/interrupts` 查看中断分配
- `udevadm monitor` 监控设备热插拔事件
- `iostat -x 1` 观察磁盘 I/O
- 编写简单字符设备驱动（`register_chrdev` + `file_operations`）

### 第 7 章：网络协议栈

**原理：**
- TCP/IP 协议栈概览
- Linux 网络子系统的分层架构
- socket 编程模型与内核实现
- Netfilter 与 iptables/nftables
- eBPF 基础

**实践：**
- `ip addr`、`ip route`、`ss -tlnp`、`tcpdump` 网络诊断
- 编写 C++ 程序：TCP echo server/client（socket / bind / listen / accept）
- `strace -e trace=network curl http://example.com` 跟踪网络系统调用
- `iptables -L -n -v` 查看规则计数；编写简单 Netfilter 内核模块

### 第 8 章：系统调用

**原理：**
- 系统调用概念与 POSIX 接口
- x86-64 系统调用实现 —— `syscall` 指令
- Linux syscall 表与 vsyscall/vDSO
- 编写简单的系统调用模块

**实践：**
- `strace -c ls` 统计各 syscall 频次
- `perf stat -e syscalls:sys_enter_* ls` 用 perf 跟踪 syscall
- `cat /proc/kallsyms | grep sys_call_table` 查看 syscall 表地址
- `cat /proc/kallsyms | grep vdso` 了解 vDSO 映射
- 编写 C++ 程序：内联汇编触发 `syscall` 指令直接调用 `getpid`
- 编写内核模块添加自定义 syscall

### 第 9 章：安全与权限

**原理：**
- DAC（自主访问控制）—— 用户/组/权限位
- Capabilities 机制
- ACL（访问控制列表）
- LSM（Linux Security Module）框架
- SELinux / AppArmor 简介

**实践：**
- `chmod`、`chown`、`umask` 权限实验
- `getcap`、`setcap`、`capsh --print` 操作 Capabilities
- `setfacl`、`getfacl` 操作 ACL
- `getenforce`、`audit2allow` 等 SELinux 命令演示
- 编写 C++ 程序：`setuid()` 提权与 `prctl(PR_SET_KEEPCAPS)` 控制 Capabilities

### 第 10 章：虚拟化与容器

**原理：**
- 硬件辅助虚拟化 —— KVM
- Linux Namespaces —— 进程隔离
- Cgroups —— 资源控制
- 轻量级虚拟化 —— 容器原理

**实践：**
- `unshare --pid --fork --mount-proc bash` 手工创建 Namespace 隔离环境
- `nsenter -t <pid> -n bash` 进入已有 Namespace
- `ls /sys/fs/cgroup/` 查看 cgroup 子系统；编写 `cgroup` 限制 CPU 使用
- 使用 `pivot_root` + `chroot` 制作最小容器文件系统
- `ls /dev/kvm` 检测 KVM 支持；`virt-manager` / `qemu-system-x86_64` 创建虚拟机

### 附录

- A：常用 Linux 内核调试工具 —— GDB、ftrace、perf、strace
- B：如何编译与安装自定义内核
- C：推荐阅读与参考资料

## 构建方法

```bash
# 安装依赖
pip install -r requirements.txt

# 构建 HTML
make html

# 构建 PDF（需要 LaTeX）
make latexpdf

# 清理
make clean

# 本地预览
make serve
```

## 项目结构

```
.
├── README.md              # 项目规划（本文件）
├── .gitignore
├── Makefile               # Sphinx 构建入口
├── requirements.txt       # Python 依赖
├── source/
│   ├── conf.py            # Sphinx 配置
│   ├── index.rst          # 文档首页
│   ├── glossary.rst       # 术语表
│   ├── _static/           # 静态资源
│   ├── _templates/        # Jinja2 模板
│   ├── chapter01_intro/   # 第 1 章
│   ├── chapter02_boot/    # 第 2 章
│   ├── chapter03_process/ # 第 3 章
│   ├── chapter04_memory/  # 第 4 章
│   ├── chapter05_filesystem/ # 第 5 章
│   ├── chapter06_device/  # 第 6 章
│   ├── chapter07_network/ # 第 7 章
│   ├── chapter08_syscall/ # 第 8 章
│   ├── chapter09_security/# 第 9 章
│   ├── chapter10_virt/    # 第 10 章
│   └── appendix/          # 附录
└── build/                 # 构建输出（gitignore）
```

## 许可证

本教程采用 **双许可** 方式：

| 内容 | 许可证 |
|---|---|
| 文档（`.rst` 文件） | [CC BY 4.0](LICENSE) |
| 示例代码（C++ / C 程序） | [MIT](LICENSE-code) |