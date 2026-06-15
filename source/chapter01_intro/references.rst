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
     - 操作系统发展史、关键人物与时间线
   * - :doc:`02_what_is_os`
     - OS 定义、职责、特权级与启动梗概
   * - :doc:`03_os_types`
     - 批处理/分时/实时、宏/微内核对比
   * - :doc:`04_linux_intro`
     - Linux 历史、发行版、内核配置查看
   * - :doc:`lab_sysinfo`
     - ``uname``、``/proc``、``vmstat``、``sysinfo()`` 实践
   * - :doc:`outlook`
     - RISC-V、Rust 内核等延伸趋势

示例代码
==========================

- ``source/code/chap01/sysinfo.cpp`` —— 对应 :doc:`lab_sysinfo` 中的 C++ 示例

手册页与内核文档
==========================

- ``man 2 uname``、``man 2 sysinfo`` —— :doc:`lab_sysinfo` 使用的系统调用
- ``man 5 proc``、``man 8 sysctl`` —— :doc:`lab_sysinfo` 中 ``/proc`` 与 ``/proc/sys`` 实验
- ``man 1 vmstat`` —— 系统实时状态观测
- 内核 ``kernel/sys.c`` —— ``sysinfo``、``uname`` 等实现（见附录 :doc:`/appendix/a3_references` 第 1–2 章路线）

书籍与在线资料
==========================

- `Operating Systems: Three Easy Pieces <https://pages.cs.wisc.edu/~remzi/OSTEP/>`_ —— OS 概念.free 在线
- `The Linux Programming Interface <https://man7.org/tlpi/>`_ 第 2、11 章 —— 系统信息获取
- `Linux 内核文档 <https://docs.kernel.org/>`_ —— 总索引
- 附录 :doc:`/appendix/a3_references` —— 全书推荐书单与源码浏览器

相关章节
==========================

- :doc:`/chapter02_boot/index` —— 启动流程（呼应 :doc:`02_what_is_os` 中的启动梗概）
- :doc:`/chapter08_syscall/index` —— :doc:`lab_sysinfo` 中 ``uname()`` 的底层机制
- :doc:`/chapter04_memory/index` —— ``/proc/meminfo`` 与内存管理
- 附录 :doc:`/appendix/a1_debug_tools` —— ``strace`` 跟踪 :doc:`lab_sysinfo` 中的系统调用
