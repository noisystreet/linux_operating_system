贡献指南
========

感谢参与 **Linux 操作系统教程** 的改进。本文说明文档与示例代码的协作约定。

环境准备
========

.. code-block:: bash

   pip install -r requirements.txt
   make html
   make -C source/code user test

- **文档构建**：仓库根目录 ``make html``，输出在 ``build/html/``
- **示例代码**：``source/code/`` 下 ``make user`` 编译、``make test`` 冒烟测试
- **本地预览**：``make serve`` 后在 http://localhost:8000 查看

文档规范（reStructuredText）
============================

- 粗体使用 ``:strong:\`文本\``` ，不要用 Markdown 的 ``**粗体**``
- 反引号与全角标点之间加空格，例如 ``\`text\` （说明）``
- 表格优先使用 ``.. list-table::``
- 章节标题下划线长度须与标题文字一致
- 示例代码目录 ``source/code/**`` 已在 ``conf.py`` 的 ``exclude_patterns`` 中排除，不参与 Sphinx 构建

示例代码规范
============

- 语言标准：C++17；需要线程时加 ``-pthread``
- 许可：示例代码采用 MIT（``LICENSE-code``）
- 每章示例放在 ``source/code/chapNN/``，章内 ``Makefile`` 引用 ``Makefile.common``
- 新增用户态程序后：更新 ``source/code/README.rst``、对应 ``lab_*.rst`` 的 ``.. note::``，并将可自动验证的程序加入顶层 ``Makefile`` 的 ``test`` 目标
- 需 root、内核头文件或交互的示例：在 README 与 lab 中明确标注，不纳入默认 ``make test``

提交前检查
==========

.. code-block:: bash

   make html
   make -C source/code clean user test

CI 会在 Pull Request 上自动运行上述检查。

Pull Request 建议
=================

- 一次 PR 聚焦一个主题（如「第 4 章修订」或「补充 CI」）
- 提交信息使用完整句子，说明改了什么、为什么
- 若修改 lab 正文中的代码片段，请同步更新 ``source/code/`` 中对应文件

许可证
======

- 文档（``.rst``）：CC BY 4.0（``LICENSE``）
- 示例代码：MIT（``LICENSE-code``）
