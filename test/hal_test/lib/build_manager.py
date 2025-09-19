#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
VSF HAL Driver Template Test - Build Manager
构建管理模块
"""

import subprocess
import logging
import os
from pathlib import Path
from typing import Dict, Any, Tuple, Optional

logger = logging.getLogger(__name__)


class BuildManager:
    """构建管理器类"""

    def __init__(self, config_data: Dict[str, Any], project_root: str):
        """
        初始化构建管理器

        Args:
            config_data: 配置数据
            project_root: 项目根目录
        """
        self.config_data = config_data
        self.project_root = Path(project_root)
        self.build_dir = self.project_root / "build_test"

    def check_build_tools(self) -> bool:
        """
        检查构建工具是否可用

        Returns:
            工具检查通过返回True，失败返回False
        """
        logger.info("检查必要的构建工具...")

        required_tools = ["cmake", "make"]
        missing_tools = []

        for tool in required_tools:
            try:
                subprocess.run(
                    [tool, "--version"],
                    capture_output=True,
                    check=True,
                    timeout=10,
                )
            except (
                subprocess.CalledProcessError,
                subprocess.TimeoutExpired,
                FileNotFoundError,
            ):
                missing_tools.append(tool)

        if missing_tools:
            logger.error("以下构建工具未安装或不在 PATH 中:")
            for tool in missing_tools:
                logger.error(f"  - {tool}")
            return False

        logger.info("所有必要构建工具检查完成")
        return True

    def setup_build_environment(self) -> bool:
        """
        设置构建环境

        Returns:
            设置成功返回True，失败返回False
        """
        try:
            # 彻底清理构建相关文件
            self._thorough_cleanup()

            # 重新创建构建目录
            self.build_dir.mkdir(parents=True, exist_ok=True)

            # 设置环境变量
            os.environ["VSF_PATH"] = str(self.project_root)

            logger.info(f"构建环境设置完成，构建目录: {self.build_dir}")
            return True

        except Exception as e:
            logger.error(f"构建环境设置失败: {e}")
            return False

    def _thorough_cleanup(self):
        """彻底清理构建相关文件"""
        import shutil

        cleanup_paths = [
            self.build_dir,  # 主构建目录
            self.project_root / "CMakeFiles",  # 根目录的CMake文件
            self.project_root / "CMakeCache.txt",  # CMake缓存
            self.project_root / "cmake_install.cmake",  # CMake安装脚本
        ]

        # 查找并清理所有可能的CMake缓存文件
        chip_name = self.config_data.get("CHIP_NAME", "mychip")
        test_project_dir = (
            self.project_root
            / "example"
            / "template"
            / "project"
            / "cmake"
            / chip_name
        )

        if test_project_dir.exists():
            cmake_files_in_project = [
                test_project_dir / "CMakeFiles",
                test_project_dir / "CMakeCache.txt",
                test_project_dir / "cmake_install.cmake",
            ]
            cleanup_paths.extend(cmake_files_in_project)

        for path in cleanup_paths:
            if path.exists():
                try:
                    if path.is_dir():
                        shutil.rmtree(path)
                        logger.debug(f"删除目录: {path}")
                    else:
                        path.unlink()
                        logger.debug(f"删除文件: {path}")
                except Exception as e:
                    logger.warning(f"清理文件失败 {path}: {e}")

        logger.info("彻底清理构建文件完成")

    def run_cmake_configure(self, generator: str = None) -> Tuple[bool, str]:
        """
        运行CMake配置

        Args:
            generator: CMake生成器 (None=默认, "Ninja", "Unix Makefiles")

        Returns:
            (是否成功, 输出信息)
        """
        logger.info("配置 CMake 项目...")

        chip_name = self.config_data["CHIP_NAME"]
        test_project = (
            self.project_root
            / "example"
            / "template"
            / "project"
            / "cmake"
            / chip_name
        )

        if not test_project.exists():
            error_msg = f"测试项目目录不存在: {test_project}"
            logger.error(error_msg)
            return False, error_msg

        try:
            # 切换到构建目录
            original_cwd = os.getcwd()
            os.chdir(self.build_dir)

            # 构建CMake命令
            cmake_cmd = ["cmake", str(test_project)]
            if generator:
                cmake_cmd.extend(["-G", generator])
                logger.info(f"使用构建生成器: {generator}")

            # 运行CMake配置
            result = subprocess.run(
                cmake_cmd, capture_output=True, text=True, timeout=300
            )

            # 恢复工作目录
            os.chdir(original_cwd)

            # 保存输出到文件
            cmake_log = self.build_dir / "cmake_output.log"
            with open(cmake_log, "w", encoding="utf-8") as f:
                f.write(f"Return code: {result.returncode}\n")
                f.write("STDOUT:\n")
                f.write(result.stdout)
                f.write("\nSTDERR:\n")
                f.write(result.stderr)

            if result.returncode == 0:
                logger.info("CMake 配置成功")
                return True, result.stdout
            else:
                logger.warning(f"CMake 配置失败，返回码: {result.returncode}")
                return False, result.stderr

        except subprocess.TimeoutExpired:
            error_msg = "CMake 配置超时"
            logger.error(error_msg)
            return False, error_msg
        except Exception as e:
            error_msg = f"CMake 配置异常: {e}"
            logger.error(error_msg)
            return False, error_msg

    def run_cmake_build(self) -> Tuple[bool, str]:
        """
        运行CMake构建（使用cmake --build，支持Make/Ninja等构建工具）

        Returns:
            (是否成功, 输出信息)
        """
        logger.info("开始构建...")

        try:
            # 使用cmake --build命令，让CMake选择合适的构建工具
            result = subprocess.run(
                ["cmake", "--build", str(self.build_dir)],
                capture_output=True,
                text=True,
                timeout=600,
                cwd=str(self.project_root),  # 在项目根目录执行
            )

            # 保存输出到文件
            build_log = self.build_dir / "build_output.log"
            with open(build_log, "w", encoding="utf-8") as f:
                f.write(f"Return code: {result.returncode}\n")
                f.write("STDOUT:\n")
                f.write(result.stdout)
                f.write("\nSTDERR:\n")
                f.write(result.stderr)

            if result.returncode == 0:
                logger.info("构建成功！模板验证通过！")
                return True, result.stdout
            else:
                logger.warning(f"构建失败，返回码: {result.returncode}")
                return False, result.stderr

        except subprocess.TimeoutExpired:
            error_msg = "构建超时"
            logger.error(error_msg)
            return False, error_msg
        except Exception as e:
            error_msg = f"构建异常: {e}"
            logger.error(error_msg)
            return False, error_msg

    def run_full_build_test(self, generator: str = None) -> Tuple[bool, str]:
        """
        运行完整的构建测试

        Args:
            generator: CMake生成器 (None=默认, "Ninja", "Unix Makefiles")

        Returns:
            (测试是否通过, 测试结果描述)
        """
        logger.info("开始构建测试...")

        # 检查构建工具
        if not self.check_build_tools():
            return False, "构建工具检查失败"

        # 设置构建环境
        if not self.setup_build_environment():
            return False, "构建环境设置失败"

        # 运行CMake配置
        cmake_success, cmake_output = self.run_cmake_configure(generator)

        # 运行CMake构建（如果CMake成功）
        build_success = False
        build_output = ""
        if cmake_success:
            build_success, build_output = self.run_cmake_build()

        # 处理构建结果
        if cmake_success and build_success:
            result = "构建测试完全通过！模板验证成功！"
            logger.info(result)
            return True, result
        elif not cmake_success:
            logger.error("CMake 配置失败")
            logger.error("CMake 错误输出:")
            logger.error("-" * 50)
            logger.error(cmake_output)
            logger.error("-" * 50)
            return False, f"CMake 配置失败，详细错误信息已输出"
        elif not build_success:
            logger.error("构建失败")
            logger.error("构建错误输出:")
            logger.error("-" * 50)
            logger.error(build_output)
            logger.error("-" * 50)
            return False, f"构建失败，详细错误信息已输出"
        else:
            return False, "未知构建错误"

    def cleanup_build_directory(self) -> bool:
        """
        清理构建目录

        Returns:
            清理成功返回True，失败返回False
        """
        try:
            if self.build_dir.exists():
                import shutil

                shutil.rmtree(self.build_dir)
                logger.info("构建目录已清理")
            return True
        except Exception as e:
            logger.error(f"清理构建目录失败: {e}")
            return False
