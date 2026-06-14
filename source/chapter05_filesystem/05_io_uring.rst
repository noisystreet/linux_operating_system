======================
文件 I/O 与 io_uring
======================

应用程序调用 read/write 读写文件，数据往往经过:strong:`页缓存` （page cache）而非直接访问磁盘。传统 I/O 模型在超高并发场景下成为瓶颈，:strong:`io_uring` 带来了新的异步 I/O 接口。本节介绍文件 I/O 路径、O_DIRECT 和 io_uring。

读写的完整路径
========================

普通缓冲 I/O（buffered I/O）的读路径：

.. code-block:: text

   read(fd, buf, len)
   → 检查 page cache 是否有所需页
   → 命中：直接从 page cache 拷贝到用户 buf
   → 未命中：发起读 I/O，从磁盘加载到 page cache，再拷贝到用户 buf

写路径：

.. code-block:: text

   write(fd, buf, len)
   → 数据拷贝到 page cache（可能标记为 dirty）
   → 立即返回（write 返回时数据未必在磁盘）
   → 后台 pdflush/flusher 线程定期将 dirty 页写回磁盘

:strong:`pdflush` 或 ``kworker`` 根据 ``dirty_ratio``、``dirty_expire_centisecs`` 等参数决定写回时机。``sync()``、``fsync()``、``fdatasync()`` 可强制刷盘。

page cache 的作用
========================

:strong:`页缓存` 将最近访问的文件数据缓存在内存中：

- :strong:`读加速` ：重复读取同一文件无需再次访问磁盘
- :strong:`写合并` ：多次小写可合并为顺序写
- :strong:`预读` （readahead）：顺序读时内核提前加载后续页

``free`` 中的 ``buff/cache``  largely 是 page cache，可回收给应用程序。不要用 ``free`` 的 ``used`` 判断内存紧张，应看 ``available``。

.. code-block:: bash

   # 查看 page cache 统计
   grep -E 'Cached|Dirty' /proc/meminfo

   # 清空 page cache（仅测试用，需 root）
   # echo 3 | sudo tee /proc/sys/vm/drop_caches

O_DIRECT：绕过 page cache
==================================

:strong:`O_DIRECT` 标志使 read/write 绕过 page cache，在用户缓冲与磁盘间直接 DMA（需满足对齐要求）。适用于数据库等自行管理缓存的应用，避免双重缓存。

.. code-block:: cpp

   int fd = open("data.bin", O_RDWR | O_DIRECT);
   // buf 和 len 通常需 512 字节对齐

限制：对齐要求高、不支持 mmap、小 I/O 效率可能更低。多数应用使用默认缓冲 I/O 即可。

同步与异步 I/O
========================

:strong:`同步 I/O` ：read/write 在数据完成前阻塞。简单直观，高并发时线程阻塞等待 I/O。

:strong:`异步 I/O` （AIO，``libaio``）：提交 I/O 请求后立即返回，完成后通过信号或回调通知。Linux 原生 AIO 仅支持 O_DIRECT，且对普通文件支持有限，使用较少。

:strong:`io_uring` （2019 年起，Linux 5.1+）是现代的异步 I/O 接口，通过共享环形队列减少系统调用开销。

io_uring 简介
========================

io_uring 包含两个环形队列：

- :strong:`提交队列` （SQ）：用户放入 I/O 请求（read、write、open、accept 等）
- :strong:`完成队列` （CQ）：内核放入完成结果

用户态与内核态共享内存映射的 ring buffer，批量提交、批量收割，大幅减少 ``ioctl`` 和上下文切换。高 QPS 场景（如 NVMe SSD、网络服务器）性能提升显著。

基本使用流程（概念）：

.. code-block:: text

   1. io_uring_queue_init()  创建 ring
   2. 准备 io_uring_sqe，设置操作类型和参数
   3. io_uring_submit()      提交到内核
   4. io_uring_wait_cqe()    等待完成
   5. io_uring_cqe_seen()    标记已处理

库 ``liburing`` 封装了上述接口。数据库（如 PostgreSQL）、Web 服务器等已开始集成 io_uring。

I/O 调度器与块层
========================

数据从 page cache 写回时，经过:strong:`块层` （block layer）和:strong:`I/O 调度器` 到达驱动。调度器合并相邻请求、排序以减少寻道（HDD）或均衡负载（SSD）。常见调度器：mq-deadline、none（NVMe 常用）、bfq（桌面交互）。

``iostat -x 1`` 可观察磁盘利用率、await、svctm 等（第 6 章实践会用到）。

零拷贝
========================

:strong:`sendfile()``、:strong:`splice()`` 等系统调用可在文件与 socket 间直接传递数据，无需经过用户空间，减少拷贝次数。Nginx、静态文件服务器常用 ``sendfile`` 发送文件。

.. code-block:: c

   sendfile(out_fd, in_fd, offset, count);

文件 I/O 是应用与存储的交汇点。下一节通过命令和 C++ 程序观察 inode、挂载和读写行为。
