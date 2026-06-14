======================
Socket 编程
======================

Socket 是应用程序使用网络的标准接口。无论是 Web 服务器、SSH 客户端还是数据库连接，底层都依赖 socket。本节介绍 TCP/UDP socket 编程流程、常用 API 和内核中的对应实现。

TCP 服务器流程
========================

TCP 是面向连接、可靠的字节流协议。服务器典型步骤：

.. code-block:: text

   socket()   → 创建 socket
   bind()     → 绑定本地 IP 和端口
   listen()   → 监听连接请求
   accept()   → 接受连接，返回新 socket
   read()/write() → 与客户端通信
   close()    → 关闭连接

.. code-block:: cpp

   int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
   struct sockaddr_in addr{};
   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = INADDR_ANY;
   addr.sin_port = htons(8080);
   bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr));
   listen(listen_fd, SOMAXCONN);
   int conn_fd = accept(listen_fd, nullptr, nullptr);
   // read/write conn_fd
   close(conn_fd);
   close(listen_fd);

``SOMAXCONN`` 为监听队列最大长度。``accept()`` 阻塞直到有客户端连接。

TCP 客户端流程
========================

.. code-block:: text

   socket()
   connect()  → 连接服务器
   read()/write()
   close()

.. code-block:: cpp

   int fd = socket(AF_INET, SOCK_STREAM, 0);
   struct sockaddr_in addr{};
   addr.sin_family = AF_INET;
   addr.sin_port = htons(8080);
   inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
   connect(fd, (struct sockaddr*)&addr, sizeof(addr));
   // read/write fd
   close(fd);

UDP 编程
========================

UDP 无连接、不保证可靠，适合 DNS、视频流等场景：

.. code-block:: cpp

   int fd = socket(AF_INET, SOCK_DGRAM, 0);
   // 服务器: bind() 后 recvfrom()
   // 客户端: sendto() 直接发送
   sendto(fd, buf, len, 0, (struct sockaddr*)&dest, sizeof(dest));
   recvfrom(fd, buf, sizeof(buf), 0, nullptr, nullptr);

常用 socket 选项
========================

.. code-block:: cpp

   int opt = 1;
   setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

:strong:`SO_REUSEADDR` ：允许绑定处于 TIME_WAIT 状态的端口，服务器重启时常用。

.. code-block:: cpp

   struct timeval tv{5, 0};
   setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

:strong:`SO_RCVTIMEO` / ``SO_SNDTIMEO`` ：设置收发超时。

I/O 多路复用
========================

单线程服务多连接需:strong:`I/O 多路复用` ：

.. list-table::
   :header-rows: 1
   :widths: 15 45

   * - API
     - 特点
   * - select
     - 传统，fd 数量有限制（FD_SETSIZE）
   * - poll
     - 无 fd 数量硬限制，仍 O(n) 扫描
   * - epoll
     - Linux 高效方案，O(1) 就绪通知

.. code-block:: cpp

   int epfd = epoll_create1(0);
   struct epoll_event ev{};
   ev.events = EPOLLIN;
   ev.data.fd = listen_fd;
   epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);
   epoll_wait(epfd, events, MAX_EVENTS, -1);

高并发服务器（Nginx、Redis）广泛使用 epoll。

内核实现要点
========================

- ``sys_socket()`` 创建 socket，分配 fd 和 ``struct socket``
- ``sys_bind()`` 检查端口冲突，将地址写入 ``struct sock``
- ``sys_listen()`` 设置 TCP_LISTEN 状态
- ``sys_accept()`` 从已完成连接队列取出 ``struct sock``，创建新 socket
- ``sys_connect()`` 发起 TCP 三次握手（客户端）
- 数据通过 ``tcp_sendmsg``/``tcp_recvmsg`` 与 skb 交互

查看 socket 状态
========================

.. code-block:: bash

   ss -tlnp          # TCP 监听端口
   ss -tan           # 所有 TCP 连接及状态
   netstat -tlnp     # 传统工具（部分系统需 net-tools）

``ss`` 直接从内核获取信息，比 ``netstat`` 更快。状态 ``LISTEN``、``ESTAB``、``TIME-WAIT`` 等对应 TCP 状态机。

Socket 提供通用网络 API，而防火墙和包过滤在另一层介入——下一节介绍 Netfilter。
