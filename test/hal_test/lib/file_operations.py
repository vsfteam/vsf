#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
VSF HAL Driver Template Test - File Operations
文件操作模块
"""

import shutil
import logging
import re
from pathlib import Path
from typing import Dict, Any, List

from .path_manager import PathManager

logger = logging.getLogger(__name__)


class FileOperations:
    """文件操作类"""

    def __init__(self, config_data: Dict[str, Any], project_root: str):
        """
        初始化文件操作器

        Args:
            config_data: 配置数据
            project_root: 项目根目录
        """
        self.config_data = config_data
        self.project_root = Path(project_root)
        self.path_manager = PathManager(self.project_root)

    def cleanup_previous_test_files(self) -> bool:
        """
        清理之前的测试文件

        Returns:
            清理成功返回True，失败返回False
        """
        logger.info("清理之前的测试文件（如果存在）...")

        try:
            vendor = self.config_data["VENDOR"]
            chip_name = self.config_data["CHIP_NAME"]
            device_lower = self.config_data["DEVICE_LOWER"]
            vendor_upper = self.config_data["VENDOR_UPPER"]
            device_upper = self.config_data["DEVICE_UPPER"]

            # 删除之前的测试目录
            test_project_dir = self.path_manager.get_cmake_project_dir(
                chip_name
            )
            if test_project_dir.exists():
                shutil.rmtree(test_project_dir)
                logger.info(
                    f"删除之前的测试项目目录: "
                    f"{self.path_manager.get_relative_path(test_project_dir)}"
                )

            # 删除之前的驱动目录
            driver_dir = self.path_manager.get_vendor_driver_dir(vendor)
            if driver_dir.exists():
                shutil.rmtree(driver_dir)
                logger.info(
                    f"删除之前的测试驱动目录: "
                    f"{self.path_manager.get_relative_path(driver_dir)}"
                )

            # 删除之前的目标配置
            target_dir = self.path_manager.cmake_targets_dir / vendor
            if target_dir.exists():
                shutil.rmtree(target_dir)
                logger.info(
                    f"删除之前的测试目标配置: "
                    f"{self.path_manager.get_relative_path(target_dir)}"
                )

            # 删除之前的构建目录
            build_dir = self.project_root / "build_test"
            if build_dir.exists():
                shutil.rmtree(build_dir)
                logger.info("删除之前的构建测试目录: build_test")

            # 删除测试配置文件
            config_file = self.path_manager.get_test_config_file(device_lower)
            if config_file.exists():
                config_file.unlink()
                logger.info(f"删除测试配置文件: vsf_usr_cfg_{device_lower}.h")

            # 清理主驱动头文件中的测试厂商定义
            self._cleanup_driver_header(vendor_upper)

            # 清理主配置文件中的测试设备定义
            self._cleanup_main_config(device_upper, device_lower)

            logger.info("清理完成")
            return True

        except Exception as e:
            logger.error(f"清理失败: {e}")
            return False

    def _cleanup_driver_header(self, vendor_upper: str):
        """
        清理主驱动头文件中的测试定义

        Args:
            vendor_upper: 大写厂商名称
        """
        driver_header = self.path_manager.get_hal_main_driver_header()
        if not driver_header.exists():
            return

        try:
            with open(driver_header, "r", encoding="utf-8") as f:
                content = f.read()

            # 删除之前添加的测试厂商定义
            pattern = (
                rf"^#   elif    defined\(__{re.escape(vendor_upper)}__\)\n"
                rf'#       define  VSF_DRIVER_HEADER.*'
                rf'{re.escape(vendor_upper.lower())}/driver\.h".*\n'
            )
            content = re.sub(pattern, "", content, flags=re.MULTILINE)

            with open(driver_header, "w", encoding="utf-8") as f:
                f.write(content)

            logger.info("清理主驱动头文件中的测试定义")

        except Exception as e:
            logger.warning(f"清理主驱动头文件失败: {e}")

    def _cleanup_main_config(self, device_upper: str, device_lower: str):
        """
        清理主配置文件中的测试设备定义

        Args:
            device_upper: 大写设备名称
            device_lower: 小写设备名称
        """
        main_config = (
            self.project_root
            / "example"
            / "template"
            / "config"
            / "vsf_usr_cfg.h"
        )
        if not main_config.exists():
            return

        try:
            with open(main_config, "r", encoding="utf-8") as f:
                content = f.read()

            # 删除之前添加的测试设备定义
            pattern = (
                rf"^#elif   defined\(__{re.escape(device_upper)}__\)\n"
                rf'#   include.*vsf_usr_cfg_{re.escape(device_lower)}\.h".*\n'
            )
            content = re.sub(pattern, "", content, flags=re.MULTILINE)

            with open(main_config, "w", encoding="utf-8") as f:
                f.write(content)

            logger.info("清理主配置文件中的测试定义")

        except Exception as e:
            logger.warning(f"清理主配置文件失败: {e}")

    def copy_directory(self, src: str, dst: str) -> bool:
        """
        复制目录

        Args:
            src: 源目录路径
            dst: 目标目录路径

        Returns:
            复制成功返回True，失败返回False
        """
        src_path = Path(src)
        dst_path = Path(dst)

        if not src_path.exists():
            logger.error(f"源目录不存在: {src}")
            return False

        try:
            if dst_path.exists():
                shutil.rmtree(dst_path)

            shutil.copytree(src_path, dst_path)
            logger.info(f"复制目录: {src_path.name} -> {dst_path.name}")
            return True

        except Exception as e:
            logger.error(f"复制目录失败: {e}")
            return False

    def rename_directory(
        self, old_name: str, new_name: str, parent_dir: str
    ) -> bool:
        """
        重命名目录

        Args:
            old_name: 旧目录名
            new_name: 新目录名
            parent_dir: 父目录路径

        Returns:
            重命名成功返回True，失败返回False
        """
        parent_path = Path(parent_dir)
        old_path = parent_path / old_name
        new_path = parent_path / new_name

        if not old_path.exists():
            logger.warning(f"要重命名的目录不存在: {old_path}")
            return False

        try:
            old_path.rename(new_path)
            logger.info(f"重命名目录: {old_name} -> {new_name}")
            return True

        except Exception as e:
            logger.error(f"重命名目录失败: {e}")
            return False

    def create_directory(self, dir_path: str) -> bool:
        """
        创建目录

        Args:
            dir_path: 目录路径

        Returns:
            创建成功返回True，失败返回False
        """
        try:
            Path(dir_path).mkdir(parents=True, exist_ok=True)
            logger.info(f"创建目录: {dir_path}")
            return True

        except Exception as e:
            logger.error(f"创建目录失败: {e}")
            return False

    def update_main_driver_header(self) -> bool:
        """
        更新主驱动头文件

        Returns:
            更新成功返回True，失败返回False
        """
        logger.info("更新主驱动头文件...")

        driver_header = self.path_manager.get_hal_main_driver_header()
        if not driver_header.exists():
            logger.error(f"主驱动头文件不存在: {driver_header}")
            return False

        try:
            vendor_upper = self.config_data["VENDOR_UPPER"]
            vendor_name = self.config_data["VENDOR"]  # 保持原始大小写

            with open(driver_header, "r", encoding="utf-8") as f:
                content = f.read()

            # 在示例注释之前添加新的厂商定义
            new_definition = (
                f"#   elif    defined(__{vendor_upper}__)\n"
                f'#       define  VSF_DRIVER_HEADER       "'
                f'./{vendor_name}/driver.h"\n'
            )

            # 查找插入位置（在 /* example 之前）
            insert_pattern = r"(/\* example)"
            if re.search(insert_pattern, content):
                content = re.sub(
                    insert_pattern, new_definition + r"\1", content
                )
            else:
                # 如果找不到 /* example，在文件末尾添加
                content += "\n" + new_definition

            with open(driver_header, "w", encoding="utf-8") as f:
                f.write(content)

            logger.info(
                f"已更新主驱动头文件，添加了 "
                f"{self.config_data['VENDOR']} 厂商定义"
            )
            return True

        except Exception as e:
            logger.error(f"更新主驱动头文件失败: {e}")
            return False

    def update_main_config_file(self) -> bool:
        """
        更新主配置文件

        Returns:
            更新成功返回True，失败返回False
        """
        logger.info("更新主配置文件...")

        main_config = (
            self.project_root
            / "example"
            / "template"
            / "config"
            / "vsf_usr_cfg.h"
        )
        if not main_config.exists():
            logger.error(f"主配置文件不存在: {main_config}")
            return False

        try:
            device_upper = self.config_data["DEVICE_UPPER"]
            device_lower = self.config_data["DEVICE_LOWER"]

            with open(main_config, "r", encoding="utf-8") as f:
                content = f.read()

            # 在AIC8800条件之前添加新的设备条件
            new_condition = (
                f"#elif   defined(__{device_upper}__)\n"
                f'#   include "./vsf_usr_cfg/vsf_usr_cfg_{device_lower}.h"\n'
            )

            # 查找插入位置（在 #elif   defined(__AIC8800__) 之前）
            insert_pattern = r"(#elif   defined\(__AIC8800__\))"
            if re.search(insert_pattern, content):
                content = re.sub(insert_pattern, new_condition + r"\1", content)
            else:
                # 如果找不到AIC8800，在文件末尾添加
                content += "\n" + new_condition

            with open(main_config, "w", encoding="utf-8") as f:
                f.write(content)

            logger.info(
                f"已更新主配置文件，添加了 "
                f"{self.config_data['DEVICE']} 设备定义"
            )
            return True

        except Exception as e:
            logger.error(f"更新主配置文件失败: {e}")
            return False

    def replace_template_variables_in_files(
        self, file_paths: List[str]
    ) -> bool:
        """
        批量替换文件中的模板变量

        Args:
            file_paths: 文件路径列表

        Returns:
            替换成功返回True，失败返回False
        """
        logger.info("替换文件中的模板变量...")

        success = True

        # 定义替换规则
        replacements = {
            "${VENDOR}": self.config_data["VENDOR_UPPER"],
            "${SERIES}": self.config_data["SERIES_UPPER"],
            "${DEVICE}": self.config_data["DEVICE_UPPER"],
            "__SERIES_NAME_A__": self.config_data["SERIES_UPPER"],
            "__DEVICE_NAME_A__": self.config_data["DEVICE"],
        }

        # 处理外设相关的IP变量
        peripherals = [
            "GPIO",
            "I2C",
            "I2S",
            "SPI",
            "USART",
            "TIMER",
            "ADC",
            "DAC",
            "DMA",
            "PWM",
            "RTC",
            "RNG",
            "WDT",
            "FLASH",
            "ETH",
            "SDIO",
        ]
        for peripheral in peripherals:
            replacements[
                f"${{{self.config_data['SERIES_UPPER']}/{peripheral}_IP}}"
            ] = self.config_data["SERIES_UPPER"]
            replacements[f"${{{peripheral}_IP}}"] = self.config_data[
                "SERIES_UPPER"
            ]
            replacements[f"${{{peripheral.lower()}_ip}}"] = self.config_data[
                "SERIES"
            ].lower()

        for file_path in file_paths:
            if not self._replace_variables_in_file(file_path, replacements):
                success = False

        return success

    def _replace_variables_in_file(
        self, file_path: str, replacements: Dict[str, str]
    ) -> bool:
        """
        替换单个文件中的变量

        Args:
            file_path: 文件路径
            replacements: 替换规则字典

        Returns:
            替换成功返回True，失败返回False
        """
        path = Path(file_path)
        if not path.exists():
            logger.warning(f"文件不存在: {file_path}")
            return False

        try:
            with open(path, "r", encoding="utf-8") as f:
                content = f.read()

            # 执行替换
            for old_value, new_value in replacements.items():
                content = content.replace(old_value, new_value)

            with open(path, "w", encoding="utf-8") as f:
                f.write(content)

            return True

        except Exception as e:
            logger.error(f"替换文件变量失败 {file_path}: {e}")
            return False
