# VSF -- Versaloon Software Framework
[![GitHub](https://img.shields.io/github/license/vsfteam/vsf.svg)](https://github.com/vsfteam/vsf/blob/master/LICENSE)
[![windows-build](https://github.com/vsfteam/vsf/actions/workflows/windows-build.yml/badge.svg)](https://github.com/vsfteam/vsf/actions/workflows/windows-build.yml)
[![cmake-native-build](https://github.com/vsfteam/vsf/actions/workflows/cmake-native-build.yml/badge.svg)](https://github.com/vsfteam/vsf/actions/workflows/cmake-native-build.yml)
[![cmake-arm-cross-build](https://github.com/vsfteam/vsf/actions/workflows/cmake-arm-cross-build.yml/badge.svg)](https://github.com/vsfteam/vsf/actions/workflows/cmake-arm-cross-build.yml)

[中文](README_zh.md) |

Full name of VSF is "Versaloon Software Framework". VSF is an open-source software framework for embedded applications based on Apache2 license. VSF includes hal, a pre-emptive multi-thread kernel, services and components. VSF is implemented by C language with object-oriented programming methods(PLOOC).

## Overall Framework


## Catalogue
| Name               | Description                                               |
| ------------------ | --------------------------------------------------------- |
| document           | document                                                  |
| doxygen            | configuration of doxygen                                  |
| example            | demo codes                                                |
| hardware           | hardware of VSF development boards                        |
| patch              | some patches(for 3rd-party components, etc)               |
| script             |                                                           |
|  &emsp;cmake       | cmake scripts                                             |
| source             | source code of VSF                                        |
|  &emsp;component   | component(filesystem, stacks, UI, drivers, etc)           |
|  &emsp;hal         | hardware abstraction layer(ARCH support, chip drivers)    |
|  &emsp;kernel      | VSF pre-emptive/collaborative kernel                      |
|  &emsp;osa_service | software services based on kernel                         |
|  &emsp;service     | software services                                         |
|  &emsp;shell       | shell to use application softwares for other systems      |
|  &emsp;utilities   | base software utilities(preprocessor, compiler, list, etc)|

## Kernel
Pre-emptive kernel based on event-driven architecture, supporting mcs51, 8bit MCU, 32/64 bit ARM, riscv, x86, etc.

- event-driven architecture, sleep if no event occured, lower-power naturally
- in pre-emptive mode, task switch is implemented by hardware swi(software interrupt), priority of task is the priority of the swi.
- pre-emptive scheduling for tasks with different priority, collaborative scheduling for tasks with the same priority
- can run in other RTOS as a task, or as swi ISR
- different tasks
  - event handler task -- minimum resources usage, 20 bytes ram usage for minimum configuration, 40 bytes ram usage for normal configuration
  - pt task
  - thread with dedicated stack -- depending on setjmp in libc
  - fsm task
  - other tasks in shell, eg pthread
- IPC: semaphore, mutex, trigger, queue, etc

## Components
- Reasonable framework for code re-use
- declarative development model
- standard interface, standard port for 3rd-party components
- software components
  - distbus -- distributed bus framework
  - fifo
  - heap
  - json
  - pool
  - stream
  - trace
- Components
  - fs -- filesystem with vfs support
  - input -- input sub-system
  - mal -- memory abstraction layer(normally known as 'block' device)
  - scsi -- SCSI device
  - tcpip -- TCPIP stack and netdrv drivers
  - ui -- UI and display drivers
  - usb -- USB host/slave stacks
  - bt -- bt stack(using btstack)

## HAL -- hardware abstraction layer
- standard hal interface and API -- eg: vsf_spi_init can be used with hardware SPI, GPIO emulated SPI, SPI from external usb device, etc
- IP core driver -- simplify porting by implement clock, reset, interrupt functions which is not provided by IP core
- template for different intefaces and functionalities
- different level of API
  - lv0 -- for porting
  - lv1 -- for user, implement by template
  - lv2 -- for specific application, eg: arduino
- interfaces
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

## Shell
To use applications from other system, shell can be used.

- SDL -- use applications based on SDL directly
- linux -- use applications based on linux directly
  - posix
  - devfs
  - socket
  - console
  - libraries
    - libusb
    - libgen
    - libsdl2
    - libcurl
    - etc

## 3rd-party
| Software          | Path                                                          | License    | Link                                           |
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
