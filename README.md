# VSF -- Versaloon Software Framework
[![GitHub](https://img.shields.io/github/license/vsfteam/vsf.svg)](https://github.com/vsfteam/vsf/blob/master/LICENSE)
[![Build Status](https://github.com/vsfteam/vsf/actions/workflows/vsf-actions.yml/badge.svg)](https://github.com/vsfteam/vsf/actions/workflows/vsf-actions.yml)

[中文](README_zh.md) |

Full name of VSF is "Versaloon Software Framework". VSF is a open-source software framework for embedded applications based on Apache2. VSF includes hal, a pre-emptive multi-thread kernel and many services or components. VSF is implemented by C language, and using object-oriented programming methods.

## Overall Framework


## Catalogue
| Name           | Description                                               |
| -------------- | --------------------------------------------------------- |
| document       | document                                                  |
| doxygen        | configuration of doxygen                                  |
| example        | demo codes                                                |
| hardware       | hardware of VSF development boards                        |
| patch          | some patches(for 3rd-party components, etc)               |
| script         |                                                           |
|  -cmake        | cmake scripts                                             |
| source         | source code of VSF                                        |
|  -component    | component(filesystem, stacks, UI, drivers, etc)           |
|  -hal          | hardware abstraction layer(ARCH support, chip drivers)    |
|  -kernel       | VSF pre-emptive/collaborative kernel                      |
|  -osa_service  | software services based on kernel                         |
|  -service      | software services                                         |
|  -shell        | shell to use application softwares for other systems      |
|  -utilities    | base software utilities(preprocessor, compiler, list, etc)|

## Kernel
Pre-emptive kernel based on event-driven architecture, supporting mcs51, 8bit MCU, 32/64 bit ARM, riscv, x86, etc.

- event-driven architecture, sleep if no event occured
- in pre-emptive mode, task switch is implemented by hardware swi(software interrupt), priority of task is the priority of the swi.
- pre-emptive scheduling for different priority, collaborative scheduling for the same priority
- can be run in other RTOS
- different tasks
  - event handler task -- minimum resources usage, 20 bytes ram usage for minimum configuration, 40 bytes ram usage for normal configuration
  - pt task
  - thread with dedicated stack -- depending on setjmp in libc
  - fsm task
  - other tasks in shell, eg pthread
- IPC like semaphore, mutex, trigger, queue, etc

## Components
- Reasonable framework for code re-use
- declarative development model
- standard interface, standard port for 3rd-party components
- software components
  - distbus -- distribute bus framework
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

- SDL -- to use application code based on SDL
- linux -- to use application code based on linux
  - posix
  - devfs
  - socket
  - console
  - some libraries
    - libusb
    - libgen
