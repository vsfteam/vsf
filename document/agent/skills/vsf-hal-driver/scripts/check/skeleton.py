#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
check-driver-skeleton.py - 驱动代码骨架相似度检查

从 HAL 驱动模板中提取"不应该被修改"的骨架代码，检查实际驱动是否保留了这些骨架。

用法:
    python3 check-driver-skeleton.py <template.c> <driver.c>

示例:
    python3 check-driver-skeleton.py \\
        vsf/source/hal/driver/template/__series_name_a__/common/usart/usart.c \\
        vsf/source/hal/driver/RaspberryPi/RP2040/uart/uart.c
"""

import re
import sys
import argparse
from pathlib import Path
from dataclasses import dataclass
from typing import List, Tuple, Optional

# Bootstrap: ensure scripts/ is on sys.path so _lib is importable
_SCRIPTS_DIR = Path(__file__).parent.parent.resolve()
if str(_SCRIPTS_DIR) not in sys.path:
    sys.path.insert(0, str(_SCRIPTS_DIR))

from _lib.checker import extract_functions, _parser, _walk_all


@dataclass
class Anchor:
    """基准点 — 模板中不应该被修改的代码片段"""
    kind: str           # 类型: function_sig, macro, struct_decl, multiline_macro, include
    name: str           # 标识名（如函数名、宏名）
    text: str           # 规范化后的文本（用于匹配）
    raw_text: str       # 原始文本（用于显示）
    line_no: int        # 在模板中的起始行号
    is_flexible: bool   # 是否允许值不同


_VSF_MCONNECT_RE = re.compile(r'VSF_MCONNECT\s*\([^)]+\)', re.DOTALL)


def preprocess_template(content: str) -> str:
    """预处理模板：只保留 HW 路径，去除 IPCore 路径和条件编译注释"""
    lines = content.split('\n')
    result = []
    in_hw = False
    in_ipcore = False

    for line in lines:
        stripped = line.strip()
        if stripped == '// HW':
            in_hw = True
            continue
        elif stripped == '// HW end':
            in_hw = False
            continue
        elif stripped == '// IPCore':
            in_ipcore = True
            continue
        elif stripped == '// IPCore end':
            in_ipcore = False
            continue

        if in_hw or (not in_hw and not in_ipcore):
            result.append(line)

    return '\n'.join(result)


def normalize_signature(text: str) -> str:
    """规范化函数签名：统一所有不影响语义的空白差异"""
    text = ' '.join(text.split())
    # 去掉多行宏反斜杠
    text = text.replace(' \\ ', ' ')
    text = text.replace('\\ ', ' ')
    text = text.replace(' \\', ' ')
    # 指针声明：void * param / void *param -> void*param
    text = re.sub(r'\*\s+(?=[a-zA-Z_])', '*', text)
    # 括号紧贴：param ) { -> param){ 或 param) { -> param){
    text = re.sub(r'\)\s*\{', '){', text)
    # 逗号后空格标准化
    text = re.sub(r',\s+', ',', text)
    # 左括号后空格去除
    text = re.sub(r'\(\s+', '(', text)
    # 右括号前空格去除
    text = re.sub(r'\s+\)', ')', text)
    # 保留关键字之间的空格（如 typedef struct）
    text = re.sub(r'typedef\s+struct', 'typedef struct', text)
    return text


def normalize_whitespace(text: str) -> str:
    """通用空白规范化"""
    return ' '.join(text.split())


def extract_macro_name(line: str) -> Optional[str]:
    """从 #define 行提取宏名"""
    match = re.match(r'#define\s+([A-Za-z_][A-Za-z0-9_]*)', line.strip())
    return match.group(1) if match else None


def _extract_vsf_mconnect_name(sig_text: str) -> Optional[str]:
    """从函数签名中提取 VSF_MCONNECT(...) 完整文本（压缩为单行）"""
    m = _VSF_MCONNECT_RE.search(sig_text)
    if m:
        return ' '.join(m.group(0).split())
    return None


