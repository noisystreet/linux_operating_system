======================
同步与互斥
======================

多进程或多线程访问共享资源时，若缺乏协调，会导致数据不一致、死锁等问题。:strong:`同步` （synchronization）机制确保同一时刻只有一个执行流访问临界区，或按约定顺序访问。本节介绍互斥锁、信号量、条件变量和 RCU。

临界区与竞态条件
========================

:strong:`临界区` （critical section）是访问共享资源的代码段。:strong:`竞态条件` （race condition）指多个执行流并发进入临界区，结果依赖于调度顺序，行为不可预测。

经典例子：两个线程各执行 100 万次 ``counter++``，期望结果为 200 万，实际往往小于 200 万——因为 ``counter++`` 非原子，涉及读-改-写三步，可能被交错执行。

解决思路：:strong:`互斥` （mutual exclusion）——同一时刻仅一个执行流在临界区内。

互斥锁（Mutex）
========================

:strong:`互斥锁` 是最基本的同步原语。加锁后，其他尝试加锁的线程阻塞，直到持有者解锁。

POSIX 接口：

.. code-block:: cpp

   pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
   pthread_mutex_lock(&mutex);
   // 临界区
   pthread_mutex_unlock(&mutex);

C++11 标准库：

.. code-block:: cpp

   #include <mutex>
   std::mutex mtx;
   {
       std::lock_guard<std::mutex> lock(mtx);
       // 临界区
   }  // 自动解锁

.. warning::

   加锁后务必解锁，否则会导致死锁。使用 ``lock_guard`` 或 ``unique_lock`` 可避免忘记解锁。不要在持有锁时调用可能阻塞的未知代码，以免降低并发度或引发死锁。

信号量（Semaphore）
========================

:strong:`信号量` 维护一个计数器，支持 ``wait`` （P 操作，减 1，若为 0 则阻塞）和 ``post`` （V 操作，加 1，唤醒等待者）。

- :strong:`二元信号量` ：取值为 0 或 1，等价于互斥锁
- :strong:`计数信号量` ：可用于资源池（如连接池最多 N 个连接）

.. code-block:: cpp

   sem_t sem;
   sem_init(&sem, 0, 1);   // 初始值 1，用于互斥
   sem_wait(&sem);
   // 临界区
   sem_post(&sem);

POSIX 命名信号量 ``sem_open()`` 可用于进程间同步。

条件变量（Condition Variable）
==================================

:strong:`条件变量` 用于"等待某个条件成立"。线程在条件不满足时阻塞，其他线程改变条件后 ``signal`` 唤醒。

典型模式：生产者-消费者

.. code-block:: cpp

   pthread_mutex_lock(&mutex);
   while (queue.empty())
       pthread_cond_wait(&cond, &mutex);   // 释放锁并等待
   // 消费
   pthread_mutex_unlock(&mutex);

``pthread_cond_wait`` 必须在循环中检查条件，防止虚假唤醒。条件变量常与互斥锁配合，因为检查条件和等待必须是原子的。

死锁
========================

:strong:`死锁` 指多个执行流互相等待对方持有的锁，永远无法继续。四个必要条件（Coffman 条件）：

1. 互斥：资源只能被一个持有者占用
2. 持有并等待：持有锁的同时等待其他锁
3. 不可抢占：锁只能由持有者主动释放
4. 循环等待：存在等待环

预防死锁：按固定顺序加锁、使用 ``try_lock`` 超时、避免嵌套锁。调试时可用 ``pstack``、``gdb`` 查看线程栈，或启用 ``pthread_mutex`` 的错误检查属性。

读写锁
========================

:strong:`读写锁` （rwlock）允许多个读者同时访问，写者独占。适合读多写少场景。

.. code-block:: cpp

   pthread_rwlock_rdlock(&rwlock);   // 读锁
   pthread_rwlock_wrlock(&rwlock);   // 写锁
   pthread_rwlock_unlock(&rwlock);

RCU：读-拷贝-更新
========================

:strong:`RCU` （Read-Copy-Update）是 Linux 内核广泛使用的无锁同步机制，适合读多写极少场景（如路由表、链表遍历）。

读者无需加锁，直接访问共享数据。写者修改时：分配新副本 → 修改副本 → 原子替换指针 → 等待所有读者离开旧数据后释放旧副本。

RCU 的"宽限期"（grace period）保证读者不会看到已释放的内存。用户态有 ``liburcu`` 等实现，但主要应用于内核（``include/linux/rcupdate.h``）。

