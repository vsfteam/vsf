"""FlashRunner — backward-compat alias for ProgramCapability.

New adapters implement ``capabilities.program.ProgramCapability`` directly.
This module exists so existing runners (PluginRunner, SWD, UF2) don't break.
"""

from vsf_bench.capabilities.program import ProgramCapability


class FlashRunner(ProgramCapability):
    """Backward-compat shim.  Prefer extending ProgramCapability for new code."""
