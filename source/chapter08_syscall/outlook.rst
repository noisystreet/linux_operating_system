======================
延伸与趋势
======================

系统调用作为用户态/内核态边界的设计五十年未变；x86-64 的 ``syscall`` 指令与 ``sys_call_table`` 仍是理解内核的入口。变化体现在:strong:`边界加固` 、:strong:`批量异步接口` 与:strong:`vDSO 扩展` 。

工业界现状
==========================

glibc 封装 ``read``、``write``、``clone`` 等；``strace``、``perf trace`` 与 ``raw_syscall`` 演示（``lab_syscall_asm``）是分析行为的标准手段。

值得关注的变化
==========================

- :strong:`seccomp 与 syscall 过滤` ：Chrome、Docker、systemd 广泛限制 syscall 集合；``seccomp_demo``（第 9 章）与 ``PR_SET_NO_NEW_PRIVS`` 组合构成沙箱基础。
- :strong:`io_uring 作为 syscall 批处理` ：单次 ``enter`` 提交多个操作，减少用户/内核切换，可视为「syscall 层之上的新抽象」（第 5 章）。
- :strong:`vDSO 扩展` ：除 ``gettimeofday`` 外，部分调度与随机数接口可在用户态完成；阅读 ``vdso.so`` 与 ``arch/x86/entry/vdso/`` 理解何时仍需真实 syscall。
- :strong:`syscall user dispatch（SUD）` 等研究/特性` ：为特定执行环境重定向 syscall 处理，多见于虚拟化与兼容层，理解边界有助于读 KVM 与容器运行时源码。

与本教程的衔接
==========================

自定义 syscall 补丁文档（``lab_syscall_asm``）说明主线内核:strong:`不鼓励` 动态增 syscall；生产扩展优先 ``ioctl``、netlink、BPF。

进一步了解
==========================

- ``man 2 syscalls``、内核 ``Documentation/admin-guide/syscall-user-dispatch.rst``
- 第 9 章 LSM 在 syscall 返回路径上的钩子
- syzkaller 模糊测试与 syscall 兼容性

.. note::

   本节约 2025–2026 年方向撰写。
