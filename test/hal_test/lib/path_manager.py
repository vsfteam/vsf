#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
路径管理器 - 统一管理项目中的路径拼接
Path Manager - Centralized path management for the project
"""

from pathlib import Path
from typing import Dict, Any


class PathManager:
    """路径管理器 - 统一管理所有项目路径"""

    def __init__(self, project_root: Path):
        """
        初始化路径管理器

        Args:
            project_root: 项目根目录
        """
        self.project_root = Path(project_root)
        self._validate_project_root()

    def _validate_project_root(self):
        """验证项目根目录的有效性"""
        if not self.project_root.exists():
            raise ValueError(f"项目根目录不存在: {self.project_root}")

        # 检查是否为VSF项目根目录
        vsf_marker_files = ["source", "example", "script"]
        missing_markers = [
            marker for marker in vsf_marker_files
            if not (self.project_root / marker).exists()
        ]

        if missing_markers:
            raise ValueError(
                f"不是有效的VSF项目根目录，缺少目录: {missing_markers}"
            )

    # ==================== 基础目录属性 ====================

    @property
    def source_dir(self) -> Path:
        """源码目录"""
        return self.project_root / "source"

    @property
    def example_dir(self) -> Path:
        """示例目录"""
        return self.project_root / "example"

    @property
    def script_dir(self) -> Path:
        """脚本目录"""
        return self.project_root / "script"

    @property
    def hal_driver_dir(self) -> Path:
        """HAL驱动目录"""
        return self.source_dir / "hal" / "driver"

    @property
    def template_project_dir(self) -> Path:
        """模板项目目录"""
        return self.example_dir / "template" / "project"

    @property
    def cmake_targets_dir(self) -> Path:
        """CMake目标配置目录"""
        return self.script_dir / "cmake" / "targets"

    @property
    def template_config_dir(self) -> Path:
        """模板配置目录"""
        return self.example_dir / "template" / "config"

    # ==================== HAL驱动相关路径 ====================

    def get_hal_driver_template_dir(self) -> Path:
        """获取HAL驱动模板目录"""
        # 使用原始的HAL驱动模板进行测试
        return self.hal_driver_dir / "template" / "__series_name_a__"

    def get_vendor_driver_dir(self, vendor: str) -> Path:
        """
        获取厂商驱动目录

        Args:
            vendor: 厂商名称

        Returns:
            厂商驱动目录路径
        """
        return self.hal_driver_dir / vendor

    def get_series_driver_dir(self, vendor: str, series: str) -> Path:
        """
        获取系列驱动目录

        Args:
            vendor: 厂商名称
            series: 系列名称

        Returns:
            系列驱动目录路径
        """
        return self.get_vendor_driver_dir(vendor) / series

    def get_device_driver_dir(
        self, vendor: str, series: str, device: str
    ) -> Path:
        """
        获取设备驱动目录

        Args:
            vendor: 厂商名称
            series: 系列名称
            device: 设备名称

        Returns:
            设备驱动目录路径
        """
        return self.get_series_driver_dir(vendor, series) / device

    def get_hal_main_driver_header(self) -> Path:
        """获取主驱动头文件路径"""
        return self.hal_driver_dir / "driver.h"

    # ==================== 项目相关路径 ====================

    def get_cmake_project_dir(self, chip_name: str) -> Path:
        """
        获取CMake项目目录

        Args:
            chip_name: 芯片名称

        Returns:
            CMake项目目录路径
        """
        return self.template_project_dir / "cmake" / chip_name

    def get_aic8800_template_project_dir(self) -> Path:
        """获取通用项目模板目录（已废弃，现在完全使用模板生成）"""
        # 此方法已废弃，现在完全使用模板生成项目文件
        raise NotImplementedError("此方法已废弃，现在完全使用模板生成项目文件")

    def get_cmake_target_dir(self, vendor: str, series: str) -> Path:
        """
        获取CMake目标配置目录

        Args:
            vendor: 厂商名称
            series: 系列名称

        Returns:
            CMake目标配置目录路径
        """
        return self.cmake_targets_dir / vendor / series

    def get_cmake_target_file(
        self, vendor: str, series: str, chip_name_upper: str
    ) -> Path:
        """
        获取CMake目标配置文件

        Args:
            vendor: 厂商名称
            series: 系列名称
            chip_name_upper: 芯片名称（大写）

        Returns:
            CMake目标配置文件路径
        """
        target_dir = self.get_cmake_target_dir(vendor, series)
        return target_dir / f"{chip_name_upper}.cmake"

    # ==================== 配置文件相关路径 ====================

    def get_vsf_usr_cfg_dir(self) -> Path:
        """获取VSF用户配置目录"""
        return self.template_config_dir / "vsf_usr_cfg"

    def get_test_config_file(self, device_lower: str) -> Path:
        """
        获取测试配置文件路径

        Args:
            device_lower: 设备名称（小写）

        Returns:
            测试配置文件路径
        """
        return (
            self.get_vsf_usr_cfg_dir() / f"vsf_usr_cfg_{device_lower}.h"
        )

    # ==================== 设备特定文件路径 ====================

    def get_startup_file(self, device_dir: Path, device_lower: str) -> Path:
        """
        获取启动文件路径

        Args:
            device_dir: 设备目录
            device_lower: 设备名称（小写）

        Returns:
            启动文件路径
        """
        return device_dir / f"startup_{device_lower}.S"

    def get_device_header_file(self, device_dir: Path) -> Path:
        """
        获取设备头文件路径

        Args:
            device_dir: 设备目录

        Returns:
            设备头文件路径
        """
        return device_dir / "device.h"

    def get_driver_c_file(self, device_dir: Path) -> Path:
        """
        获取driver.c文件路径

        Args:
            device_dir: 设备目录

        Returns:
            driver.c文件路径
        """
        return device_dir / "driver.c"

    # ==================== 通用外设文件路径 ====================

    def get_common_peripheral_files(
        self, series_dir: Path
    ) -> Dict[str, Path]:
        """
        获取通用外设文件路径

        Args:
            series_dir: 系列目录

        Returns:
            外设文件路径字典
        """
        common_dir = series_dir / "common"
        return {
            "gpio": common_dir / "gpio" / "gpio.h",
            "i2c": common_dir / "i2c" / "i2c.h",
            "spi": common_dir / "spi" / "spi.h",
            "uart": common_dir / "uart" / "uart.h",
            "adc": common_dir / "adc" / "adc.h",
            "timer": common_dir / "timer" / "timer.h",
        }

    # ==================== 构建清理相关路径 ====================

    def get_build_cleanup_paths(self) -> list[Path]:
        """
        获取构建清理路径列表

        Returns:
            需要清理的构建文件路径列表
        """
        return [
            self.project_root / "CMakeFiles",
            self.project_root / "CMakeCache.txt",
            self.project_root / "cmake_install.cmake",
            self.project_root / "Makefile",
            self.project_root / "build",
        ]

    # ==================== 路径验证和创建 ====================

    def ensure_directory_exists(self, path: Path) -> bool:
        """
        确保目录存在，如果不存在则创建

        Args:
            path: 目录路径

        Returns:
            成功返回True，失败返回False
        """
        try:
            path.mkdir(parents=True, exist_ok=True)
            return True
        except Exception:
            return False

    def get_relative_path(self, path: Path) -> Path:
        """
        获取相对于项目根目录的相对路径

        Args:
            path: 绝对路径

        Returns:
            相对路径
        """
        try:
            return path.relative_to(self.project_root)
        except ValueError:
            return path

    # ==================== 配置数据路径生成 ====================

    def generate_paths_from_config(
        self, config_data: Dict[str, Any]
    ) -> Dict[str, Path]:
        """
        根据配置数据生成常用路径

        Args:
            config_data: 配置数据字典

        Returns:
            路径字典
        """
        vendor = config_data.get('VENDOR', '')
        series = config_data.get('SERIES', '')
        device = config_data.get('DEVICE', '')
        device_lower = config_data.get('DEVICE_LOWER', '')
        chip_name = config_data.get('CHIP_NAME', '')
        chip_name_upper = config_data.get('CHIP_NAME_UPPER', '')

        paths = {
            # 驱动相关路径
            'vendor_driver_dir': self.get_vendor_driver_dir(vendor),
            'series_driver_dir': self.get_series_driver_dir(vendor, series),
            'device_driver_dir': self.get_device_driver_dir(
                vendor, series, device
            ),
            'hal_template_dir': self.get_hal_driver_template_dir(),

            # 项目相关路径
            'cmake_project_dir': self.get_cmake_project_dir(chip_name),
            'aic8800_template_dir': self.get_aic8800_template_project_dir(),
            'cmake_target_dir': self.get_cmake_target_dir(vendor, series),
            'cmake_target_file': self.get_cmake_target_file(
                vendor, series, chip_name_upper
            ),

            # 配置文件路径
            'test_config_file': self.get_test_config_file(device_lower),
            'vsf_usr_cfg_dir': self.get_vsf_usr_cfg_dir(),

            # 主要文件路径
            'hal_main_driver_header': self.get_hal_main_driver_header(),
        }

        return paths

    def __str__(self) -> str:
        """字符串表示"""
        return f"PathManager(project_root={self.project_root})"

    def __repr__(self) -> str:
        """调试表示"""
        return self.__str__()