def parse_signature(sig_text: str) -> dict:
    """解析函数签名，提取返回类型、函数名、参数列表"""
    # 去掉末尾的 {
    sig = sig_text.rstrip().rstrip('{').strip()

    # 提取参数列表 ( ... )
    param_match = re.search(r'\((.*)\)\s*$', sig, re.DOTALL)
    params_str = param_match.group(1) if param_match else ''

    # 提取函数名之前的部分（返回类型）
    before_params = sig[:param_match.start()] if param_match else sig

    # 提取 VSF_MCONNECT(...) 作为函数名
    func_match = re.search(r'(VSF_MCONNECT\s*\([^)]+\))\s*$', before_params.strip())
    func_name = func_match.group(1) if func_match else before_params.strip().split()[-1]
    ret_type = before_params[:func_match.start()].strip() if func_match else ''

    # 解析参数
    params = []
    if params_str.strip():
        # 按逗号分割参数，但处理嵌套括号
        depth = 0
        current = ''
        for c in params_str:
            if c == '(':
                depth += 1
            elif c == ')':
                depth -= 1
            elif c == ',' and depth == 0:
                params.append(current.strip())
                current = ''
                continue
            current += c
        if current.strip():
            params.append(current.strip())

    return {
        'ret_type': ret_type,
        'func_name': func_name,
        'params': params,
        'raw': sig_text,
    }


def compare_signatures(template_sig: str, driver_sig: str) -> Tuple[bool, str]:
    """详细比较两个函数签名，返回 (是否匹配, 差异描述)"""
    t = parse_signature(template_sig)
    d = parse_signature(driver_sig)

    diffs = []

    # 比较返回类型
    t_ret = normalize_signature(t['ret_type'])
    d_ret = normalize_signature(d['ret_type'])
    if t_ret != d_ret:
        diffs.append(f"返回类型: 模板'{t_ret}' vs 驱动'{d_ret}'")

    # 比较参数数量
    if len(t['params']) != len(d['params']):
        diffs.append(f"参数数量: 模板{len(t['params'])}个 vs 驱动{len(d['params'])}个")
    else:
        # 逐个参数比较
        for i, (tp, dp) in enumerate(zip(t['params'], d['params'])):
            tp_norm = normalize_signature(tp)
            dp_norm = normalize_signature(dp)
            if tp_norm != dp_norm:
                # 检查是类型不同还是参数名不同
                tp_type = re.sub(r'\s*[a-zA-Z_][a-zA-Z0-9_]*\s*$', '', tp_norm).strip()
                dp_type = re.sub(r'\s*[a-zA-Z_][a-zA-Z0-9_]*\s*$', '', dp_norm).strip()
                tp_name = tp_norm[len(tp_type):].strip() if tp_type else tp_norm
                dp_name = dp_norm[len(dp_type):].strip() if dp_type else dp_norm

                if tp_type != dp_type:
                    diffs.append(f"参数{i+1}类型: 模板'{tp}' vs 驱动'{dp}'")
                elif tp_name != dp_name:
                    diffs.append(f"参数{i+1}名称: 模板'{tp_name}' vs 驱动'{dp_name}'")
                else:
                    diffs.append(f"参数{i+1}: 模板'{tp}' vs 驱动'{dp}'")

    if diffs:
        return False, '; '.join(diffs[:3])
    return True, ''


def find_function_in_driver(func_name: str, driver_content: str) -> Optional[str]:
    """在驱动中查找指定 VSF_MCONNECT(...) 函数名的完整签名"""
    funcs = extract_functions(driver_content)
    for func in funcs:
        # 从 func["lines"] 中提取签名（到第一个 { 为止）
        sig_lines = []
        for line in func["lines"]:
            sig_lines.append(line)
            if '{' in line:
                break
        sig = '\n'.join(sig_lines)
        if func_name in sig:
            return sig
    return None


