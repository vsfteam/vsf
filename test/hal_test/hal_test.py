#!/usr/bin/env python3
"""
VSF HAL Driver Test Script - Pythonic Version
VSF HAL驱动测试脚本 - Python化重构版本

用于测试HAL驱动结构的正确性，通过复制现成的模板文件来创建测试驱动结构
"""

import logging
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional, Tuple

logger = logging.getLogger(__name__)


@dataclass
class TestConfig:
    """测试配置类 - 集中管理所有配置参数"""

    # 基本参数
    vendor: str = "FAKE_VENDOR"
    series: str = "FAKE_SERIES"
    device: str = "FAKE_DEVICE"

    # 文件处理规则
    device_header_pattern: str = "__device.h"
    search_patterns: List[str] = field(default_factory=lambda: ["*.h", "*.c"])
    exclude_files: List[str] = field(default_factory=lambda: ["__device.h"])
    ipcore_start_marker: str = "// IPCore"
    ipcore_end_marker: str = "// IPCore end"

    # 构建配置
    default_generator: str = "Ninja"

    # 支持的外设列表
    peripherals: List[str] = field(
        default_factory=lambda: [
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
    )

    def __post_init__(self):
        """初始化后设置路径"""
        self.project_root = Path(__file__).parent.parent.parent
        self._setup_paths()

    def _setup_paths(self):
        """设置所有路径"""
        # HAL驱动相关路径
        self.hal_driver_template_dir = self.project_root / "source/hal/driver/template"
        self.hal_main_driver_header = self.project_root / "source/hal/driver/driver.h"

        # 模板文件路径
        self.template_series_dir = self.hal_driver_template_dir / "__series_name_a__"
        self.template_device_dir = self.template_series_dir / "__device_name_a__"
        self.template_device_h_file = self.template_device_dir / "device.h"

        # 测试驱动路径
        self.vendor_driver_dir = self.project_root / f"source/hal/driver/{self.vendor}"
        self.series_driver_dir = self.vendor_driver_dir / self.series
        self.vendor_driver_file = self.vendor_driver_dir / "driver.h"

        # 项目和构建路径
        self.cmake_project_dir = self.project_root / "example/template/project/cmake/fake_chip"
        self.build_test_dir = self.project_root / "build_test"

        # 清理路径列表
        self.cleanup_paths = [
            self.vendor_driver_dir,
            self.build_test_dir,
            self.project_root / "CMakeFiles",
            self.project_root / "CMakeCache.txt",
            self.project_root / "cmake_install.cmake",
            self.project_root / "Makefile",
            self.project_root / "build",
        ]


class FileProcessor:
    """文件处理器 - 封装所有文件操作"""

    def __init__(self, config: TestConfig):
        self.config = config

    def copy_file(self, source: Path, target: Path) -> None:
        """复制文件，自动创建目标目录"""
        if not source.exists():
            raise FileNotFoundError(f"Source file does not exist: {source}")

        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, target)
        logger.info(f"Copied: {source} -> {target}")

    def replace_in_file(self, file_path: Path, replacements: Dict[str, str]) -> None:
        """在文件中进行字符串替换"""
        if not file_path.exists():
            raise FileNotFoundError(f"File does not exist: {file_path}")

        content = file_path.read_text(encoding="utf-8")

        for old, new in replacements.items():
            content = content.replace(old, new)

        file_path.write_text(content, encoding="utf-8")
        logger.info(f"Modified: {file_path} ({len(replacements)} replacements)")

    def clean_fake_vendor_lines(self, file_path: Path) -> None:
        """清理文件中的FAKE_VENDOR相关行"""
        content = file_path.read_text(encoding="utf-8")
        filtered_lines = [line for line in content.splitlines(keepends=True) if "FAKE_VENDOR" not in line]
        file_path.write_text("".join(filtered_lines), encoding="utf-8")
        logger.info(f"Cleaned FAKE_VENDOR lines from: {file_path}")

    def insert_at_pattern(self, file_path: Path, pattern: str, new_content: str, insert_before: bool = True) -> None:
        """在匹配模式处插入内容"""
        content = file_path.read_text(encoding="utf-8")

        # 检查内容是否已存在
        if new_content.strip() and new_content.strip() in content:
            return  # 静默跳过已存在的内容

        if not re.search(pattern, content):
            raise ValueError(f"Pattern '{pattern}' not found in {file_path}")

        if insert_before:
            content = re.sub(pattern, new_content + r"\1", content)
        else:
            content = re.sub(pattern, r"\1" + new_content, content)

        file_path.write_text(content, encoding="utf-8")
        logger.info(f"Modified: {file_path} (inserted content)")

    def remove_ipcore_section(self, file_path: Path) -> None:
        """删除文件中的IPCore部分"""
        if not file_path.exists():
            raise FileNotFoundError(f"File does not exist: {file_path}")

        content = file_path.read_text(encoding="utf-8")
        pattern = rf"{re.escape(self.config.ipcore_start_marker)}.*?{re.escape(self.config.ipcore_end_marker)}"
        new_content = re.sub(pattern, "", content, flags=re.DOTALL)
        file_path.write_text(new_content, encoding="utf-8")
        # 只在实际删除了内容时才记录日志
        if content != new_content:
            logger.info(f"Removed IPCore section from: {file_path}")

    def cleanup_build_artifacts(self) -> None:
        """清理构建产物"""
        cleaned_items = []
        for path in self.config.cleanup_paths:
            try:
                if path.is_dir():
                    shutil.rmtree(path)
                    cleaned_items.append(f"directory: {path}")
                elif path.exists():
                    path.unlink()
                    cleaned_items.append(f"file: {path}")
            except (FileNotFoundError, PermissionError, OSError) as e:
                logger.warning(f"Failed to cleanup {path}: {e}")

        if cleaned_items:
            items_preview = ", ".join(cleaned_items[:3])
            suffix = "..." if len(cleaned_items) > 3 else ""
            logger.info(f"Cleaned up {len(cleaned_items)} items: {items_preview}{suffix}")


