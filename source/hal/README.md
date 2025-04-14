# 硬件抽象层 (HAL) 文档

[TOC]

## 概述
硬件抽象层 (Hardware Abstraction Layer，简称 HAL) 是一种软件层，用于隐藏硬件的具体实现细节，为上层应用提供统一的接口。这使得应用程序能够在不同的硬件平台上运行，而不需要修改代码。

## 硬件抽象层 (HAL) 目录

HAL 目录包含 VSF 的硬件抽象层实现代码。该目录采用模块化设计，将不同硬件功能分类到对应子目录中，便于代码维护和扩展。通过标准化接口，上层应用可以用统一方式访问不同硬件平台的功能，减少平台依赖性。

```
hal/
├── arch/                  # 架构相关实现
├── driver/                # 设备驱动
├── utilities/             # 实用工具库
├── vsf_hal.c              # VSF HAL 层实现文件，提供 vsf_hal_init() 等 API
├── vsf_hal.h              # VSF HAL 层主头文件，定义公共接口
├── vsf_hal_cfg.h          # VSF HAL 配置文件
```

### 架构 (arch) 目录

架构目录包含针对不同处理器架构的底层实现，为 HAL 提供基础功能支持。`template` 目录提供了新架构移植的参考模板，用于简化将 VSF 移植到新的处理器架构的过程。这些模板文件包含了实现新架构所需的基本框架和关键函数。

```
arch/
├── arm/                   # ARM 架构实现
│   ├── cortex-m/          # Cortex-M 系列处理器实现
│   ├── cortex-a/          # Cortex-A 系列处理器实现
│   ├── cortex-r/          # Cortex-R 系列处理器实现
│   └── arm9/              # ARM9 处理器实现
├── rv/                    # RISC-V 架构实现
├── mcs51/                 # MCS51 架构实现
├── generic/               # 通用平台实现（如 Linux、macOS 等）
│   ├── linux/             # Linux 平台实现
│   └── macos/             # macOS 平台实现
├── common/                # 跨架构共享代码
├── template/              # 新架构移植参考模板
│   ├── template_generic.h # 模板头文件
│   └── template_generic.c # 模板实现文件
└── ...                    # 其他架构相关文件
```

### 驱动 (driver) 目录

驱动目录主要包含各芯片厂商（如ST、Allwinner、RaspberryPi等）的特定驱动实现，按照厂商和芯片系列进行组织。这些驱动实现了HAL接口，为特定硬件平台提供标准化的访问方式。此外，该目录还包含一些通用支持模块，用于简化驱动开发和提供跨平台功能。

```
driver/
├── Allwinner/             # 全志芯片驱动
│   └── F1CX00S/           # F1C100S 系列芯片驱动
├── RaspberryPi/           # 树莓派芯片驱动
│   └── RP2040/            # RP2040 系列芯片驱动
├── ST/                    # ST 芯片驱动
│   ├── STM32F103/         # STM32F103 系列芯片驱动
│   ├── STM32F730/         # STM32F730 系列芯片驱动
│   └── STM32H7XX/         # STM32H7 系列芯片驱动
├── arm/                   # ARM 通用驱动
│   └── mps2/              # MPS2 开发板驱动
├── generic/               # 通用平台驱动
│   └── linux/             # Linux 平台驱动
├── vsf/                   # 软件模拟和通用驱动
│   ├── distbus/           # 分布式总线驱动
│   ├── 74HC165_GPIO/      # 74HC165 GPIO 扩展芯片驱动
│   ├── 74HC595_GPIO/      # 74HC595 GPIO 扩展芯片驱动
│   ├── GPIO_I2C/          # GPIO 模拟 I2C 驱动
│   └── TIMER_GPIO_PWM/    # 定时器 GPIO PWM 驱动
├── x86/                   # x86 架构驱动
│   └── win/               # Windows 平台驱动
├── common/                # 通用外设驱动实现
│   ├── gpio/              # 通用 GPIO 驱动接口
│   ├── uart/              # 通用 UART 驱动接口
│   ├── usb/               # 通用 USB 驱动接口
│   ├── flash/             # 通用闪存驱动接口
│   ├── sdio/              # 通用 SDIO 驱动接口
│   └── template/          # 通用驱动模板接口
│── template/              # 驱动移植模板
└── ...                    # 其他架构相关文件
```

