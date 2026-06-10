"""Cortex-M debug target (ARMv6-M / v7-M / v8-M).

Provides ``CortexMTarget`` — register map, SCB fault decoding, Thumb-2
return-address detection, and AAPCS assertion extraction.
"""

from __future__ import annotations

import struct
from typing import TYPE_CHECKING

from vsf_bench.targets.base import DebugTarget, _is_printable_ascii

if TYPE_CHECKING:
    from vsf_bench.utils.debug import DebugSession


class CortexMTarget(DebugTarget):
    """ARM Cortex-M (v6M / v7M / v8M) debug target.

    Covers:
      - Register set: R0-R12, SP, LR, PC, xPSR
      - SCB fault registers at 0xE000ED00
      - Thumb-2 instruction encoding (bit 0 set on branch targets)
      - AAPCS calling convention for assertion extraction
    """

    _SCB_BASE = 0xE000ED00

    # ── core registers ─────────────────────────────────────

    @property
    def core_reg_names(self) -> list[str]:
        return [f"r{i}" for i in range(13)] + ["sp", "lr", "pc", "xpsr"]

    # ── fault registers ────────────────────────────────────

    @property
    def fault_reg_addrs(self) -> dict[str, int]:
        return {
            "CFSR":  self._SCB_BASE + 0x28,
            "HFSR":  self._SCB_BASE + 0x2C,
            "MMFAR": self._SCB_BASE + 0x34,
            "BFAR":  self._SCB_BASE + 0x38,
        }

    def classify_fault(self, fault_regs: dict[str, int]) -> str:
        """Map CFSR bits to human-readable fault names (ARMv7-M).

        CFSR (0xE000ED28) is a composite of:
          - UFSR  [31:16]  UsageFault
          - BFSR  [15:8]   BusFault
          - MMFSR [7:0]    MemManage
        """
        cfsr = fault_regs.get("CFSR", 0)
        if cfsr == 0:
            return "NoFault"

        names = []
        # UFSR (UsageFault) — bits 31:16
        if cfsr & (1 << 25): names.append("DIVBYZERO")
        if cfsr & (1 << 24): names.append("UNALIGNED")
        if cfsr & (1 << 19): names.append("NOCP")
        if cfsr & (1 << 18): names.append("INVPC")
        if cfsr & (1 << 17): names.append("INVSTATE")
        if cfsr & (1 << 16): names.append("UNDEFINSTR")
        # BFSR (BusFault) — bits 15:8
        if cfsr & (1 << 15): names.append("BFARVALID")
        if cfsr & (1 << 13): names.append("BSTKERR")
        if cfsr & (1 << 12): names.append("BUNSTKERR")
        if cfsr & (1 << 11): names.append("IMPRECISERR")
        if cfsr & (1 << 10): names.append("PRECISERR")
        if cfsr & (1 <<  8): names.append("IBUSERR")
        # MMFSR (MemManage) — bits 7:0
        if cfsr & (1 <<  7): names.append("MMARVALID")
        if cfsr & (1 <<  4): names.append("MSTKERR")
        if cfsr & (1 <<  3): names.append("MUNSTKERR")
        if cfsr & (1 <<  1): names.append("DACCVIOL")
        if cfsr & (1 <<  0): names.append("IACCVIOL")

        return "/".join(names) if names else f"UNKNOWN(0x{cfsr:08X})"

    # ── address validation (no-ELF fallback) ────────────────

    def is_plausible_code_addr(self, addr: int) -> bool:
        """Generic Cortex-M heuristic: above vector table, below system space."""
        real_addr = addr & ~1
        return 0x00001000 <= real_addr < 0xE0000000

    # ── return address encoding ─────────────────────────────

    def is_return_address(self, value: int) -> bool:
        """Cortex-M: return address has Thumb bit (bit 0) set."""
        return (value & 1) != 0

    def decode_return_address(self, value: int) -> int:
        """Clear Thumb bit to get the actual instruction address."""
        return value & ~1

    # ── assertion extraction ────────────────────────────────

    def extract_assertion_info(self, dbg: "DebugSession", regs: dict[str, int],
                               pc_func: str) -> dict | None:
        """Extract assertion parameters from vsf_trace_assert stack frame.

        Requires:
          - PC is in ``vsf_trace_assert`` (checked by caller via *pc_func*)
          - AAPCS calling convention: R0-R3 caller-saved, callee pushes R4-R11+LR
          - vsf_trace_assert prologue: ``PUSH {R5,R6,R7,LR}``, then saves
            func(R3)→[SP+0], expr(R0)→[SP+4] before calling vsf_trace_error
        """
        if pc_func != "vsf_trace_assert":
            return None

        sp = regs["SP"]
        pc = regs["PC"] & ~1
        result: dict = {}

        # Approach 1: saved LR from stack → resolve caller via DWARF
        saved_lr = dbg.read32(sp + 0x0C)
        if self.is_return_address(saved_lr):
            caller_addr = self.decode_return_address(saved_lr)
            if dbg._elf is not None:
                caller_frame = dbg._elf.resolve(caller_addr)
                if caller_frame.function:
                    result["caller_func"] = caller_frame.function
                if caller_frame.file:
                    result["file"] = caller_frame.file
                if caller_frame.line:
                    result["line"] = caller_frame.line

        # Approach 2: scan literal pool after vsf_trace_assert
        if "file" not in result or "line" not in result:
            try:
                self._scan_assert_literal_pool(dbg, pc, result)
            except Exception:
                pass

        # Approach 3: read saved func/expr from stack
        try:
            expr_ptr = dbg.read32(sp + 4)
            if self.is_plausible_data_addr(expr_ptr):
                s = dbg.read_str(expr_ptr, 128)
                if s and _is_printable_ascii(s):
                    result["expression"] = s
            func_ptr = dbg.read32(sp)
            if self.is_plausible_data_addr(func_ptr):
                s = dbg.read_str(func_ptr, 128)
                if s and _is_printable_ascii(s) and "func" not in result:
                    result["func"] = s
        except Exception:
            pass

        return result if result else None

    def _scan_assert_literal_pool(self, dbg: "DebugSession", pc: int,
                                   result: dict) -> None:
        """Read literal pool after the Thumb B . at *pc* for assertion metadata."""
        data = dbg.read_mem(pc + 2, 64)

        for i in range(0, len(data) - 4, 4):
            val = struct.unpack_from("<I", data, i)[0]
            if not self.is_plausible_data_addr(val):
                continue
            try:
                s = dbg.read_str(val, 256)
                if not s or len(s) <= 3:
                    continue
                if (('\\' in s or '/' in s) and ('.' in s)) or s.endswith(('.c', '.h')):
                    if "file" not in result:
                        result["file"] = s.replace('\\', '/').split('/')[-1]
                        result["file_path"] = s
                        continue
                if any(op in s for op in ['!=', '==', '<=', '>=', '<', '>',
                                           '&&', '||', '!', 'false', 'true', 'NULL']):
                    if "expression" not in result:
                        result["expression"] = s
                        continue
            except Exception:
                continue

        for i in range(0, len(data) - 4, 4):
            val = struct.unpack_from("<I", data, i)[0]
            if 0 < val < 10000 and "line" not in result:
                result["line"] = val
                break
