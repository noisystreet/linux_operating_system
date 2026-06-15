================================
实践：Cgroup 与容器实验
================================

查看系统 cgroup 布局，体验资源限制，并检测 KVM 与 Docker 环境。

.. note::

   辅助脚本 ``check_kvm.sh``、``limit_cpu.sh``、``setup_veth_netns.sh`` 见 ``source/code/chap10/`` 。

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

chroot 仅改变根目录视图，**不是** 完整容器。对比 ``unshare --mount`` 的隔离范围。

手工 cgroup v2 内存限制
==========================

不依赖 Docker，直接用 cgroup v2 限制进程内存（**需 root** ）：

.. code-block:: bash

   CGROUP=/sys/fs/cgroup/demo_mem
   sudo mkdir -p "$CGROUP"
   echo "+memory" | sudo tee /sys/fs/cgroup/cgroup.subtree_control 2>/dev/null || true
   echo "32M" | sudo tee "$CGROUP/memory.max"

   # 在受限 cgroup 中运行内存消耗程序
   echo $$ | sudo tee "$CGROUP/cgroup.procs"
   # 或: systemd-run --scope -p MemoryMax=32M stress-ng --vm 1 --vm-bytes 64M

   cat "$CGROUP/memory.current"
   cat "$CGROUP/memory.events"    # 查看是否触发 oom_kill

进程超出 ``memory.max`` 时，内核通过 cgroup OOM 杀死该 cgroup 内进程，而非拖垮整机。

QEMU 最小虚拟机
==========================

在已启用 KVM 的环境（``./check_kvm.sh`` 验证）：

.. code-block:: bash

   # 下载 cloud 镜像（示例，以 Ubuntu 为例）
   # wget https://cloud-images.ubuntu.com/releases/24.04/release/ubuntu-24.04-server-cloudimg-amd64.img

   qemu-system-x86_64 \
       -enable-kvm \
       -m 1024 \
       -smp 2 \
       -hda ubuntu-24.04-server-cloudimg-amd64.img \
       -netdev user,id=net0,hostfwd=tcp::2222-:22 \
       -device virtio-net-pci,netdev=net0 \
       -nographic

``-enable-kvm`` 使用硬件加速；``virtio-net-pci`` 为半虚拟化网卡，性能优于全模拟 e1000。``hostfwd`` 将宿主机 2222 端口转发到客户机 22，便于 SSH 登录。完整桌面体验可用 ``virt-manager`` 图形管理。

叠加 Namespace + cgroup 的最小容器
======================================

将第 10 章理论串联为可复现步骤（实验环境）：

.. code-block:: bash

   # 1. 准备 rootfs（debootstrap 或解压容器镜像层）
   sudo debootstrap --variant=minbase jammy /tmp/rootfs http://archive.ubuntu.com/ubuntu

   # 2. 创建 cgroup 并限制 CPU
   CG=/sys/fs/cgroup/mini_ctr
   sudo mkdir -p "$CG"
   echo "50000 100000" | sudo tee "$CG/cpu.max"

   # 3. 在隔离环境中启动（简化版，未含 pivot_root）
   sudo unshare --pid --fork --mount-proc --uts --ipc --net \
       --mount bash -c '
     hostname mini-ctr
     mount --bind /tmp/rootfs /tmp/rootfs
     chroot /tmp/rootfs /bin/bash -c "echo hello from container; ps aux"
   '

完整生产级容器还需 ``pivot_root``、挂载 ``/proc``/``/sys``、配置 ``/etc/resolv.conf``、设置 capability 与 seccomp。Docker/runc 自动化了上述步骤；理解分步操作有助于排查容器启动失败。

清理
==========================

.. code-block:: bash

   docker rm -f limited nstest 2>/dev/null
   sudo ip netns del ns1 2>/dev/null
   sudo rm -rf /sys/fs/cgroup/demo_mem /sys/fs/cgroup/mini_ctr 2>/dev/null

恭喜完成第 10 章实践。附录汇总调试工具、内核编译和参考资料；本章完整索引见 :doc:`references`，全书趋势见 :doc:`/appendix/a4_outlook_index`。
