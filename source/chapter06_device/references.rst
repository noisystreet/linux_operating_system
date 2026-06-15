==========================
参考资料与补充阅读
==========================

本章内容索引
==========================

.. list-table::
   :header-rows: 1
   :widths: 28 52

   * - 节
     - 主题
   * - :doc:`01_history`
     - 设备管理演进
   * - :doc:`02_driver_model`
     - 总线-设备-驱动、sysfs、probe、uevent
   * - :doc:`03_device_types`
     - 字符/块/网络设备
   * - :doc:`04_io_subsystem`
     - 中断、DMA、I/O 调度器
   * - :doc:`05_udev`
     - udev 规则、设备节点
   * - :doc:`lab_device_tools`
     - ``lsmod``、``/sys``、``iostat``
   * - :doc:`lab_char_driver`
     - ``hello_chardev`` 字符设备、``ioctl`` 扩展
   * - :doc:`outlook`
     - Rust 驱动、DPDK/SPDK、VFIO

示例代码
==========================

- ``source/code/chap06/hello_chardev.c``、``chardev_test.cpp`` —— 见 :doc:`lab_char_driver`；模块需 ``make kernel``

手册页与内核文档
==========================

- ``man 4 lsmod``、``man 8 modprobe`` —— :doc:`lab_device_tools`
- ``man 7 udev``、``man 1 udevadm`` —— :doc:`05_udev`
- ``man 1 iostat`` —— :doc:`04_io_subsystem`、:doc:`lab_device_tools`
- 内核 ``drivers/base/core.c``、``Documentation/driver-api/`` —— 附录 :doc:`/appendix/a3_references` 第 6 章路线

书籍与在线资料
==========================

- *Linux Device Drivers* （LDD3）相关章节 —— 字符设备经典参考
- 内核 ``Documentation/admin-guide/devices.txt``
- 附录 :doc:`/appendix/a2_build_kernel` —— 模块编译环境

相关章节
==========================

- :doc:`/chapter05_filesystem/index` —— 块设备与文件系统（:doc:`03_device_types`）
- :doc:`/chapter07_network/index` —— 网络设备驱动（:doc:`03_device_types`）
- :doc:`/chapter10_virt/index` —— VFIO 设备直通（:doc:`outlook`）
