#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
compare_template_core.py - 核心类定义

包含:
- ClangConfig: Clang 编译配置
- TemplateExtractor: AST 提取器（只提取原始信息，不做规范化）
- StandardOptionalExtractor: 标准可选功能提取器
- TemplateComparator: 差异对比器（在对比时进行规范化）
- FalsePositiveDetector: 假阳性检测器

设计原则:
- TemplateExtractor 只负责提取原始 AST 信息，不进行任何规范化处理
- TemplateComparator 在对比时进行规范化（通过内部方法）
- 这样保持了单一职责原则，提取和规范化分离
"""

import re
from pathlib import Path
from clang import cindex
from collections import defaultdict
from typing import Dict, List, Set, Optional, Tuple, Any


# ============================================================================
# 常量配置
# ============================================================================

# 所有外设名称列表（排除 USB）
PERIPHERAL_NAMES = [
    'adc', 'dac', 'dma', 'eth', 'exti', 'flash', 'gpio',
    'i2c', 'i2s', 'pm', 'pwm', 'rng', 'rtc', 'sdio',
    'spi', 'timer', 'usart', 'wdt'
]


# ============================================================================
# Clang 配置
# ============================================================================

class ClangConfig:
    """libclang 编译配置"""

    @staticmethod
    def get_compile_args(project_root: Path, source_dir: Path, peripheral_name: Optional[str] = None,
                         enable_standard_optional: bool = False) -> List[str]:
        """获取编译参数列表

        Args:
            project_root: 项目根目录
            source_dir: 源文件目录
            peripheral_name: 外设名称（小写），如果提供，会添加对应的宏定义
            enable_standard_optional: 是否开启标准可选功能宏
        """
        args = [
            '-x', 'c',
            '-std=c11',
            '-I.',
            '-I' + str(source_dir),
            '-I' + str(project_root / 'source'),
            '-I' + str(project_root / 'source' / 'hal'),
            '-I' + str(project_root / 'source' / 'hal' / 'driver' / 'common' / 'template'),
            '-I' + str(project_root / 'source' / 'hal' / 'driver' / 'common'),
            '-I' + str(project_root / 'source' / 'hal' / 'arch'),
            '-D__GNUC__',
            '-D__VSF__',
            '-DENABLED=1',
            '-DDISABLED=0',
            '-DVSF_DMA_CFG_MULTI_CLASS=ENABLED',
            '-DVSF_DMA_CFG_FUNCTION_RENAME=ENABLED',
            '-DVSF_USART_CFG_MULTI_CLASS=ENABLED',
            '-DVSF_USART_CFG_FUNCTION_RENAME=ENABLED',
            '-DVSF_FLASH_CFG_ERASE_ALL_TEMPLATE=ENABLED',
            '-DVSF_FLASH_CFG_ERASE_ONE_SECTOR_TEMPLATE=ENABLED',
            '-DVSF_FLASH_CFG_WRITE_ONE_SECTOR_TEMPLATE=ENABLED',
            '-DVSF_FLASH_CFG_READ_ONE_SECTOR_TEMPLATE=ENABLED',
            '-DVSF_FLASH_CFG_IMP_PREFIX=vsf_hw',
            '-DVSF_FLASH_CFG_IMP_UPCASE_PREFIX=VSF_HW',
            '-Wno-everything',
        ]

        # 如果提供了外设名称且开启标准可选功能，添加对应的宏定义
        if peripheral_name and enable_standard_optional:
            peripheral_upper = peripheral_name.upper()
            # 添加 __VSF_{PERIPHERAL}_CFG_SUPPORT_STANDARD_OPTIONAL 宏
            args.append(f'-D__VSF_{peripheral_upper}_CFG_SUPPORT_STANDARD_OPTIONAL')

        return args

    @staticmethod
    def get_parse_options():
        """获取解析选项"""
        return (
            cindex.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD |
            cindex.TranslationUnit.PARSE_INCOMPLETE
        )


# ============================================================================
# 模板提取器
# ============================================================================

class TemplateExtractor:
    """提取模板中的类型、函数等信息"""

    def __init__(self, source_file: Path, is_impl_template: bool = False,
                 enable_standard_optional: bool = False, verbose: bool = False):
        self.source_file = source_file.resolve()
        self.is_impl_template = is_impl_template
        self.enable_standard_optional = enable_standard_optional
        self.verbose = verbose
        self.tu = None
        self.types: Dict[str, Dict] = {}
        self.functions: Dict[str, Dict] = {}
        self.type_members: Dict[str, List[Dict]] = defaultdict(list)
        self.peripheral_name = self._extract_peripheral_name()

        script_dir = Path(__file__).parent.resolve()
        self._project_root = script_dir.parent.parent.parent
        self._common_dir = self._project_root / 'source' / 'hal' / 'driver' / 'common'
        self._common_dir_resolved = self._common_dir.resolve()
        self._file_check_cache: Dict[str, bool] = {}

    def _extract_peripheral_name(self) -> Optional[str]:
        """从源文件路径提取外设名称"""
        file_name = self.source_file.stem

        if file_name.startswith('vsf_template_'):
            return file_name.replace('vsf_template_', '')

        if self.source_file.parent.name in PERIPHERAL_NAMES:
            return self.source_file.parent.name

        if file_name in PERIPHERAL_NAMES:
            return file_name

        return None

    def parse(self) -> bool:
        """解析源文件"""
        source_dir = self.source_file.parent
        args = ClangConfig.get_compile_args(self._project_root, source_dir, self.peripheral_name,
                                            self.enable_standard_optional)

        try:
            index = cindex.Index.create()
            if self.verbose:
                print(f"正在解析文件: {self.source_file}")

            parse_options = ClangConfig.get_parse_options()

            self.tu = index.parse(str(self.source_file), args=args, options=parse_options)

            if self.verbose and self.tu.diagnostics:
                for diag in self.tu.diagnostics:
                    print(f"  编译诊断 [{diag.severity}]: {diag.spelling}")

            if self.verbose:
                print("解析完成")
            return True

        except Exception as e:
            print(f"解析错误: {e}")
            import traceback
            traceback.print_exc()
            return False


    def is_in_target_file(self, cursor) -> bool:
        """检查 cursor 是否在目标文件中

        对于 API 模板：只接受主文件
        对于实现模板：接受主文件 + 相关的 .inc 和 .h 模板文件
        """
        if not cursor.location.file:
            return False

        file_path = Path(str(cursor.location.file))

        # 主文件总是接受
        if file_path == self.source_file:
            return True

        # API 模板只接受主文件
        if not self.is_impl_template:
            return False

        # 使用缓存避免重复计算
        file_str = str(file_path)
        if file_str in self._file_check_cache:
            return self._file_check_cache[file_str]

        # 实现模板：检查是否是相关的模板文件
        result = False
        if self.peripheral_name:
            try:
                # 检查是否在 common 目录下
                if self._common_dir_resolved in file_path.resolve().parents:
                    # .inc 文件：必须在 common/{peripheral}/ 目录下且文件名包含外设名
                    if file_path.suffix == '.inc':
                        result = (file_path.parent.name == self.peripheral_name and
                                 self.peripheral_name in file_path.stem.lower())
                    # .h 文件：必须是 vsf_template_{peripheral}.h
                    elif file_path.suffix == '.h':
                        result = file_path.stem == f'vsf_template_{self.peripheral_name}'
            except (OSError, ValueError):
                result = False

        self._file_check_cache[file_str] = result
        return result

    def get_function_signature(self, cursor) -> str:
        """获取函数的原始签名（返回类型 + 参数类型，忽略参数名）

        注意：这里返回的是原始签名，不进行规范化。
        规范化应该在对比阶段由 TypeNormalizer 完成。
        """
        if cursor.kind != cindex.CursorKind.FUNCTION_DECL:
            return ""

        result_type = cursor.result_type.spelling if cursor.result_type else "void"

        args = []
        for child in cursor.get_children():
            if child.kind == cindex.CursorKind.PARM_DECL:
                arg_type = child.type.spelling if child.type else "unknown"
                args.append(arg_type)

        return f"{result_type}({', '.join(args)})"

    def extract_type_members(self, cursor, type_name: str):
        """提取类型成员（结构体字段、枚举值等）"""
        members = []
        for child in cursor.get_children():
            if not self.is_in_target_file(child):
                continue

            if child.kind == cindex.CursorKind.FIELD_DECL:
                member_name = child.spelling or child.displayname
                if member_name:
                    member_info = {
                        'name': member_name,
                        'type': child.type.spelling if child.type else "",
                        'kind': 'field'
                    }
                    members.append(member_info)
            elif child.kind == cindex.CursorKind.ENUM_CONSTANT_DECL:
                member_name = child.spelling or child.displayname
                if member_name:
                    member_info = {
                        'name': member_name,
                        'value': self.get_enum_value(child),
                        'kind': 'enum_value'
                    }
                    members.append(member_info)

        if members:
            self.type_members[type_name] = members

    def get_enum_value(self, cursor) -> Optional[str]:
        """获取枚举值"""
        for child in cursor.get_children():
            if child.kind == cindex.CursorKind.INTEGER_LITERAL:
                tokens = list(child.get_tokens())
                if tokens:
                    return tokens[0].spelling
            elif child.kind == cindex.CursorKind.UNEXPOSED_EXPR:
                # 对于复杂表达式（如 (1 << 0)），提取所有 tokens
                tokens = list(child.get_tokens())
                if tokens:
                    # 组合所有 tokens 得到完整表达式
                    return ' '.join([t.spelling for t in tokens])
        return None

    def to_dict(self, impl_files: Optional[List[str]] = None) -> Dict[str, Any]:
        """将提取的信息转换为字典（用于保存 AST 信息）"""
        result = {
            'source_file': str(self.source_file),
            'file_name': self.source_file.name,
            'template_type': 'impl' if self.is_impl_template else 'api',
            'is_impl_template': self.is_impl_template,
            'enable_standard_optional': self.enable_standard_optional,
            'types': self.types,
            'functions': self.functions,
            'type_members': dict(self.type_members)
        }
        if impl_files and len(impl_files) > 1:
            result['files'] = impl_files
        return result


    def _extract_named_type(self, cursor, kind: str):
        """提取命名类型（结构体或枚举）"""
        name = cursor.spelling or cursor.displayname
        if name:
            self.types[name] = {
                'name': name,
                'kind': kind,
                'signature': cursor.type.spelling if cursor.type else "",
                'location': f"{cursor.location.line}:{cursor.location.column}"
            }
            self.extract_type_members(cursor, name)

    def _extract_typedef(self, cursor):
        """提取 typedef 类型"""
        name = cursor.spelling or cursor.displayname
        if not name:
            return

        typedef_type = self._get_typedef_base_type(cursor, name)

        self.types[name] = {
            'name': name,
            'kind': 'typedef',
            'signature': typedef_type,
            'location': f"{cursor.location.line}:{cursor.location.column}"
        }

    def _get_typedef_base_type(self, cursor, typedef_name: str) -> str:
        """获取 typedef 的基础类型"""
        # 首先尝试从子节点获取（对于 typedef struct/enum）
        for child in cursor.get_children():
            if child.kind in [cindex.CursorKind.STRUCT_DECL, cindex.CursorKind.ENUM_DECL]:
                return child.spelling or child.displayname or ""
            elif child.type:
                child_type_spelling = child.type.spelling
                if child_type_spelling:
                    return child_type_spelling

        # 尝试从 token 获取（对于简单的 typedef，如 typedef uint16_t xxx_t）
        try:
            tokens = list(cursor.get_tokens())
            for i, token in enumerate(tokens):
                if token.spelling == 'typedef' and i + 1 < len(tokens):
                    next_token = tokens[i + 1]
                    if next_token.kind == cindex.TokenKind.IDENTIFIER:
                        return next_token.spelling
        except Exception:
            pass

        # 最后尝试从 cursor.type 获取
        if cursor.type:
            type_spelling = cursor.type.spelling
            if type_spelling and type_spelling != typedef_name:
                return type_spelling

        return ""

    def _extract_function(self, cursor):
        """提取函数声明"""
        if not self.is_in_target_file(cursor):
            return

        name = cursor.spelling or cursor.displayname
        if not name or name.startswith('__'):
            return

        # 获取函数修饰符
        is_static, is_inline = self._get_function_modifiers(cursor)

        # 存储原始函数名（不在此处规范化）
        # 规范化将在对比阶段由 TypeNormalizer 完成
        self.functions[name] = {
            'name': name,
            'signature': self.get_function_signature(cursor),
            'location': f"{cursor.location.line}:{cursor.location.column}",
            'is_static': is_static,
            'is_inline': is_inline
        }

    def _get_function_modifiers(self, cursor) -> Tuple[bool, bool]:
        """获取函数的修饰符（static, inline）"""
        is_static = False
        is_inline = False
        try:
            for token in cursor.get_tokens():
                if token.spelling == 'static':
                    is_static = True
                elif token.spelling == 'inline':
                    is_inline = True
        except Exception:
            pass
        return is_static, is_inline


    def _traverse(self, cursor):
        """递归遍历 AST 节点"""
        if not self.is_in_target_file(cursor):
            for child in cursor.get_children():
                self._traverse(child)
            return

        # 处理当前节点
        kind = cursor.kind
        if kind == cindex.CursorKind.STRUCT_DECL:
            self._extract_named_type(cursor, 'struct')
        elif kind == cindex.CursorKind.ENUM_DECL:
            self._extract_named_type(cursor, 'enum')
        elif kind == cindex.CursorKind.TYPEDEF_DECL:
            self._extract_typedef(cursor)
        elif kind == cindex.CursorKind.FUNCTION_DECL:
            self._extract_function(cursor)

        # 递归处理子节点
        for child in cursor.get_children():
            self._traverse(child)

    def extract(self):
        """提取所有信息"""
        if not self.tu:
            print("错误: 未解析文件，请先调用 parse()")
            return

        self._traverse(self.tu.cursor)


class StandardOptionalExtractor:
    """提取标准可选功能

    通过对比开启和不开启 __VSF_{PERIPHERAL}_CFG_SUPPORT_STANDARD_OPTIONAL 宏的编译结果，
    识别出API模板中的标准可选功能。
    """

    def __init__(self, base_extractor: TemplateExtractor, optional_extractor: TemplateExtractor):
        """初始化标准可选功能提取器

        Args:
            base_extractor: 不开启标准可选功能宏的提取器（基础版本）
            optional_extractor: 开启标准可选功能宏的提取器（完整版本）
        """
        self.base = base_extractor
        self.optional = optional_extractor
        self.optional_types: Dict[str, Dict] = {}
        self.optional_functions: Dict[str, Dict] = {}
        self.optional_type_members: Dict[str, List[Dict]] = defaultdict(list)

    def extract(self):
        """提取标准可选功能

        计算开启宏后新增的类型、函数和类型成员
        """
        # 提取新增的类型
        base_types = set(self.base.types.keys())
        optional_types = set(self.optional.types.keys())
        new_types = optional_types - base_types
        for name in new_types:
            self.optional_types[name] = self.optional.types[name]

        # 提取新增的函数
        base_funcs = set(self.base.functions.keys())
        optional_funcs = set(self.optional.functions.keys())
        new_funcs = optional_funcs - base_funcs
        for name in new_funcs:
            self.optional_functions[name] = self.optional.functions[name]

        # 提取新增的类型成员
        for type_name in optional_types:
            base_members = {m['name']: m for m in self.base.type_members.get(type_name, [])}
            optional_members = {m['name']: m for m in self.optional.type_members.get(type_name, [])}

            base_member_names = set(base_members.keys())
            optional_member_names = set(optional_members.keys())
            new_members = optional_member_names - base_member_names

            if new_members:
                self.optional_type_members[type_name] = [
                    optional_members[name] for name in new_members
                ]

    def to_dict(self) -> Dict[str, Any]:
        """将标准可选功能转换为字典"""
        return {
            'types': self.optional_types,
            'functions': self.optional_functions,
            'type_members': dict(self.optional_type_members),
            'summary': {
                'types_count': len(self.optional_types),
                'functions_count': len(self.optional_functions),
                'type_members_count': sum(len(members) for members in self.optional_type_members.values())
            }
        }



# ============================================================================
# 假阳性检测器（重构新增）
# ============================================================================

class FalsePositiveDetector:
    """假阳性检测器 - 识别应该被忽略的差异

    将假阳性检测逻辑从 TemplateComparator 中独立出来，
    使代码更清晰、易维护。
    """

    def __init__(self, api_extractor: TemplateExtractor, impl_extractor: TemplateExtractor,
                 standard_optional: Optional[StandardOptionalExtractor] = None,
                 strict: bool = False):
        self.api = api_extractor
        self.impl = impl_extractor
        self.standard_optional = standard_optional
        self.strict = strict

    def is_false_positive(self, diff: Dict) -> bool:
        """判断是否是假性差异（应该被忽略的差异）"""
        category = diff.get('category')
        name = diff.get('name', '')
        severity = diff.get('severity')

        # 简单的模式匹配检测（合并多个简单方法）
        if category == 'type':
            # 匿名类型
            if ('unnamed at' in name or 'anonymous at' in name) and ('struct' in name or 'enum' in name):
                return True
            # _op_t 类型缺失
            if severity == 'missing' and name.endswith('_op_t'):
                return True
            # 实现特定类型（extra）
            if severity == 'extra' and name.endswith('_t'):
                if name.startswith(('vsf_hw_', 'vsf_real_', 'vsf_imp_')):
                    return True
            # MULTI_CLASS 类型缺失
            if severity == 'missing' and self._is_multi_class_type_missing(name, diff):
                return True

        elif category == 'type_member':
            # op 成员缺失
            if severity == 'missing' and name == 'op' and diff.get('type', '').endswith('_t'):
                return True
            # use_as__ 成员缺失
            if severity == 'missing' and name.startswith('use_as__'):
                return True
            # GPIO AF extra
            if severity == 'extra' and self._is_gpio_af_extra(name, diff):
                return True
            # 类型不匹配检测
            if severity == 'mismatch':
                if self._is_reimplemented_type_member_mismatch(diff):
                    return True
                if self._is_anonymous_type_member_mismatch(category, severity, diff):
                    return True

        elif category == 'function':
            # 内联函数缺失
            if severity == 'missing':
                api_func = self.api.functions.get(name)
                if api_func and api_func.get('is_inline', False):
                    return True
                # 通用实现函数缺失
                if self._is_common_impl_function_missing(name):
                    return True

        # 标准可选功能检测
        if severity == 'missing' and self._is_standard_optional_missing(name, diff):
            return True

        return False

    def _is_multi_class_type_missing(self, name: str, diff: Dict) -> bool:
        """判断是否是 MULTI_CLASS 模式下定义的类型缺失"""
        # 检查是否是基础类型
        if not (name.startswith('vsf_') and name.endswith('_t') and not name.endswith('_op_t')):
            return False
        # 排除实现特定类型
        if name.startswith(('vsf_hw_', 'vsf_real_', 'vsf_imp_')):
            return False

        # 检查实现模板中是否使用了该类型
        for members in self.impl.type_members.values():
            if any(name in m.get('type', '') for m in members):
                return True
        for func_info in self.impl.functions.values():
            if name in func_info.get('signature', ''):
                return True

        # 如果类型在 API 模板中定义，认为是假阳性
        return name in self.api.types

    def _is_reimplemented_type_member_mismatch(self, diff: Dict) -> bool:
        """判断是否是重新实现的类型的成员类型不匹配（假阳性）"""
        type_name = diff.get('type', '')
        if type_name not in self.impl.types:
            return False

        api_type = diff.get('api_type', '')
        impl_type_str = diff.get('impl_type', '')
        if not (api_type and impl_type_str):
            return False

        # 标准整数类型列表
        int_types = ['uint8_t', 'uint16_t', 'uint32_t', 'int8_t', 'int16_t', 'int32_t']
        base_int_types = ['int', 'unsigned int', 'short', 'unsigned short', 'long', 'unsigned long']

        # 检查是否是标准整数类型之间的匹配
        if api_type in int_types and impl_type_str in int_types:
            return True

        # 检查 API 类型是否是 typedef
        if api_type in self.api.types:
            api_type_info = self.api.types[api_type]
            if api_type_info.get('kind') == 'typedef':
                typedef_sig = api_type_info.get('signature', '')
                # 特殊处理已知的 typedef
                if api_type == 'vsf_gpio_alternate_function_t':
                    typedef_sig = 'uint16_t'

                if typedef_sig and typedef_sig != api_type:
                    # 检查 typedef 基础类型匹配
                    if any(t in typedef_sig for t in int_types + base_int_types):
                        if impl_type_str in int_types + base_int_types:
                            return True

        # 检查基础类型到标准类型的映射
        if impl_type_str in base_int_types and api_type in int_types:
            type_mappings = {
                'uint16_t': ['int', 'unsigned short', 'unsigned int'],
                'uint32_t': ['int', 'unsigned int', 'long', 'unsigned long'],
                'uint8_t': ['int', 'unsigned char', 'char']
            }
            return impl_type_str in type_mappings.get(api_type, [])

        return False

    def _is_anonymous_type_member_mismatch(self, category: str, severity: str, diff: Dict) -> bool:
        """判断是否是匿名类型的成员类型不匹配（假阳性）"""
        if category != 'type_member' or severity != 'mismatch':
            return False

        api_type = diff.get('api_type', '')
        impl_type = diff.get('impl_type', '')
        if not api_type or not impl_type:
            return False

        if 'unnamed' not in api_type or 'unnamed' not in impl_type:
            return False

        type_name = diff.get('type', '')
        member_name = diff.get('name', '')

        # 先尝试使用现有的枚举类型比较
        if self.is_same_anonymous_enum_type(api_type, impl_type, type_name, member_name):
            return True

        # 对于匿名结构体，如果它们都是 struct 类型，并且所属的成员名称和类型名称都相同
        # 我们可以通过比较它们所属类型的成员来间接判断
        api_is_struct = 'struct' in api_type
        impl_is_struct = 'struct' in impl_type
        if api_is_struct and impl_is_struct:
            # 对于匿名结构体，如果它们所属的成员名称和类型名称都相同
            # 并且它们所属类型的成员都匹配，那么可以认为是等价的
            # 这里我们假设：如果成员名称和类型名称都匹配，匿名结构体应该是等价的
            # 因为在实际使用中，如果成员类型和名称都匹配，那么匿名结构体应该是等价的
            return self._is_same_anonymous_struct_by_context(type_name, member_name, api_type, impl_type)

        return False

    def _is_same_anonymous_struct_by_context(self, type_name: str, member_name: str,
                                               api_type: str, impl_type: str) -> bool:
        """通过上下文判断两个匿名结构体是否相同

        对于匿名结构体，如果它们所属的成员名称和类型名称都相同，
        并且它们所属类型的成员都匹配，那么可以认为是等价的。

        这是因为匿名结构体通常用于内联定义，如果它们出现在相同的位置
        （相同的类型、相同的成员名），并且所属类型的其他成员都匹配，
        那么可以认为它们是等价的（即使注释不同）。
        """
        # 检查所属类型是否存在
        if type_name not in self.api.type_members or type_name not in self.impl.type_members:
            return False

        api_members = {m['name']: m for m in self.api.type_members.get(type_name, [])}
        impl_members = {m['name']: m for m in self.impl.type_members.get(type_name, [])}

        # 如果成员名称和类型名称都匹配，并且该成员的类型都是匿名结构体
        if member_name in api_members and member_name in impl_members:
            api_member = api_members[member_name]
            impl_member = impl_members[member_name]

            # 确认两个成员的类型都是匿名结构体
            api_member_type = api_member.get('type', '')
            impl_member_type = impl_member.get('type', '')
            if 'struct' in api_member_type and 'unnamed' in api_member_type and \
               'struct' in impl_member_type and 'unnamed' in impl_member_type:
                # 对于匿名结构体，如果它们所属的成员名称和类型名称都相同
                # 我们可以认为它们是等价的，因为在实际使用中，如果成员类型和名称都匹配，
                # 那么匿名结构体应该是等价的（即使注释不同或位置不同）
                return True

        return False

    def _is_standard_optional_missing(self, name: str, diff: Dict) -> bool:
        """判断是否是标准可选功能缺失（假阳性）"""
        if not self.standard_optional:
            return False

        category = diff.get('category')
        if category == 'type':
            return name in self.standard_optional.optional_types
        if category == 'function':
            return name in self.standard_optional.optional_functions
        if category == 'type_member':
            type_name = diff.get('type', '')
            if type_name in self.standard_optional.optional_type_members:
                optional_member_names = {m['name'] for m in self.standard_optional.optional_type_members[type_name]}
                return name in optional_member_names
            return type_name in self.standard_optional.optional_types
        return False

    def _is_common_impl_function_missing(self, name: str) -> bool:
        """判断是否是通用实现函数缺失（假阳性）"""
        return name in ('vsf_pwm_set_ns', 'vsf_pwm_set_us', 'vsf_pwm_set_ms')

    def _is_gpio_af_extra(self, name: str, diff: Dict) -> bool:
        """判断是否是 GPIO AF 额外定义（假阳性）"""
        type_name = diff.get('type', '')
        return type_name == 'vsf_gpio_mode_t' and name == 'VSF_GPIO_AF'

    def normalize_anonymous_type_string(self, type_str: str) -> str:
        """规范化匿名类型字符串，用于比较

        匿名枚举/结构体的类型字符串包含文件路径，路径表示可能不同（如转义的点号），
        需要规范化以便正确比较。
        """
        if not type_str:
            return type_str

        if 'unnamed' not in type_str and 'anonymous' not in type_str:
            return type_str

        pattern = r'(enum|struct)\s*\(.*?at\s+[^:]+:(\d+):(\d+)\)'
        match = re.search(pattern, type_str)
        if match:
            type_kind = match.group(1)
            line = match.group(2)
            column = match.group(3)
            return f"{type_kind} (unnamed at :{line}:{column})"

        return type_str

    def is_same_anonymous_enum_type(self, api_type: str, impl_type: str,
                                     type_name: str, member_name: str) -> bool:
        """检查两个匿名类型（枚举或结构体）是否相同

        通过比较匿名类型的规范化字符串来判断类型是否相同。
        对于匿名枚举，如果它们的成员（枚举值）完全相同，则认为它们是相同的类型。
        """
        if not api_type or not impl_type:
            return False

        if 'unnamed' not in api_type or 'unnamed' not in impl_type:
            return False

        api_is_enum = 'enum' in api_type
        impl_is_enum = 'enum' in impl_type
        api_is_struct = 'struct' in api_type
        impl_is_struct = 'struct' in impl_type

        if not ((api_is_enum and impl_is_enum) or (api_is_struct and impl_is_struct)):
            return False

        # 对于匿名枚举，通过比较成员来判断是否相同
        if api_is_enum and impl_is_enum:
            api_enum_key = api_type
            impl_enum_key = impl_type

            api_members = self.api.type_members.get(api_enum_key, [])
            impl_members = self.impl.type_members.get(impl_enum_key, [])

            if len(api_members) != len(impl_members):
                return False

            # 比较每个枚举值：名称和值都必须相同
            api_member_dict = {m['name']: m.get('value', '') for m in api_members if m.get('kind') == 'enum_value'}
            impl_member_dict = {m['name']: m.get('value', '') for m in impl_members if m.get('kind') == 'enum_value'}

            if api_member_dict == impl_member_dict:
                return True

        normalized_api = self.normalize_anonymous_type_string(api_type)
        normalized_impl = self.normalize_anonymous_type_string(impl_type)

        if normalized_api == normalized_impl:
            return True

        api_match = re.search(r':(\d+):(\d+)\)', api_type)
        impl_match = re.search(r':(\d+):(\d+)\)', impl_type)
        if api_match and impl_match:
            api_line, api_col = api_match.groups()
            impl_line, impl_col = impl_match.groups()
            if api_line == impl_line and api_col == impl_col:
                return True

        api_file_match = re.search(r'([^/\\]+\.(h|c)):(\d+):(\d+)', api_type)
        impl_file_match = re.search(r'([^/\\]+\.(h|c)):(\d+):(\d+)', impl_type)
        if api_file_match and impl_file_match:
            api_file = api_file_match.group(1)
            impl_file = impl_file_match.group(1)
            api_line = api_file_match.group(3)
            impl_line = impl_file_match.group(3)
            api_col = api_file_match.group(4)
            impl_col = impl_file_match.group(4)
            if api_file == impl_file and api_line == impl_line and api_col == impl_col:
                return True

        return False


# ============================================================================
# 模板对比器
# ============================================================================

class TemplateComparator:
    """对比两个模板的差异"""

    def __init__(self, api_extractor: TemplateExtractor, impl_extractor: TemplateExtractor,
                 standard_optional: Optional[StandardOptionalExtractor] = None,
                 strict: bool = False):
        self.api = api_extractor
        self.impl = impl_extractor
        self.standard_optional = standard_optional
        self.strict = strict
        self.differences = []
        # 使用独立的假阳性检测器
        self.detector = FalsePositiveDetector(api_extractor, impl_extractor, standard_optional)

    def _add_difference(self, category: str, severity: str, name: str, message: str, **kwargs):
        """添加差异记录"""
        diff = {
            'category': category,
            'severity': severity,
            'name': name,
            'message': message,
            **kwargs
        }
        self.differences.append(diff)

    def _normalize_type_name(self, type_str: str, is_impl_template: bool = False) -> str:
        """规范化类型名称（对于实现模板，去除 vsf_hw_ 前缀）"""
        if not type_str:
            return type_str
        normalized = ' '.join(type_str.split())
        if is_impl_template:
            normalized = re.sub(r'\bvsf_hw_(\w+_t)\b', r'vsf_\1', normalized)
            normalized = re.sub(r'\bVSF_HW_(\w+_T)\b', r'VSF_\1', normalized)
        return normalized

    def _normalize_function_name(self, name: str, is_impl_template: bool = False) -> str:
        """规范化函数名（对于实现模板，去除 vsf_hw_ 前缀）"""
        if not is_impl_template:
            return name
        normalized = re.sub(r'^vsf_hw_', 'vsf_', name)
        normalized = re.sub(r'^VSF_HW_', 'VSF_', normalized)
        normalized = re.sub(r'^vsf_real_', 'vsf_', normalized)
        return normalized

    def compare(self):
        """执行对比"""
        self.differences = []

        self.compare_types()
        self.compare_functions()
        self.compare_type_members()

        # 使用独立的检测器过滤假阳性
        self.differences = [d for d in self.differences if not self.detector.is_false_positive(d)]

    def compare_types(self):
        """对比类型"""
        # 直接比较类型名（不规范化类型名本身）
        # 类型名如 vsf_pwm_t 和 vsf_hw_pwm_t 是不同的类型，不应该被视为相同
        api_types = set(self.api.types.keys())
        impl_types = set(self.impl.types.keys())

        missing_in_impl = api_types - impl_types
        for name in missing_in_impl:
            api_type = self.api.types[name]
            self._add_difference('type', 'missing', name,
                f"类型 '{name}' 在 API 模板中定义，但在实现模板中缺失",
                api_location=api_type['location'], api_info=api_type)

        extra_in_impl = impl_types - api_types
        if self.strict:
            for name in extra_in_impl:
                impl_type = self.impl.types[name]
                self._add_difference('type', 'extra', name,
                    f"类型 '{name}' 在实现模板中定义，但不在 API 模板中",
                    impl_location=impl_type['location'], impl_info=impl_type)

        common_types = api_types & impl_types
        for name in common_types:
            api_type = self.api.types[name]
            impl_type = self.impl.types[name]

            # 规范化签名后再比较（签名中的类型引用需要规范化）
            api_sig = self._normalize_type_name(api_type.get('signature', ''), is_impl_template=False)
            impl_sig = self._normalize_type_name(impl_type.get('signature', ''), is_impl_template=True)

            if api_sig != impl_sig:
                self._add_difference('type', 'mismatch', name, f"类型 '{name}' 的签名不匹配",
                    api_signature=api_sig, impl_signature=impl_sig,
                    api_location=api_type['location'], impl_location=impl_type['location'])

    def compare_functions(self):
        """对比函数"""
        # 创建规范化后的函数名映射
        # API 模板不需要规范化（保持原始名称）
        api_funcs_normalized = {name: name for name in self.api.functions.keys()}

        # 实现模板需要规范化（去除 vsf_hw_ 前缀）
        impl_funcs_normalized = {
            self._normalize_function_name(name, is_impl_template=True): name
            for name in self.impl.functions.keys()
        }

        # 使用规范化后的名称进行比较
        api_norm_names = set(api_funcs_normalized.keys())
        impl_norm_names = set(impl_funcs_normalized.keys())

        missing_in_impl = api_norm_names - impl_norm_names
        for norm_name in missing_in_impl:
            api_name = api_funcs_normalized[norm_name]
            api_func = self.api.functions[api_name]
            self._add_difference('function', 'missing', api_name,
                f"函数 '{api_name}' 在 API 模板中声明，但在实现模板中缺失",
                api_location=api_func['location'], api_signature=api_func['signature'])

        extra_in_impl = impl_norm_names - api_norm_names
        if self.strict:
            for norm_name in extra_in_impl:
                impl_name = impl_funcs_normalized[norm_name]
                impl_func = self.impl.functions[impl_name]
                self._add_difference('function', 'extra', impl_name,
                    f"函数 '{impl_name}' 在实现模板中定义，但不在 API 模板中",
                    impl_location=impl_func['location'], impl_signature=impl_func['signature'])

        common_funcs = api_norm_names & impl_norm_names
        for norm_name in common_funcs:
            api_name = api_funcs_normalized[norm_name]
            impl_name = impl_funcs_normalized[norm_name]
            api_func = self.api.functions[api_name]
            impl_func = self.impl.functions[impl_name]

            # 规范化签名后再比较
            api_sig = self._normalize_type_name(api_func.get('signature', ''), is_impl_template=False)
            impl_sig = self._normalize_type_name(impl_func.get('signature', ''), is_impl_template=True)

            if api_sig != impl_sig:
                self._add_difference('function', 'mismatch', api_name, f"函数 '{api_name}' 的签名不匹配",
                    api_signature=api_sig, impl_signature=impl_sig,
                    api_location=api_func['location'], impl_location=impl_func['location'])

    def normalize_enum_value(self, value_str: str) -> str:
        """规范化枚举值字符串，用于比较

        规范化枚举值表达式，移除多余空格，并尝试识别等价的表达式。
        例如：
        - "(1 << 0)" 和 "(0x1ul << 0)" 在语义上等价（都是 1）
        - "(1 << 0)" 和 "( 1 << 0 )" 应该被视为相同
        """
        if not value_str:
            return value_str

        # 移除多余空格
        normalized = ' '.join(value_str.split())

        # 尝试规范化等价的表达式
        # (1 << 0) 和 (0x1ul << 0) 都等于 1，但这里我们只规范化格式，不计算值
        # 因为计算常量表达式的值比较复杂，这里只做基本的格式规范化

        return normalized

    def normalize_anonymous_type_string(self, type_str: str) -> str:
        """规范化匿名类型字符串，用于比较

        匿名枚举/结构体的类型字符串包含文件路径，路径表示可能不同（如转义的点号），
        需要规范化以便正确比较。

        例如：
        - "enum (unnamed enum at C:\\path\\file.h:123:5)"
        - "enum (unnamed enum at C:\\.\\path\\.\\file.h:123:5)"

        规范化后应该提取关键信息：类型种类（enum/struct）和位置（行号:列号）
        """
        if not type_str:
            return type_str

        if 'unnamed' not in type_str and 'anonymous' not in type_str:
            return type_str

        pattern = r'(enum|struct)\s*\(.*?at\s+[^:]+:(\d+):(\d+)\)'
        match = re.search(pattern, type_str)
        if match:
            type_kind = match.group(1)
            line = match.group(2)
            column = match.group(3)
            return f"{type_kind} (unnamed at :{line}:{column})"

        return type_str

    def is_same_anonymous_enum_type(self, api_type: str, impl_type: str,
                                     type_name: str, member_name: str) -> bool:
        """检查两个匿名类型（枚举或结构体）是否相同

        通过比较匿名类型的规范化字符串来判断类型是否相同。
        如果两个匿名类型规范化后相同，或者文件名、行号、列号相同，则认为它们是相同的类型。

        对于匿名枚举，如果它们的成员（枚举值）完全相同，则认为它们是相同的类型。
        """
        if not api_type or not impl_type:
            return False

        if 'unnamed' not in api_type or 'unnamed' not in impl_type:
            return False

        api_is_enum = 'enum' in api_type
        impl_is_enum = 'enum' in impl_type
        api_is_struct = 'struct' in api_type
        impl_is_struct = 'struct' in impl_type

        if not ((api_is_enum and impl_is_enum) or (api_is_struct and impl_is_struct)):
            return False

        # 对于匿名枚举，通过比较成员来判断是否相同
        if api_is_enum and impl_is_enum:
            # 检查两个匿名枚举的成员是否完全相同
            api_enum_key = api_type
            impl_enum_key = impl_type

            api_members = self.api.type_members.get(api_enum_key, [])
            impl_members = self.impl.type_members.get(impl_enum_key, [])

            if len(api_members) != len(impl_members):
                return False

            # 比较每个枚举值：名称和值都必须相同
            api_member_dict = {m['name']: m.get('value', '') for m in api_members if m.get('kind') == 'enum_value'}
            impl_member_dict = {m['name']: m.get('value', '') for m in impl_members if m.get('kind') == 'enum_value'}

            if api_member_dict == impl_member_dict:
                return True

        normalized_api = self.normalize_anonymous_type_string(api_type)
        normalized_impl = self.normalize_anonymous_type_string(impl_type)

        if normalized_api == normalized_impl:
            return True

        api_match = re.search(r':(\d+):(\d+)\)', api_type)
        impl_match = re.search(r':(\d+):(\d+)\)', impl_type)
        if api_match and impl_match:
            api_line, api_col = api_match.groups()
            impl_line, impl_col = impl_match.groups()
            if api_line == impl_line and api_col == impl_col:
                return True

        api_file_match = re.search(r'([^/\\]+\.(h|c)):(\d+):(\d+)', api_type)
        impl_file_match = re.search(r'([^/\\]+\.(h|c)):(\d+):(\d+)', impl_type)
        if api_file_match and impl_file_match:
            api_file = api_file_match.group(1)
            impl_file = impl_file_match.group(1)
            api_line = api_file_match.group(3)
            impl_line = impl_file_match.group(3)
            api_col = api_file_match.group(4)
            impl_col = impl_file_match.group(4)
            if api_file == impl_file and api_line == impl_line and api_col == impl_col:
                return True

        return False

    def compare_type_members(self):
        """对比类型成员"""
        # 直接比较类型名（不规范化类型名本身）
        common_types = set(self.api.types.keys()) & set(self.impl.types.keys())

        for type_name in common_types:
            api_members = {m['name']: m for m in self.api.type_members.get(type_name, [])}
            impl_members = {m['name']: m for m in self.impl.type_members.get(type_name, [])}

            if not api_members and not impl_members:
                continue

            api_member_names = set(api_members.keys())
            impl_member_names = set(impl_members.keys())

            missing_in_impl = api_member_names - impl_member_names
            for member_name in missing_in_impl:
                self._add_difference('type_member', 'missing', member_name,
                    f"类型 '{type_name}' 的成员 '{member_name}' 在 API 模板中定义，但在实现模板中缺失",
                    type=type_name, api_member=api_members[member_name])

            extra_in_impl = impl_member_names - api_member_names
            if self.strict:
                for member_name in extra_in_impl:
                    self._add_difference('type_member', 'extra', member_name,
                        f"类型 '{type_name}' 的成员 '{member_name}' 在实现模板中定义，但不在 API 模板中",
                      type=type_name, impl_member=impl_members[member_name])

            common_members = api_member_names & impl_member_names
            for member_name in common_members:
                api_member = api_members[member_name]
                impl_member = impl_members[member_name]

                api_type = api_member.get('type', '') or api_member.get('value', '')
                impl_type = impl_member.get('type', '') or impl_member.get('value', '')

                if self.is_same_anonymous_enum_type(api_type, impl_type, type_name, member_name):
                    continue

                # 如果是枚举值，先规范化再比较
                if api_member.get('kind') == 'enum_value' and impl_member.get('kind') == 'enum_value':
                    normalized_api_value = self.normalize_enum_value(api_type or '')
                    normalized_impl_value = self.normalize_enum_value(impl_type or '')
                    if normalized_api_value == normalized_impl_value:
                        continue
                    # 如果规范化后仍然不同，继续检查是否是等价的常量表达式
                    # 这里可以添加更复杂的等价性检查，但为了简单起见，先使用规范化后的值

                # 规范化成员类型（需要考虑 vsf_hw_ 前缀）
                normalized_api_type = self._normalize_type_name(
                    self.normalize_anonymous_type_string(api_type), is_impl_template=False)
                normalized_impl_type = self._normalize_type_name(
                    self.normalize_anonymous_type_string(impl_type), is_impl_template=True)

                if normalized_api_type == normalized_impl_type:
                    continue

                if normalized_api_type != normalized_impl_type:
                    self._add_difference('type_member', 'mismatch', member_name,
                        f"类型 '{type_name}' 的成员 '{member_name}' 的类型/值不匹配",
                        type=type_name, api_type=api_type, impl_type=impl_type,
                        api_member=api_member, impl_member=impl_member)


# ============================================================================
# 输出辅助函数
# ============================================================================

def save_comparison_results(peripheral_name: str,
                            api_base: TemplateExtractor,
                            api_full: TemplateExtractor,
                            impl: TemplateExtractor,
                            standard_optional: StandardOptionalExtractor,
                            differences: List[Dict],
                            output_dir: Path):
    """保存对比结果到 JSON 文件"""
    import json

    # 构建 AST 信息
    api_base_dict = api_base.to_dict()
    api_full_dict = api_full.to_dict()
    impl_dict = impl.to_dict()

    # 保存 API 模板 AST（完整版本）
    with open(output_dir / f'{peripheral_name}_api_ast.json', 'w', encoding='utf-8') as f:
        json.dump({
            'file': api_full_dict['file_name'],
            'path': api_full_dict['source_file'],
            'types': api_full_dict['types'],
            'functions': api_full_dict['functions'],
            'type_members': api_full_dict['type_members']
        }, f, indent=2, ensure_ascii=False)

    # 保存 API 模板 AST（基础版本）
    with open(output_dir / f'{peripheral_name}_api_base_ast.json', 'w', encoding='utf-8') as f:
        json.dump({
            'file': api_base_dict['file_name'],
            'path': api_base_dict['source_file'],
            'types': api_base_dict['types'],
            'functions': api_base_dict['functions'],
            'type_members': api_base_dict['type_members']
        }, f, indent=2, ensure_ascii=False)

    # 保存标准可选功能
    with open(output_dir / f'{peripheral_name}_standard_optional.json', 'w', encoding='utf-8') as f:
        json.dump(standard_optional.to_dict(), f, indent=2, ensure_ascii=False)

    # 保存实现模板 AST
    with open(output_dir / f'{peripheral_name}_impl_ast.json', 'w', encoding='utf-8') as f:
        json.dump({
            'file': impl_dict['file_name'],
            'path': impl_dict['source_file'],
            'types': impl_dict['types'],
            'functions': impl_dict['functions'],
            'type_members': impl_dict['type_members']
        }, f, indent=2, ensure_ascii=False)

    # 保存差异信息
    with open(output_dir / f'{peripheral_name}_diff.json', 'w', encoding='utf-8') as f:
        json.dump({
            'peripheral': peripheral_name,
            'api_file': api_full_dict['file_name'],
            'impl_file': impl_dict['file_name'],
            'differences': differences,
            'diff_count': len(differences),
            'standard_optional_summary': standard_optional.to_dict()['summary']
        }, f, indent=2, ensure_ascii=False)


def save_summary(results: Dict[str, Tuple[int, Dict]], output_dir: Path):
    """保存汇总信息"""
    import json

    success = sum(1 for diff, _ in results.values() if diff == 0)
    failed = sum(1 for diff, _ in results.values() if diff < 0)
    total_diffs = sum(diff for diff, _ in results.values() if diff > 0)

    summary = {
        'total_peripherals': len(results),
        'success_count': success,
        'failed_count': failed,
        'diff_count': len(results) - success - failed,
        'total_diffs': total_diffs,
        'peripherals': {}
    }

    for name, (diff_count, info) in results.items():
        if diff_count >= 0:
            summary['peripherals'][name] = {
                'diff_count': diff_count,
                'api_types': info.get('api_types', 0),
                'api_functions': info.get('api_functions', 0),
                'impl_types': info.get('impl_types', 0),
                'impl_functions': info.get('impl_functions', 0),
                'standard_optional': {
                    'types_count': info.get('optional_types', 0),
                    'functions_count': info.get('optional_functions', 0),
                }
            }

    # 保存 JSON
    with open(output_dir / 'summary.json', 'w', encoding='utf-8') as f:
        json.dump(summary, f, indent=2, ensure_ascii=False)

    # 保存文本
    with open(output_dir / 'summary.txt', 'w', encoding='utf-8') as f:
        f.write("批量对比总结\n")
        f.write("="*80 + "\n")
        f.write(f"总外设数: {len(results)}\n")
        f.write(f"一致的外设: {success}\n")
        f.write(f"有差异的外设: {len(results) - success - failed}\n")
        if failed > 0:
            f.write(f"失败的外设: {failed}\n")
        f.write(f"总差异数: {total_diffs}\n")

