================================
实践：TCP Socket 编程
================================

实现一个简单的 TCP echo 服务：客户端发送字符串，服务器原样返回。涵盖 socket、bind、listen、accept、connect 完整流程。

.. note::

   本节 ``echo_server``、``echo_client`` 源码见 ``source/code/chap07/`` 。在 ``source/code`` 目录执行 ``make user`` 可一键编译。

Echo 服务器
==========================

.. code-block:: cpp

   #include <iostream>
   #include <cstring>
   #include <unistd.h>
   #include <sys/socket.h>
   #include <netinet/in.h>

   int main() {
       int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
       if (listen_fd < 0) { perror("socket"); return 1; }

       int opt = 1;
       setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

       sockaddr_in addr{};
       addr.sin_family = AF_INET;
       addr.sin_addr.s_addr = INADDR_ANY;
       addr.sin_port = htons(8080);

       if (bind(listen_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
           perror("bind"); return 1;
       }
       if (listen(listen_fd, SOMAXCONN) < 0) {
           perror("listen"); return 1;
       }

       std::cout << "Echo 服务器监听 8080...\n";

       while (true) {
           int conn_fd = accept(listen_fd, nullptr, nullptr);
           if (conn_fd < 0) { perror("accept"); continue; }

           char buf[1024];
           ssize_t n = read(conn_fd, buf, sizeof(buf) - 1);
           if (n > 0) {
               buf[n] = '\0';
               std::cout << "收到: " << buf;
               write(conn_fd, buf, n);
           }
           close(conn_fd);
       }
       close(listen_fd);
       return 0;
   }

Echo 客户端
==========================

.. code-block:: cpp

   #include <iostream>
   #include <cstring>
   #include <unistd.h>
   #include <arpa/inet.h>
   #include <sys/socket.h>
   #include <netinet/in.h>

   int main(int argc, char* argv[]) {
       const char* host = argc > 1 ? argv[1] : "127.0.0.1";
       const char* msg = argc > 2 ? argv[2] : "Hello, TCP!\n";

       int fd = socket(AF_INET, SOCK_STREAM, 0);
       if (fd < 0) { perror("socket"); return 1; }

       sockaddr_in addr{};
       addr.sin_family = AF_INET;
       addr.sin_port = htons(8080);
       if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
           std::cerr << "无效地址\n"; return 1;
       }

       if (connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
           perror("connect"); return 1;
       }

       write(fd, msg, strlen(msg));

       char buf[1024];
       ssize_t n = read(fd, buf, sizeof(buf) - 1);
       if (n > 0) {
           buf[n] = '\0';
           std::cout << "回显: " << buf;
       }

       close(fd);
       return 0;
   }

编译与运行
==========================

.. code-block:: bash

   g++ -std=c++17 -Wall -o echo_server echo_server.cpp
   g++ -std=c++17 -Wall -o echo_client echo_client.cpp

   # 终端 1
   ./echo_server

   # 终端 2
   ./echo_client 127.0.0.1 "test message"

   # 或用 nc 测试
   echo "hello" | nc 127.0.0.1 8080

观察连接
==========================

服务器运行时，在另一终端：

.. code-block:: bash

   ss -tan | grep 8080

应看到 ``LISTEN`` 和 ``ESTAB`` 状态。``tcpdump -i lo port 8080`` 可观察三次握手和数据包。

UDP 变体（可选）
==========================

将 ``SOCK_STREAM`` 改为 ``SOCK_DGRAM``，服务器用 ``recvfrom``/``sendto``，客户端用 ``sendto``，无需 ``listen``/``accept``。体会无连接协议的区别。

应用通过 socket 使用网络，而 socket 底层依赖:strong:`系统调用` 进入内核。下一章深入系统调用的实现机制。

拓展阅读
==========

- ``man 2 socket``、``man 7 tcp``
- 《UNIX 网络编程》卷 1（Stevens）
- 内核源码 ``net/socket.c``、``net/ipv4/tcp.c``

本章完整参考资料与各节交叉索引见 :doc:`references`。
