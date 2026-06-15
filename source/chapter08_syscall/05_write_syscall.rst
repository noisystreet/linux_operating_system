======================
write 系统调用追踪
======================

以 ``write`` 为例，从用户态调用追踪到内核实现，串联前面各节概念。``write`` 是最简单的 I/O 系统调用之一，却涉及 fd 表、VFS、page cache 等子系统。

用户态入口
========================

.. code-block:: c

   ssize_t write(int fd, const void *buf, size_t count);

glibc 将 ``write`` 映射为系统调用号 1（x86-64）：

.. code-block:: text

   write() → __libc_write() → __syscall_write()
   → syscall(SYS_write, fd, buf, count)

strace 验证：

.. code-block:: bash

   strace -e write echo hello

输出类似：``write(1, "hello\n", 6) = 6``。fd 1 为 stdout。

常见错误返回值：

.. list-table::
   :header-rows: 1
   :widths: 18 42

   * - 返回值
     - 含义
   * - ``-EBADF``
     - fd 无效或已关闭
   * - ``-EFAULT``
     - ``buf`` 指向不可访问的用户地址
   * - ``-EPIPE``
     - 写入已关闭读端的管道（默认触发 SIGPIPE）
   * - ``-ENOSPC``
     - 磁盘或配额已满

内核入口：sys_write
========================

``SYSCALL_DEFINE3(write, ...)`` 调用 ``ksys_write()`` （``fs/read_write.c``）：

.. code-block:: text

   ksys_write(fd, buf, count)
   → fget_light(fd)           # 从 fd 表获取 struct file
   → vfs_write(file, buf, count)
   → file->f_op->write_iter() # 或 write
   → 具体文件系统或 tty、pipe、socket 实现

若 fd 指向普通文件，最终数据进入 page cache；若指向终端，由 tty 驱动输出；若指向 socket，进入网络栈。

不同后端的路径分叉
========================

``struct file`` 的 ``f_op`` 决定最终处理函数。下表概括常见 fd 类型：

.. list-table::
   :header-rows: 1
   :widths: 22 48

   * - fd 类型
     - write 路径
   * - 普通文件（ext4 等）
     - ``generic_file_write_iter`` → page cache dirty 页
   * - 终端 ``/dev/tty``
     - ``tty_write`` → 控制台驱动
   * - 管道 pipe
     - ``pipe_write`` → 环形缓冲区
   * - socket
     - ``sock_write_iter`` → 协议栈发送队列

可用 ``ls -l /proc/self/fd/`` 查看 fd 指向，``readlink`` 显示目标路径或 ``socket:[inode]`` 等形式。

内核源码导读（节选）
========================

``fs/read_write.c`` 中 ``vfs_write`` 的核心逻辑（简化）：

.. code-block:: c

   ssize_t vfs_write(struct file *file, const char __user *buf,
                     size_t count, loff_t *pos)
   {
       if (!(file->f_mode & FMODE_WRITE))
           return -EBADF;
       if (!file->f_op->write_iter)
           return -EINVAL;
       return file->f_op->write_iter(&kiocb, &iov_iter);
   }

对常规文件，``ext4_file_write_iter`` 最终调用 ``generic_perform_write`` ，将用户数据通过 ``copy_from_user`` 拷入 page cache。这与第 5 章 ``06_io_uring`` 讨论的缓冲 I/O 路径一致。

VFS 层
========================

``vfs_write()`` （``fs/read_write.c``）：

1. 检查 ``file`` 写权限
2. 调用 ``file->f_op->write_iter`` 或 ``generic_file_write_iter``
3. 更新 ``file->f_pos`` 偏移

对于常规文件，``generic_file_write_iter`` 将数据拷贝到 page cache 的 dirty 页，由后台 flusher 写回磁盘。

copy_from_user
========================

``buf`` 指向用户空间，内核必须用 ``copy_from_user`` 或 ``access_ok`` + 直接访问（在确认安全后）读取：

.. code-block:: c

   if (copy_from_user(kernel_buf, buf, count))
       return -EFAULT;

错误指针访问返回 ``-EFAULT``，避免内核崩溃。

返回值
========================

成功：返回实际写入字节数（可能小于 ``count``，如磁盘满、非阻塞 socket）。失败：返回 ``-errno``。``echo hello`` 写入 fd 1 成功返回 6。

与其他系统调用的关系
========================

.. code-block:: text

   printf("hello")  → 用户态缓冲
   → fflush() 或缓冲区满
   → write(1, "hello", 5)
   → sys_write → vfs_write → tty 驱动 → 控制台

``read`` 路径对称，从设备或文件读入用户 ``buf``，用 ``copy_to_user``。

自定义系统调用（概念）
========================

教学或实验可在内核中添加 ``sys_hello``：

1. 在 ``syscall_64.tbl`` 添加：``common  hello  sys_hello``
2. 实现 ``SYSCALL_DEFINE0(hello) { pr_info("Hello from syscall\n"); return 0; }``
3. 重新编译内核，用户态 ``syscall(__NR_hello)``

生产环境极少这样做——新功能倾向通过 ``ioctl``、netlink 或 eBPF 扩展。内核模块无法动态注册 syscall。

用 ftrace 观察 write 路径
========================

除 strace 外，可用内核追踪观察 ``vfs_write`` 或 ``ext4_file_write_iter`` 的调用频率：

.. code-block:: bash

   # 需 root；按内核符号是否可用调整函数名
   sudo perf probe vfs_write
   sudo perf stat -e probe:vfs_write -a sleep 1

或用 ``bpftrace`` 一行统计各进程 write 次数（见第 7 章 eBPF 节）。将用户态 strace 与内核探针结合，可区分"库缓冲导致的 write"与"实际进入内核的次数"。

实践环节将用 strace、perf 观察系统调用，并用内联汇编直接调用 ``getpid`` 和 ``write``。