class TemplateProcessor:
    """模板处理器 - 处理模板文件的复制和变量替换"""

    def __init__(self, config: TestConfig, file_processor: FileProcessor):
        self.config = config
        self.file_processor = file_processor

    def copy_template_tree(self) -> Tuple[Path, Path]:
        """复制模板目录树并重命名"""
        # 复制整个模板目录
        if self.config.vendor_driver_dir.exists():
            raise FileExistsError(f"Target directory already exists: {self.config.vendor_driver_dir}")

        shutil.copytree(self.config.hal_driver_template_dir, self.config.vendor_driver_dir)
        logger.info(
            f"Copied template directory: {self.config.hal_driver_template_dir} -> {self.config.vendor_driver_dir}"
        )

        # 重命名系列目录
        old_series_dir = self.config.vendor_driver_dir / "__series_name_a__"
        new_series_dir = self.config.vendor_driver_dir / self.config.series
        old_series_dir.rename(new_series_dir)
        logger.info(f"Renamed directory: __series_name_a__ -> {self.config.series}")

        # 重命名设备目录
        old_device_dir = new_series_dir / "__device_name_a__"
        new_device_dir = new_series_dir / self.config.device
        old_device_dir.rename(new_device_dir)
        logger.info(f"Renamed directory: __device_name_a__ -> {self.config.device}")

        return new_series_dir, new_device_dir

    def process_device_header(self, device_dir: Path) -> None:
        """处理设备头文件"""
        device_h = device_dir / "device.h"

        # 复制模板并替换变量
        shutil.copy2(self.config.template_device_h_file, device_h)
        logger.info(f"Copied: {self.config.template_device_h_file} -> {device_h}")

        replacements = {"vendor_header.h": "../common/vendor/fake_chip.h"}
        self.file_processor.replace_in_file(device_h, replacements)

    def update_main_driver_header(self) -> None:
        """更新主驱动头文件"""
        new_definition = (
            f"#   elif    defined(__{self.config.vendor}__)\n"
            f'#       define  VSF_DRIVER_HEADER       "./{self.config.vendor}/driver.h"\n'
        )

        self.file_processor.insert_at_pattern(self.config.hal_main_driver_header, r"(/\* example)", new_definition)

    def update_device_definitions(self, series_dir: Path) -> None:
        """更新设备定义文件"""
        device_header = series_dir / "__device.h"

        # 插入设备定义
        new_device_definition = (
            f"#elif   defined(__{self.config.device}__)\n"
            f'#   define  VSF_HAL_DEVICE_HEADER               "./{self.config.device}/device.h"\n'
        )

        self.file_processor.insert_at_pattern(device_header, r"(#else)", new_device_definition)

        # 替换设备名称
        replacements = {"__DEVICE_NAME_A__": self.config.device}
        self.file_processor.replace_in_file(device_header, replacements)

    def update_vendor_driver(self) -> None:
        """更新厂商驱动文件"""
        # 插入厂商定义
        new_vendor_definition = (
            f"#elif   defined(__{self.config.device}__)\n"
            f'#    define  VSF_HAL_DRIVER_HEADER       "./{self.config.series}/{self.config.device}/driver.h"\n'
        )

        self.file_processor.insert_at_pattern(self.config.vendor_driver_file, r"(#else)", new_vendor_definition)

        # 替换变量
        replacements = {
            "${VENDOR}": self.config.vendor,
            "${SERIES}": self.config.series,
        }
        self.file_processor.replace_in_file(self.config.vendor_driver_file, replacements)

    def process_peripheral_files(self, series_dir: Path) -> None:
        """处理外设相关文件"""
        # 收集需要处理的文件
        files_to_process = []
        for pattern in self.config.search_patterns:
            files_to_process.extend(series_dir.rglob(pattern))

        # 过滤排除的文件（但保留driver.h）
        files_to_process = [
            f for f in files_to_process if f.name not in self.config.exclude_files or f.name == "driver.h"
        ]

        # 生成替换映射
        replacements = {
            "${VENDOR}": self.config.vendor,
            "${SERIES}": self.config.series,
            "${DEVICE}": self.config.device,
        }

        # 添加外设替换映射
        for peripheral in self.config.peripherals:
            replacements.update({
                f"${{SERIES/{peripheral}_IP}}": self.config.series,
                f"${{{peripheral}_IP}}": self.config.series,
                f"${{{peripheral.lower()}_ip}}": self.config.series.lower(),
            })

        # 处理每个文件
        processed_count = 0
        for file_path in files_to_process:
            try:
                # 删除IPCore部分
                self.file_processor.remove_ipcore_section(file_path)

                # 替换所有变量
                self.file_processor.replace_in_file(file_path, replacements)
                processed_count += 1

            except Exception as e:
                logger.warning(f"Failed to process {file_path}: {e}")

        logger.info(f"Processed {processed_count} peripheral files")


