======================
操作系统发展史
======================

要理解操作系统是什么，不妨先看看它是怎么来的。操作系统的演进史，实际上就是计算机硬件史的一面镜子——每一代硬件能力的跃升，都推动着操作系统的形态发生质变。

从第一台电子计算机诞生至今，操作系统的演变大致可以分为四个阶段。

零阶段：没有操作系统
========================

1940 年代，像 ENIAC 这样的计算机还没有"操作系统"这个概念。程序员直接操作硬件：拨动开关、插拔电缆来编程。一台机器一次只运行一个程序，从开始到结束独占全部资源。

到了 1950 年代，:strong:`穿孔卡片` （punched card）和:strong:`批处理` （batch processing）出现。用户把程序写在卡片上，操作员一次摞一摞地喂给计算机。这虽然提高了机器利用率，但程序之间仍然没有隔离——一个程序可以把整个系统写坏。

批处理与多道程序
========================

1960 年代，晶体管的普及让计算机变得更可靠、更昂贵。为了不让 CPU 在等待 I/O 时空转，:strong:`多道程序` （multiprogramming）应运而生：内存中同时驻留多个程序，当一个程序等待 I/O 时，CPU 切换到另一个程序运行。

这个阶段的操作系统雏形已经具备了三个核心能力：

- :strong:`内存保护` ：防止一个程序越界读写另一个程序的内存
- :strong:`调度` ：决定哪个程序下一个运行
- :strong:`I/O 管理` ：协调多个程序对设备的访问

IBM System/360 系列上的 OS/360 是这个时期的代表。它的设计者 Frederick Brooks 后来在《人月神话》中回忆，OS/360 的开发复杂度远超预期——这也从侧面说明，操作系统从一开始就是最复杂的软件之一。

分时系统与 Unix
========================

多道程序虽然提高了吞吐量，但与用户的交互方式仍然是批处理式的。程序员把卡片交给操作员，几小时甚至一天后才能拿到结果。这显然不够友好。

1960 年代早期，MIT 的 :strong:`CTSS` （Compatible Time-Sharing System）开创了:strong:`分时` （time-sharing）技术：CPU 在多个终端用户之间快速切换，让每个人感觉自己在独占机器。

分时的思想直接催生了 Unix。1969 年，Ken Thompson 和 Dennis Ritchie 在贝尔实验室的一台废弃的 PDP-7 上写下了 Unix 的雏形。Unix 带来的几项设计至今仍在深刻影响我们：

- :strong:`一切皆文件` （everything is a file）：设备、管道、socket 都通过文件描述符操作
- :strong:`模块化工具` ：一个程序只做一件事，通过管道组合
- :strong:`C 语言实现` ：Unix 首次用高级语言编写，使其具备了可移植性

Unix 的分支演化出了一条蔚为壮观的家谱。其中一个分支经 BSD 演化出了 macOS，另一个分支经 Minix 走向了 Linux。

个人计算机时代
========================

1970-80 年代，微处理器诞生，计算机从机房走进了个人桌面。早期的个人计算机操作系统如 CP/M、MS-DOS 功能十分有限：单用户、单任务、没有内存保护、没有多用户。

MS-DOS 的设计哲学是"够用就好"——它只需要加载程序、提供基本的文件操作。应用程序可以直接访问硬件，系统崩溃是家常便饭。

这个时期也有两条有趣的分支：

- Macintosh System Software（后来的 Mac OS）：率先将图形用户界面带入主流
- MINIX：Andrew Tanenbaum 为教学目的编写的类 Unix 微型内核

MINIX 虽然本身并未广泛商用，但它为 Linus Torvalds 提供了学习和参考的基础。

GNU/Linux 与开源运动
========================

1984 年，Richard Stallman 发起了 :strong:`GNU 项目`，目标是创建一个完全自由的类 Unix 操作系统。到 1990 年代初，GNU 已经拥有了编译器（GCC）、编辑器（Emacs）、shell（Bash）等几乎所有用户态工具——只缺一个内核。

1991 年，芬兰大学生 Linus Torvalds 在邮件列表中宣布了一个"业余项目"：

.. code-block:: text

   Hello everybody out there using minix -
   I'm doing a (free) operating system (just a hobby, won't be big and
   professional like gnu) for 386(486) AT clones.

这个"业余项目"就是 Linux 内核。它采用了 :strong:`GPL 许可证`，加上 GNU 工具链的成熟生态，GNU/Linux 组合迅速崛起。今天，Linux 运行在从嵌入式设备到超级计算机的几乎所有平台上。

现代操作系统格局
========================

进入 21 世纪，操作系统领域呈现出多元并存的格局：

- :strong:`Linux` ：统治服务器、云计算、嵌入式、超级计算领域
- :strong:`Windows` ：桌面与企业市场的主导者
- :strong:`macOS / iOS` ：基于 Darwin（BSD 变体），消费电子领域的标杆
- :strong:`Android` ：基于 Linux 内核，全球最大的移动操作系统
- :strong:`RTOS` ：FreeRTOS、Zephyr 等实时操作系统，用于嵌入式场景

云原生与容器化是近年最具影响力的趋势。容器本质上不是新技术——它利用了 Linux 内核已有的 Namespace 和 Cgroup 机制，但 Docker 等工具将它们包装成了易用的开发体验。

回头看，操作系统的演进有一个清晰的规律：:strong:`每一次硬件能力的跃迁，都让操作系统的抽象层升高一层`。从裸机编程到批处理，从分时到虚拟化，操作系统一直在帮我们管理日益复杂的硬件，让开发者能站在更高的层次上思考问题。

下一节，我们正式定义什么是操作系统，以及它到底做了些什么。

关键人物与事件时间线
==========================

.. list-table::
   :header-rows: 1
   :widths: 15 25 60

   * - 年代
     - 人物 / 项目
     - 意义
   * - 1960s
     - Frederick Brooks、OS/360
     - 大型批处理操作系统，催生《人月神话》
   * - 1969
     - Ken Thompson、Dennis Ritchie、Unix
     - 用 C 重写内核，奠定可移植操作系统范式
   * - 1973
     - Xerox PARC、Alto
     - 图形界面与鼠标交互的先驱
   * - 1983
     - Richard Stallman、GNU 项目
     - 自由软件运动，为 Linux 提供用户态工具链
   * - 1987
     - MINIX（Andrew Tanenbaum）
     - 教学用微内核，启发 Linus 编写 Linux
   * - 1991
     - Linus Torvalds、Linux 0.01
     - 开源宏内核操作系统，后成为服务器与云基础设施主流
   * - 2007
     - Google、Android
     - Linux 内核进入移动设备，用户规模达数十亿
   * - 2013
     - Docker
     - 将 Namespace/Cgroup 包装为易用容器，推动云原生

趣闻：Linux 的吉祥物企鹅 Tux，源于 Linus 在澳大利亚被小企鹅咬伤的经历——后来这只企鹅成了开源社区最知名的图标之一。

---