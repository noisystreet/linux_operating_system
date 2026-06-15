======================
I/O 子系统
======================

设备驱动与硬件之间的数据传输，依赖 **中断** 通知和 **DMA** 搬运。块设备 I/O 还经过 I/O 调度器和多队列块层。本节介绍这些机制如何协同工作。

程序控制 I/O（PIO）
========================

**程序控制 I/O** （PIO，Programmed I/O）由 CPU 逐字节从设备端口或内存映射 I/O 区域读写数据。实现简单，但 CPU 全程参与，效率低，仅用于慢速设备或配置寄存器。

中断驱动 I/O
========================

**中断** 使设备在 I/O 完成或需要服务时通知 CPU。流程：

1. 驱动向设备发出 I/O 命令
2. CPU 继续执行其他任务（或睡眠等待）
3. 设备完成后触发中断
4. CPU 跳转到中断处理程序（ISR）
5. ISR 处理完成事件，唤醒等待的进程

.. code-block:: bash

   cat /proc/interrupts

输出列出各 IRQ 号、各 CPU 核心处理次数、设备名称。共享中断（如 PCI 设备共享 IRQ 线）时，多个设备名出现在同一行。

中断处理分 **上半部** （top half，ISR，快速、不可阻塞）和 **下半部** （bottom half，软中断、tasklet、workqueue，可延迟处理繁重工作）。

DMA
========================

**DMA** （Direct Memory Access）允许设备直接与内存交换数据，无需 CPU 逐字节搬运。驱动分配 DMA 缓冲区，告诉设备缓冲区物理地址，设备完成传输后通过中断通知。

.. note::

   DMA 使用 **物理地址** 。驱动通过 ``dma_alloc_coherent()`` 或 ``scatter-gather`` 列表获取 DMA 可用内存，内核处理 IOMMU 映射（若存在）。

大容量传输（磁盘读写、网络包）几乎总是 DMA，PIO 仅用于控制寄存器访问。

块 I/O 栈
========================

块设备 I/O 自顶向下经过：

.. code-block:: text

   文件系统（ext4 等）
   → page cache
   → 通用块层（bio）
   → I/O 调度器（可选）
   → 块设备驱动（nvme、sda）
   → 硬件

**bio** （block I/O）描述一次 I/O 请求：起始扇区、方向、数据页。多个 bio 可合并（merge）以减少中断次数。

I/O 调度器
========================

**I/O 调度器** 对 bio 排序、合并，优化磁盘访问：

.. list-table::
   :header-rows: 1
   :widths: 20 50

   * - 调度器
     - 特点
   * - mq-deadline
     - 默认之一，兼顾延迟和吞吐，防止饥饿
   * - none
     - 不做调度，NVMe 多队列时常用
   * - bfq
     - 桌面友好，交互进程 I/O 优先
   * - kyber
     - 低延迟导向

.. code-block:: bash

   cat /sys/block/sda/queue/scheduler
   # 查看与切换调度器（需 root）

SSD 和 NVMe 延迟低、并行度高，传统为 HDD 设计的调度器收益减小，``none`` 在 NVMe 上常见。

多队列块层（blk-mq）
========================

**blk-mq** （multi-queue block layer）为每 CPU 或每 NUMA 节点维护独立硬件队列，减少锁竞争，匹配 NVMe 的多队列硬件架构。高 IOPS 场景下显著提升性能。

观察 I/O 性能
========================

.. code-block:: bash

   iostat -x 1

关键列：

- ``r/s``、``w/s`` ：每秒读写次数
- ``rkB/s``、``wkB/s`` ：每秒读写 KB 数
- ``await`` ：平均 I/O 等待时间（ms）
- ``%util`` ：设备利用率，接近 100% 表示饱和

设备驱动处理硬件细节，用户空间如何感知设备插拔和创建节点？下一节介绍 udev。
