# Configuration file for the Sphinx documentation builder.
# Created by os-tutorial project.

import os
import sys

# -- Project information -----------------------------------------------------

project = 'Linux 操作系统教程'
copyright = '2026, os-tutorial'
author = 'os-tutorial'

release = '0.2.0'
version = '0.2.0'

# -- General configuration ---------------------------------------------------

extensions = [
    'sphinx_rtd_theme',      # Read the Docs 主题
    'sphinxcontrib.mermaid', # Mermaid 图示
    'sphinx.ext.todo',       # 待办事项
    'sphinx.ext.mathjax',    # 数学公式
    'sphinx.ext.ifconfig',   # 条件包含
    'sphinx.ext.extlinks',   # 外部链接缩写
]

templates_path = ['_templates']
source_suffix = '.rst'
root_doc = 'index'
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store', 'code/**']

# 图片/表格自动编号
numfig = True
numfig_format = {'figure': '图 %s', 'table': '表 %s', 'code-block': '清单 %s'}

# 代码高亮风格
pygments_style = 'tango'

# i18n
language = 'zh_CN'
locale_dirs = ['locale/']

# -- Options for HTML output -------------------------------------------------

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
html_title = 'Linux 操作系统教程'
html_show_sourcelink = True
html_css_files = ['custom.css']

# -- Options for LaTeX output ------------------------------------------------

latex_engine = 'xelatex'
latex_elements = {
    'preamble': r'''
\usepackage{xeCJK}
\setCJKmainfont{Noto Sans CJK SC}
\setCJKmonofont{Noto Sans CJK SC}
''',
}

latex_documents = [
    (root_doc, 'os-tutorial.tex', 'Linux 操作系统教程',
     'os-tutorial', 'manual'),
]

# -- Options for todo extension ----------------------------------------------

todo_include_todos = True

# -- extlinks configuration --------------------------------------------------

extlinks = {
    'kernel-doc': ('https://docs.kernel.org/%s', None),
    'source': ('https://github.com/torvalds/linux/blob/master/%s', None),
}