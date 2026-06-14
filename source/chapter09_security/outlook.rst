======================
延伸与趋势
======================

DAC（owner/group/permission）+ capability + LSM + seccomp 构成 Linux 纵深防御。趋势是:strong:`更细粒度沙箱` 、:strong:`可编程 MAC` 与:strong:`机密计算` 将信任边界推进到硬件。

工业界现状
==========================

Ubuntu 默认 AppArmor，RHEL 系默认 SELinux；容器默认 seccomp profile 与 capability drop。``lab_security_tools``、``lab_security_program`` 覆盖命令与 C++ 实验。

值得关注的变化
==========================

- :strong:`Landlock` ：非特权进程自愿限制自身文件系统访问，适合沙箱运行时；与 seccomp（syscall 过滤）互补，见 ``05_lsm`` LSM 堆叠。
- :strong:`BPF LSM` ：用 eBPF 程序挂载 LSM 钩子，云厂商实现自定义策略而不重编译 SELinux 策略。连接第 7 章 eBPF 与第 9 章 MAC。
- :strong:`机密计算（TEE）` ：AMD SEV、Intel TDX 在 KVM 中加密 VM 内存，威胁模型从「防其他租户」扩展到「防宿主机」。与第 10 章 KVM 节呼应。
- :strong:`最小权限镜像` ：distroless、只读根、非 root 用户运行容器成为供应链安全共识；仍依赖本章 DAC/capability 语义。

与本教程的衔接
==========================

SELinux ``audit2allow`` 排错、ACL 协作目录、``seccomp_demo`` 已在本章 lab 中给出入口；勿在生产盲目 ``setenforce 0`` 或放宽策略。

进一步了解
==========================

- 内核 ``Documentation/security/``、AppArmor/SELinux 官方指南
- NIST 零信任架构与 OS 层控制映射
- 第 8 章 syscall 边界是 LSM/seccomp 的挂载点

.. note::

   安全策略因组织与合规要求差异大，本节约 2025–2026 年技术方向撰写，非合规建议。
