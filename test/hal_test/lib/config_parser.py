#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
VSF HAL Driver Template Test - Configuration Parser
配置文件解析器模块
"""

import yaml
import logging
from pathlib import Path
from typing import Dict, List, Any, Optional

logger = logging.getLogger(__name__)


class ConfigParser:
    """配置文件解析器类"""

    def __init__(self, config_file: str):
        """
        初始化配置解析器

        Args:
            config_file: 配置文件路径
        """
        self.config_file = Path(config_file)
        self.config_data: Dict[str, Any] = {}
        self.peripherals: List[Dict[str, Any]] = []

    def parse(self) -> Dict[str, Any]:
        """
        解析YAML配置文件

        Returns:
            解析后的配置数据

        Raises:
            FileNotFoundError: 配置文件不存在
            yaml.YAMLError: YAML格式错误
        """
        if not self.config_file.exists():
            raise FileNotFoundError(f"配置文件不存在: {self.config_file}")

        logger.info(f"解析配置文件: {self.config_file}")

        try:
            with open(self.config_file, "r", encoding="utf-8") as f:
                raw_config = yaml.safe_load(f)

            self.config_data = self._process_config(raw_config)
            logger.info("配置文件解析完成")
            return self.config_data

        except yaml.YAMLError as e:
            logger.error(f"YAML格式错误: {e}")
            raise
        except Exception as e:
            logger.error(f"配置文件解析失败: {e}")
            raise

    def _process_config(self, raw_config: Dict[str, Any]) -> Dict[str, Any]:
        """
        处理原始配置数据

        Args:
            raw_config: 原始配置数据

        Returns:
            处理后的配置数据
        """
        config = {}

        # 处理基本测试参数
        test_params = raw_config.get("test_params", {})
        config.update(
            {
                "VENDOR": test_params.get("vendor", ""),
                "SERIES": test_params.get("series", ""),
                "DEVICE": test_params.get("device", ""),
                "CHIP_NAME": test_params.get("chip_name", ""),
                "VENDOR_UPPER": test_params.get("vendor", "").upper(),
                "SERIES_UPPER": test_params.get("series", "").upper(),
                "DEVICE_UPPER": test_params.get("device", "").upper(),
                "CHIP_NAME_UPPER": test_params.get("chip_name", "").upper(),
                "DEVICE_LOWER": test_params.get("device", "").lower(),
            }
        )

        # 处理内存配置
        memory_config = raw_config.get("memory_config", {})
        config.update(
            {
                "HEAP_SIZE": memory_config.get("heap_size", "0x2000"),
                "SYSTIMER_FREQ": memory_config.get(
                    "systimer_freq", "48000000ul"
                ),
                "FLASH_ORIGIN": memory_config.get("flash_origin", "0x08000000"),
                "FLASH_LENGTH": memory_config.get("flash_length", "512K"),
                "RAM_ORIGIN": memory_config.get("ram_origin", "0x20000000"),
                "RAM_LENGTH": memory_config.get("ram_length", "128K"),
                "HEAP_SIZE_BYTES": memory_config.get(
                    "heap_size_bytes", "0x1000"
                ),
                "STACK_SIZE_BYTES": memory_config.get(
                    "stack_size_bytes", "0x800"
                ),
            }
        )

        # 计算注释
        heap_bytes = int(memory_config.get("heap_size_bytes", "0x1000"), 16)
        stack_bytes = int(memory_config.get("stack_size_bytes", "0x800"), 16)
        config.update(
            {
                "HEAP_SIZE_COMMENT": f"{heap_bytes // 1024}KB",
                "STACK_SIZE_COMMENT": f"{stack_bytes // 1024}KB",
            }
        )

        # 处理CMSIS配置
        cmsis_config = raw_config.get("cmsis_config", {})
        config.update(
            {
                "CM4_REV": cmsis_config.get("cm4_rev", "0x0001U"),
                "MPU_PRESENT": cmsis_config.get("mpu_present", "1U"),
                "NVIC_PRIO_BITS": cmsis_config.get("nvic_prio_bits", "4U"),
                "VENDOR_SYSTICK_CONFIG": cmsis_config.get(
                    "vendor_systick_config", "0U"
                ),
                "FPU_PRESENT": cmsis_config.get("fpu_present", "1U"),
                "VTOR_PRESENT": cmsis_config.get("vtor_present", "1U"),
                "ARCH_PRI_NUM": cmsis_config.get("arch_pri_num", 64),
                "ARCH_PRI_BIT": cmsis_config.get("arch_pri_bit", 6),
            }
        )

        # 处理外设配置
        self.peripherals = raw_config.get("peripherals", [])
        config["PERIPHERALS"] = self.peripherals

        # 处理禁用配置
        build_config = raw_config.get("build_config", {})
        config.update(self._process_disabled_config(build_config))

        # 处理模板配置
        template_config = raw_config.get("template_config", {})
        config["template_config"] = template_config

        return config

    def _process_disabled_config(
        self, build_config: Dict[str, Any]
    ) -> Dict[str, str]:
        """
        处理禁用的组件和demo配置

        Args:
            build_config: 构建配置

        Returns:
            处理后的禁用配置
        """
        result = {}

        # 处理禁用的组件
        disabled_components = build_config.get("disabled_components", [])
        components_text = "\n".join(
            [f"set({comp} 0)" for comp in disabled_components]
        )
        result["DISABLED_COMPONENTS"] = components_text

        # 处理禁用的demo
        disabled_demos = build_config.get("disabled_demos", [])
        demos_text = "\n".join(
            [f"#define {demo:<40}DISABLED" for demo in disabled_demos]
        )
        result["DISABLED_DEMOS"] = demos_text

        return result

    def get_peripheral_by_name(self, name: str) -> Optional[Dict[str, Any]]:
        """
        根据名称获取外设配置

        Args:
            name: 外设名称

        Returns:
            外设配置字典，如果不存在返回None
        """
        for peripheral in self.peripherals:
            if peripheral.get("name") == name:
                return peripheral
        return None

    def get_all_peripherals(self) -> List[Dict[str, Any]]:
        """
        获取所有外设配置

        Returns:
            外设配置列表
        """
        return self.peripherals.copy()

    def show_config_summary(self):
        """显示配置摘要"""
        logger.info("配置摘要：")
        logger.info(f"  厂商: {self.config_data.get('VENDOR')}")
        logger.info(f"  系列: {self.config_data.get('SERIES')}")
        logger.info(f"  设备: {self.config_data.get('DEVICE')}")
        logger.info(f"  芯片名称: {self.config_data.get('CHIP_NAME')}")
        logger.info(f"  外设数量: {len(self.peripherals)}")
        logger.info(f"  堆大小: {self.config_data.get('HEAP_SIZE')}")
        logger.info(
            f"  系统定时器频率: {self.config_data.get('SYSTIMER_FREQ')}"
        )