以下是几个关键通用支持模块：
- **template 目录**：提供驱动移植模板，确保所有驱动实现遵循统一的设计规范，更多使用可以参考 [template](driver/template/README.md)
- **common 目录**：为 HAL 层提供模板和多平台类型支持，包含通用外设驱动接口，使不同芯片厂商的驱动可以采用一致的接口规范。
  - **common/template 目录**：包含各种外设接口的标准化模板，定义了统一的参数结构、功能接口和行为规范。这些模板提供了各类外设（如 GPIO、UART、I2C、SPI 等）的标准接口定义，确保不同厂商实现的驱动具有一致的调用方式和行为特性。每个模板通常包含完整的函数原型、数据结构、枚举类型和宏定义，为驱动开发提供全面的指导。
- **vsf 目录**：提供了一系列通用的外设实现，主要是软件模拟和通用驱动，例如使用 GPIO 模拟 I2C、定时器 GPIO PWM 驱动等，无需特定硬件支持即可实现标准接口功能。

#### 驱动模板 (template) 目录

驱动模板目录提供了标准化的框架，可用于以下三种场景：

1. **硬件外设驱动开发**：为特定芯片厂商 (VENDOR) 的特定系列 (SERIES) 中的设备 (DEVICE) 添加外设驱动。
   - 这些驱动在 VSF 中被称为 HW 驱动
   - 代码中使用 "HW" - "HW end" 注释段标记相关实现
   - VSF_XXXX_CFG_REIMPLEMENT_XXXX 是提供给 HW 驱动根据情况重定义

2. **IP 核驱动实现**：为特定 IP 核 (XXX_IP) 实现驱动。
   - IP 核驱动的用户是外设驱动
   - 代码中使用 "IPCore" - "IPCore end" 注释段标记相关实现

3. **模拟驱动实现**：实现使用标准外设接口的模拟驱动。
   - 模拟驱动的用户是应用开发者
   - 与 IP 核驱动类似，但不需要 REIMPLEMENT（无寄存器）
   - 实际应用案例：GPIO 模拟 I2C

```
driver/template/
├── __series_name_a__/     # 示例系列名称目录
│   ├── common/            # 系列通用实现
│   │   ├── i2c/           # I2C 总线驱动
│   │   │   ├── i2c.h      # I2C 驱动头文件
│   │   │   └── i2c.c      # I2C 驱动实现文件
│   │   ├── spi/           # SPI 总线驱动
│   │   │   └── spi.h      # SPI 驱动头文件
│   │   ├── usart/         # 通用同步异步收发器
│   │   │   ├── usart.h    # USART 驱动头文件
│   │   │   └── usart.c    # USART 驱动实现文件
│   │   └── ...            # 其他外设驱动
│   └── __device_name_a__/ # 具体设备配置目录
│       ├── device.h       # 设备硬件配置文件
│       ├── driver.h       # 设备驱动接口文件
│       └── driver.c       # 设备驱动实现文件
├── README.md              # 驱动开发指南和使用说明
└── ...                    # 其他模板文件
```

##### 设备配置目录 (__device_name_a__)

`__device_name_a__` 目录是实现特定设备驱动的核心配置部分，包含以下关键文件：

1. **device.h** - 设备配置文件
   - 定义设备的硬件参数（中断优先级、外设数量等）
   - 配置具体外设的中断号、寄存器基地址等硬件信息
   - 通过宏定义映射具体硬件资源到驱动层

2. **driver.h** - 驱动接口文件
   - 包含所有启用的外设驱动头文件
   - 设置驱动名称前缀和命名规则
   - 通过条件编译决定包含哪些驱动模块

3. **driver.c** - 驱动实现文件
   - 实现 `vsf_driver_init()` 函数用于硬件初始化

