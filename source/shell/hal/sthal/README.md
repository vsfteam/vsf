# STHAL 包装层

STHAL是一个基于VSF HAL API的包装层，旨在尽可能实现与ST HAL API的兼容性。
这使得熟悉ST HAL的开发者可以更容易地将代码迁移到VSF平台。

[TOC]

## 支持的外设

- GPIO (通用输入/输出接口)
- UART/USART (通用异步收发器)
- USART (通用同步异步收发器)
- SMARTCARD (智能卡接口)
- IRDA (红外数据协会接口)
- SPI (串行外设接口)
- QSPI (四线串行外设接口)
- I2C (内部集成电路总线)
- FLASH (闪存)
- TIM (定时器) (基本功能)
- WWDG (窗口看门狗)

## 尚未实现的外设

- ADC (模数转换器)
- DMA (直接内存访问)
- EXTI (外部中断)
- RTC (实时时钟)

## 使用说明

```c
#include "sthal.h"

int main(void)
{
    HAL_Init();

    // do some thing
}
```

## 注意事项

- STHAL 包装层依赖 VSF 的 HAL 模块，包括 arch（架构）和 driver（驱动）部分。
- 使用 STHAL 时，需要确保 VSF 的 HAL 模块已正确配置和初始化。
- 部分 API 的实现依赖于底层硬件的特定功能支持，请参考各 API 的支持状态说明。

## 外设API说明

### GPIO

| API | 说明 | 支持状态 |
| --- | --- | --- |
| `HAL_GPIO_Init()` | 初始化GPIO引脚 | ✅ 支持 |
| `HAL_GPIO_DeInit()` | 反初始化GPIO引脚 | ✅ 支持 |
| `HAL_GPIO_ReadPin()` | 读取GPIO引脚状态 | ✅ 支持 |
| `HAL_GPIO_WritePin()` | 写入GPIO引脚状态 | ✅ 支持 |
| `HAL_GPIO_TogglePin()` | 翻转GPIO引脚状态 | ✅ 支持 |
| `HAL_GPIO_EXTI_IRQHandler()` | GPIO外部中断处理 | ✅ 支持 |
| `HAL_GPIO_EXTI_Callback()` | GPIO外部中断回调函数 | ✅ 支持 |
| `HAL_GPIO_LockPin()` | 锁定GPIO引脚配置 | ❌ 不支持 |

### 串行通信接口

#### UART (通用异步收发器)

| API | 说明 | 支持状态 |
| --- | --- | --- |
| `HAL_UART_Init()` | 初始化UART | ✅ 支持 |
| `HAL_UART_DeInit()` | 反初始化UART | ✅ 支持 |
| `HAL_UART_Transmit()` | 阻塞式数据发送 | ✅ 支持 |
| `HAL_UART_Receive()` | 阻塞式数据接收 | ✅ 支持 |
| `HAL_UART_Transmit_IT()` | 中断式数据发送 | ✅ 支持 |
| `HAL_UART_Receive_IT()` | 中断式数据接收 | ✅ 支持 |
| `HAL_UART_Transmit_DMA()` | DMA方式数据发送 | ✅ 支持 |
| `HAL_UART_Receive_DMA()` | DMA方式数据接收 | ✅ 支持 |
| `HAL_UART_IRQHandler()` | UART中断处理 | ✅ 支持 |
| `HAL_UART_TxCpltCallback()` | 发送完成回调 | ✅ 支持 |
| `HAL_UART_RxCpltCallback()` | 接收完成回调 | ✅ 支持 |
| `HAL_UART_ErrorCallback()` | 错误处理回调 | ✅ 支持 |
| `HAL_UART_DMAPause()` | 暂停DMA传输 | ⚠️ 需要VSF_USART_CTRL_REQUEST_RX_PAUSE和TX_PAUSE支持 |
| `HAL_UART_DMAResume()` | 恢复DMA传输 | ⚠️ 需要VSF_USART_CTRL_REQUEST_RX_RESUME和TX_RESUME支持 |
| `HAL_HalfDuplex_EnableTransmitter()` | 半双工模式使能发送 | ⚠️ 需要VSF_USART_CTRL_HALF_DUPLEX_ENABLE_TRANSMITTER支持 |
| `HAL_HalfDuplex_EnableReceiver()` | 半双工模式使能接收 | ⚠️ 需要VSF_USART_CTRL_HALF_DUPLEX_ENABLE_RECEIVER支持 |
| `HAL_LIN_Init()` | 初始化LIN模式 | ⚠️ 需要VSF_USART_LIN_ENABLE支持 |
| `HAL_MultiProcessor_Init()` | 初始化多处理器模式 | ⚠️ 需要多个VSF USART多处理器相关宏支持 |
| `HAL_MultiProcessor_EnterMuteMode()` | 进入静默模式 | ⚠️ 需要VSF_USART_CTRL_MULTI_PROCESSOR_ENTER_MUTE_MODE支持 |
| `HAL_MultiProcessor_ExitMuteMode()` | 退出静默模式 | ⚠️ 需要VSF_USART_CTRL_MULTI_PROCESSOR_EXIT_MUTE_MODE支持 |

