# Agents

本项目使用 AI 辅助编写，本文件为 AI agent 提供项目上下文和协作约定。

## 项目概述

基于 Linux 的操作系统原理入门教程，使用 Sphinx + reStructuredText 构建。
以 Linux 为主线，系统讲解操作系统核心概念与实现原理，每章配以动手实践。

## 语言

- 文档语言：**中文（简体）**，除专有名词（如 Linux、CPU、SELinux）和核心概念（如 fork、mmap、namespace）等使用原文外，正文均使用中文
- 与用户交流：使用用户消息的語言
- 示例代码：**现代 C++**（C++11/14/17/20），内核模块部分使用 C
- 文档格式：**reStructuredText**（.rst）

## 目录结构

```
/
├── README.md              # 项目规划（目标、章节大纲、构建方法）
├── .readthedocs.yaml      # Read the Docs 部署配置
├── LICENSE                # CC BY 4.0（文档）
├── LICENSE-code           # MIT（示例代码）
├── Makefile               # Sphinx 构建入口
│                          #   make html     — 构建 HTML
│                          #   make latexpdf — 构建 PDF
│                          #   make serve    — 启动本地预览
├── requirements.txt       # sphinx + sphinx-rtd-theme
├── agents.md              # 本文件 — AI agent 上下文
├── source/
│   ├── conf.py            # Sphinx 配置（中文、RTD 主题、numfig、extlinks）
│   ├── index.rst          # 文档首页 + toctree（链接各章）
│   ├── glossary.rst       # 术语表
│   ├── _static/
│   │   └── custom.css     # 中文排版优化
│   ├── _templates/        # Jinja2 模板
│   ├── code/              # 可编译的示例源码
│   │   ├── README.rst
│   │   ├── chap01/ .. chap10/
│   ├── chapter01_intro/   # 操作系统概述
│   ├── chapter02_boot/    # 系统启动
│   ├── chapter03_process/ # 进程与线程管理
│   ├── chapter04_memory/  # 内存管理
│   ├── chapter05_filesystem/ # 文件系统
│   ├── chapter06_device/  # 设备与 I/O 管理
│   ├── chapter07_network/ # 网络协议栈
│   ├── chapter08_syscall/ # 系统调用
│   ├── chapter09_security/# 安全与权限
│   ├── chapter10_virt/    # 虚拟化与容器
│   └── appendix/          # 附录
└── build/                 # 构建输出（gitignore）
```

## 章节文件约定

每章的目录结构：

```
chapterXX_xxxx/
├── index.rst              # 章首页 + toctree
├── 01_topic.rst           # 原理小节（数字前缀控制顺序）
├── 02_topic.rst
├── lab_tools.rst          # 实践小节：系统命令操作
└── lab_program.rst        # 实践小节：C++ 示例程序
```

- 原理小节以 `NN_` 数字前缀排序
- 实践小节以 `lab_` 前缀标识，**命令操作在前、编程示例在后**
- 每个 rst 文件的标题使用 `=====` 式上划线

## 写作风格

- **浅入深出**——从具体现象或简单示例切入，逐步深入到原理和内核实现
- **夹叙夹议**——叙述的同时穿插分析和评论，说明"为什么这样设计"而非只讲"是什么"
- **自然过渡**——节与节之间、原理与实践之间有承接关系，避免生硬跳转
- **拒绝罗列**——不写成知识点清单或笔记摘要，每段应有叙述逻辑和主线
- 每节以一段引言开头，说明本节要解决的问题
- 原理部分：先讲清楚概念，再结合 Linux 具体实现
- 实践部分：可执行的命令或可编译的 C++ 程序
- 涉及内核代码时：给出源码路径（如 `kernel/sched/fair.c`）和关键行号
- 使用 `.. note::`、`.. warning::`、`.. hint::` 等 admonition 做补充说明
- 使用 `.. code-block:: cpp` / `.. code-block:: console` 标记代码块语言
- 使用 `.. todo::` 标记待撰写内容

## 构建验证

```bash
make clean && make html
```

构建应在全部源文件上通过，无 warning 或 error。
HTML 输出位于 `build/html/`。

## 协作约定

1. 优先修改已有文件，不随意创建新章节文件
2. 添加新文件后必须更新对应 `index.rst` 的 `toctree`
3. 每次修改 rst 后运行 `make html` 验证构建
4. 示例代码应完整可编译，注明编译方法（如 `g++ -std=c++17 -Wall -o prog prog.cpp`）