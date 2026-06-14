======================
容器原理
======================

:strong:`容器` 是操作系统级虚拟化：共享内核，通过 Namespace 隔离视图、Cgroup 限制资源、镜像提供可重复的文件系统。本节介绍 OCI 标准、Docker 架构和最小容器的构成。

容器的构成
========================

一个运行中的容器包含：

.. code-block:: text

   容器进程（在隔离的 PID/NET/MNT/... namespace 中）
   + 根文件系统（来自镜像层，可能 overlay 挂载）
   + cgroup 资源限制
   + capabilities/seccomp/AppArmor 等安全约束

与 VM 对比：无独立内核，启动快、开销小；隔离弱于硬件虚拟化。

OCI 标准
========================

:strong:`OCI` （Open Container Initiative）定义：

- :strong:`runtime-spec` ：如何运行容器（runc、crun）
- :strong:`image-spec` ：镜像格式（层、manifest、config.json）

Docker 推动标准化后，containerd、CRI-O、Podman 等均实现 OCI，镜像可跨运行时通用。

Docker 架构（简化）
========================

.. code-block:: text

   docker CLI
   → dockerd（守护进程）
   → containerd（容器运行时）
   → runc（OCI 运行时，创建 namespace+cgroup，exec 进程）

镜像存储在:strong:`层` （layer）中，只读；容器可写层用:strong:`overlayfs` 联合挂载：

.. code-block:: text

   容器视图 = overlay(可写层, 镜像层 N, ..., 镜像层 1)

删除容器可丢弃可写层，镜像层保留复用。

镜像与 Dockerfile
========================

.. code-block:: dockerfile

   FROM ubuntu:24.04
   RUN apt update && apt install -y nginx
   EXPOSE 80
   CMD ["nginx", "-g", "daemon off;"]

``docker build`` 每层生成新镜像层，缓存加速重复构建。``docker run`` 从镜像创建容器实例。

chroot 与 pivot_root
========================

早期"容器"用:strong:`chroot` 改变进程根目录，仅隔离文件系统，不隔离 PID/网络：

.. code-block:: bash

   sudo chroot /path/to/rootfs /bin/bash

:strong:`pivot_root` 是更彻底的根切换，容器运行时用于设置独立根文件系统。现代容器结合 mount namespace，比单纯 chroot 安全得多。

最小容器手工构建（概念）
========================

.. code-block:: bash

   # 1. 准备 rootfs（debootstrap 或解压镜像）
   # 2. unshare 创建 namespace
   sudo unshare --pid --fork --mount-proc --mount \
       --uts --ipc --net bash
   # 3. 在 namespace 内 pivot_root 或 mount bind rootfs
   # 4. 将进程加入 cgroup
   # 5. exec 容器内 init/应用

Docker 自动化了上述步骤。实践环节将用 ``unshare`` 和 cgroup 分步体验。

Kubernetes 简述
========================

Kubernetes 在节点上通过 kubelet 调用 CRI（containerd/CRI-O）管理 Pod。每个 Pod 通常共享 Network namespace（内容器 localhost 互通），可有独立 PID namespace（取决于配置）。资源 limits 映射 cgroup。

安全考量
========================

- 不以 privileged 模式运行容器除非必要
- 只读根文件系统、drop capabilities
- 扫描镜像漏洞，最小化基础镜像
- 敏感工作负载考虑 Kata（VM 级隔离）或 gVisor

容器与 KVM 共同构成现代 Linux 基础设施。下一节动手实验 namespace 和 cgroup。
