"""Centralized builder type registry.

Both hardware_map.py (for validation) and pipeline.py (for dispatch)
import from here. Add new built-in builders here.
"""

from vsf_bench.builders.cmake_builder import CMakeBuilder
from vsf_bench.builders.iar_builder import IARBuilder

BUILDER_TYPES: dict[str, type] = {
    "cmake": CMakeBuilder,
    "iar": IARBuilder,
}


def get_builder_class(type_name: str) -> type | None:
    return BUILDER_TYPES.get(type_name)
