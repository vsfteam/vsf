"""Debug target registry.

Provides ``get_debug_target(target_name)`` — the factory function that
returns the right ``DebugTarget`` subclass for a pyOCD target name.
"""

from vsf_bench.targets.base import DebugTarget
from vsf_bench.targets.cortex_m import CortexMTarget


def get_debug_target(target_name: str) -> DebugTarget:
    """Return the right ``DebugTarget`` for a pyOCD target name.

    Mapping::

        "cortex_m"   → CortexMTarget
        "cortex_m0"  → CortexMTarget
        "cortex_m4"  → CortexMTarget
        "cortex_m7"  → CortexMTarget
        ...

    Unknown / unlisted targets default to ``CortexMTarget``.
    """
    name_lower = target_name.lower()
    if "cortex_m" in name_lower:
        return CortexMTarget()

    # Future:
    # if "cortex_a" in name_lower:
    #     return CortexATarget()
    # if "riscv" in name_lower:
    #     return RiscVTarget()

    return CortexMTarget()
