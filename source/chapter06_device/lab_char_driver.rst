================================
实践：简单字符设备驱动
================================

通过编写一个最小的:strong:`字符设备驱动` 内核模块，理解 ``file_operations``、设备号注册和用户空间访问 ``/dev`` 节点的完整流程。

.. note::

   驱动源码 ``hello_chardev.c`` 与用户态测试 ``chardev_test.cpp`` 见 ``source/code/chap06/`` 。用户态程序执行 ``make user`` 编译；内核模块需安装头文件后执行 ``make kernel``。

.. warning::

   内核模块开发需要与当前运行内核版本匹配的内核头文件和构建环境。以下实验建议在虚拟机中进行，错误的模块可能导致系统崩溃。生产环境勿随意加载未验证模块。

环境准备
==========================

.. code-block:: bash

   # Debian/Ubuntu：安装内核头文件和构建工具
   sudo apt install build-essential linux-headers-$(uname -r)

   mkdir ~/chardev_lab && cd ~/chardev_lab

驱动源码
==========================

:strong:`hello_chardev.c` ：实现可读写的内存缓冲区字符设备。

.. code-block:: c

   #include <linux/module.h>
   #include <linux/kernel.h>
   #include <linux/fs.h>
   #include <linux/uaccess.h>
   #include <linux/device.h>
   #include <linux/cdev.h>

   #define DEVICE_NAME "hello_chardev"
   #define BUF_SIZE 256

   static char buffer[BUF_SIZE];
   static int buf_len;
   static dev_t dev_num;
   static struct cdev char_cdev;
   static struct class *char_class;

   static ssize_t dev_read(struct file *f, char __user *user_buf,
                           size_t count, loff_t *offset) {
       if (*offset >= buf_len)
           return 0;
       if (count > buf_len - *offset)
           count = buf_len - *offset;
       if (copy_to_user(user_buf, buffer + *offset, count))
           return -EFAULT;
       *offset += count;
       return count;
   }

   static ssize_t dev_write(struct file *f, const char __user *user_buf,
                            size_t count, loff_t *offset) {
       if (count >= BUF_SIZE)
           count = BUF_SIZE - 1;
       if (copy_from_user(buffer, user_buf, count))
           return -EFAULT;
       buffer[count] = '\0';
       buf_len = count;
       *offset = count;
       return count;
   }

   static const struct file_operations fops = {
       .owner = THIS_MODULE,
       .read  = dev_read,
       .write = dev_write,
   };

   static int __init hello_init(void) {
       int ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
       if (ret < 0) return ret;

       cdev_init(&char_cdev, &fops);
       ret = cdev_add(&char_cdev, dev_num, 1);
       if (ret < 0) goto err_cdev;

       char_class = class_create(DEVICE_NAME);
       if (IS_ERR(char_class)) { ret = PTR_ERR(char_class); goto err_class; }

       device_create(char_class, NULL, dev_num, NULL, DEVICE_NAME);
       pr_info("hello_chardev: loaded, major=%d\n", MAJOR(dev_num));
       return 0;

   err_class:
       cdev_del(&char_cdev);
   err_cdev:
       unregister_chrdev_region(dev_num, 1);
       return ret;
   }

   static void __exit hello_exit(void) {
       device_destroy(char_class, dev_num);
       class_destroy(char_class);
       cdev_del(&char_cdev);
       unregister_chrdev_region(dev_num, 1);
       pr_info("hello_chardev: unloaded\n");
   }

   module_init(hello_init);
   module_exit(hello_exit);
   MODULE_LICENSE("GPL");
   MODULE_AUTHOR("os-tutorial");
   MODULE_DESCRIPTION("Minimal char device driver");

Makefile
==========================

.. code-block:: makefile

   obj-m += hello_chardev.o

   all:
   	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

   clean:
   	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules clean

编译与加载
==========================

.. code-block:: bash

   make
   sudo insmod hello_chardev.ko
   dmesg | tail -5

