# VSF 文档向导

## 1. 文档的目录结构

&emsp;&emsp;语言后缀：zh中文、en英文，默认为英文。

|名字|作用|
|----|---|
|res|资源文件目录|
|&emsp;image|图片资源目录|
|&emsp;&emsp;xx|特定语言的图片文件|
|src|源代码目录|
|&emsp;xxxx|特定demo源代码目录|
|&emsp;&emsp;main.c|主代码|
|&emsp;&emsp;vsf_usr_cfg.h|vsf配置代码|
|README.md|文档内容，默认为英语版|
|README_xx.md|文档内容，特定语言版|

&emsp;&emsp;阅读文档之前，需要先搭建测试环境，请参考[测试环境搭建](playground/README_zh.md)。特定文档目录中的src目录下的代码，可以直接放到playground中，替换原来的main.c和vsf_usr_cfg.h来测试。

## 2. 目录
### 2.1 [测试环境搭建](playground/README_zh.md)
### 2.2 [内核](kernel/README_zh.md)
- [IPC（任务间通信）](kernel/ipc/README_zh.md)
- [事件驱动](kernel/eda/README_zh.md)
- [独立堆栈线程任务](kernel/thread/README_zh.md)
- pt/task任务
- fsm任务