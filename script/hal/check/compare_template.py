#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""批量对比所有外设的 API 模板和实现模板"""

import sys
from pathlib import Path
from typing import Optional, Tuple, Dict
from concurrent.futures import ProcessPoolExecutor, as_completed
from clang import cindex

from compare_template_core import (
    PERIPHERAL_NAMES,
    TemplateExtractor,
    StandardOptionalExtractor,
    TemplateComparator,
    save_comparison_results,
    save_summary,
)


def configure_libclang(libclang_path: Optional[str] = None) -> bool:
    """配置 libclang 库"""
    from os import environ

    # 如果提供了路径，使用提供的路径
    if libclang_path:
        if not Path(libclang_path).is_file():
            print(f"错误: libclang 路径无效: {libclang_path}")
            return False
        try:
            cindex.Config.set_library_file(libclang_path)
            cindex.Index.create()
            print(f"libclang 配置成功: {libclang_path}")
            return True
        except Exception as e:
            print(f"错误: libclang 配置失败: {e}")
            return False

    # 如果没有提供路径，尝试从环境变量获取
    path = environ.get('LIBCLANG_PATH')
    if path and Path(path).is_file():
        try:
            cindex.Config.set_library_file(path)
            cindex.Index.create()
            print(f"libclang 配置成功（环境变量）: {path}")
            return True
        except Exception as e:
            print(f"警告: 环境变量中的 libclang 配置失败: {e}")

    # 如果都没有提供，尝试让 Python 自动查找（适用于 Ubuntu 等系统）
    try:
        cindex.Index.create()
        print("libclang 配置成功（自动查找）")
        return True
    except Exception as e:
        print(f"错误: libclang 自动查找失败: {e}")
        print("提示: 请通过 -l 参数指定 libclang 路径，或设置 LIBCLANG_PATH 环境变量")
        return False


def find_project_root() -> Path:
    """查找项目根目录
    
    从脚本所在目录向上查找，直到找到包含 source 目录的目录
    """
    script_path = Path(__file__).resolve()
    current = script_path.parent
    
    # 向上查找，直到找到包含 source 目录的目录
    while current != current.parent:  # 未到达根目录
        if (current / "source").is_dir():
            return current
        current = current.parent
    
    # 如果没找到，回退到原来的方式
    return script_path.parents[3]


def get_peripherals(project_root: Path, vendor: str = 'FAKE_VENDOR', series: str = 'FAKE_SERIES'):
    """获取所有外设的模板路径

    Args:
        project_root: 项目根目录
        vendor: 厂商名称，如 'Artery', 'FAKE_VENDOR'
        series: 系列名称，如 'AT32F402_405', 'FAKE_SERIES'
    """
    api_dir = project_root / 'source/hal/driver/common/template'
    impl_dir = project_root / 'source/hal/driver' / vendor / series / 'common'

    return [
        (name, api_dir / f'vsf_template_{name}.h', impl_dir / name / f'{name}.c')
        for name in PERIPHERAL_NAMES
        if (api_dir / f'vsf_template_{name}.h').exists()
        and (impl_dir / name / f'{name}.c').exists()
    ]


def merge_header_into_impl(impl: TemplateExtractor, impl_file: Path, verbose: bool = False):
    """合并实现模板的头文件"""
    header = impl_file.parent / f"{impl_file.stem}.h"
    if not header.exists():
        return

    impl_h = TemplateExtractor(header, is_impl_template=True, verbose=verbose)
    if not impl_h.parse():
        return

    impl_h.extract()
    impl.types.update(impl_h.types)
    impl.functions.update(impl_h.functions)

    # 合并类型成员
    for type_name, members in impl_h.type_members.items():
        if type_name not in impl.type_members:
            impl.type_members[type_name] = members
        else:
            existing = {m['name'] for m in impl.type_members[type_name]}
            impl.type_members[type_name].extend(
                m for m in members if m['name'] not in existing
            )


