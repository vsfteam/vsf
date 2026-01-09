# VSF SPI 驱动测试设计文档

## 目录

1. [概述](#概述)
2. [测试代码组织结构](#测试代码组织结构)
3. [测试类型设计](#测试类型设计)
4. [测试用例设计](#测试用例设计)
5. [硬件需求](#硬件需求)
6. [测试框架使用](#测试框架使用)
7. [测试执行流程](#测试执行流程)

## 概述

本文档描述了基于 VSF Test 框架为 VSF SPI 驱动实现测试的设计方案。测试包括单元测试、功能测试和集成测试，旨在全面验证 SPI 驱动的正确性和可靠性。

### VSF SPI API 概览

VSF SPI 驱动提供以下主要 API：

- **初始化与配置**
  - `vsf_spi_init()` - 初始化 SPI 实例
  - `vsf_spi_fini()` - 反初始化 SPI 实例
  - `vsf_spi_get_configuration()` - 获取配置
  - `vsf_spi_capability()` - 查询能力

- **使能与控制**
  - `vsf_spi_enable()` - 使能 SPI
  - `vsf_spi_disable()` - 禁用 SPI
  - `vsf_spi_irq_enable()` - 使能中断
  - `vsf_spi_irq_disable()` - 禁用中断
  - `vsf_spi_irq_clear()` - 清除中断标志
  - `vsf_spi_status()` - 获取状态

- **片选控制**
  - `vsf_spi_cs_active()` - 激活片选
  - `vsf_spi_cs_inactive()` - 取消片选

- **数据传输**
  - `vsf_spi_fifo_transfer()` - FIFO 模式传输
  - `vsf_spi_request_transfer()` - 请求异步传输
  - `vsf_spi_cancel_transfer()` - 取消传输
  - `vsf_spi_get_transferred_count()` - 获取传输计数

- **控制命令**
  - `vsf_spi_ctrl()` - 控制命令（如 QSPI 配置）

## 测试代码组织结构

### 推荐的目录结构

```
test/
└── vsf_test/
    └── spi/                          # SPI 驱动测试目录
        ├── README.md                 # 本文档
        ├── unit/                     # 单元测试
        │   ├── test_spi_init.c       # 初始化测试
        │   ├── test_spi_config.c     # 配置测试
        │   ├── test_spi_capability.c # 能力查询测试
        │   └── test_spi_mode.c       # 模式转换测试
        ├── functional/               # 功能测试
        │   ├── test_spi_transfer.c   # 基本传输测试
        │   ├── test_spi_modes.c      # SPI 模式测试
        │   ├── test_spi_cs.c         # 片选控制测试
        │   ├── test_spi_irq.c        # 中断测试
        │   └── test_spi_dma.c        # DMA 测试（如果支持）
        ├── integration/              # 集成测试
        │   ├── test_spi_loopback.c   # 回环测试
        │   ├── test_spi_multi_device.c # 多设备测试
        │   └── test_spi_with_dma.c   # SPI + DMA 集成
        ├── test_spi_main.c           # 测试主入口
        └── test_spi_common.h         # 测试公共头文件
```

### 备选方案：靠近驱动代码

如果希望测试代码更靠近驱动实现，也可以放在：

```
source/hal/driver/common/spi/
└── test/                             # SPI 驱动的测试代码
    ├── unit/
    ├── functional/
    └── integration/
```

**推荐使用第一种方案**（放在 `test/vsf_test/spi/`），因为：
- 测试代码与源代码分离，便于管理
- 符合 VSF 项目的测试代码组织习惯
- 便于 CI/CD 集成

## 测试类型设计

### 1. 单元测试（Unit Tests）

**目标**：测试单个函数/模块，尽量不依赖硬件或最小化硬件依赖。

**特点**：
- 快速执行
- 可重复性强
- 不依赖外部硬件（或仅依赖最小硬件）
- 主要测试参数验证、错误处理、边界条件

**测试内容**：

#### 1.1 初始化测试 (`test_spi_init.c`)
- `vsf_spi_init()` 空指针检查
- `vsf_spi_init()` 有效参数测试
- `vsf_spi_init()` 无效配置测试
- `vsf_spi_fini()` 测试

#### 1.2 配置测试 (`test_spi_config.c`)
- `vsf_spi_get_configuration()` 测试
- 配置参数验证（时钟频率、模式等）
- 配置边界值测试

#### 1.3 能力查询测试 (`test_spi_capability.c`)
- `vsf_spi_capability()` 返回值验证
- 能力标志位检查
- 不同硬件平台的能力差异测试

#### 1.4 模式转换测试 (`test_spi_mode.c`)
- `vsf_spi_mode_to_data_bits()` 测试
- `vsf_spi_data_bits_to_mode()` 测试
- 模式枚举值验证

**硬件需求标记**：`hw_req=none` 或 `hw_req=spi`（仅需 SPI 硬件，不需要通信）

### 2. 功能测试（Functional Tests）

**目标**：测试 SPI 驱动的完整功能，需要硬件支持。

**特点**：
- 需要真实的 SPI 硬件
- 可能需要辅助设备（如 SPI 从机）
- 测试实际的数据传输
- 验证驱动与硬件的交互

**测试内容**：

#### 2.1 基本传输测试 (`test_spi_transfer.c`)
- `vsf_spi_request_transfer()` 基本收发
- `vsf_spi_fifo_transfer()` FIFO 模式
- 不同数据长度测试（1 字节、多字节、大块数据）
- 仅发送模式测试
- 仅接收模式测试
- 全双工模式测试

#### 2.2 SPI 模式测试 (`test_spi_modes.c`)
- MODE 0/1/2/3 测试
- 不同数据位宽测试（8bit、16bit、32bit）
- MSB/LSB 顺序测试
- 不同时钟频率测试

#### 2.3 片选控制测试 (`test_spi_cs.c`)
- `vsf_spi_cs_active()` / `vsf_spi_cs_inactive()` 测试
- 多片选切换测试
- 片选时序验证

#### 2.4 中断测试 (`test_spi_irq.c`)
- 中断使能/禁用测试
- 传输完成中断测试
- 错误中断测试
- 中断清除测试

#### 2.5 DMA 测试 (`test_spi_dma.c`)（如果硬件支持）
- DMA 模式传输测试
- DMA 与中断协同测试
- DMA 错误处理测试

**硬件需求标记**：`hw_req=spi` 或 `hw_req=spi+assist_device`

### 3. 集成测试（Integration Tests）

**目标**：测试 SPI 与其他模块的协同工作。

**特点**：
- 需要多个模块协同
- 测试实际应用场景
- 验证系统级功能

**测试内容**：

#### 3.1 回环测试 (`test_spi_loopback.c`)
- 硬件回环测试（MOSI 与 MISO 短接）
- 验证数据传输的完整性
- 不同模式下的回环测试

#### 3.2 多设备测试 (`test_spi_multi_device.c`)
- 多个 SPI 从设备切换测试
- 片选切换时序测试
- 不同设备的不同配置测试

#### 3.3 SPI + DMA 集成测试 (`test_spi_with_dma.c`)
- SPI 与 DMA 协同工作测试
- 大数据块传输测试
- 性能测试

**硬件需求标记**：`hw_req=spi+loopback_hw` 或 `hw_req=spi+multi_device`

## 测试用例设计

### 测试用例命名规范

- 单元测试：`__test_spi_<function>_<scenario>()`
- 功能测试：`__test_spi_<feature>_<scenario>()`
- 集成测试：`__test_spi_<integration>_<scenario>()`

示例：
- `__test_spi_init_null_ptr()` - 单元测试：初始化空指针
- `__test_spi_transfer_basic()` - 功能测试：基本传输
- `__test_spi_loopback_8bit()` - 集成测试：8 位回环

### 测试用例配置字符串格式

```c
"test_name hw_req=<requirements> [param1=value1] [param2=value2]"
```

**硬件需求标记**：
- `none` - 不需要硬件
- `spi` - 仅需要 SPI 硬件（不需要通信）
- `spi+assist_device` - 需要 SPI 硬件和辅助设备
- `spi+loopback_hw` - 需要 SPI 硬件和回环硬件（MOSI/MISO 短接）
- `spi+multi_device` - 需要 SPI 硬件和多个从设备

**示例**：
```c
VSF_TEST_ADD(__test_spi_init_null_ptr, "spi_init_null_ptr hw_req=none", 0)
VSF_TEST_ADD(__test_spi_transfer_basic, "spi_transfer_basic hw_req=spi+assist_device", 0)
VSF_TEST_ADD(__test_spi_loopback, "spi_loopback hw_req=spi+loopback_hw", 0)
```

### 测试用例模板

#### 单元测试模板

```c
static void __test_spi_init_null_ptr(void)
{
    vsf_spi_cfg_t cfg = {0};
    vsf_err_t err = vsf_spi_init(NULL, &cfg);
    VSF_TEST_ASSERT(vsf_test, err != VSF_ERR_NONE);
}

static void __test_spi_init_valid(void)
{
    vsf_spi_t *spi = &vsf_spi[0];  // 假设有 SPI0
    vsf_spi_cfg_t cfg = {
        .mode = VSF_SPI_MODE_0,
        .clock_hz = 1000000,
    };
    vsf_err_t err = vsf_spi_init(spi, &cfg);
    VSF_TEST_ASSERT(vsf_test, err == VSF_ERR_NONE);
}
```

#### 功能测试模板

```c
static void __test_spi_transfer_basic(void)
{
    vsf_spi_t *spi = &vsf_spi[0];

    // 初始化 SPI
    vsf_spi_cfg_t cfg = {
        .mode = VSF_SPI_MODE_0,
        .clock_hz = 1000000,
    };
    vsf_spi_init(spi, &cfg);
    vsf_spi_enable(spi);

    // 准备测试数据
    uint8_t tx_buffer[256];
    uint8_t rx_buffer[256];
    for (int i = 0; i < sizeof(tx_buffer); i++) {
        tx_buffer[i] = i;
    }

    // 执行传输（需要辅助设备配合）
    vsf_err_t err = vsf_spi_request_transfer(spi, tx_buffer, rx_buffer, sizeof(tx_buffer));
    VSF_TEST_ASSERT(vsf_test, err == VSF_ERR_NONE);

    // 验证数据（需要辅助设备回环）
    // VSF_TEST_ASSERT(vsf_test, memcmp(tx_buffer, rx_buffer, sizeof(tx_buffer)) == 0);
}
```

#### 集成测试模板

```c
static void __test_spi_loopback(void)
{
    // 需要硬件支持：MOSI 和 MISO 短接
    vsf_spi_t *spi = &vsf_spi[0];

    vsf_spi_cfg_t cfg = {
        .mode = VSF_SPI_MODE_0,
        .clock_hz = 1000000,
    };
    vsf_spi_init(spi, &cfg);
    vsf_spi_enable(spi);

    uint8_t tx_data[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t rx_data[4] = {0};

    vsf_err_t err = vsf_spi_request_transfer(spi, tx_data, rx_data, sizeof(tx_data));
    VSF_TEST_ASSERT(vsf_test, err == VSF_ERR_NONE);

    // 验证回环数据
    VSF_TEST_ASSERT(vsf_test, memcmp(tx_data, rx_data, sizeof(tx_data)) == 0);
}
```

## 硬件需求

### 被测设备要求

1. **必需**：
   - SPI 硬件外设
   - 看门狗（用于测试超时保护）
   - 测试串口（用于与辅助设备通信）

2. **可选**：
   - DMA 控制器（用于 DMA 测试）
   - 多个 SPI 外设（用于多设备测试）

### 辅助设备要求

根据测试类型，可能需要以下辅助设备：

1. **基本功能测试**：
   - SPI 从机设备（可以是另一块开发板或 USB 转 SPI 适配器）
   - 能够接收和发送 SPI 数据

2. **回环测试**：
   - 硬件回环：将 MOSI 与 MISO 短接
   - 或使用支持回环模式的 SPI 从机

3. **多设备测试**：
   - 多个 SPI 从机设备
   - 或支持多片选的 SPI 从机

### 硬件连接示例

#### 基本 SPI 通信测试
```
被测设备 (SPI Master)         辅助设备 (SPI Slave)
    MOSI  --------------->  MOSI
    MISO  <---------------  MISO
    SCK   --------------->  SCK
    CS    --------------->  CS
```

#### 回环测试
```
被测设备 (SPI Master)
    MOSI  ----+
              |
    MISO  <---+
    SCK   (正常连接)
    CS    (正常连接)
```

## 测试框架使用

### VSF Test 框架初始化

```c
#include "vsf.h"

static vsf_test_t __test = {
    .wdt = {
        .internal = {
            .init = vsf_test_hal_wdt_init,
            .feed = vsf_test_hal_wdt_feed,
        },
        .external = {
            .init = vsf_test_stdio_wdt_init,
            .feed = vsf_test_stdio_wdt_feed,
        },
    },
    .reboot = {
        .internal = vsf_arch_reset,
        .external = vsf_test_stdio_reboot,
    },
    .data = {
        .init = vsf_test_stdio_data_init,
        .sync = vsf_test_stdio_data_sync,
    }
};

static vsf_test_t *test = &__test;
```

### 测试用例注册

```c
// 静态方式（推荐，更节省 RAM）
static const vsf_test_case_t __test_cases[] = {
    VSF_TEST_ADD(__test_spi_init_null_ptr, "spi_init_null_ptr hw_req=none", 0),
    VSF_TEST_ADD(__test_spi_init_valid, "spi_init_valid hw_req=spi", 0),
    VSF_TEST_ADD(__test_spi_transfer_basic, "spi_transfer_basic hw_req=spi+assist_device", 0),
};

vsf_test_init(test, (vsf_test_case_t *)__test_cases, dimof(__test_cases));
```

### 运行测试

```c
int main(void)
{
    // ... 初始化 vsf_test ...

    vsf_test_init(test, __test_cases, dimof(__test_cases));
    vsf_test_run_tests(test);

    return 0;
}
```

### 异常处理

在异常处理函数中调用 `vsf_test_reboot()`：

```c
void HardFault_Handler(void)
{
    vsf_test_reboot(test, VSF_TEST_RESULT_FAULT_HANDLER_FAIL,
                    __FILE__, __LINE__, __FUNCTION__, "HardFault occurred");
}
```

## 测试执行流程

### 1. 测试准备

1. **硬件准备**：
   - 连接被测设备与辅助设备（如需要）
   - 配置硬件回环（如需要）
   - 连接测试串口

2. **软件准备**：
   - 编译测试程序
   - 烧录到被测设备
   - 启动辅助设备程序（如 `vsf_test_assist.py`）

### 2. 测试执行

1. **启动测试**：
   - 被测设备上电或复位
   - 测试框架自动开始执行测试用例

2. **测试过程**：
   - 每个测试用例独立执行
   - 测试失败或超时后，看门狗复位设备
   - 测试框架从持久化存储恢复状态，继续下一个测试

3. **结果收集**：
   - 测试结果通过串口发送到辅助设备
   - 辅助设备记录测试结果
   - 所有测试完成后，辅助设备输出测试报告

### 3. 测试报告

辅助设备会生成测试报告，包括：
- 测试用例总数
- 通过的测试用例数
- 失败的测试用例数
- 跳过的测试用例数
- 每个测试用例的详细结果
- 失败测试的错误信息

## 测试覆盖目标

### API 覆盖

- [x] `vsf_spi_init()` - 初始化
- [x] `vsf_spi_fini()` - 反初始化
- [x] `vsf_spi_get_configuration()` - 获取配置
- [x] `vsf_spi_capability()` - 能力查询
- [x] `vsf_spi_enable()` - 使能
- [x] `vsf_spi_disable()` - 禁用
- [x] `vsf_spi_irq_enable()` - 中断使能
- [x] `vsf_spi_irq_disable()` - 中断禁用
- [x] `vsf_spi_irq_clear()` - 中断清除
- [x] `vsf_spi_status()` - 状态查询
- [x] `vsf_spi_cs_active()` - 片选激活
- [x] `vsf_spi_cs_inactive()` - 片选取消
- [x] `vsf_spi_fifo_transfer()` - FIFO 传输
- [x] `vsf_spi_request_transfer()` - 异步传输
- [x] `vsf_spi_cancel_transfer()` - 取消传输
- [x] `vsf_spi_get_transferred_count()` - 传输计数
- [x] `vsf_spi_ctrl()` - 控制命令

### 功能覆盖

- [x] 所有 SPI 模式（MODE 0/1/2/3）
- [x] 不同数据位宽（8/16/32 bit）
- [x] 不同时钟频率
- [x] MSB/LSB 顺序
- [x] 仅发送模式
- [x] 仅接收模式
- [x] 全双工模式
- [x] 中断模式
- [x] DMA 模式（如果支持）
- [x] 多片选支持
- [x] QSPI 模式（如果支持）

### 错误处理覆盖

- [x] 空指针检查
- [x] 无效参数检查
- [x] 配置错误处理
- [x] 传输错误处理
- [x] 中断错误处理

## 注意事项

1. **测试隔离**：每个测试用例应该是独立的，不依赖其他测试用例的状态。

2. **资源清理**：测试完成后应清理资源（禁用 SPI、清除中断等）。

3. **超时设置**：为需要等待的测试设置合理的超时时间。

4. **硬件依赖**：在测试用例配置字符串中明确标记硬件需求，便于筛选和运行。

5. **辅助设备同步**：需要辅助设备的测试，应确保辅助设备已准备好。

6. **数据验证**：对于数据传输测试，应验证数据的完整性和正确性。

## 后续扩展

1. **性能测试**：添加 SPI 传输性能测试（吞吐量、延迟等）
2. **压力测试**：长时间运行测试、大数据量测试
3. **兼容性测试**：不同硬件平台的兼容性测试
4. **自动化测试**：集成到 CI/CD 流程中

## 参考资源

- [VSF Test 框架文档](../README.md)
- [VSF SPI 驱动模板](../../../../source/hal/driver/common/template/vsf_template_spi.h)
- [VSF Test 辅助工具](../../../component/test/vsf_test/vsf_test_assist.py)

