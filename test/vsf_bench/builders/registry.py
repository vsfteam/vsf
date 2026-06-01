"""Centralized builder type registry.

Both hardware_map.py (for validation) and pipeline.py (for dispatch)
import from here. Add new built-in builders here.
"""

from vsf_bench.builders.cmake_builder import CMakeBuilder

BUILDER_TYPES: dict[str, type] = {
    "cmake": CMakeBuilder,
}


def get_builder_class(type_name: str) -> type | None:
    return BUILDER_TYPES.get(type_name)