#### USART (通用同步异步收发器)

| API | 说明 | 支持状态 |
| --- | --- | --- |
| `HAL_USART_Init()` | 初始化USART | ✅ 支持 |
| `HAL_USART_DeInit()` | 反初始化USART | ✅ 支持 |
| `HAL_USART_Transmit()` | 阻塞式数据发送 | ✅ 支持 |
| `HAL_USART_Receive()` | 阻塞式数据接收 | ✅ 支持 |
| `HAL_USART_TransmitReceive()` | 阻塞式数据收发 | ✅ 支持 |
| `HAL_USART_Transmit_IT()` | 中断式数据发送 | ✅ 支持 |
| `HAL_USART_Receive_IT()` | 中断式数据接收 | ✅ 支持 |
| `HAL_USART_TransmitReceive_IT()` | 中断式数据收发 | ✅ 支持 |
| `HAL_USART_Transmit_DMA()` | DMA方式数据发送 | ✅ 支持 |
| `HAL_USART_Receive_DMA()` | DMA方式数据接收 | ✅ 支持 |
| `HAL_USART_TransmitReceive_DMA()` | DMA方式数据收发 | ✅ 支持 |
| `HAL_USART_IRQHandler()` | USART中断处理 | ✅ 支持 |
| `HAL_USART_TxCpltCallback()` | 发送完成回调 | ✅ 支持 |
| `HAL_USART_RxCpltCallback()` | 接收完成回调 | ✅ 支持 |
| `HAL_USART_TxRxCpltCallback()` | 收发完成回调 | ✅ 支持 |
| `HAL_USART_ErrorCallback()` | 错误处理回调 | ✅ 支持 |
| `HAL_USART_AbortCpltCallback()` | 中止完成回调 | ✅ 支持 |
| `HAL_USART_Abort()` | 中止传输 | ✅ 支持 |
| `HAL_USART_Abort_IT()` | 中断式中止传输 | ✅ 支持 |
| `HAL_USART_DMAPause()` | 暂停DMA传输 | ✅ 支持 |
| `HAL_USART_DMAResume()` | 恢复DMA传输 | ✅ 支持 |
| `HAL_USART_DMAStop()` | 停止DMA传输 | ✅ 支持 |

#### SMARTCARD (智能卡接口)

| API | 说明 | 支持状态 |
| --- | --- | --- |
| `HAL_SMARTCARD_Init()` | 初始化SMARTCARD | ✅ 支持 |
| `HAL_SMARTCARD_DeInit()` | 反初始化SMARTCARD | ✅ 支持 |
| `HAL_SMARTCARD_Transmit()` | 阻塞式数据发送 | ✅ 支持 |
| `HAL_SMARTCARD_Receive()` | 阻塞式数据接收 | ✅ 支持 |
| `HAL_SMARTCARD_Transmit_IT()` | 中断式数据发送 | ✅ 支持 |
| `HAL_SMARTCARD_Receive_IT()` | 中断式数据接收 | ✅ 支持 |
| `HAL_SMARTCARD_Transmit_DMA()` | DMA方式数据发送 | ✅ 支持 |
| `HAL_SMARTCARD_Receive_DMA()` | DMA方式数据接收 | ✅ 支持 |
| `HAL_SMARTCARD_IRQHandler()` | SMARTCARD中断处理 | ✅ 支持 |
| `HAL_SMARTCARD_Abort()` | 中止传输 | ✅ 支持 |
| `HAL_SMARTCARD_AbortTransmit()` | 中止发送 | ✅ 支持 |
| `HAL_SMARTCARD_AbortReceive()` | 中止接收 | ✅ 支持 |
| `HAL_SMARTCARD_Abort_IT()` | 中断式中止传输 | ✅ 支持 |
| `HAL_SMARTCARD_AbortTransmit_IT()` | 中断式中止发送 | ✅ 支持 |
| `HAL_SMARTCARD_AbortReceive_IT()` | 中断式中止接收 | ✅ 支持 |
| `HAL_SMARTCARD_TxCpltCallback()` | 发送完成回调 | ✅ 支持 |
| `HAL_SMARTCARD_RxCpltCallback()` | 接收完成回调 | ✅ 支持 |
| `HAL_SMARTCARD_ErrorCallback()` | 错误处理回调 | ✅ 支持 |

