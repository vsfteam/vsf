# 《VSF测试环境搭建》

&emsp;&emsp;VSF测试环境用于简单评估VSF的各个功能，可以在PC上直接编译运行（基于x86 win/linux/mac的移植），也可以在qemu模拟环境中运行。

## 1. 目录结构
|目录|描述|
|----|----|
|config|配置文件目录|
|- arch|arch相关的配置文件目录|
|project|项目工程|
|- vc|visual studio工程|

## 2. 应用代码
&emsp;&emsp;应用层代码为main.c，应用层配置为vsf_usr_cfg.h。可以通过修改这2个代码，来测试不同的功能。

## 3. Windows（Win7/Win10）

&emsp;&emsp;Windows下，目前使用Visual Studio 2019作为IDE。如果需要使用一些外设的话，不同的外设需要做一些额外的软件安装以及配置。如果是在Win7系统下，需要再config/arch/vsf_usr_cfg_win.h中，去掉__WIN7__宏的注释。

### 3.1 Visual Studio 2019 安装

1. [下载](https://visualstudio.microsoft.com/zh-hans/vs/)：免费使用的话，下载Community 2019版本。
2. 运行下载的安装程序，并且按照提示，直到安装界面。按照如下选择后，点击安装按钮。
   - “工作负荷”页中，在“说面应用和移动应用”中，选择“使用C++的桌面开发”以及“通用Windows平台开发”
   - “单个组件”页中，搜索“llvm”，勾选出现的2个选项：“适用于 v142 的 C++ Clang-cl 生成工具(x64/x86)”和“适用于 Windows 的 C++ Clang 编译器(12.0.0)”。其中，版本号可能会不同。
3. 双击打开project/vc/vsf_playground.sln，菜单中的“生成(B)”下，选择“生成解决方案(B)”，测试编译是否通过。
4. 在菜单中的“调试(D)”下，选择“开始调试(S)”运行代码。在菜单中的“调试(D)”下，选择“停止调试(E)”停止运行。

### 3.2 外设驱动

- USB主机 -- 通过winusb/libusb实现
- USB设备 -- 通过usbip实现
- 文件系统 -- 通过winfs驱动实现、通过USB主机扩展U盘实现
- 音频设备 -- 通过winsound驱动实现、通过USB主机扩展USB声卡实现
- 显示设备 -- 通过SDL实现、通过USB主机扩展指定VGA显示适配器实现
- 网络设备 -- 通过winpcap实现、通过USB主机扩展ECM有线网卡实现
- 蓝牙设备 -- 通过USB主机扩展指定蓝牙dongle实现
- 输入设备 -- 通过SDL实现、通过USB主机扩展键鼠等设备实现

#### 3.2.1 USB主机
&emsp;&emsp;TBD