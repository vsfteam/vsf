"""Debug module — pyOCD-based crash dump and backtrace for BH1098.

Provides ``DebugSession`` for script-driven exception analysis
without GDB interaction.  AI-friendly: pure Python, no interactive prompts.

Hardware requirement: CMSIS-DAP or J-Link probe connected to board SWD pins.

Usage::

    from vsf_bench.utils.debug import DebugSession

    with DebugSession("cortex_m") as dbg:
        dump = dbg.crash_dump()
        print(f"Fault: {dump.fault_type}  PC: 0x{dump.pc:08X}")
"""

from __future__ import annotations

import json
from dataclasses import dataclass, field


@dataclass
class StackFrame:
    """One frame in a stack backtrace."""
    pc: int
    sp: int = 0
    lr: int = 0
    function: str = ""
    file: str = ""
    line: int = 0

    def to_dict(self) -> dict[str, object]:
        d: dict[str, object] = {"pc": f"0x{self.pc:08X}"}
        if self.sp:
            d["sp"] = f"0x{self.sp:08X}"
        if self.lr:
            d["lr"] = f"0x{self.lr:08X}"
        if self.function:
            d["function"] = self.function
        if self.file:
            d["file"] = self.file
        if self.line:
            d["line"] = self.line
        return d


@dataclass
class CrashDump:
    """Full crash context: fault classification, registers, stack backtrace."""
    fault_type: str = ""
    pc: int = 0
    sp: int = 0
    lr: int = 0
    cfsr: int = 0
    hfsr: int = 0
    mmfar: int = 0
    bfar: int = 0
    core_regs: dict[str, int] = field(default_factory=dict)
    stack: list[StackFrame] = field(default_factory=list)

    def to_dict(self) -> dict:
        return {
            "fault": self.fault_type,
            "pc": f"0x{self.pc:08X}",
            "sp": f"0x{self.sp:08X}",
            "lr": f"0x{self.lr:08X}",
            "cfsr": f"0x{self.cfsr:08X}",
            "hfsr": f"0x{self.hfsr:08X}",
            "mmfar": f"0x{self.mmfar:08X}",
            "bfar": f"0x{self.bfar:08X}",
            "regs": {k: f"0x{v:08X}" for k, v in self.core_regs.items()},
            "stack": [f.to_dict() for f in self.stack],
        }

    def to_json(self, indent: int = 2) -> str:
        return json.dumps(self.to_dict(), indent=indent)