#### IRDA (红外数据协会接口)

| API | 说明 | 支持状态 |
| --- | --- | --- |
| `HAL_IRDA_Init()` | 初始化IRDA | ✅ 支持 |
| `HAL_IRDA_DeInit()` | 反初始化IRDA | ✅ 支持 |
| `HAL_IRDA_Transmit()` | 阻塞式数据发送 | ✅ 支持 |
| `HAL_IRDA_Receive()` | 阻塞式数据接收 | ✅ 支持 |
| `HAL_IRDA_Transmit_IT()` | 中断式数据发送 | ✅ 支持 |
| `HAL_IRDA_Receive_IT()` | 中断式数据接收 | ✅ 支持 |
| `HAL_IRDA_Transmit_DMA()` | DMA方式数据发送 | ✅ 支持 |
| `HAL_IRDA_Receive_DMA()` | DMA方式数据接收 | ✅ 支持 |
| `HAL_IRDA_IRQHandler()` | IRDA中断处理 | ✅ 支持 |
| `HAL_IRDA_TxCpltCallback()` | 发送完成回调 | ✅ 支持 |
| `HAL_IRDA_RxCpltCallback()` | 接收完成回调 | ✅ 支持 |
| `HAL_IRDA_ErrorCallback()` | 错误处理回调 | ✅ 支持 |

### SPI (串行外设接口)

| API | 说明 | 支持状态 |
| --- | --- | --- |
| `HAL_SPI_Init()` | 初始化SPI | ✅ 支持 |
| `HAL_SPI_DeInit()` | 反初始化SPI | ✅ 支持 |
| `HAL_SPI_Transmit()` | 阻塞式数据发送 | ✅ 支持 |
| `HAL_SPI_Receive()` | 阻塞式数据接收 | ✅ 支持 |
| `HAL_SPI_TransmitReceive()` | 阻塞式数据收发 | ✅ 支持 |
| `HAL_SPI_Transmit_IT()` | 中断式数据发送 | ✅ 支持 |
| `HAL_SPI_Receive_IT()` | 中断式数据接收 | ✅ 支持 |
| `HAL_SPI_TransmitReceive_IT()` | 中断式数据收发 | ✅ 支持 |
| `HAL_SPI_Transmit_DMA()` | DMA方式数据发送 | ✅ 支持 |
| `HAL_SPI_Receive_DMA()` | DMA方式数据接收 | ✅ 支持 |
| `HAL_SPI_TransmitReceive_DMA()` | DMA方式数据收发 | ✅ 支持 |
| `HAL_SPI_IRQHandler()` | SPI中断处理 | ✅ 支持 |
| `HAL_SPI_TxCpltCallback()` | 发送完成回调 | ✅ 支持 |
| `HAL_SPI_RxCpltCallback()` | 接收完成回调 | ✅ 支持 |
| `HAL_SPI_TxRxCpltCallback()` | 收发完成回调 | ✅ 支持 |
| `HAL_SPI_ErrorCallback()` | 错误处理回调 | ✅ 支持 |

### QSPI (四线串行外设接口)

