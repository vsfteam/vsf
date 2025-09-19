#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
VSF HAL Driver Template Test - Template Processor
模板处理器模块
"""

import logging
import tempfile
from pathlib import Path
from typing import Dict, Any, List, Optional
from jinja2 import Template, Environment, FileSystemLoader, TemplateError

logger = logging.getLogger(__name__)


class TemplateProcessor:
    """模板处理器类"""

    def __init__(self, config_data: Dict[str, Any], template_dir: str):
        """
        初始化模板处理器

        Args:
            config_data: 配置数据
            template_dir: 模板目录路径
        """
        self.config_data = config_data
        self.template_dir = Path(template_dir)

        # 设置Jinja2环境
        self.jinja_env = Environment(
            loader=FileSystemLoader(str(self.template_dir)),
            trim_blocks=True,
            lstrip_blocks=True,
        )

        # 添加自定义过滤器
        self.jinja_env.filters["upper"] = str.upper
        self.jinja_env.filters["lower"] = str.lower
        self.jinja_env.filters["hex_format"] = self._hex_format_filter

    def _hex_format_filter(self, value: str, width: int = 8) -> str:
        """
        十六进制格式化过滤器

        Args:
            value: 十六进制字符串
            width: 格式化宽度

        Returns:
            格式化后的十六进制字符串
        """
        if isinstance(value, str) and value.startswith("0x"):
            hex_val = int(value, 16)
            return f"0x{hex_val:0{width}X}UL"
        return value

    def process_template(
        self,
        template_file: str,
        output_file: str,
        additional_context: Optional[Dict[str, Any]] = None,
    ) -> bool:
        """
        处理单个模板文件

        Args:
            template_file: 模板文件路径
            output_file: 输出文件路径
            additional_context: 额外的模板上下文

        Returns:
            处理成功返回True，失败返回False
        """
        template_path = Path(template_file)
        output_path = Path(output_file)

        if not template_path.exists():
            logger.error(f"模板文件不存在: {template_file}")
            return False

        logger.info(f"处理模板: {template_path.name} -> {output_path.name}")

        try:
            # 准备模板上下文
            context = self.config_data.copy()
            if additional_context:
                context.update(additional_context)

            # 读取并处理模板
            with open(template_path, "r", encoding="utf-8") as f:
                template_content = f.read()

            # 使用简单的字符串替换处理 {{}} 格式的变量
            processed_content = self._process_simple_template(
                template_content, context
            )

            # 确保输出目录存在
            output_path.parent.mkdir(parents=True, exist_ok=True)

            # 写入输出文件
            with open(output_path, "w", encoding="utf-8") as f:
                f.write(processed_content)

            logger.info(f"模板处理完成: {output_file}")
            return True

        except Exception as e:
            logger.error(f"模板处理失败: {e}")
            return False

    def _process_simple_template(
        self, content: str, context: Dict[str, Any]
    ) -> str:
        """
        处理简单的模板替换（兼容原有的{{}}格式）

        Args:
            content: 模板内容
            context: 替换上下文

        Returns:
            处理后的内容
        """
        processed = content

        # 处理基本变量替换
        for key, value in context.items():
            if isinstance(value, (str, int)):
                processed = processed.replace(f"{{{{{key}}}}}", str(value))

        # 处理特殊的多行内容替换
        if (
            "DISABLED_COMPONENTS" in context
            and "{{DISABLED_COMPONENTS}}" in processed
        ):
            processed = processed.replace(
                "{{DISABLED_COMPONENTS}}", context["DISABLED_COMPONENTS"]
            )

        if "DISABLED_DEMOS" in context and "{{DISABLED_DEMOS}}" in processed:
            processed = processed.replace(
                "{{DISABLED_DEMOS}}", context["DISABLED_DEMOS"]
            )

        return processed

    def process_special_template(
        self, template_type: str, template_file: str, output_file: str
    ) -> bool:
        """
        处理需要动态生成内容的特殊模板

        Args:
            template_type: 模板类型 ('device_h', 'startup_s')
            template_file: 模板文件路径
            output_file: 输出文件路径

        Returns:
            处理成功返回True，失败返回False
        """
        if template_type == "device_h":
            return self._process_device_header_template(
                template_file, output_file
            )
        elif template_type == "startup_s":
            return self._process_startup_template(template_file, output_file)
        else:
            # 对于其他模板，使用标准处理
            return self.process_template(template_file, output_file)

    def _process_device_header_template(
        self, template_file: str, output_file: str
    ) -> bool:
        """
        处理设备头文件模板

        Args:
            template_file: 模板文件路径
            output_file: 输出文件路径

        Returns:
            处理成功返回True，失败返回False
        """
        from .peripheral_generator import PeripheralGenerator

        # 生成外设相关的动态内容
        generator = PeripheralGenerator(self.config_data)

        additional_context = {
            "IRQ_ENUM": generator.generate_irq_enum(),
            "REGISTER_BASE_ADDRESSES": generator.generate_register_base_addresses(),
            "VSF_HW_CONFIG": generator.generate_vsf_hw_config(),
        }

        return self.process_template(
            template_file, output_file, additional_context
        )

    def _process_startup_template(
        self, template_file: str, output_file: str
    ) -> bool:
        """
        处理启动文件模板

        Args:
            template_file: 模板文件路径
            output_file: 输出文件路径

        Returns:
            处理成功返回True，失败返回False
        """
        from .peripheral_generator import PeripheralGenerator

        # 生成中断向量表
        generator = PeripheralGenerator(self.config_data)

        additional_context = {
            "INTERRUPT_VECTORS": generator.generate_interrupt_vectors(),
        }

        return self.process_template(
            template_file, output_file, additional_context
        )

    def process_all_templates(
        self, template_mappings: List[Dict[str, str]]
    ) -> bool:
        """
        批量处理模板文件

        Args:
            template_mappings: 模板映射列表，每个元素包含 'template', 'output', 'type'(可选)

        Returns:
            全部处理成功返回True，有失败返回False
        """
        success = True

        for mapping in template_mappings:
            template_file = mapping["template"]
            output_file = mapping["output"]
            template_type = mapping.get("type", "normal")

            if template_type in ["device_h", "startup_s"]:
                result = self.process_special_template(
                    template_type, template_file, output_file
                )
            else:
                result = self.process_template(template_file, output_file)

            if not result:
                success = False
                logger.error(f"模板处理失败: {template_file}")

        return success

    def verify_template_processing(self, output_files: List[str]) -> bool:
        """
        验证模板处理结果

        Args:
            output_files: 输出文件列表

        Returns:
            验证通过返回True，失败返回False
        """
        logger.info("验证模板处理结果...")

        missing_files = []
        template_vars_remaining = []

        # 检查文件是否存在
        for file_path in output_files:
            path = Path(file_path)
            if not path.exists():
                missing_files.append(file_path)

        if missing_files:
            logger.error("缺少以下文件:")
            for file_path in missing_files:
                logger.error(f"  - {file_path}")
            return False

        # 检查是否还有未替换的模板变量
        template_patterns = [
            "${VENDOR}",
            "${SERIES}",
            "${DEVICE}",
            "__SERIES_NAME_A__",
            "__DEVICE_NAME_A__",
            "{{VENDOR}}",
            "{{SERIES}}",
            "{{DEVICE}}",
        ]

        for file_path in output_files:
            try:
                with open(file_path, "r", encoding="utf-8") as f:
                    content = f.read()

                for pattern in template_patterns:
                    if pattern in content:
                        template_vars_remaining.append((file_path, pattern))
            except Exception as e:
                logger.warning(f"无法检查文件 {file_path}: {e}")

        if template_vars_remaining:
            logger.warning("仍有未替换的模板变量:")
            for file_path, pattern in template_vars_remaining:
                logger.warning(f"  - {pattern} in {file_path}")
        else:
            logger.info("所有模板变量都已替换")

        logger.info("模板处理结果验证完成")
        return True