def extract_templates(api_file: Path, impl_file: Path, verbose: bool = False):
    """提取 API 和实现模板"""
    # 提取 API 模板（基础版本和完整版本）
    api_base = TemplateExtractor(api_file, is_impl_template=False, enable_standard_optional=False, verbose=verbose)
    api_full = TemplateExtractor(api_file, is_impl_template=False, enable_standard_optional=True, verbose=verbose)

    if not (api_base.parse() and api_full.parse()):
        return None, None, None, None

    api_base.extract()
    api_full.extract()

    # 提取标准可选功能
    standard_optional = StandardOptionalExtractor(api_base, api_full)
    standard_optional.extract()

    # 提取实现模板
    impl = TemplateExtractor(impl_file, is_impl_template=True, verbose=verbose)
    if not impl.parse():
        return None, None, None, None

    impl.extract()
    merge_header_into_impl(impl, impl_file, verbose)

    return api_base, api_full, impl, standard_optional


def compare_peripheral(name: str, api_file: Path, impl_file: Path,
                       output_dir: Optional[Path] = None,
                       libclang_path: Optional[str] = None,
                       verbose: bool = False,
                       allow_extra: bool = False) -> Tuple[str, int, Dict]:
    """对比单个外设（多进程调用）"""
    try:
        if libclang_path:
            cindex.Config.set_library_file(libclang_path)

        # 提取模板
        api_base, api_full, impl, standard_optional = extract_templates(api_file, impl_file, verbose)
        if api_full is None:
            return name, -1, {'error': '模板解析失败'}

        # 对比
        comparator = TemplateComparator(api_full, impl, standard_optional, strict=not allow_extra)
        comparator.compare()

        # 保存结果
        if output_dir:
            save_comparison_results(name, api_base, api_full, impl, standard_optional,
                                   comparator.differences, output_dir)

        return name, len(comparator.differences), {
            'api_types': len(api_full.types),
            'api_functions': len(api_full.functions),
            'impl_types': len(impl.types),
            'impl_functions': len(impl.functions),
            'optional_types': len(standard_optional.optional_types),
            'optional_functions': len(standard_optional.optional_functions),
            'differences': comparator.differences,
        }

    except Exception as e:
        return name, -1, {'error': str(e)}


def print_results(results: Dict, peripherals_count: int):
    """打印统计结果"""
    success = sum(1 for diff, _ in results.values() if diff == 0)
    failed = sum(1 for diff, _ in results.values() if diff < 0)
    has_diff = peripherals_count - success - failed

    print(f"\n{'='*60}")
    print(f"总外设数: {peripherals_count}")
    print(f"一致: {success}, 有差异: {has_diff}, 失败: {failed}")
    print(f"{'='*60}")