| API | 说明 | 支持状态 |
| --- | --- | --- |
| `HAL_QSPI_Init()` | 初始化QSPI | ✅ 支持 |
| `HAL_QSPI_DeInit()` | 反初始化QSPI | ✅ 支持 |
| `HAL_QSPI_Command()` | 发送QSPI命令 | ✅ 支持 |
| `HAL_QSPI_Command_IT()` | 中断式发送QSPI命令 | ⚠️ 需改进 (目前使用模拟中断，应改用硬件中断) |
| `HAL_QSPI_Transmit()` | 阻塞式数据发送 | ✅ 支持 |
| `HAL_QSPI_Receive()` | 阻塞式数据接收 | ✅ 支持 |
| `HAL_QSPI_Transmit_IT()` | 中断式数据发送 | ✅ 支持 |
| `HAL_QSPI_Receive_IT()` | 中断式数据接收 | ✅ 支持 |
| `HAL_QSPI_Transmit_DMA()` | DMA方式数据发送 | ✅ 支持 |
| `HAL_QSPI_Receive_DMA()` | DMA方式数据接收 | ✅ 支持 |
| `HAL_QSPI_AutoPolling()` | 自动轮询模式 | ⚠️ TODO (需要VSF_SPI_CTRL_QSPI_QSPI_AUTO_POLLING支持) |
| `HAL_QSPI_AutoPolling_IT()` | 中断式自动轮询 | ⚠️ TODO (需要VSF_SPI_CTRL_QSPI_QSPI_AUTO_POLLING支持) |
| `HAL_QSPI_MemoryMapped()` | 内存映射模式 | ⚠️ TODO (需要VSF_SPI_CTRL_QSPI_QSPI_MEMORY_MAP_SET支持) |
| `HAL_QSPI_IRQHandler()` | QSPI中断处理 | ✅ 支持 |
| `HAL_QSPI_CmdCpltCallback()` | 命令完成回调 | ✅ 支持 |
| `HAL_QSPI_RxCpltCallback()` | 接收完成回调 | ✅ 支持 |
| `HAL_QSPI_TxCpltCallback()` | 发送完成回调 | ✅ 支持 |
| `HAL_QSPI_StatusMatchCallback()` | 状态匹配回调 | ⚠️ TODO |
| `HAL_QSPI_TimeOutCallback()` | 超时回调 | ⚠️ TODO |
| `HAL_QSPI_ErrorCallback()` | 错误处理回调 | ⚠️ TODO |
| `HAL_QSPI_AbortCpltCallback()` | 中止完成回调 | ✅ 支持 |
| `HAL_QSPI_Abort()` | 中止QSPI传输 | ✅ 支持 |
| `HAL_QSPI_Abort_IT()` | 中断式中止QSPI传输 | ✅ 支持 |

### I2C

| API | 说明 | 支持状态 |
| --- | --- | --- |
| `HAL_I2C_Init()` | 初始化I2C | ✅ 支持 |
| `HAL_I2C_DeInit()` | 反初始化I2C | ✅ 支持 |
| `HAL_I2C_Master_Transmit()` | 主机模式阻塞式数据发送 | ✅ 支持 |
| `HAL_I2C_Master_Receive()` | 主机模式阻塞式数据接收 | ✅ 支持 |
| `HAL_I2C_Slave_Transmit()` | 从机模式阻塞式数据发送 | ✅ 支持 |
| `HAL_I2C_Slave_Receive()` | 从机模式阻塞式数据接收 | ✅ 支持 |
| `HAL_I2C_Mem_Write()` | 内存写入 | ✅ 支持 |
| `HAL_I2C_Mem_Read()` | 内存读取 | ✅ 支持 |
| `HAL_I2C_Master_Transmit_IT()` | 主机模式中断式数据发送 | ✅ 支持 |
| `HAL_I2C_Master_Receive_IT()` | 主机模式中断式数据接收 | ✅ 支持 |
| `HAL_I2C_Slave_Transmit_IT()` | 从机模式中断式数据发送 | ✅ 支持 |
| `HAL_I2C_Slave_Receive_IT()` | 从机模式中断式数据接收 | ✅ 支持 |
| `HAL_I2C_Mem_Write_IT()` | 中断式内存写入 | ✅ 支持 |
| `HAL_I2C_Mem_Read_IT()` | 中断式内存读取 | ✅ 支持 |
| `HAL_I2C_Master_Transmit_DMA()` | 主机模式DMA方式数据发送 | ✅ 支持 |
| `HAL_I2C_Master_Receive_DMA()` | 主机模式DMA方式数据接收 | ✅ 支持 |
| `HAL_I2C_Slave_Transmit_DMA()` | 从机模式DMA方式数据发送 | ✅ 支持 |
| `HAL_I2C_Slave_Receive_DMA()` | 从机模式DMA方式数据接收 | ✅ 支持 |
| `HAL_I2C_Mem_Write_DMA()` | DMA方式内存写入 | ✅ 支持 |
| `HAL_I2C_Mem_Read_DMA()` | DMA方式内存读取 | ✅ 支持 |
| `HAL_I2C_IRQHandler()` | I2C中断处理 | ✅ 支持 |
| `HAL_I2C_EnableListen_IT()` | 使能监听模式 | ⚠️ 需要VSF_I2C_IRQ_MASK_ENABLE_LISTEN支持 |
| `HAL_I2C_DisableListen_IT()` | 禁用监听模式 | ⚠️ 需要VSF_I2C_IRQ_MASK_DISABLE_LISTEN支持 |
| `HAL_I2C_Master_Abort_IT()` | 主机模式中断方式终止传输 | ⚠️ 需要VSF_I2C_CTRL_MASTER_ABORT支持 |
| `HAL_I2C_Slave_Seq_Transmit_IT()` | 从机模式顺序传输 | ⚠️ 需要特定宏支持 |
| `HAL_I2C_Slave_Seq_Receive_IT()` | 从机模式顺序接收 | ⚠️ 需要特定宏支持 |
| `HAL_I2C_Slave_Seq_Transmit_DMA()` | 从机模式DMA顺序传输 | ⚠️ 需要特定宏支持 |
| `HAL_I2C_Slave_Seq_Receive_DMA()` | 从机模式DMA顺序接收 | ⚠️ 需要特定宏支持 |
| `HAL_I2CEx_ConfigAnalogFilter()` | 配置模拟滤波器 | ⚠️ 需要VSF_I2C_CTRL_ANALOG_FILTER支持 |
| `HAL_I2CEx_ConfigDigitalFilter()` | 配置数字滤波器 | ⚠️ 需要VSF_I2C_CTRL_DIGITAL_FILTER支持 |

