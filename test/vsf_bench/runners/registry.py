"""Centralized runner type registry.

Both hardware_map.py (for validation) and pipeline.py (for dispatch)
import from here. Add new built-in runners here.
"""

from vsf_bench.runners.swd_runner import SWDRunner
from vsf_bench.runners.uf2_runner import UF2Runner

RUNNER_TYPES: dict[str, type] = {
    "openocd": SWDRunner,
    "uf2": UF2Runner,
}


def get_runner_class(type_name: str) -> type | None:
    return RUNNER_TYPES.get(type_name)
