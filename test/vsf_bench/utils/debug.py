"""Debug module — pyOCD-based crash dump and backtrace.

Provides ``DebugSession`` for script-driven exception analysis
without GDB interaction.  AI-friendly: pure Python, no interactive prompts.

Hardware requirement: CMSIS-DAP or J-Link probe connected to board SWD pins.

Architecture-specific logic (register names, fault decoding, return-address
detection, assertion extraction) is delegated to ``DebugTarget`` subclasses.
See ``debug_target.py``.

Usage::

    from vsf_bench.utils.debug import DebugSession

    with DebugSession("cortex_m", elf_path="app.out") as dbg:
        dump = dbg.crash_dump()
        print(f"Fault: {dump.fault_type}  PC: 0x{dump.pc:08X}")
"""

from __future__ import annotations

import fnmatch
import json
import struct
from dataclasses import dataclass, field
from pathlib import Path


# ── Data Classes ────────────────────────────────────────────


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
    # Resolved symbols
    pc_func: str = ""
    lr_func: str = ""
    # Assertion context (populated when PC is in a known assert function)
    assertion: dict | None = None

    def to_dict(self) -> dict:
        d = {
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
        if self.pc_func:
            d["pc_func"] = self.pc_func
        if self.lr_func:
            d["lr_func"] = self.lr_func
        if self.assertion:
            d["assertion"] = self.assertion
        return d

    def to_json(self, indent: int = 2) -> str:
        return json.dumps(self.to_dict(), indent=indent)


# ── ELF Context ─────────────────────────────────────────────


class ElfContext:
    """Resolve addresses to function names and source locations using ELF/DWARF.

    Extracts valid code/data address ranges from ELF sections for address
    validation without platform-specific hardcoding.

    Parameters:
        elf_path: Path to ELF/.out file.
    """

    def __init__(self, elf_path: str | Path):
        self._path = Path(elf_path)
        if not self._path.exists():
            raise FileNotFoundError(f"ELF not found: {elf_path}")

        self._func_by_addr: dict[int, tuple[str, int, int]] = {}  # addr → (name, start, size)
        self._var_by_name: dict[str, tuple[int, int]] = {}         # name → (addr, size) for variables
        self._addr_line: dict[int, tuple[str, int]] = {}           # addr → (file, line)
        self._code_ranges: list[tuple[int, int]] = []              # (start, end) from ELF sections
        self._loaded = False

    # ── loading ─────────────────────────────────────────────

    def _ensure_loaded(self):
        if self._loaded:
            return
        try:
            from elftools.elf.elffile import ELFFile
        except ImportError:
            raise RuntimeError("pyelftools not installed. Run: pip install pyelftools")

        with open(self._path, "rb") as f:
            elf = ELFFile(f)
            self._load_code_ranges(elf)
            self._load_symbols(elf)
            self._load_dwarf_lines(elf)

        self._loaded = True

    def _load_code_ranges(self, elf):
        """Extract valid address ranges from allocated (SHF_ALLOC) ELF sections."""
        ranges: list[tuple[int, int]] = []
        for sec in elf.iter_sections():
            flags = sec.header.sh_flags
            if flags & 0x2:  # SHF_ALLOC — occupies memory at runtime
                addr = sec.header.sh_addr
                size = sec.header.sh_size
                if addr > 0 and size > 0:
                    ranges.append((addr, addr + size))
        ranges.sort()
        merged = []
        for start, end in ranges:
            if merged and start <= merged[-1][1] + 0x1000:
                merged[-1] = (merged[-1][0], max(merged[-1][1], end))
            else:
                merged.append((start, end))
        self._code_ranges = merged

    def _load_symbols(self, elf):
        """Build address→function and name→variable mappings from .symtab."""
        symtab = elf.get_section_by_name(".symtab")
        if symtab is None:
            return
        for sym in symtab.iter_symbols():
            sym_type = sym["st_info"]["type"]
            addr = sym["st_value"]
            size = sym["st_size"]
            name = sym.name
            if addr == 0 or not name:
                continue

            if sym_type == "STT_FUNC":
                real_addr = addr & ~1
                self._func_by_addr[real_addr] = (name, real_addr, size)
            elif sym_type == "STT_OBJECT":
                self._var_by_name[name] = (addr, size)

    def _load_dwarf_lines(self, elf):
        """Build address→(file, line) mapping from DWARF .debug_line."""
        if not elf.has_dwarf_info():
            return
        try:
            dwarf = elf.get_dwarf_info()
        except Exception:
            return
        for cu in dwarf.iter_CUs():
            line_prog = dwarf.line_program_for_CU(cu)
            if line_prog is None:
                continue
            try:
                entries = list(line_prog.get_entries())
            except Exception:
                continue
            for entry in entries:
                if entry is None or entry.state is None:
                    continue
                addr = entry.state.address
                file_entry = line_prog["file_entry"][entry.state.file - 1] if entry.state.file else None
                if addr is not None and file_entry is not None:
                    fname = file_entry.name.decode("utf-8", errors="replace") if isinstance(file_entry.name, bytes) else str(file_entry.name)
                    self._addr_line[addr] = (fname, entry.state.line)

    # ── queries ─────────────────────────────────────────────

    @property
    def code_ranges(self) -> list[tuple[int, int]]:
        """Return list of (start, end) valid address ranges from ELF sections."""
        self._ensure_loaded()
        return list(self._code_ranges)

    def is_valid_addr(self, addr: int) -> bool:
        """Check if *addr* falls within any allocated ELF section.

        Returns False when ELF has no allocatable sections (caller should
        fall back to the architecture-specific heuristic).
        """
        self._ensure_loaded()
        real_addr = addr & ~1
        if not self._code_ranges:
            return False
        for start, end in self._code_ranges:
            if start <= real_addr < end:
                return True
        return False

    def resolve(self, addr: int) -> StackFrame:
        """Resolve an address to a StackFrame with function/file/line."""
        self._ensure_loaded()
        real_addr = addr & ~1
        frame = StackFrame(pc=addr)

        best_func = None
        best_start = 0
        for func_addr, (name, start, size) in self._func_by_addr.items():
            if start <= real_addr < start + size:
                if best_func is None or start > best_start:
                    best_func = name
                    best_start = start
        if best_func:
            frame.function = best_func

        line_info = self._addr_line.get(real_addr)
        if line_info:
            frame.file, frame.line = line_info
        else:
            best_addr = None
            for a in self._addr_line:
                if a <= real_addr and (best_addr is None or a > best_addr):
                    best_addr = a
            if best_addr is not None:
                frame.file, frame.line = self._addr_line[best_addr]
        return frame

    def get_function(self, addr: int) -> str:
        """Return function name containing *addr*, or empty string."""
        self._ensure_loaded()
        real_addr = addr & ~1
        best_func = ""
        best_start = 0
        for func_addr, (name, start, size) in self._func_by_addr.items():
            if start <= real_addr < start + size:
                if not best_func or start > best_start:
                    best_func = name
                    best_start = start
        return best_func

    # ── variable lookup ─────────────────────────────────────

    def get_variable(self, name: str) -> tuple[int, int] | None:
        """Return (address, size) for a global/static variable, or None."""
        self._ensure_loaded()
        return self._var_by_name.get(name)

    def find_variables(self, pattern: str) -> list[tuple[str, int, int]]:
        """Return [(name, addr, size), ...] for variables matching *pattern*.

        *pattern* supports fnmatch wildcards (``*``, ``?``).
        """
        self._ensure_loaded()
        result = []
        for name, (addr, size) in self._var_by_name.items():
            if fnmatch.fnmatch(name, pattern):
                result.append((name, addr, size))
        result.sort(key=lambda x: x[1])  # sort by address
        return result



class DebugSession:
    """pyOCD debug session for embedded targets.

    Thin wrapper around pyOCD.  Does NOT flash or erase — this is a read-only
    debug tool for crash analysis and memory inspection.

    Architecture-specific behaviour (register names, fault decoding,
    return-address detection) is delegated to a ``DebugTarget`` instance
    selected by *target* name.

    Parameters:
        target: pyOCD target name (e.g. ``"cortex_m"``).
        probe: optional probe unique ID to disambiguate multiple probes.
        elf_path: optional path to ELF/.out file for symbol resolution.
    """

    def __init__(self, target: str = "cortex_m", probe: str | None = None,
                 elf_path: str | None = None):
        from vsf_bench.targets import get_debug_target

        self._target_name = target
        self._target = get_debug_target(target)
        self._probe = probe
        self._session = None
        self._elf = ElfContext(elf_path) if elf_path else None

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
            raise RuntimeError("pyOCD not installed. Run: pip install pyocd")
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
    def _pyocd_target(self):
        if self._session is None:
            raise RuntimeError("Not connected — call connect() first")
        return self._session.target

    # ── basic debug ops ────────────────────────────────────

    def halt(self) -> None:
        """Halt the target CPU."""
        self._pyocd_target.halt()

    def resume(self) -> None:
        """Resume the target CPU."""
        self._pyocd_target.resume()

    def read_core_regs(self) -> dict[str, int]:
        """Read core registers (names defined by the architecture target)."""
        t = self._pyocd_target
        regs: dict[str, int] = {}
        for name in self._target.core_reg_names:
            val = t.read_core_register(name)
            regs[name.upper()] = val
        return regs

    def read_fault_regs(self) -> dict[str, int]:
        """Read fault status/address registers (delegates to arch target)."""
        return self._target.read_fault_regs(self)

    def read_mem(self, addr: int, length: int) -> bytes:
        """Read *length* bytes from *addr*."""
        return bytes(self._pyocd_target.read_memory_block8(addr, length))

    def read32(self, addr: int) -> int:
        """Read a 32-bit word from *addr*."""
        data = self.read_mem(addr, 4)
        return struct.unpack("<I", data)[0]

    def read_str(self, addr: int, max_len: int = 256) -> str:
        """Read a null-terminated ASCII string from *addr*."""
        data = self.read_mem(addr, max_len)
        null_pos = data.find(b'\x00')
        if null_pos >= 0:
            data = data[:null_pos]
        return data.decode("utf-8", errors="replace")

    # ── breakpoints ──────────────────────────────────────

    def set_breakpoint(self, addr: int) -> None:
        """Set a hardware breakpoint at *addr*."""
        self._pyocd_target.set_breakpoint(addr)

    def remove_breakpoint(self, addr: int) -> None:
        """Remove a hardware breakpoint at *addr*."""
        self._pyocd_target.remove_breakpoint(addr)

    def get_breakpoint_addresses(self) -> list[int]:
        """Return list of currently set breakpoint addresses."""
        return list(self._pyocd_target.get_breakpoint_addresses())

    def run(self, timeout: float = 10.0) -> None:
        """Resume CPU and wait until halted (by breakpoint)."""
        import time
        HALTED = self._pyocd_target.State.HALTED
        self._pyocd_target.resume()
        deadline = time.time() + timeout
        while time.time() < deadline:
            if self._pyocd_target.get_state() == HALTED:
                return
            time.sleep(0.1)
        raise TimeoutError(f"Breakpoint not hit within {timeout}s")

    # ── variable inspection ────────────────────────────────

    def read_variable(self, name: str) -> dict | None:
        """Read a global/static variable's value from the target.

        Requires an ELF file for symbol lookup.  Returns a dict with:
          name, addr, size, bytes (hex), and type-appropriate interpretations
          (uint8/16/32/64, string if printable ASCII).
        """
        if self._elf is None:
            return None
        self._elf._ensure_loaded()
        info = self._elf.get_variable(name)
        if info is None:
            return None
        addr, size = info
        if size == 0:
            return None

        # Read at most 1024 bytes for large arrays/structs
        read_len = min(size, 1024)
        data = self.read_mem(addr, read_len)
        return _format_variable(name, addr, size, data)

    def dump_variables(self, names: list[str]) -> list[dict]:
        """Read multiple variables.  Unknown names are reported with error."""
        results = []
        for name in names:
            result = self.read_variable(name)
            if result is None:
                results.append({"name": name, "error": "not found"})
            else:
                results.append(result)
        return results

    def dump_variables_by_pattern(self, pattern: str) -> list[dict]:
        """Read all variables matching fnmatch *pattern*."""
        if self._elf is None:
            return []
        self._elf._ensure_loaded()
        matches = self._elf.find_variables(pattern)
        results = []
        for name, addr, size in matches:
            read_len = min(size, 1024)
            data = self.read_mem(addr, read_len)
            results.append(_format_variable(name, addr, size, data))
        return results

    # ── stack unwind ───────────────────────────────────────

    def stack_backtrace(self, max_frames: int = 32) -> list[StackFrame]:
        """Walk the call stack from current register state.

        Scans the stack for values that look like return addresses (using
        architecture-specific detection) within valid code ranges.

        When ELF is available, validates addresses against ELF section ranges
        and annotates frames with function names and source locations.
        """
        if self._elf is not None:
            self._elf._ensure_loaded()

        regs = self.read_core_regs()
        pc = regs["PC"]
        sp = regs["SP"]
        lr = regs["LR"]

        frames: list[StackFrame] = []

        # Frame #0 from live registers
        frame = StackFrame(pc=pc, sp=sp, lr=lr)
        if self._elf is not None:
            resolved = self._elf.resolve(pc)
            frame.function = resolved.function
            frame.file = resolved.file
            frame.line = resolved.line
        frames.append(frame)

        # Scan stack for return addresses
        stack_data = self.read_mem(sp, min(256, max_frames * 8))
        visited = {pc & ~1}

        for _ in range(max_frames - 1):
            found = False
            for offset in range(0, len(stack_data) - 4, 4):
                candidate = struct.unpack_from("<I", stack_data, offset)[0]
                if not self._target.is_return_address(candidate):
                    continue
                cand_addr = self._target.decode_return_address(candidate)
                if cand_addr in visited:
                    continue
                if not self._is_plausible_code_addr(cand_addr):
                    continue

                visited.add(cand_addr)
                prev_frame = StackFrame(pc=cand_addr, lr=candidate)
                if self._elf is not None:
                    resolved = self._elf.resolve(cand_addr)
                    prev_frame.function = resolved.function
                    prev_frame.file = resolved.file
                    prev_frame.line = resolved.line
                frames.append(prev_frame)
                found = True
                break

            if not found:
                break

        return frames[:max_frames]

    def _is_plausible_code_addr(self, addr: int) -> bool:
        """Check if *addr* could be a valid code address.

        When ELF is available, uses ELF section ranges exclusively.
        Falls back to the architecture heuristic only when no ELF is loaded.
        """
        if self._elf is not None:
            return self._elf.is_valid_addr(addr)
        return self._target.is_plausible_code_addr(addr)

    # ── crash dump ─────────────────────────────────────────

    def crash_dump(self) -> CrashDump:
        """Halt CPU, capture full crash context, then resume.

        When ELF is provided, resolves symbols and extracts assertion info.
        """
        if self._elf is not None:
            self._elf._ensure_loaded()

        self.halt()
        try:
            regs = self.read_core_regs()
            faults = self.read_fault_regs()
            stack = self.stack_backtrace()
            fault_type = self._target.classify_fault(faults)

            # Resolve PC and LR to function names
            pc_func = ""
            lr_func = ""
            if self._elf is not None:
                pc_func = self._elf.get_function(regs["PC"])
                lr_func = self._elf.get_function(regs["LR"])

            # Extract assertion info (architecture-specific)
            assertion = self._target.extract_assertion_info(self, regs, pc_func)

            # Classify as Assertion when in assert function with no hardware fault
            if assertion and fault_type == "NoFault":
                fault_type = "Assertion"

        finally:
            try:
                self.resume()
            except Exception:
                pass

        return CrashDump(
            fault_type=fault_type,
            pc=regs["PC"],
            sp=regs["SP"],
            lr=regs["LR"],
            cfsr=faults.get("CFSR", 0),
            hfsr=faults.get("HFSR", 0),
            mmfar=faults.get("MMFAR", 0),
            bfar=faults.get("BFAR", 0),
            core_regs=regs,
            stack=stack,
            pc_func=pc_func,
            lr_func=lr_func,
            assertion=assertion,
        )


# ── Helpers ──────────────────────────────────────────────────


def _format_variable(name: str, addr: int, size: int, data: bytes) -> dict:
    """Format a variable value for display.

    Returns a dict with raw bytes and type-appropriate interpretations.
    """
    result: dict = {
        "name": name,
        "addr": f"0x{addr:08X}",
        "size": size,
        "bytes": data.hex() if len(data) <= 64 else data[:64].hex() + "...",
    }

    # Integer interpretations (little-endian, common for Cortex-M)
    if len(data) >= 1:
        result["uint8"] = data[0]
    if len(data) >= 2:
        result["uint16"] = struct.unpack("<H", data[:2])[0]
    if len(data) >= 4:
        result["uint32"] = struct.unpack("<I", data[:4])[0]
    if len(data) >= 8:
        result["uint64"] = struct.unpack("<Q", data[:8])[0]

    # Pointer interpretation (if value looks like an address)
    if 4 <= len(data) <= 8:
        ptr_val = struct.unpack("<I", data[:4])[0]
        if 0x1000 <= ptr_val < 0xE0000000:
            result["pointer"] = f"0x{ptr_val:08X}"

    # String interpretation
    null_pos = data.find(b'\x00')
    if null_pos > 0:
        text = data[:null_pos]
        if _is_printable(text):
            result["string"] = text.decode("utf-8", errors="replace")
    elif null_pos == 0:
        result["string"] = "(empty)"
    elif len(data) <= 128 and _is_printable(data):
        result["string"] = data.decode("utf-8", errors="replace")

    return result


def _is_printable(data: bytes) -> bool:
    """Check if *data* is all printable ASCII."""
    return all(32 <= b < 127 or b in (10, 13, 9) for b in data)