### FLASH

| API | 说明 | 支持状态 |
| --- | --- | --- |
| `HAL_FLASH_Program()` | 编程FLASH | ✅ 支持 |
| `HAL_FLASH_Program_IT()` | 中断模式编程FLASH | ✅ 支持 |
| `HAL_FLASH_Erase()` | 擦除FLASH | ✅ 支持 |
| `HAL_FLASH_Erase_IT()` | 中断模式擦除FLASH | ✅ 支持 |
| `HAL_FLASH_Lock()` | 锁定FLASH | ✅ 支持 |
| `HAL_FLASH_Unlock()` | 解锁FLASH | ✅ 支持 |
| `HAL_FLASH_OB_Lock()` | 锁定选项字节 | ✅ 支持 |
| `HAL_FLASH_OB_Unlock()` | 解锁选项字节 | ✅ 支持 |
| `HAL_FLASH_IRQHandler()` | FLASH中断处理 | ✅ 支持 |
| `HAL_FLASH_EndOfOperationCallback()` | 操作结束回调 | ✅ 支持 |
| `HAL_FLASH_OperationErrorCallback()` | 操作错误回调 | ✅ 支持 |

### TIM (定时器)

| API | 说明 | 支持状态 |
| --- | --- | --- |
| `HAL_TIM_Base_Init()` | 初始化基本定时器 | ✅ 支持 |
| `HAL_TIM_Base_DeInit()` | 反初始化基本定时器 | ✅ 支持 |
| `HAL_TIM_Base_Start()` | 启动基本定时器 | ✅ 支持 |
| `HAL_TIM_Base_Stop()` | 停止基本定时器 | ✅ 支持 |
| `HAL_TIM_Base_Start_IT()` | 中断模式启动基本定时器 | ✅ 支持 |
| `HAL_TIM_Base_Stop_IT()` | 中断模式停止基本定时器 | ✅ 支持 |
| `HAL_TIM_Base_Start_DMA()` | DMA模式启动基本定时器 | ✅ 支持 |
| `HAL_TIM_Base_Stop_DMA()` | DMA模式停止基本定时器 | ✅ 支持 |
| `HAL_TIM_OC_Init()` | 初始化输出比较模式 | ✅ 支持 |
| `HAL_TIM_IC_Init()` | 初始化输入捕获模式 | ✅ 支持 |
| `HAL_TIM_PWM_Init()` | 初始化PWM模式 | ✅ 支持 |
| `HAL_TIM_IRQHandler()` | 定时器中断处理 | ✅ 支持 |
| `HAL_TIM_PeriodElapsedCallback()` | 定时周期结束回调 | ✅ 支持 |

> **备注**：TIM模块实现了基本定时器功能，但高级功能如PWM控制、输入捕获等可能未完全实现或需要额外配置。

### WWDG (窗口看门狗)

| API | 说明 | 支持状态 |
| --- | --- | --- |
| `HAL_WWDG_Init()` | 初始化窗口看门狗 | ✅ 支持 (需要硬件支持reset_soc或reset_cpu) |
| `HAL_WWDG_Refresh()` | 刷新窗口看门狗计数器 | ✅ 支持 |
| `HAL_WWDG_IRQHandler()` | 窗口看门狗中断处理函数 | ✅ 支持 |
| `HAL_WWDG_EarlyWakeupCallback()` | 早期唤醒回调函数 | ✅ 支持 (需要硬件支持early_wakeup) |
| `HAL_WWDG_RegisterCallback()` | 注册回调函数 | ✅ 支持 (当USE_HAL_WWDG_REGISTER_CALLBACKS=1) |
| `HAL_WWDG_UnRegisterCallback()` | 注销回调函数 | ✅ 支持 (当USE_HAL_WWDG_REGISTER_CALLBACKS=1) |