def extract_anchors(content: str) -> List[Anchor]:
    """从模板内容中提取所有基准点（使用 tree-sitter）

    *content* 应当已经由 preprocess_template() 过滤过 IPCore 块。
    """
    tree = _parser().parse(content.encode(), encoding="utf8")
    root = tree.root_node
    anchors: List[Anchor] = []

    for node in _walk_all(root):
        kind = node.type

        if kind == "function_definition":
            # 找到 compound_statement 子节点，签名在它之前
            compound = None
            for child in node.children:
                if child.type == "compound_statement":
                    compound = child
                    break
            if compound is None:
                continue

            sig = node.text[:compound.start_byte - node.start_byte].decode()
            func_name = _extract_vsf_mconnect_name(sig)
            if not func_name:
                continue

            anchors.append(Anchor(
                kind='function_sig',
                name=func_name,
                text=normalize_signature(sig),
                raw_text=sig,
                line_no=node.start_point[0] + 1,
                is_flexible=False
            ))

        elif kind == "preproc_function_def":
            macro_text = node.text.decode()
            first_line = macro_text.split('\n')[0]
            macro_name = extract_macro_name(first_line)
            if macro_name and '_IMP_LV0' in macro_name:
                anchors.append(Anchor(
                    kind='multiline_macro',
                    name=macro_name,
                    text=normalize_whitespace(macro_text),
                    raw_text=macro_text,
                    line_no=node.start_point[0] + 1,
                    is_flexible=False
                ))

        elif kind == "preproc_def":
            macro_text = node.text.decode().strip()
            macro_name = extract_macro_name(macro_text)
            if macro_name and macro_name.startswith('VSF_'):
                key_suffixes = [
                    '_CFG_IMP_PREFIX',
                    '_CFG_IMP_UPCASE_PREFIX',
                    '_CFG_MULTI_CLASS',
                    '_CFG_REIMPLEMENT_API_',
                    '_CFG_MODE_CHECK_UNIQUE',
                    '_CFG_IRQ_MASK_CHECK_UNIQUE',
                ]
                if any(s in macro_name for s in key_suffixes):
                    anchors.append(Anchor(
                        kind='macro',
                        name=macro_name,
                        text=normalize_whitespace(macro_text),
                        raw_text=macro_text,
                        line_no=node.start_point[0] + 1,
                        is_flexible=True
                    ))

        elif kind == "type_definition":
            td_text = node.text.decode().strip()
            first_line = td_text.split('\n')[0].strip()
            if 'typedef struct' in first_line and 'VSF_MCONNECT' in first_line:
                anchors.append(Anchor(
                    kind='struct_decl',
                    name='struct_decl',
                    text=normalize_signature(first_line),
                    raw_text=first_line,
                    line_no=node.start_point[0] + 1,
                    is_flexible=False
                ))

        elif kind == "preproc_include":
            inc_text = node.text.decode().strip()
            if '_template.inc' in inc_text:
                anchors.append(Anchor(
                    kind='include',
                    name='_template.inc',
                    text=normalize_whitespace(inc_text),
                    raw_text=inc_text,
                    line_no=node.start_point[0] + 1,
                    is_flexible=False
                ))

    return anchors


def match_anchor(anchor: Anchor, driver_content: str) -> Tuple[bool, Optional[str]]:
    """在驱动内容中匹配基准点"""

    # 函数签名：使用详细比较
    if anchor.kind == 'function_sig':
        driver_sig = find_function_in_driver(anchor.name, driver_content)
        if not driver_sig:
            return False, "函数未实现"

        driver_norm = normalize_signature(driver_sig)
        if anchor.text == driver_norm:
            return True, None

        # 详细比较
        ok, detail = compare_signatures(anchor.raw_text, driver_sig)
        if ok:
            return True, None
        return False, detail

    # 多行宏：骨架匹配
    if anchor.kind == 'multiline_macro':
        driver_norm = normalize_signature(driver_content)
        key_patterns = []
        for raw_line in anchor.raw_text.split('\n'):
            line = raw_line.strip().rstrip('\\').strip()
            if not line:
                continue

            norm = normalize_signature(line)

            if line.startswith('#define'):
                key_patterns.append(('exact', norm))
            elif re.search(r'VSF_MCONNECT\s*\([^)]*_t\)', line):
                key_patterns.append(('exact', norm))
            elif re.search(r'VSF_MCONNECT\s*\([^)]+\)\s*=\s*\{', line):
                key_patterns.append(('exact', norm))
            elif re.match(r'\.[A-Za-z_][A-Za-z0-9_]*\s*=', line):
                field_match = re.match(r'(\.[A-Za-z_][A-Za-z0-9_]*\s*=)', line)
                if field_match:
                    key_patterns.append(('field', field_match.group(1).strip()))
            elif '__HAL_OP' in line or line == '};':
                key_patterns.append(('exact', norm))
            elif re.search(r'void\s+VSF_MCONNECT\s*\([^)]+_IRQHandler\)', line):
                key_patterns.append(('exact', norm))
            elif 'vsf_hal_irq_enter' in line or 'vsf_hal_irq_leave' in line:
                key_patterns.append(('exact', norm))
            elif re.search(r'VSF_MCONNECT\s*\(__,\s*[^)]+_irqhandler\)\s*\(', line):
                key_patterns.append(('exact', norm))
            elif line == '&' or line.startswith('&VSF_MCONNECT'):
                key_patterns.append(('exact', norm))
            elif line in ('{', '}', ');', '('):
                key_patterns.append(('exact', norm))

        missing = []
        for ptype, pattern in key_patterns:
            if ptype == 'exact':
                if pattern not in driver_norm:
                    missing.append(pattern[:50])
            elif ptype == 'field':
                field_name = pattern.rstrip('=').strip()
                field_re = re.escape(field_name) + r'\s*=\s*[^;,}]+'
                if not re.search(field_re, driver_content):
                    missing.append(f"{field_name} = ...")

        if not missing:
            return True, None
        return False, f"缺少 {len(missing)} 个骨架: {', '.join(missing[:3])}"

    # 宏定义：检查宏名是否存在
    if anchor.kind == 'macro' and anchor.is_flexible:
        name_part = anchor.text.split(None, 2)[:2]
        if len(name_part) >= 2:
            name_pattern = ' '.join(name_part)
            if name_pattern in normalize_whitespace(driver_content):
                return True, None
        return False, "宏未定义（使用模板默认值）"

    # 其他：精确匹配
    if anchor.kind == 'struct_decl':
        driver_norm = normalize_signature(driver_content)
    else:
        driver_norm = normalize_whitespace(driver_content)
    if anchor.text in driver_norm:
        return True, None

    return False, "未找到"


