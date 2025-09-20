#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
VSF HAL Driver Template Test Script (Python Version)
VSF HAL驱动模板测试脚本 (Python重构版本)

用于测试HAL驱动模板的正确性，使用通用项目模板作为测试基础
"""

import sys
import logging
from pathlib import Path
from typing import Optional

import click
import colorama
from colorama import Fore, Style

# 添加lib目录到Python路径
sys.path.insert(0, str(Path(__file__).parent / "lib"))

from lib import (
    ConfigParser,
    TemplateProcessor,
    FileOperations,
    BuildManager,
    PathManager,
)

# 初始化colorama
colorama.init()

# 获取脚本目录和项目根目录
SCRIPT_DIR = Path(__file__).parent
PROJECT_ROOT = SCRIPT_DIR.parent.parent
CONFIG_DIR = SCRIPT_DIR / "config"
TEMPLATE_DIR = SCRIPT_DIR / "templates"
DEFAULT_CONFIG = CONFIG_DIR / "test_config.yaml"


class Logger:
    """自定义日志记录器"""

    @staticmethod
    def setup_logging(verbose: bool = False):
        """设置日志配置"""
        level = logging.DEBUG if verbose else logging.INFO

        # 创建自定义格式器
        class ColoredFormatter(logging.Formatter):
            COLORS = {
                'DEBUG': Fore.CYAN,
                'INFO': Fore.BLUE,
                'WARNING': Fore.YELLOW,
                'ERROR': Fore.RED,
                'CRITICAL': Fore.MAGENTA,
            }

            def format(self, record):
                color = self.COLORS.get(record.levelname, '')
                record.levelname = (
                    f"{color}[{record.levelname}]{Style.RESET_ALL}"
                )
                return super().format(record)

        # 配置根日志记录器
        logging.basicConfig(
            level=level,
            format='%(levelname)s %(message)s',
            handlers=[logging.StreamHandler(sys.stdout)]
        )

        # 设置自定义格式器
        for handler in logging.root.handlers:
            handler.setFormatter(ColoredFormatter())

    @staticmethod
    def success(message: str):
        """成功信息"""
        print(f"{Fore.GREEN}[SUCCESS]{Style.RESET_ALL} {message}")

    @staticmethod
    def info(message: str):
        """信息"""
        print(f"{Fore.BLUE}[INFO]{Style.RESET_ALL} {message}")

    @staticmethod
    def warning(message: str):
        """警告"""
        print(f"{Fore.YELLOW}[WARNING]{Style.RESET_ALL} {message}")

    @staticmethod
    def error(message: str):
        """错误"""
        print(f"{Fore.RED}[ERROR]{Style.RESET_ALL} {message}")


class VSFHALTestManager:
    """VSF HAL测试管理器"""

    def __init__(self, config_file: str):
        """
        初始化测试管理器

        Args:
            config_file: 配置文件路径
        """
        self.config_file = config_file
        self.config_data = {}

        # 初始化各个模块
        self.config_parser: Optional[ConfigParser] = None
        self.file_ops: Optional[FileOperations] = None
        self.template_processor: Optional[TemplateProcessor] = None
        self.build_manager: Optional[BuildManager] = None
        self.path_manager: Optional[PathManager] = None

    def _get_template_file(self, template_key: str) -> str:
        """
        获取模板文件路径

        Args:
            template_key: 模板文件键名

        Returns:
            模板文件的完整路径
        """
        template_config = self.config_data.get('template_config', {})
        template_files = template_config.get('template_files', {})
        template_name = template_files.get(
            template_key, f"{template_key}.template"
        )
        return str(TEMPLATE_DIR / template_name)

    def _get_file_naming_pattern(self, pattern_key: str, **kwargs) -> str:
        """
        获取文件命名模式

        Args:
            pattern_key: 命名模式键名
            **kwargs: 用于格式化的参数

        Returns:
            格式化后的文件名
        """
        template_config = self.config_data.get('template_config', {})
        naming_patterns = template_config.get('file_naming_patterns', {})
        pattern = naming_patterns.get(pattern_key, pattern_key)
        return pattern.format(**kwargs)

    def _get_directory_pattern(self, pattern_key: str, **kwargs) -> str:
        """
        获取目录结构模式

        Args:
            pattern_key: 目录模式键名
            **kwargs: 用于格式化的参数

        Returns:
            格式化后的目录名
        """
        template_config = self.config_data.get('template_config', {})
        directory_structure = template_config.get('directory_structure', {})
        pattern = directory_structure.get(pattern_key, pattern_key)
        return pattern.format(**kwargs)

    def _get_file_processing_rules(self) -> dict:
        """
        获取文件处理规则

        Returns:
            文件处理规则字典
        """
        template_config = self.config_data.get('template_config', {})
        return template_config.get('file_processing_rules', {
            'search_patterns': ["*.h", "*.c"],
            'exclude_files': ["__device.h"],
            'ipcore_markers': {
                'start': "// IPCore",
                'end': "// IPCore end"
            }
        })

    def _get_build_options(self) -> dict:
        """
        获取构建选项

        Returns:
            构建选项字典
        """
        template_config = self.config_data.get('template_config', {})
        return template_config.get('build_options', {
            'default_generator': "Unix Makefiles",
            'available_generators': ["Ninja", "Unix Makefiles"]
        })

    def _process_template_file(
        self, template_key: str, output_path: str,
        file_naming_key: Optional[str] = None, log_message: Optional[str] = None
    ) -> bool:
        """
        通用的模板文件处理方法

        Args:
            template_key: 模板文件键名
            output_path: 输出文件路径
            file_naming_key: 文件命名模式键名（可选）
            log_message: 日志消息（可选）

        Returns:
            处理成功返回True，失败返回False
        """
        if self.template_processor is None:
            return False

        template_file = self._get_template_file(template_key)

        if file_naming_key:
            output_file = str(
                Path(output_path) / self._get_file_naming_pattern(
                    file_naming_key
                )
            )
        else:
            output_file = output_path

        if log_message:
            logging.info(log_message)

        return self.template_processor.process_template(
            template_file, output_file
        )

    def initialize(self) -> bool:
        """
        初始化所有模块

        Returns:
            初始化成功返回True，失败返回False
        """
        try:
            # 初始化配置解析器
            self.config_parser = ConfigParser(self.config_file)
            self.config_data = self.config_parser.parse()

            # 初始化路径管理器
            self.path_manager = PathManager(PROJECT_ROOT)

            # 初始化其他模块
            self.file_ops = FileOperations(
                self.config_data, str(PROJECT_ROOT)
            )
            self.template_processor = TemplateProcessor(
                self.config_data, str(TEMPLATE_DIR)
            )
            self.build_manager = BuildManager(
                self.config_data, str(PROJECT_ROOT)
            )

            return True

        except Exception as e:
            logging.error(f"初始化失败: {e}")
            return False

    def cleanup_previous_files(self) -> bool:
        """清理之前的测试文件"""
        logging.info("清理之前的测试文件...")
        if self.file_ops is None:
            return False
        return self.file_ops.cleanup_previous_test_files()

    def create_test_target(self) -> bool:
        """创建测试目标配置"""
        logging.info("创建测试目标配置文件...")

        vendor = self.config_data['VENDOR']
        series = self.config_data['SERIES']
        chip_name_upper = self.config_data['CHIP_NAME_UPPER']

        # 获取目标目录和文件路径
        if self.path_manager is None or self.file_ops is None:
            return False

        target_dir = self.path_manager.get_cmake_target_dir(vendor, series)
        target_file = self.path_manager.get_cmake_target_file(
            vendor, series, chip_name_upper
        )

        # 创建目标目录
        if not self.file_ops.create_directory(str(target_dir)):
            return False

        # 处理目标配置模板
        template_file = self._get_template_file("target_cmake")

        if self.template_processor is None:
            return False

        if not self.template_processor.process_template(
            template_file, str(target_file)
        ):
            return False

        Logger.success(f"创建目标配置文件: {target_file}")
        return True

    def create_test_project(self) -> bool:
        """创建测试项目"""
        logging.info("创建测试项目...")

        chip_name = self.config_data['CHIP_NAME']

        # 获取目标项目路径
        if self.path_manager is None or self.file_ops is None:
            return False

        dst_project = str(self.path_manager.get_cmake_project_dir(chip_name))

        # 创建项目目录
        if not self.file_ops.create_directory(dst_project):
            return False

        # 使用模板生成CMakeLists.txt
        if not self._create_project_cmake_from_template(dst_project):
            return False

        # 使用模板生成linker.ld
        if not self._create_project_linker_from_template(dst_project):
            return False

        Logger.success("测试项目创建完成")
        return True

    def _create_project_cmake_from_template(self, project_dir: str) -> bool:
        """使用模板创建项目CMakeLists.txt"""
        return self._process_template_file(
            "project_cmake", project_dir, "cmake_file",
            "使用模板创建项目 CMakeLists.txt..."
        )

    def _create_project_linker_from_template(self, project_dir: str) -> bool:
        """使用模板创建项目linker.ld"""
        return self._process_template_file(
            "linker", project_dir, "linker_file",
            "使用模板创建项目 linker.ld..."
        )

    def create_test_driver(self) -> bool:
        """创建测试驱动"""
        logging.info("创建测试驱动...")

        vendor = self.config_data['VENDOR']
        series = self.config_data['SERIES']

        # 获取源驱动和目标驱动路径（测试HAL驱动模板）
        if self.path_manager is None or self.file_ops is None:
            return False

        src_driver = str(self.path_manager.get_hal_driver_template_dir())
        dst_vendor_dir = str(self.path_manager.get_vendor_driver_dir(vendor))
        dst_driver = str(
            self.path_manager.get_series_driver_dir(vendor, series)
        )

        # 创建厂商目录
        if not self.file_ops.create_directory(dst_vendor_dir):
            return False

        # 创建厂商级别的文件
        if not self._create_vendor_files(dst_vendor_dir):
            return False

        # 复制系列驱动
        if not self.file_ops.copy_directory(src_driver, dst_driver):
            return False

        # 创建系列级别的文件
        if not self._create_series_files(dst_driver):
            return False

        # 创建设备级别的文件
        if not self._create_device_files(dst_driver):
            return False

        Logger.success("测试驱动创建完成")
        return True

    def _create_vendor_files(self, vendor_dir: str) -> bool:
        """创建厂商级别的文件"""
        # 创建厂商级别的CMakeLists.txt
        if not self._process_template_file(
            "vendor_cmake", vendor_dir, "cmake_file"
        ):
            return False

        # 创建厂商级别的driver.h
        return self._process_template_file(
            "vendor_driver_h", str(Path(vendor_dir) / "driver.h")
        )

    def _create_series_files(self, series_dir: str) -> bool:
        """创建系列级别的文件"""
        series_path = Path(series_dir)

        # 创建系列级别的CMakeLists.txt
        if not self._process_template_file(
            "series_cmake", str(series_path), "cmake_file"
        ):
            return False

        # 创建common目录的CMakeLists.txt
        return self._process_template_file(
            "common_cmake", str(series_path / "common"), "cmake_file"
        )

    def _create_device_files(self, series_dir: str) -> bool:
        """创建设备级别的文件"""
        series_path = Path(series_dir)
        device = self.config_data['DEVICE']

        # 重命名设备目录
        old_device_dir = series_path / self._get_directory_pattern(
            "old_device_dir_pattern"
        )
        new_device_dir = series_path / self._get_directory_pattern(
            "new_device_dir_pattern", device=device
        )

        if old_device_dir.exists():
            if self.file_ops is None:
                return False
            if not self.file_ops.rename_directory(
                self._get_directory_pattern("old_device_dir_pattern"),
                device,
                str(series_path)
            ):
                return False

            # 创建设备级别的文件
            device_dir = str(new_device_dir)

            # 创建设备CMakeLists.txt
            if self.template_processor is None:
                return False

            template_file = self._get_template_file("device_cmake")
            output_file = str(
                new_device_dir / self._get_file_naming_pattern("cmake_file")
            )
            if not self.template_processor.process_template(
                template_file, output_file
            ):
                return False

            # 创建启动文件
            if not self._create_startup_file(device_dir):
                return False

            # 创建driver.c文件
            if not self._create_driver_c_file(device_dir):
                return False

            # 创建设备头文件
            if not self._create_device_header_file(device_dir):
                return False

        return True

    def _create_startup_file(self, device_dir: str) -> bool:
        """创建启动文件"""
        device_lower = self.config_data['DEVICE_LOWER']
        startup_file = str(
            Path(device_dir) / self._get_file_naming_pattern(
                "startup_file", device_lower=device_lower
            )
        )
        template_file = self._get_template_file("startup")

        logging.info(f"创建启动文件: {startup_file}")
        if self.template_processor is None:
            return False
        return self.template_processor.process_special_template(
            "startup_s", template_file, startup_file
        )

    def _create_driver_c_file(self, device_dir: str) -> bool:
        """创建driver.c文件"""
        driver_file = str(
            Path(device_dir) / self._get_file_naming_pattern("driver_file")
        )
        return self._process_template_file(
            "driver_c", driver_file,
            log_message=f"创建driver.c文件: {driver_file}"
        )

    def _create_device_header_file(self, device_dir: str) -> bool:
        """创建设备头文件"""
        device_header_file = str(
            Path(device_dir) / self._get_file_naming_pattern(
                "device_header_file"
            )
        )
        template_file = self._get_template_file("device_h")

        logging.info(f"创建设备头文件: {device_header_file}")
        if self.template_processor is None:
            return False
        return self.template_processor.process_special_template(
            "device_h", template_file, device_header_file
        )

    def update_main_files(self) -> bool:
        """更新主文件"""
        logging.info("更新主文件...")

        # 更新主驱动头文件
        if self.file_ops is None:
            return False

        if not self.file_ops.update_main_driver_header():
            return False

        # 更新主配置文件
        if not self.file_ops.update_main_config_file():
            return False

        # 创建测试配置文件
        if not self._create_test_config():
            return False

        return True

    def _create_test_config(self) -> bool:
        """创建测试配置文件"""
        logging.info("创建测试设备配置文件...")

        device_lower = self.config_data['DEVICE_LOWER']
        if self.path_manager is None or self.template_processor is None:
            return False

        test_config = str(self.path_manager.get_test_config_file(device_lower))
        template_file = self._get_template_file("vsf_usr_cfg")

        return self.template_processor.process_template(
            template_file, test_config
        )

    def replace_template_content(self) -> bool:
        """替换模板内容"""
        logging.info("替换模板内容...")

        vendor = self.config_data['VENDOR']
        series = self.config_data['SERIES']
        if self.path_manager is None:
            return False

        dst_driver = self.path_manager.get_series_driver_dir(vendor, series)

        # 替换设备头文件变量
        if not self._replace_device_header_variables(str(dst_driver)):
            return False

        # 处理模板文件的IPCore部分和变量替换
        if not self._process_template_files(str(dst_driver)):
            return False

        Logger.success("模板内容替换完成")
        return True

    def _replace_device_header_variables(self, dst_driver: str) -> bool:
        """替换设备头文件变量"""
        logging.info("替换设备头文件中的模板变量...")

        device_header = Path(dst_driver) / self._get_directory_pattern(
            "device_header_pattern"
        )
        if not device_header.exists():
            return True

        try:
            device = self.config_data['DEVICE']
            device_upper = self.config_data['DEVICE_UPPER']
            vendor_upper = self.config_data['VENDOR_UPPER']
            series_upper = self.config_data['SERIES_UPPER']

            with open(device_header, 'r', encoding='utf-8') as f:
                content = f.read()

            # 替换宏定义和路径
            replacements = {
                'defined(__DEVICE_NAME_A__)': f'defined(__{device_upper}__)',
                'defined(__DEVICE_NAME_B__)': f'defined(__{device_upper}_B__)',
                './__DEVICE_NAME_A__/device.h': f'./{device}/device.h',
                './__DEVICE_NAME_B__/device.h': f'./{device}_B/device.h',
                '${VENDOR}': vendor_upper,
                '${SERIES}': series_upper,
            }

            for old, new in replacements.items():
                content = content.replace(old, new)

            with open(device_header, 'w', encoding='utf-8') as f:
                f.write(content)

            return True

        except Exception as e:
            logging.error(f"替换设备头文件变量失败: {e}")
            return False

    def _process_template_files(self, dst_driver: str) -> bool:
        """处理模板文件的IPCore部分和变量替换"""
        logging.info("处理模板文件的IPCore部分和变量替换...")

        try:
            driver_path = Path(dst_driver)

            # 查找所有需要处理的文件
            files_to_process = []
            processing_rules = self._get_file_processing_rules()
            for pattern in processing_rules.get(
                'search_patterns', ["*.h", "*.c"]
            ):
                files_to_process.extend(driver_path.rglob(pattern))

            # 排除指定文件
            exclude_files = processing_rules.get(
                'exclude_files', ["__device.h"]
            )
            files_to_process = [
                f for f in files_to_process if f.name not in exclude_files
            ]

            # 删除IPCore部分
            logging.info("删除IPCore部分（仅保留HW部分）...")
            for file_path in files_to_process:
                self._remove_ipcore_sections(file_path)

            # 替换模板变量
            logging.info("替换模板变量...")
            if self.file_ops is None:
                return False

            file_paths = [str(f) for f in files_to_process]
            return self.file_ops.replace_template_variables_in_files(
                file_paths
            )

        except Exception as e:
            logging.error(f"处理模板文件失败: {e}")
            return False

    def _remove_ipcore_sections(self, file_path: Path):
        """删除文件中的IPCore部分"""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                lines = f.readlines()

            new_lines = []
            in_ipcore_section = False
            processing_rules = self._get_file_processing_rules()
            ipcore_markers = processing_rules.get('ipcore_markers', {
                'start': "// IPCore",
                'end': "// IPCore end"
            })

            for line in lines:
                stripped = line.strip()
                if stripped == ipcore_markers.get('start', "// IPCore"):
                    in_ipcore_section = True
                    continue
                elif stripped == ipcore_markers.get('end', "// IPCore end"):
                    in_ipcore_section = False
                    continue

                if not in_ipcore_section:
                    new_lines.append(line)

            with open(file_path, 'w', encoding='utf-8') as f:
                f.writelines(new_lines)

        except Exception as e:
            logging.warning(f"删除IPCore部分失败 {file_path}: {e}")

    def verify_template_structure(self) -> bool:
        """验证模板结构"""
        logging.info("验证模板结构...")

        vendor = self.config_data['VENDOR']
        series = self.config_data['SERIES']
        device = self.config_data['DEVICE']
        chip_name = self.config_data['CHIP_NAME']

        # 获取驱动和项目目录
        if self.path_manager is None:
            return False

        dst_driver = self.path_manager.get_series_driver_dir(vendor, series)
        dst_project = self.path_manager.get_cmake_project_dir(chip_name)

        # 获取通用外设文件路径
        peripheral_files = self.path_manager.get_common_peripheral_files(
            dst_driver
        )

        # 检查必要的文件
        required_files = [
            str(
                dst_driver / self._get_directory_pattern(
                    "device_header_pattern"
                )
            ),
            str(
                dst_driver / device / self._get_file_naming_pattern(
                    "device_header_file"
                )
            ),
            str(peripheral_files["gpio"]),
            str(peripheral_files["i2c"]),
            str(peripheral_files["spi"]),
            str(dst_project / self._get_file_naming_pattern("cmake_file")),
        ]

        if self.template_processor is None:
            return False

        return self.template_processor.verify_template_processing(
            required_files
        )

    def run_build_test(self, generator: Optional[str] = None) -> bool:
        """运行构建测试"""
        logging.info(
            "尝试构建测试（预期会失败，因为使用测试芯片名称）..."
        )

        # 在构建测试前进行额外的清理
        self._pre_build_cleanup()

        if self.build_manager is None:
            return False

        build_options = self._get_build_options()
        default_generator = build_options.get(
            'default_generator', "Unix Makefiles"
        )
        test_passed, result = self.build_manager.run_full_build_test(
            generator or default_generator
        )

        if test_passed:
            Logger.success("构建测试通过")
        else:
            logging.info("构建测试按预期失败，但模板结构验证已完成")

        return test_passed

    def _pre_build_cleanup(self):
        """构建前的额外清理"""
        import shutil

        # 获取需要清理的构建文件路径
        if self.path_manager is None:
            return

        cleanup_paths = self.path_manager.get_build_cleanup_paths()

        for path in cleanup_paths:
            if path.exists():
                try:
                    if path.is_dir():
                        shutil.rmtree(path)
                    else:
                        path.unlink()
                    logging.debug(f"清理构建残留: {path}")
                except Exception as e:
                    logging.warning(f"清理构建残留失败 {path}: {e}")

        logging.info("构建前额外清理完成")


@click.command()
@click.option('-c', '--config', default=str(DEFAULT_CONFIG),
              help=f'指定配置文件 (默认: {DEFAULT_CONFIG})')
@click.option('--clean-only', is_flag=True,
              help='只清理之前的测试文件，不进行新的测试')
@click.option('--no-build', is_flag=True,
              help='跳过构建步骤，只生成文件')
@click.option('--verbose', is_flag=True,
              help='显示详细输出')
@click.option(
    '--generator', '-g', default=None,
    help='CMake构建生成器 (默认为配置文件中的默认值)'
)
def main(
    config: str,
    clean_only: bool,
    no_build: bool,
    verbose: bool,
    generator: Optional[str]
):
    """VSF HAL Driver Template Test Script (Python Version)"""

    # 设置日志
    Logger.setup_logging(verbose)

    Logger.info("开始 VSF HAL Driver Template 测试 (Python重构版本)")
    Logger.info("=" * 50)

    try:
        # 初始化测试管理器
        test_manager = VSFHALTestManager(config)
        if not test_manager.initialize():
            Logger.error("测试管理器初始化失败")
            sys.exit(1)

        # 显示配置摘要
        if test_manager.config_parser is not None:
            test_manager.config_parser.show_config_summary()

        # 清理之前的测试文件
        if not test_manager.cleanup_previous_files():
            Logger.error("清理之前的测试文件失败")
            sys.exit(1)

        # 如果只是清理，则退出
        if clean_only:
            Logger.success("清理完成")
            sys.exit(0)

        # 创建测试文件和目录
        steps = [
            ("创建测试目标配置", test_manager.create_test_target),
            ("创建测试项目", test_manager.create_test_project),
            ("创建测试驱动", test_manager.create_test_driver),
            ("更新主文件", test_manager.update_main_files),
            ("替换模板内容", test_manager.replace_template_content),
            ("验证模板结构", test_manager.verify_template_structure),
        ]

        for step_name, step_func in steps:
            logging.info(f"执行步骤: {step_name}")
            if not step_func():
                Logger.error(f"步骤失败: {step_name}")
                sys.exit(1)
            Logger.success(f"步骤完成: {step_name}")

        # 构建测试（如果不跳过）
        if not no_build:
            test_manager.run_build_test(generator)
        else:
            logging.info("跳过构建步骤")

        Logger.success("模板测试完成！")
        Logger.info("测试文件已保留，可使用 --clean-only 选项进行清理")

    except KeyboardInterrupt:
        Logger.warning("测试被用户中断")
        sys.exit(1)
    except Exception as e:
        Logger.error(f"测试过程中发生异常: {e}")
        if verbose:
            import traceback
            traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    # 使用click装饰器时，main函数会自动接收参数
    # 这里直接调用main()即可，click会处理命令行参数
    try:
        # type: ignore - click装饰器会自动处理参数
        main()  # type: ignore
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
