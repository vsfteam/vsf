# VSF 文档向导

## 1. 文档的目录结构

&emsp;&emsp;语言后缀：zh中文、en英文，默认为英文。

|名字|作用|
|----|---|
|res|资源文件目录|
|- image|图片资源目录|
|-- xx|特定语言的图片文件|
|src|源代码目录|
|- main.c|主代码|
|- vsf_usr_cfg.h|vsf配置代码|
|README.md|文档内容，默认为英语版|
|README_xx.md|文档内容，特定语言版|

&emsp;&emsp;阅读文档之前，需要先搭建测试环境，请参考[测试环境搭建](playground/README_zh.md)。特定文档目录中的src目录下的代码，可以直接放到playground中，替换原来的main.c和vsf_usr_cfg.h来测试。

## 2. 内容
### 2.1 [测试环境搭建](playground/README_zh.md)
### 2.2 [内核](kernel/README_zh.md)
#### 2.2.1 [独立堆栈线程任务](kernel/thread/README_zh.md)