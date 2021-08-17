# VSF -- Versaloon Software Framework
[![GitHub](https://img.shields.io/github/license/vsfteam/vsf.svg)](https://github.com/vsfteam/vsf/blob/master/LICENSE)
[![Build Status](https://github.com/vsfteam/vsf/actions/workflows/vsf-actions.yml/badge.svg)](https://github.com/vsfteam/vsf/actions/workflows/vsf-actions.yml)

[English](README.md) |

VSF全称是Versaloon Software Framework，是一个基于Apache2.0协议的开源嵌入式软件平台框架。包含了从底层硬件的hal驱动、抢占式多任务内核、各种服务和组件。全部代码使用C语言，以及面向对象的方式实现。

## 整体框架


## 目录
| 目录名         | 描述                                                    |
| -------------- | ------------------------------------------------------- |
| document       | 文档                                                    |
| doxygen        | doxygen配置                                             |
| example        | 示例代码                                                |
| hardware       | VSF开发板硬件资料                                       |
| patch          | 一些补丁（第三方库补丁等等）                            |
| script         | 一些工具脚本                                            |
|  - cmake       | cmake工具脚本                                           |
| source         | VSF源代码                                               |
|  - component   | 组件（文件系统、协议栈、UI、外部芯片驱动）              |
|  - hal         | 硬件抽象层（芯片arch支持、芯片驱动）                    |
|  - kernel      | 内核                                                    |
|  - osa_service | 依赖内核的软件服务组件                                  |
|  - service     | 软件服务组件                                            |
|  - shell       | “皮肤”                                                  |
|  - utilities   | 基础软件工具（一些预处理功能、编译器支持、列表等等）    |

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

## [文档](document/README_zh.md)