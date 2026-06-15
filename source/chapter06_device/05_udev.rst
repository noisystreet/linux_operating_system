======================
udev 与设备管理
======================

内核识别设备后，用户空间需要创建 ``/dev`` 节点、设置权限、建立符号链接。**udev** 是 Linux 上负责这项工作的设备管理器，根据内核通过 netlink 发送的事件和规则文件完成配置。

udev 的角色
========================

.. code-block:: text

   内核发现设备 → 通过 netlink 发送 uevent
   → udevd 接收事件
   → 匹配规则（/etc/udev/rules.d/、/usr/lib/udev/rules.d/）
   → 创建 /dev 节点、symlink、权限、触发脚本

udev 替代了早期的 devfs 和静态 ``/dev`` 管理，支持热插拔：插入 U 盘、USB 设备时自动创建节点并可能自动挂载（取决于桌面环境策略）。

监控 udev 事件
========================

.. code-block:: bash

   # 实时监控所有 udev 事件
   sudo udevadm monitor

   # 仅内核事件
   sudo udevadm monitor --kernel

插入 USB 设备时，可看到 ``add`` 事件及 ``DEVNAME``、``ID_SERIAL``、``SUBSYSTEM`` 等属性。

查看设备属性
========================

.. code-block:: bash

   udevadm info /dev/sda
   udevadm info -a -n /dev/sda    # 沿设备链的属性

属性用于编写匹配规则，如 ``KERNEL=="sda"``、``SUBSYSTEM=="block"``、``ATTR{size}``。

udev 规则
========================

规则文件位于 ``/etc/udev/rules.d/`` （管理员）和 ``/usr/lib/udev/rules.d/`` （软件包）。文件名 ``NN-name.rules``，数字越小优先级越高。

规则格式（简化）：

.. code-block:: text

   匹配条件, 匹配条件, ...  动作, 动作, ...

示例：为特定 USB 设备设置权限

.. code-block:: text

   SUBSYSTEM=="usb", ATTR{idVendor}=="1234", ATTR{idProduct}=="5678", MODE="0666"

常用动作：

- ``NAME`` ：设备节点名
- ``SYMLINK`` ：创建符号链接
- ``MODE``、``OWNER``、``GROUP`` ：权限
- ``RUN`` ：执行外部程序

修改规则后需 ``sudo udevadm control --reload-rules`` 并触发事件或重插设备。

/dev 下的符号链接
========================

udev 创建便于管理的符号链接：

.. code-block:: bash

   ls -l /dev/disk/by-uuid/
   ls -l /dev/disk/by-partlabel/
   ls -l /dev/input/by-id/

挂载时推荐使用 ``UUID`` 或 ``LABEL``，避免 ``/dev/sda1`` 因设备顺序变化而指错分区：

.. code-block:: text

   # /etc/fstab 示例
   UUID=xxxx-xxxx  /  ext4  defaults  0  1

与 systemd 的协作
========================

systemd 与 udev 紧密集成：``systemd-udevd`` 即 udev 守护进程。设备就绪可触发 ``systemd`` 单元（``SYSTEMD_WANTS``），实现设备依赖的服务启动（如加密分区解锁后挂载）。

设备子系统从内核驱动到用户空间节点，构成完整的 I/O 栈。下一节通过命令和简单的字符设备驱动实验，把理论落到实处。