class BuildSystem:
    """构建系统管理器"""

    def __init__(self, config: TestConfig):
        self.config = config

    def run_cmake_configure(self, generator: str) -> bool:
        """运行CMake配置"""
        cmd = [
            "cmake",
            "-S",
            str(self.config.cmake_project_dir),
            "-B",
            str(self.config.build_test_dir),
            "-G",
            generator,
        ]

        logger.info("Running CMake configure...")

        try:
            # 直接输出到终端，不捕获输出
            result = subprocess.run(cmd, timeout=300)
            if result.returncode != 0:
                logger.error("CMake configure failed")
                return False
            logger.info("CMake configure completed successfully")
            return True
        except subprocess.TimeoutExpired:
            logger.error("CMake configure timed out")
            return False

    def run_cmake_build(self) -> bool:
        """运行CMake构建"""
        cmd = ["cmake", "--build", str(self.config.build_test_dir)]

        logger.info("Running CMake build...")

        try:
            # 直接输出到终端，不捕获输出
            result = subprocess.run(cmd, timeout=600)
            if result.returncode != 0:
                logger.error("CMake build failed")
                return False
            logger.info("CMake build completed successfully")
            return True
        except subprocess.TimeoutExpired:
            logger.error("CMake build timed out")
            return False

    def run_full_build_test(self, generator: Optional[str] = None) -> bool:
        """运行完整的构建测试"""
        generator = generator or self.config.default_generator

        if not self.run_cmake_configure(generator):
            return False

        return self.run_cmake_build()


class HALTestRunner:
    """HAL测试运行器 - 主要的测试协调器"""

    def __init__(self, config: Optional[TestConfig] = None):
        self.config = config or TestConfig()
        self.file_processor = FileProcessor(self.config)
        self.template_processor = TemplateProcessor(self.config, self.file_processor)
        self.build_system = BuildSystem(self.config)

    def setup_logging(self, level: int = logging.INFO) -> None:
        """设置日志"""
        logging.basicConfig(
            level=level,
            format="%(asctime)s - %(levelname)s - %(message)s",
            handlers=[logging.StreamHandler(sys.stdout)],
        )

    def cleanup_environment(self) -> None:
        """清理测试环境"""
        # 清理构建产物
        self.file_processor.cleanup_build_artifacts()

        # 清理主驱动头文件中的FAKE_VENDOR行
        self.file_processor.clean_fake_vendor_lines(self.config.hal_main_driver_header)

    def run_template_workflow(self) -> bool:
        """运行模板处理工作流"""
        try:
            # 1. 复制模板目录树并重命名
            series_dir, device_dir = self.template_processor.copy_template_tree()

            # 2. 处理设备头文件
            self.template_processor.process_device_header(device_dir)

            # 3. 更新主驱动头文件
            self.template_processor.update_main_driver_header()

            # 4. 更新设备定义
            self.template_processor.update_device_definitions(series_dir)

            # 5. 更新厂商驱动
            self.template_processor.update_vendor_driver()

            # 6. 处理外设文件
            self.template_processor.process_peripheral_files(series_dir)

            return True

        except Exception as e:
            logger.error(f"Template workflow failed: {e}")
            return False

    def run_full_test(self, generator: Optional[str] = None) -> bool:
        """运行完整的测试流程"""
        logger.info("Starting VSF HAL driver test...")

        try:
            # 1. 清理环境
            self.cleanup_environment()

            # 2. 运行模板工作流
            if not self.run_template_workflow():
                return False

            # 3. 运行构建测试
            if not self.build_system.run_full_build_test(generator):
                return False

            logger.info("VSF HAL driver test completed successfully!")
            return True

        except Exception as e:
            logger.error(f"Test failed: {e}")
            return False


def main():
    """主函数 - 简洁的入口点"""
    runner = HALTestRunner()
    runner.setup_logging(level=logging.INFO)

    success = runner.run_full_test()
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
