#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
VSF HAL Driver Template Test - Peripheral Generator
外设生成器模块
"""

import logging
from typing import Dict, Any, List

logger = logging.getLogger(__name__)


class PeripheralGenerator:
    """外设代码生成器类"""

    def __init__(self, config_data: Dict[str, Any]):
        """
        初始化外设生成器

        Args:
            config_data: 配置数据
        """
        self.config_data = config_data
        self.peripherals = config_data.get("PERIPHERALS", [])

    def generate_irq_enum(self) -> str:
        """
        生成中断枚举定义

        Returns:
            中断枚举代码字符串
        """
        lines = []

        for peripheral in self.peripherals:
            name = peripheral.get("name", "")
            count = peripheral.get("count", 1)
            irq_start = peripheral.get("irq_start", 0)

            lines.append(f"    // {name}")

            for i in range(count):
                irq_name = f"{name}{i}_IRQn"
                irq_value = irq_start + i
                # 格式化对齐
                padding = " " * (20 - len(irq_name))
                lines.append(f"    {irq_name}{padding}= {irq_value},")

        return "\n".join(lines)

    def generate_register_base_addresses(self) -> str:
        """
        生成寄存器基地址定义

        Returns:
            寄存器基地址代码字符串
        """
        lines = []

        for peripheral in self.peripherals:
            name = peripheral.get("name", "")
            count = peripheral.get("count", 1)
            base_addr = peripheral.get("base_addr", "0x40000000")
            addr_offset = peripheral.get("addr_offset", "0x400")

            # 转换为整数进行计算
            base_addr_int = int(base_addr, 16)
            addr_offset_int = int(addr_offset, 16)

            lines.append(f"// {name} 寄存器基地址")

            if name == "GPIO":
                # GPIO特殊处理 - 使用GPIOA, GPIOB等名称
                gpio_names = [
                    "GPIOA",
                    "GPIOB",
                    "GPIOC",
                    "GPIOD",
                    "GPIOE",
                    "GPIOF",
                    "GPIOG",
                    "GPIOH",
                ]
                for i in range(count):
                    addr = base_addr_int + i * addr_offset_int
                    define_name = f"{gpio_names[i]}_BASE"
                    padding = " " * (19 - len(define_name))
                    lines.append(
                        f"#define {define_name}{padding}0x{addr:08X}UL"
                    )

                # 添加额外的GPIO端口（保持与Bash版本一致）
                for i in range(count, 8):
                    addr = base_addr_int + i * addr_offset_int
                    define_name = f"{gpio_names[i]}_BASE"
                    padding = " " * (19 - len(define_name))
                    lines.append(
                        f"#define {define_name}{padding}0x{addr:08X}UL"
                    )

                # 添加EXTI基地址
                exti_addr = base_addr_int + 8 * addr_offset_int
                lines.append(f"#define EXTI_BASE           0x{exti_addr:08X}UL")

            elif name in [
                "ADC",
                "DAC",
                "DMA",
                "I2C",
                "I2S",
                "PWM",
                "SDIO",
                "SPI",
                "QSPI",
                "TIMER",
            ]:
                # 标准外设
                for i in range(count):
                    addr = base_addr_int + i * addr_offset_int
                    define_name = f"{name}{i}_BASE"
                    padding = " " * (19 - len(define_name))
                    lines.append(
                        f"#define {define_name}{padding}0x{addr:08X}UL"
                    )
            elif name == "ETH":
                # 以太网特殊处理
                lines.append(
                    f"#define ETH_BASE            0x{base_addr_int:08X}UL"
                )
            elif name == "FLASH":
                # Flash特殊处理
                lines.append(
                    f"#define FLASH_BASE          0x{base_addr_int:08X}UL"
                )
            elif name in ["RNG", "RTC", "USB_OTG", "WDT"]:
                # 其他特殊外设
                for i in range(count):
                    addr = base_addr_int + i * addr_offset_int
                    if count == 1:
                        define_name = f"{name}_BASE"
                    else:
                        define_name = f"{name}{i}_BASE"
                    padding = " " * (19 - len(define_name))
                    lines.append(
                        f"#define {define_name}{padding}0x{addr:08X}UL"
                    )
            else:
                # 通用处理
                for i in range(count):
                    addr = base_addr_int + i * addr_offset_int
                    define_name = f"{name}{i}_BASE"
                    padding = " " * (19 - len(define_name))
                    lines.append(
                        f"#define {define_name}{padding}0x{addr:08X}UL"
                    )

        return "\n".join(lines)

    def generate_vsf_hw_config(self) -> str:
        """
        生成VSF硬件配置

        Returns:
            VSF硬件配置代码字符串
        """
        lines = []

        for peripheral in self.peripherals:
            name = peripheral.get("name", "")
            count = peripheral.get("count", 1)
            base_addr = peripheral.get("base_addr", "0x40000000")
            addr_offset = peripheral.get("addr_offset", "0x400")
            irq_start = peripheral.get("irq_start", 0)
            special_config = peripheral.get("special_config", "")

            hw_config = self._generate_single_vsf_hw_config(
                name, count, base_addr, addr_offset, irq_start, special_config
            )
            lines.append(hw_config)

        return "\n".join(lines)

    def _generate_single_vsf_hw_config(
        self,
        peripheral_name: str,
        count: int,
        base_addr: str,
        addr_offset: str,
        irq_start: int,
        special_config: str,
    ) -> str:
        """
        生成单个外设的VSF硬件配置

        Args:
            peripheral_name: 外设名称
            count: 实例数量
            base_addr: 基地址
            addr_offset: 地址偏移
            irq_start: 中断起始号
            special_config: 特殊配置

        Returns:
            VSF硬件配置代码字符串
        """
        lines = []
        base_addr_int = int(base_addr, 16)
        addr_offset_int = int(addr_offset, 16)

        # 根据外设类型生成不同的配置
        if peripheral_name == "GPIO":
            # GPIO特殊处理
            lines.append("// GPIO")
            lines.append("")
            lines.append(
                f"#define VSF_HW_GPIO_PORT_COUNT                      {count}"
            )
            lines.append(
                f"#define VSF_HW_GPIO_PIN_COUNT                       32"
            )

            # GPIO 中断号定义
            gpio_names = ["GPIOA", "GPIOB", "GPIOC", "GPIOD"]
            for i in range(count):
                lines.append(
                    f"#define VSF_HW_GPIO{i}_IRQn                         {gpio_names[i]}_IRQn"
                )

            # GPIO 中断处理函数名称
            for i in range(count):
                lines.append(
                    f"#define VSF_HW_GPIO{i}_IRQHandler                   {gpio_names[i]}_IRQHandler"
                )

            # GPIO 寄存器指针定义
            for i in range(count):
                lines.append(
                    f"#define VSF_HW_GPIO{i}_REG                          ((void *){gpio_names[i]}_BASE)"
                )

            # GPIO 实例具体配置 - 模板系统必需
            for i in range(count):
                lines.append(
                    f"#define VSF_HW_GPIO{i}_BASE_ADDRESS                 {gpio_names[i]}_BASE"
                )

            for i in range(count):
                lines.append(
                    f"#define VSF_HW_GPIO{i}_IRQ_IDX                      {gpio_names[i]}_IRQn"
                )

            for i in range(count):
                lines.append(
                    f"#define VSF_HW_GPIO{i}_IRQ                          {gpio_names[i]}_IRQHandler"
                )

        else:
            # 其他外设的标准处理
            peripheral_upper = peripheral_name.upper()
            lines.append(f"// {peripheral_upper}")
            lines.append("")

            # 生成COUNT和MASK
            mask = (2**count) - 1
            lines.append(
                f"#define VSF_HW_{peripheral_upper}_COUNT{' ' * (28 - len(peripheral_upper))}{count}"
            )
            lines.append(
                f"#define VSF_HW_{peripheral_upper}_MASK{' ' * (29 - len(peripheral_upper))}0x{mask:02X}    // {self._get_peripheral_comment(peripheral_name, count)}"
            )

            # 生成各实例的配置
            for i in range(count):
                addr = base_addr_int + i * addr_offset_int
                irq = irq_start + i

                # 生成标准的外设配置宏定义（与Bash版本保持一致）
                lines.append(
                    f"#define VSF_HW_{peripheral_name}{i}_IRQN{' ' * (23 - len(peripheral_name) - len(str(i)))}{peripheral_name}{i}_IRQn"
                )
                lines.append(
                    f"#define VSF_HW_{peripheral_name}{i}_IRQHandler{' ' * (15 - len(peripheral_name) - len(str(i)))}{peripheral_name}{i}_IRQHandler"
                )
                lines.append(
                    f"#define VSF_HW_{peripheral_name}{i}_REG{' ' * (24 - len(peripheral_name) - len(str(i)))}((void *){peripheral_name}{i}_BASE)"
                )

                # 处理特殊配置
                if special_config:
                    lines.extend(
                        self._generate_special_config(
                            peripheral_name, i, special_config, irq
                        )
                    )

        lines.append("")  # 添加空行分隔
        return "\n".join(lines)

    def _get_peripheral_comment(self, peripheral_name: str, count: int) -> str:
        """生成外设注释"""
        peripheral_upper = peripheral_name.upper()

        if peripheral_name == "USART":
            return "USART0-USART3, UART0-UART3"
        elif peripheral_name == "TIMER":
            return "TIMER0-TIMER7"
        else:
            if count == 1:
                return f"{peripheral_upper}0"
            elif count == 2:
                return f"{peripheral_upper}0, {peripheral_upper}1"
            else:
                return f"{peripheral_upper}0-{peripheral_upper}{count - 1}"

    def _generate_special_config(
        self,
        peripheral_name: str,
        instance: int,
        special_config: str,
        base_irq: int,
    ) -> List[str]:
        """
        生成特殊配置

        Args:
            peripheral_name: 外设名称
            instance: 实例编号
            special_config: 特殊配置字符串
            base_irq: 基础中断号

        Returns:
            特殊配置代码行列表
        """
        lines = []

        if peripheral_name == "GPIO" and "EXTI" in special_config:
            # GPIO的EXTI配置
            lines.append(
                f"#define VSF_HW_GPIO{instance}_EXTI_IRQ          EXTI{instance}_IRQn"
            )

        elif peripheral_name == "ETH" and "WKUP" in special_config:
            # ETH的唤醒中断配置
            lines.append(
                f"#define VSF_HW_ETH{instance}_WKUP_IRQ          ETH_WKUP_IRQn"
            )

        elif peripheral_name == "RTC" and "ALARM" in special_config:
            # RTC的闹钟中断配置
            lines.append(
                f"#define VSF_HW_RTC{instance}_ALARM_IRQ         RTC_ALARM_IRQn"
            )

        elif peripheral_name == "USB_OTG" and "FS_HS_WKUP" in special_config:
            # USB OTG的特殊配置
            if instance == 0:
                lines.append(
                    f"#define VSF_HW_USB_OTG_FS_BASE              0x{base_irq:08X}"
                )
                lines.append(
                    f"#define VSF_HW_USB_OTG_FS_IRQ               USB_OTG_FS_IRQn"
                )
            elif instance == 1:
                lines.append(
                    f"#define VSF_HW_USB_OTG_HS_BASE              0x{base_irq:08X}"
                )
                lines.append(
                    f"#define VSF_HW_USB_OTG_HS_IRQ               USB_OTG_HS_IRQn"
                )
            lines.append(
                f"#define VSF_HW_USB_OTG_WKUP_IRQ             USB_OTG_WKUP_IRQn"
            )

        elif peripheral_name == "WDT" and "IWDT" in special_config:
            # 看门狗的特殊配置
            if instance == 0:
                lines.append(
                    f"#define VSF_HW_WDT_IRQ                      WDT_IRQn"
                )
            elif instance == 1:
                lines.append(
                    f"#define VSF_HW_IWDT_IRQ                     IWDT_IRQn"
                )

        elif peripheral_name == "USART" and "UART" in special_config:
            # USART/UART混合配置
            if instance < 4:
                lines.append(f"// USART{instance}")
            else:
                uart_idx = instance - 4
                lines.append(f"// UART{uart_idx}")

        return lines

    def generate_interrupt_vectors(self) -> str:
        """
        生成中断向量表

        Returns:
            中断向量表代码字符串
        """
        lines = []

        for peripheral in self.peripherals:
            name = peripheral.get("name", "")
            count = peripheral.get("count", 1)
            irq_start = peripheral.get("irq_start", 0)

            for i in range(count):
                irq_num = irq_start + i
                handler_name = f"{name}{i}"
                lines.append(
                    f"        .word   Default_Handler       /* {irq_num}: {handler_name} */"
                )

        return "\n".join(lines)

    def generate_peripheral_init_functions(self) -> str:
        """
        生成外设初始化函数声明

        Returns:
            外设初始化函数代码字符串
        """
        lines = []

        for peripheral in self.peripherals:
            name = peripheral.get("name", "")
            count = peripheral.get("count", 1)

            # 生成初始化函数声明
            lines.append(f"// {name} 初始化函数")
            for i in range(count):
                func_name = f"vsf_hw_{name.lower()}{i}_init"
                lines.append(f"extern void {func_name}(void);")
            lines.append("")

        return "\n".join(lines)

    def generate_peripheral_register_definitions(self) -> str:
        """
        生成外设寄存器定义

        Returns:
            外设寄存器定义代码字符串
        """
        lines = []

        for peripheral in self.peripherals:
            name = peripheral.get("name", "")
            count = peripheral.get("count", 1)

            # 生成寄存器类型定义
            lines.append(f"// {name} 寄存器类型定义")
            lines.append(f"typedef struct {{")
            lines.append(f"    // TODO: 添加 {name} 寄存器定义")
            lines.append(f"    volatile uint32_t RESERVED[64];")
            lines.append(f"}} {name.lower()}_reg_t;")
            lines.append("")

            # 生成寄存器实例定义
            for i in range(count):
                instance_name = f"{name.upper()}{i}"
                base_name = f"{name}{i}_BASE"
                lines.append(
                    f"#define {instance_name}                     (({name.lower()}_reg_t *){base_name})"
                )
            lines.append("")

        return "\n".join(lines)