class DebugSession:
    """pyOCD debug session for BH1098 (Cortex-M4F).

    Thin wrapper around pyOCD session.  Does NOT flash or erase — this is
    a read-only debug tool for crash analysis and memory inspection.

    Parameters:
        target: pyOCD target name (default ``"cortex_m"`` for BH1098H M4F).
        probe: optional probe unique ID to disambiguate multiple probes.
    """

    # SCB fault register offsets from 0xE000ED00
    _SCB_BASE = 0xE000ED00
    _CFSR_OFF  = 0x28   # Configurable Fault Status Register
    _HFSR_OFF  = 0x2C   # HardFault Status Register
    _MMFAR_OFF = 0x34   # MemManage Fault Address Register
    _BFAR_OFF  = 0x38   # BusFault Address Register

    _CORE_REG_NAMES = [f"r{i}" for i in range(13)] + ["sp", "lr", "pc", "xpsr"]

    def __init__(self, target: str = "cortex_m", probe: str | None = None):
        self._target_name = target
        self._probe = probe
        self._session = None

    # ── context manager ────────────────────────────────────

    def __enter__(self):
        self.connect()
        return self

    def __exit__(self, *args):
        self.disconnect()
        return False

    # ── connection lifecycle ───────────────────────────────

    def connect(self):
        """Open pyOCD session.  Raises RuntimeError if pyOCD unavailable."""
        try:
            from pyocd.core.helpers import ConnectHelper  # type: ignore[import-not-found]
        except ImportError:
            raise RuntimeError(
                "pyOCD not installed. Run: pip install pyocd"
            )
        kwargs = {"target_override": self._target_name}
        if self._probe:
            kwargs["unique_id"] = self._probe
        self._session = ConnectHelper.session_with_chosen_probe(**kwargs)
        self._session.open()

    def disconnect(self):
        """Close pyOCD session."""
        if self._session is not None:
            self._session.close()
            self._session = None

    @property
    def _target(self):
        if self._session is None:
            raise RuntimeError("Not connected — call connect() first")
        return self._session.target

    # ── basic debug ops ────────────────────────────────────

    def halt(self) -> None:
        """Halt the target CPU."""
        self._target.halt()

    def resume(self) -> None:
        """Resume the target CPU."""
        self._target.resume()

    def read_core_regs(self) -> dict[str, int]:
        """Return R0-R12, SP, LR, PC, xPSR as int values."""
        t = self._target
        regs: dict[str, int] = {}
        for name in self._CORE_REG_NAMES:
            val = t.read_core_register(name)
            regs[name.upper()] = val
        return regs

    def read_fault_regs(self) -> dict[str, int]:
        """Read SCB fault status/address registers.

        Returns: CFSR, HFSR, MMFAR, BFAR (all 32-bit ints).
        """
        t = self._target
        return {
            "CFSR":  t.read32(self._SCB_BASE + self._CFSR_OFF),
            "HFSR":  t.read32(self._SCB_BASE + self._HFSR_OFF),
            "MMFAR": t.read32(self._SCB_BASE + self._MMFAR_OFF),
            "BFAR":  t.read32(self._SCB_BASE + self._BFAR_OFF),
        }

    def read_mem(self, addr: int, length: int) -> bytes:
        """Read *length* bytes from *addr*."""
        return bytes(self._target.read_memory_block8(addr, length))

    # ── stack unwind ───────────────────────────────────────

    def stack_backtrace(self, max_frames: int = 32) -> list[StackFrame]:
        """Walk the call stack from current register state.

        Uses a simple AAPCS LR-chain unwind.  For BH1098 this covers
        most C functions (compiled with frame pointers off by default in
        IAR, so we fall back to LR-based heuristics).

        Returns one frame for the current PC context; deeper unwind needs
        DWARF .debug_frame which is not yet implemented.
        """
        regs = self.read_core_regs()
        frames = [
            StackFrame(
                pc=regs["PC"],
                sp=regs["SP"],
                lr=regs["LR"],
            )
        ]
        # Placeholder: deeper unwind via .debug_frame table
        # (requires pyelftools or elftools for DWARF CFI parsing)
        return frames[:max_frames]

    # ── crash dump ─────────────────────────────────────────

    def crash_dump(self) -> CrashDump:
        """Halt CPU, capture full crash context, then resume.

        Returns a CrashDump with fault classification, all core regs,
        SCB fault regs, and stack backtrace.
        """
        self.halt()
        try:
            regs = self.read_core_regs()
            faults = self.read_fault_regs()
            stack = self.stack_backtrace()
            fault_type = self._classify_fault(faults["CFSR"])
        finally:
            try:
                self.resume()
            except Exception:
                pass  # resume may fail if CPU is truly wedged

        return CrashDump(
            fault_type=fault_type,
            pc=regs["PC"],
            sp=regs["SP"],
            lr=regs["LR"],
            cfsr=faults["CFSR"],
            hfsr=faults["HFSR"],
            mmfar=faults["MMFAR"],
            bfar=faults["BFAR"],
            core_regs=regs,
            stack=stack,
        )

    @staticmethod
    def _classify_fault(cfsr: int) -> str:
        """Map CFSR bits to a human-readable fault name.

        CFSR bits (ARMv7-M Architecture Reference Manual):
          - IACCVIOL(0), DACCVIOL(1), STKERR(4), UNSTKERR(8),
            INVPC(12), INVSTATE(17), UNDEFINSTR(18), UNALIGNED(24), DIVBYZERO(25)
        """
        if cfsr == 0:
            return "NoFault"

        names = []
        if cfsr & (1 << 25):
            names.append("DIVBYZERO")
        if cfsr & (1 << 24):
            names.append("UNALIGNED")
        if cfsr & (1 << 19):
            names.append("NOCP")
        if cfsr & (1 << 18):
            names.append("UNDEFINSTR")
        if cfsr & (1 << 17):
            names.append("INVSTATE")
        if cfsr & (1 << 12):
            names.append("INVPC")
        if cfsr & (1 << 11):
            names.append("UNSTKERR")
        if cfsr & (1 << 8):
            names.append("UNSTKERR")
        if cfsr & (1 << 4):
            names.append("STKERR")
        if cfsr & (1 << 3):
            names.append("MUNSTKERR")
        if cfsr & (1 << 1):
            names.append("DACCVIOL")
        if cfsr & (1 << 0):
            names.append("IACCVIOL")

        return "/".join(names) if names else f"UNKNOWN(0x{cfsr:08X})"
