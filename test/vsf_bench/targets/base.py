"""Debug target ABC — architecture-specific CPU knowledge.

Defines ``DebugTarget``, the abstract base for architecture-specific debug
operations: register names, fault register layout, fault classification,
return-address encoding, and assertion parameter extraction.

See submodules for concrete implementations (e.g. ``cortex_m``).
"""

from __future__ import annotations

from abc import ABC, abstractmethod
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from vsf_bench.utils.debug import DebugSession


class DebugTarget(ABC):
    """Architecture-specific debug descriptor.

    Each concrete subclass provides the register map, fault register layout,
    address validation heuristics, return-address encoding, and assertion
    parameter extraction for one CPU architecture.
    """

    # ── core registers ─────────────────────────────────────

    @property
    @abstractmethod
    def core_reg_names(self) -> list[str]:
        """Ordered list of core register names for pyOCD ``read_core_register``."""
        ...

    # ── fault registers ────────────────────────────────────

    @property
    @abstractmethod
    def fault_reg_addrs(self) -> dict[str, int]:
        """Map fault register name → memory-mapped address (or empty dict if N/A)."""
        ...

    def read_fault_regs(self, dbg: "DebugSession") -> dict[str, int]:
        """Read fault status/address registers from the target.

        Default implementation reads 32-bit words at each address in
        ``fault_reg_addrs``.  Override for targets with non-memory-mapped
        fault registers.
        """
        return {name: dbg.read32(addr) for name, addr in self.fault_reg_addrs.items()}

    @abstractmethod
    def classify_fault(self, fault_regs: dict[str, int]) -> str:
        """Return a human-readable fault classification string.

        *fault_regs* is the dict returned by ``read_fault_regs()``.
        Returns ``"NoFault"`` when no fault is indicated.
        """
        ...

    # ── address validation (no-ELF fallback) ────────────────

    @abstractmethod
    def is_plausible_code_addr(self, addr: int) -> bool:
        """Check if *addr* could be a valid code address on this architecture.

        Used as fallback when no ELF is provided.  Should be conservative
        (accept rather than reject) since ELF section ranges provide the
        authoritative check when available.
        """
        ...

    def is_plausible_data_addr(self, addr: int) -> bool:
        """Check if *addr* could be a valid data address on this architecture.

        Default: same as ``is_plausible_code_addr``.  Override if the
        architecture has clearly separated code/data address spaces.
        """
        return self.is_plausible_code_addr(addr)

    # ── return address encoding ─────────────────────────────

    @abstractmethod
    def is_return_address(self, value: int) -> bool:
        """Check if a stack value looks like a return address on this architecture.

        For example, Cortex-M return addresses have bit 0 set (Thumb mode).
        """
        ...

    def decode_return_address(self, value: int) -> int:
        """Strip architecture-specific encoding bits from a return address.

        Default: returns *value* unchanged.  Cortex-M clears bit 0.
        """
        return value

    # ── assertion extraction ────────────────────────────────

    def extract_assertion_info(self, dbg: "DebugSession", regs: dict[str, int],
                               pc_func: str) -> dict | None:
        """Try to extract assertion parameters when halted in an assert function.

        *pc_func* is the resolved function name at PC (e.g. ``"vsf_trace_assert"``).
        Returns a dict with keys like ``caller_func``, ``file``, ``line``,
        ``func``, ``expression``, or None if extraction is not applicable.

        The default implementation returns None.  Override for architectures
        that use ``vsf_trace_assert`` with a known calling convention.
        """
        return None


def _is_printable_ascii(s: str) -> bool:
    """Check if *s* consists entirely of printable ASCII characters."""
    return all(32 <= ord(c) < 127 or c in '\r\n\t' for c in s)