RCU 在内核中的实现
==========================

Linux 内核 RCU 基于:strong:`发布-订阅` 模型，核心 API 在 ``include/linux/rcupdate.h``：

.. list-table::
   :header-rows: 1
   :widths: 28 52

   * - API
     - 作用
   * - ``rcu_read_lock()`` / ``rcu_read_unlock()``
     - 读者进入/离开 RCU 读侧临界区
   * - ``rcu_dereference(ptr)``
     - 读者安全解引用指针
   * - ``rcu_assign_pointer(ptr, new)``
     - 写者发布新指针
   * - ``call_rcu(head, func)``
     - 宽限期结束后回调释放旧数据
   * - ``synchronize_rcu()``
     - 写者阻塞等待宽限期结束

读侧临界区在单核上禁止抢占，在多核上不可被调度到同 CPU 的 ``synchronize_rcu`` 阻塞，因此读者:strong:`无锁` 且开销极低。

宽限期（Grace Period）
==========================

写者更新数据后，旧副本不能立即 ``kfree``——可能仍有读者持有旧指针。:strong:`宽限期` 是一段时间窗口，保证所有在更新前开始的读侧临界区都已结束：

.. code-block:: text

   写者: 分配 new → 修改 new → rcu_assign_pointer → call_rcu(释放 old)
   读者: rcu_read_lock → rcu_dereference → 访问 → rcu_read_unlock
   内核: 等待所有 CPU 经历 quiescent state → 调用 call_rcu 回调

``kernel/rcu/tree.c`` 实现 Tree RCU，跟踪每个 CPU 的静止状态（quiescent state）。``synchronize_rcu()`` 会阻塞写者直到宽限期完成。

内核中的典型用例
==========================

- :strong:`路由表` ：``fib_rules``、``rtable`` 更新，读路径无锁遍历
- :strong:`模块链表` ：``module_list`` 的插入删除
- :strong:`PID 哈希` ：进程查找与 fork/exit 并发
- :strong:`文件系统 dentry` ：部分路径缓存更新

以简化链表删除为例（内核文档风格伪代码）：

.. code-block:: c

   // 读者
   rcu_read_lock();
   struct item *p = rcu_dereference(global_head);
   while (p) { /* 遍历 */ p = rcu_dereference(p->next); }
   rcu_read_unlock();

   // 写者删除节点
   struct item *old = /* 从链表摘除 */;
   call_rcu(&old->rcu, item_free_rcu);  // 宽限期后 kfree

与互斥锁的选型
==========================

.. list-table::
   :header-rows: 1
   :widths: 22 38 38

   * - 场景
     - 推荐
     - 原因
   * - 读多写极少
     - RCU
     - 读者零锁开销
   * - 读写均衡
     - rwlock / mutex
     - RCU 写者成本较高
   * - 写频繁
     - mutex
     - RCU 宽限期延迟回收

用户态 ``pthread_rwlock`` 对应内核 rwlock；RCU 无直接 pthread 等价物，但理解 RCU 有助于阅读内核网络、VFS 等子系统源码。

阅读路线
==========================

1. ``include/linux/rcupdate.h`` —— API 定义与内存屏障语义
2. ``kernel/rcu/tree.h`` —— Tree RCU 数据结构
3. ``Documentation/RCU/`` —— 官方设计文档
4. 在 ``net/ipv4/route.c`` 或 ``fs/dcache.c`` 搜索 ``rcu_read_lock`` 查看真实用法

原子操作
========================

C++11 和 GCC 提供:strong:`原子操作` ，硬件保证单条指令完成，无需锁：

.. code-block:: cpp

   #include <atomic>
   std::atomic<int> counter{0};
   counter++;   // 原子自增

底层可能使用 x86 的 ``LOCK`` 前缀或 ``cmpxchg`` 指令。简单计数器用原子操作比互斥锁更轻量。

futex：用户态同步的基石
========================

Linux 的 pthread 互斥锁、条件变量底层依赖:strong:`futex` （Fast Userspace Mutex）。无竞争时，加锁解锁完全在用户态完成；仅当需要阻塞/唤醒时才通过 ``futex()`` 系统调用进入内核。这使常见情况下的同步开销极低。

进程与线程管理是操作系统的核心。下一节通过命令行和 C++ 程序，亲手观察进程的创建、调度和通信。
