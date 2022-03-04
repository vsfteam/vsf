# VSF -- Versaloon Software Framework
[![GitHub](https://img.shields.io/github/license/vsfteam/vsf.svg)](https://github.com/vsfteam/vsf/blob/master/LICENSE)
[![windows-build](https://github.com/vsfteam/vsf/actions/workflows/windows-build.yml/badge.svg)](https://github.com/vsfteam/vsf/actions/workflows/windows-build.yml)
[![cmake-native-build](https://github.com/vsfteam/vsf/actions/workflows/cmake-native-build.yml/badge.svg)](https://github.com/vsfteam/vsf/actions/workflows/cmake-native-build.yml)
[![cmake-arm-cross-build](https://github.com/vsfteam/vsf/actions/workflows/cmake-arm-cross-build.yml/badge.svg)](https://github.com/vsfteam/vsf/actions/workflows/cmake-arm-cross-build.yml)

[English](README.md) |

VSF全称是Versaloon Software Framework，是一个基于Apache2.0协议的开源嵌入式软件平台框架。包含了从底层硬件的hal驱动、抢占式多任务内核、各种服务和组件。全部代码使用C语言，以及面向对象的方式实现。

## 整体框架


## 目录
| 目录名             | 描述                                                    |
| ------------------ | ------------------------------------------------------- |
| document           | 文档                                                    |
| doxygen            | doxygen配置                                             |
| example            | 示例代码                                                |
| hardware           | VSF开发板硬件资料                                       |
| patch              | 一些补丁（第三方库补丁等等）                            |
| script             | 一些工具脚本                                            |
|  &emsp;cmake       | cmake工具脚本                                           |
| source             | VSF源代码                                               |
|  &emsp;component   | 组件（文件系统、协议栈、UI、外部芯片驱动）              |
|  &emsp;hal         | 硬件抽象层（芯片arch支持、芯片驱动）                    |
|  &emsp;kernel      | 内核                                                    |
|  &emsp;osa_service | 依赖内核的软件服务组件                                  |
|  &emsp;service     | 软件服务组件                                            |
|  &emsp;shell       | “皮肤”                                                  |
|  &emsp;utilities   | 基础软件工具（一些预处理功能、编译器支持、列表等等）    |

## 内核
基于事件驱动的抢占式多任务内核，支持51、8bit MCU、32/64 bit arm、riscv、x86等等各种构架的芯片。

- 事件驱动，有事件运行，没事件休眠
- 抢占模式下，任务切换由硬件实现，任务优先级就是硬件swi（software interrupt）的优先级
- 不同优先级抢占，同一优先级协作
- 可以运行在其他系统或者RTOS中，也可以运行在一个或者几个SWI中断中（和其他RTOS并存）。
- 多种任务形式
  - 事件处理任务 -- 最小资源占用，最简配置下占用20字节ram，常用配置下占用40字节ram
  - pt任务 -- 接近独立堆栈任务开发方式的共享堆栈任务
  - 独立堆栈任务 -- 依赖libc中的setjmp库
  - fsm状态机任务
  - “皮肤”中的其他任务封装形式，比如pthread
- 信号量、互斥量、触发器、队列等等常用IPC工具

## 组件
- 合理的框架设计，软件高度可以复用
- 尽可能提供申明式的开发方式
- 标准化接口，第三方软件一次性移植，全平台适配
- 软件组件/框架
  - distbus -- 分布式总线框架
  - fifo
  - heap
  - json
  - pool -- 内存池
  - stream -- 流接口
  - trace
- 组件
  - fs -- 文件系统，支持VFS（可使用第三方的文件系统）
  - input -- 输入系统
  - mal -- 块设备
  - scsi -- SCSI设备
  - tcpip -- TCPIP协议栈以及netdrv网络设备（可使用第三方的TCPIP协议栈）
  - ui -- UI以及显示设备（可使用第三方的GUI）
  - usb -- USB主从机协议栈
  - bt -- 蓝牙协议栈（使用第三方的btstack）

## 硬件抽象层
- 标准hal接口，统一API -- 比如：vsf_spi_init可以用于所有VSF中支持的SPI，包括GPIO模拟的SPI、通过USB外扩的SPI，通过分布式总线访问的远端SPI
- 简化开发的IP核驱动 -- 移植仅需要实现时钟、复位、中断等等IP核心之外的功能
- 各种接口封装模板
- 不同等级的封装接口
  - lv0 -- 方便移植
  - lv1 -- 方便使用，可以简单通过引用模板调用lv0的接口来实现
  - lv2 -- 对接各种应用（比如arduino）
- 接口
  - PM
  - GPIO
  - SPI
  - I2C
  - PWM
  - ADC
  - SWI
  - USART
  - FLASH
  - USB
  - ethernet

## “皮肤”
“皮肤”可以把VSF“伪装”成其他系统，使得可以直接使用基于其他系统的应用代码。

- SDL -- 可以直接使用一些基于SDL的应用层代码
- linux -- 可以直接使用一些基于linux的应用层代码
  - posix
  - devfs
  - socket
  - console
  - 一些lib库的实现
    - libusb
    - libgen

## 第三方
| 名字              | 路径                                                           | 许可       | 链接                                           |
|-------------------|---------------------------------------------------------------|------------|------------------------------------------------|
| btstack           | source/component/3rd-party/btstack/raw                        | Other      | https://github.com/bluekitchen/btstack         |
| coremark          | source/component/3rd-party/coremark/raw                       | Apache     | https://github.com/eembc/coremark              |
| freetype          | source/component/3rd-party/freetype/raw                       | FreeType   | https://freetype.org/                          |
| zlib              | source/component/3rd-party/zlib/raw                           | zlib       | http://zlib.net/                               |
| nuklear           | source/component/3rd-party/nuklear/raw                        | MTI        | https://github.com/Immediate-Mode-UI/Nuklear   |
| nnom              | source/component/3rd-party/nnom/raw                           | Apache 2.0 | https://github.com/majianjia/nnom              |
| lua               | source/component/3rd-party/lua/raw                            | MIT        | https://www.lua.org/                           |
| lwip              | source/component/3rd-party/lwip/raw                           | BSD        | https://savannah.nongnu.org/projects/lwip/     |
| libpng            | source/component/3rd-party/libpng/raw                         | PNG2       | https://libpng.sf.net                          |
| libjpeg-turbo     | source/component/3rd-party/libjpeg-turbo/raw                  | BSD        | https://libjpeg-turbo.org/                     |
| SDL_ttf           | source/shell/media/sdl2/3rd-party/SDL_ttf                     | zlib       | https://hg.libsdl.org/SDL_ttf/                 |
| SDL_image         | source/shell/media/sdl2/3rd-party/SDL_image                   | zlib       | https://hg.libsdl.org/SDL_image/               |
| lvgl              | source/component/3rd-party/lvgl/raw/lvgl                      | MIT        | https://lvgl.io/                               |
| lv_lib_freetype   | source/component/3rd-party/lvgl/extension/lv_lib_freetype/raw | MIT        | https://lvgl.io/                               |
| CMSIS             | source/utilities/compiler/arm/3rd-party/CMSIS                 | Apache 2.0 | https://github.com/ARM-software/CMSIS_5        |
| evm               | source/component/3rd-party/evm/raw                            | Apache 2.0 | https://github.com/scriptiot/evm               |
| LingLongGUI       | source/component/3rd-party/LingLongGUI/raw                    | Apache 2.0 | https://gitee.com/gzbkey/LingLongGUI           |
| PLOOC             | source/utilities/3rd-party/PLOOC/raw                          | Apache 2.0 | https://github.com/GorgonMeducer/PLOOC         |
| mbedtls           | source/component/3rd-party/mbedtls/raw                        | Apache 2.0 | https://tls.mbed.org/                          |
| GuiLite           | source/component/3rd-party/GuiLite/raw                        | Apache 2.0 | https://github.com/idea4good/GuiLite           |
| Segger_RTT        | source/component/3rd-party/segger/raw/RTT                     | segger     | https://wiki.segger.com/RTT                    |
| Segger_SystemView | source/component/3rd-party/segger/raw/SystemView              | segger     | https://wiki.segger.com/SystemView             |
| nuconsole         | source/component/3rd-party/nuconsole/raw                      | nuvoton    | https://www.nuvoton.com.cn/                    |
| AIC8800M_SDK      | source/hal/driver/AIC/AIC8800/vendor                          | aic        | http://www.aicsemi.com/                        |
| awtk              |                                                               | LGPL 2.1   | https://www.zlg.cn/index/pub/awtk.html         |
| littlefs          | source/component/3rd-party/littlefs/raw                       | BSD        | https://github.com/littlefs-project/littlefs   |
| getopt_long       | source/shell/sys/linux/lib/glibc/3rd-party                    | OpenBSD    | https://github.com/openbsd/src                 |
| setjmp            | source/hal/arch/x86/win                                       | BSD        |                                                |

## [文档](document/README_zh.md)
