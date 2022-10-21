# VSF 文档向导

## 1. 文档的目录结构

&emsp;&emsp;语言后缀：zh中文、en英文，无后缀为英文。

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

## 2. 目录
### 2.1 [内核](kernel/README_zh.md)
- [事件驱动](kernel/eda/README_zh.md)
- [独立堆栈线程任务](kernel/thread/README_zh.md)
### 2.2 [组件](component/README_zh.md)
- [块设备mal](component/mal/README_zh.md)