``dmesg`` 应显示 ``hello_chardev: loaded, major=XXX``。设备节点由 udev 根据 ``device_create`` 自动创建：

.. code-block:: bash

   ls -l /dev/hello_chardev

用户空间测试
==========================

.. code-block:: bash

   echo "Hello from userspace" | sudo tee /dev/hello_chardev
   sudo cat /dev/hello_chardev

或用 C++ 测试程序：

.. code-block:: cpp

   #include <iostream>
   #include <fcntl.h>
   #include <unistd.h>
   #include <cstring>

   int main() {
       int fd = open("/dev/hello_chardev", O_RDWR);
       if (fd < 0) { perror("open"); return 1; }

       const char* msg = "test from cpp\n";
       write(fd, msg, strlen(msg));

       lseek(fd, 0, SEEK_SET);
       char buf[64]{};
       read(fd, buf, sizeof(buf) - 1);
       std::cout << "读到: " << buf;

       close(fd);
       return 0;
   }

卸载模块
==========================

.. code-block:: bash

   sudo rmmod hello_chardev
   dmesg | tail -3
   ls /dev/hello_chardev    # 应不存在

关键概念回顾
==========================

- ``alloc_chrdev_region`` / ``register_chrdev_region`` ：分配主次设备号
- ``cdev_init`` + ``cdev_add`` ：注册字符设备（现代方式；旧版 ``register_chrdev`` 已不推荐）
- ``file_operations`` ：实现 read/write/ioctl 等
- ``copy_to_user`` / ``copy_from_user`` ：内核与用户空间数据拷贝，必须检查
- ``device_create`` + ``class_create`` ：在 ``/dev`` 创建设备节点

设备驱动连接硬件与内核，网络设备则将数据包交给协议栈——下一章进入 Linux 网络子系统。

拓展阅读
==========

- 内核文档 ``Documentation/driver-api/``
- ``man 4 charsets``、LDD3（Linux Device Drivers, 3rd edition）相关章节
- ``/proc/devices`` 与 ``Documentation/admin-guide/devices.txt``

用 ioctl 扩展驱动
==========================

字符设备除 read/write 外，常用 ``ioctl`` 传递设备专用命令。在 ``file_operations`` 中增加：

.. code-block:: c

   #include <linux/ioctl.h>

   #define HELLO_MAGIC 'h'
   #define HELLO_RESET _IO(HELLO_MAGIC, 0)
   #define HELLO_GET_LEN _IOR(HELLO_MAGIC, 1, int)

   static long hello_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
   {
       switch (cmd) {
       case HELLO_RESET:
           buf_len = 0;
           return 0;
       case HELLO_GET_LEN:
           return put_user(buf_len, (int __user *)arg);
       default:
           return -ENOTTY;
       }
   }

用户态通过 ``ioctl(fd, HELLO_RESET)`` 调用。``_IO`` / ``_IOR`` 宏保证命令号不与系统其他驱动冲突（需唯一 ``MAGIC`` 字符）。

Platform 设备与设备树（简介）
================================

嵌入式平台上，硬件信息常由:strong:`设备树` （Device Tree）描述，内核在启动时解析 ``.dtb`` 并创建 ``platform_device``。驱动通过 ``of_match_table`` 与设备树节点 ``compatible`` 字符串匹配：

.. code-block:: c

   static const struct of_device_id hello_of_match[] = {
       { .compatible = "example,hello-chardev" },
       { /* sentinel */ }
   };
   MODULE_DEVICE_TABLE(of, hello_of_match);

   static struct platform_driver hello_platform_driver = {
       .probe = hello_probe,
       .driver = { .name = "hello_chardev", .of_match_table = hello_of_match },
   };

PC 上常见的 PCI/USB 设备则通过总线层自动匹配，原理类似：内核根据设备 ID 绑定对应 ``driver``。本教程的 ``hello_chardev`` 采用手动注册设备号的最小方式，便于理解 ``file_operations`` 核心流程。

本章完整参考资料与各节交叉索引见 :doc:`references`。