def check_skeleton(template_path: Path, driver_path: Path) -> dict:
    """检查驱动与模板的骨架相似度"""
    template_content = template_path.read_text()
    driver_content = driver_path.read_text()

    preprocessed = preprocess_template(template_content)
    anchors = extract_anchors(preprocessed)

    matched = []
    mismatched = []

    for anchor in anchors:
        ok, reason = match_anchor(anchor, driver_content)
        if ok:
            matched.append(anchor)
        else:
            mismatched.append((anchor, reason))

    return {
        'template': str(template_path),
        'driver': str(driver_path),
        'total': len(anchors),
        'matched': len(matched),
        'mismatched': len(mismatched),
        'matched_anchors': matched,
        'mismatched_anchors': mismatched,
    }


def print_report(result: dict):
    """打印检查报告"""
    print("=" * 70)
    print("驱动骨架相似度检查报告")
    print("=" * 70)
    print(f"模板: {result['template']}")
    print(f"驱动: {result['driver']}")
    print("")
    print(f"总基准点数: {result['total']}")
    print(f"匹配:       {result['matched']}")
    print(f"不匹配:     {result['mismatched']}")

    if result['total'] > 0:
        ratio = result['matched'] / result['total'] * 100
        print(f"相似度:     {ratio:.1f}%")

    print("")

    if result['mismatched_anchors']:
        print("-" * 70)
        print("不匹配的基准点:")
        print("-" * 70)
        for anchor, reason in result['mismatched_anchors']:
            print(f"\n  [{anchor.kind}] {anchor.name}")
            print(f"  模板第 {anchor.line_no} 行")
            print(f"  差异: {reason}")
            if '\n' in anchor.raw_text:
                print("  模板内容:")
                for line in anchor.raw_text.split('\n'):
                    print(f"    {line}")
            else:
                print(f"  模板: {anchor.raw_text[:100]}")

    if result['matched_anchors']:
        print(f"\n{'-' * 70}")
        print("已匹配的基准点:")
        print("-" * 70)
        for anchor in result['matched_anchors']:
            status = "(值可不同)" if anchor.is_flexible else ""
            print(f"  [{anchor.kind}] {anchor.name} {status}")

    print(f"\n{'=' * 70}")
    if result['mismatched'] == 0:
        print("结果: 全部匹配 ✓")
    else:
        print(f"结果: {result['mismatched']} 个基准点不匹配")
    print("=" * 70)


def main():
    parser = argparse.ArgumentParser(
        description='检查驱动代码与模板骨架的相似度',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  # 检查 USART 驱动
  python3 %(prog)s \\
    vsf/source/hal/driver/template/__series_name_a__/common/usart/usart.c \\
    vsf/source/hal/driver/RaspberryPi/RP2040/uart/uart.c

  # 检查 PWM 驱动
  python3 %(prog)s \\
    vsf/source/hal/driver/template/__series_name_a__/common/pwm/pwm.c \\
    vsf/source/hal/driver/RaspberryPi/RP2040/pwm/pwm.c
        """
    )
    parser.add_argument('template', type=Path, help='模板文件路径 (.c)')
    parser.add_argument('driver', type=Path, help='驱动文件路径 (.c)')
    args = parser.parse_args()

    if not args.template.exists():
        print(f"错误: 模板文件不存在: {args.template}", file=sys.stderr)
        sys.exit(2)
    if not args.driver.exists():
        print(f"错误: 驱动文件不存在: {args.driver}", file=sys.stderr)
        sys.exit(2)

    result = check_skeleton(args.template, args.driver)
    print_report(result)

    if result['mismatched'] > 0:
        sys.exit(1)


if __name__ == '__main__':
    main()
