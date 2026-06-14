================================
实践：Cgroup 与容器实验
================================

查看系统 cgroup 布局，体验资源限制，并检测 KVM 与 Docker 环境。

查看 Cgroup 布局
==========================

.. code-block:: bash

   ls /sys/fs/cgroup/
   cat /proc/self/cgroup
   systemd-cgls | head -40

cgroup v2 统一挂载下，systemd 为每个 slice、service、容器创建子目录。

Docker 资源限制
==========================

.. code-block:: bash

   docker run -d --name limited --memory=128m --cpus=0.5 \
       alpine sleep 3600

   docker inspect limited | grep -i memory
   PID=$(docker inspect -f '{{.State.Pid}}' limited)
   cat /proc/$PID/cgroup

观察容器进程的 cgroup 路径及 memory/cpu 限制配置。

内存压力实验（谨慎）
==========================

.. warning::

   内存实验可能导致进程被 OOM 杀死，请在测试环境进行。

.. code-block:: bash

   docker run --rm -m 64m alpine sh -c 'dd if=/dev/zero of=/dev/shm/x bs=1M count=100'

小内存限制下分配大块可能失败或被 OOM kill。

KVM 检测
==========================

.. code-block:: bash

   ls -l /dev/kvm
   lsmod | grep kvm
   egrep -c '(vmx|svm)' /proc/cpuinfo   # 硬件虚拟化支持

若 ``/dev/kvm`` 存在，可用 QEMU 创建 VM（需镜像）：

.. code-block:: bash

   # 仅检查 qemu 是否安装
   which qemu-system-x86_64

chroot 最小体验
==========================

.. code-block:: bash

   mkdir -p /tmp/minroot/bin /tmp/minroot/lib
   cp /bin/ls /bin/bash /tmp/minroot/bin/
   # 复制 ldd 显示的依赖库到 /tmp/minroot/lib（因系统而异）
   sudo chroot /tmp/minroot /bin/ls

chroot 仅改变根目录视图，:strong:`不是` 完整容器。对比 ``unshare --mount`` 的隔离范围。

清理
==========================

.. code-block:: bash

   docker rm -f limited nstest 2>/dev/null

恭喜完成第 10 章实践。附录汇总调试工具、内核编译和参考资料，供深入学习。