开发者使用`template` 目录时，需要将 `${VENDOR}`、`${SERIES}`、`${DEVICE}` 等模板标记替换为实际的厂商、系列和设备名称。
例如，对于 ST 公司的 STM32H750 设备，这些标记应分别替换为 "ST"、"STM32H7XX" 和 "STM32H750"。

device.h 文件中的 `VSF_HW_XXX_COUNT` 和 `VSF_HW_XXX_MASK` 宏用于配置外设实例（其中XXX代表具体外设名称，如USART、SPI、I2C等）：
- `VSF_HW_XXX_COUNT`：用于定义从0开始的连续外设实例数量
- `VSF_HW_XXX_MASK`：用于定义不连续的外设实例掩码

提供这两个宏可以确保驱动层的实例与实际硬件一一对应。
每个外设还需要配置相应的中断号、中断处理函数和寄存器基地址等信息，这些信息将被驱动层使用以生成正确的外设实例。驱动层使用 `VSF_XXX_CFG_IMP_LV0` 宏来生成具体的实例。

以 USART（通用同步异步收发器）为例，`VSF_USART_CFG_IMP_LV0` 宏需要配置多项硬件资源信息，可能包括：
- 寄存器基地址：控制 USART 外设的各项功能
- 中断配置：定义接收、发送、错误等中断处理机制
- DMA 通道：支持高效数据传输的直接内存访问配置

因此，在 `device.h` 配置文件中，需要为每个 USART 实例提供相应的硬件资源定义：
- USART 寄存器基地址：`VSF_HW_USART0_REG`、`VSF_HW_USART1_REG`
- USART 中断号：`VSF_HW_USART0_IRQ`、`VSF_HW_USART1_IRQ`
- USART DMA 发送通道：`VSF_HW_USART0_DMA_TX`、`VSF_HW_USART1_DMA_TX`
- USART DMA 接收通道：`VSF_HW_USART0_DMA_RX`、`VSF_HW_USART1_DMA_RX`

这些定义将映射到实际的硬件资源，使 `VSF_USART_CFG_IMP_LV0` 能够正确生成 USART 外设。具体实现机制如下：

1. 驱动文件通过 `#include "hal/driver/common/usart/usart_template.inc"` 引入模板文件
2. 模板文件会根据不同的宏配置，从 `VSF_HW_USART_COUNT` 和 `VSF_HW_USART_MASK` 获取实例数量
3. 然后根据这些配置自动展开 `VSF_USART_CFG_IMP_LV0`，生成相应的外设驱动实例

##### 使用驱动模板的步骤（以USART为例）

1. **复制模板文件**：根据要实现的外设类型，从 template 目录中复制相应的模板文件到新的芯片目录下。
   ```
   # 例如，为 STM32H750 实现 USART 驱动
   cp -r driver/template/__series_name_a__/common/usart driver/ST/STM32H7XX/common/
   ```

2. **修改接口定义**：根据芯片手册，调整头文件中的接口定义以匹配芯片的特性。
   ```c
   #define VSF_USART_CFG_REIMPLEMENT_TYPE_MODE         ENABLED

   typedef enum vsf_usart_mode_t {
      ...
   } vsf_usart_mode_t;

   ...

   ```

3. **实现硬件功能**：在源文件中实现具体的硬件操作函数。
   ```c
   vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_init)(
      VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
      vsf_usart_cfg_t *cfg_ptr
   ) {
      VSF_HAL_ASSERT(NULL != usart_ptr);
      VSF_HAL_ASSERT(NULL != cfg_ptr);

      ...

      return VSF_ERR_NONE;
   }
   ```


### 实用工具 (utilities) 目录

utilities 目录包含各种辅助工具和库函数，为 HAL 和应用层提供通用功能支持。这些工具能够简化驱动开发，提高代码复用性和可维护性。

```
utilities/
├── i2c_util/              # I2C 设备访问工具
├── io_mapper/             # IO 映射器工具
├── remap/                 # 外设重映射工具
│   ├── i2c/               # I2C 重映射
│   ├── spi/               # SPI 重映射
│   ├── usart/             # USART 重映射
|   └── ...                # 其他外设重映射
└── stream/                # 流操作工具
    └── usart/             # USART 流操作
```
