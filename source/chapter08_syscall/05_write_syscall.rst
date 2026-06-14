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

实践环节将用 strace、perf 观察系统调用，并用内联汇编直接调用 ``getpid`` 和 ``write``。