def main():
    """主函数"""
    import argparse
    from os import cpu_count

    parser = argparse.ArgumentParser(description='批量对比所有外设的 API 模板和实现模板')
    parser.add_argument('-l', '--libclang', help='libclang 库路径')
    parser.add_argument('-o', '--output', help='输出目录')
    parser.add_argument('-j', '--jobs', type=int, help='并行进程数')
    parser.add_argument('-v', '--vendor', default='FAKE_VENDOR', help='厂商名称，如 Artery, FAKE_VENDOR (默认: FAKE_VENDOR)')
    parser.add_argument('-s', '--series', default='FAKE_SERIES', help='系列名称，如 AT32F402_405, FAKE_SERIES (默认: FAKE_SERIES)')
    parser.add_argument('--verbose', action='store_true', help='显示详细的解析和诊断信息')
    parser.add_argument('--allow-extra', action='store_true', help='允许驱动实现比API模板有更多的定义（类型、函数、成员等）。默认：严格模式，驱动实现必须与API模板完全一致')
    args = parser.parse_args()

    # 配置 libclang
    if not configure_libclang(args.libclang):
        sys.exit(1)

    # 获取外设列表
    project_root = find_project_root()
    peripherals = get_peripherals(project_root, args.vendor, args.series)

    if not peripherals:
        print(f"错误: 未找到任何外设模板 (vendor={args.vendor}, series={args.series})")
        print(f"实现目录: {project_root / 'source/hal/driver' / args.vendor / args.series / 'common'}")
        sys.exit(1)

    # 创建输出目录
    output_dir = None
    if args.output:
        output_dir = Path(args.output)
        if not output_dir.is_absolute():
            output_dir = (project_root / output_dir).resolve()
        output_dir.mkdir(parents=True, exist_ok=True)

    print(f"厂商: {args.vendor}, 系列: {args.series}")
    print(f"找到 {len(peripherals)} 个外设，开始对比...")

    # 并行处理
    max_workers = args.jobs or min(len(peripherals), cpu_count() or 1)
    results = {}

    with ProcessPoolExecutor(max_workers=max_workers) as executor:
        futures = {
            executor.submit(compare_peripheral, name, api, impl, output_dir, args.libclang, args.verbose, args.allow_extra): name
            for name, api, impl in peripherals
        }

        for future in as_completed(futures):
            name, diff_count, info = future.result()
            results[name] = (diff_count, info)

            # 打印进度
            if diff_count < 0:
                print(f"[ERROR] {name.upper()}: {info.get('error', '未知错误')}")
            elif diff_count == 0:
                print(f"[OK] {name.upper()}")
            else:
                print(f"[DIFF] {name.upper()}: {diff_count} 个差异")
                # 打印详细差异信息
                differences = info.get('differences', [])
                for i, diff in enumerate(differences, 1):
                    category = diff.get('category', 'unknown')
                    severity = diff.get('severity', 'unknown')
                    diff_name = diff.get('name', '')
                    message = diff.get('message', '')

                    # 格式化输出
                    severity_text = {
                        'missing': '缺失',
                        'extra': '额外',
                        'mismatch': '不匹配'
                    }.get(severity, severity)

                    category_text = {
                        'type': '类型',
                        'function': '函数',
                        'type_member': '类型成员'
                    }.get(category, category)

                    print(f"  [{i}] {category_text} - {severity_text}: {diff_name}")
                    print(f"       {message}")

                    # 打印位置信息
                    if 'api_location' in diff:
                        print(f"      API位置: {diff['api_location']}")
                    if 'impl_location' in diff:
                        print(f"      实现位置: {diff['impl_location']}")

                    # 打印签名信息（如果有）
                    if 'api_signature' in diff:
                        print(f"      API签名: {diff['api_signature']}")
                    if 'impl_signature' in diff:
                        print(f"      实现签名: {diff['impl_signature']}")

                    # 打印类型信息（对于类型成员不匹配）
                    if 'api_type' in diff and 'impl_type' in diff:
                        print(f"      API类型: {diff['api_type']}")
                        print(f"      实现类型: {diff['impl_type']}")

    # 打印统计
    print_results(results, len(peripherals))

    # 保存汇总
    if output_dir:
        save_summary(results, output_dir)
        print(f"结果已保存到: {output_dir}")

    # 计算总差异数和错误数
    total_diffs = sum(diff for diff, _ in results.values() if diff > 0)
    total_errors = sum(1 for diff, _ in results.values() if diff < 0)

    # 返回错误码逻辑
    if total_errors > 0:
        # 有解析错误，返回 1
        sys.exit(1)
    elif args.allow_extra:
     # 宽松模式：只有 missing（缺失）才算错误
        critical_diffs = 0
        for name, (diff_count, info) in results.items():
            if diff_count > 0:
                differences = info.get('differences', [])
                for diff in differences:
                    # 只统计 missing 类型的差异
                    if diff.get('severity') == 'missing':
                        critical_diffs += 1
        
        if critical_diffs > 0:
            exit_code = min(critical_diffs, 255)
            sys.exit(exit_code)
        else:
            sys.exit(0)
    else:
        # 严格模式：所有差异都算错误
        if total_diffs > 0:
            exit_code = min(total_diffs, 255)
            sys.exit(exit_code)
        else:
            sys.exit(0)


if __name__ == '__main__':
    main()
