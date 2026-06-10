"""vsf-bench-debug — pyOCD crash dump and backtrace.

Usage::

    vsf-bench-debug crash-dump --board b1 [--project <name>] [--elf <path>] <board_dir>
    vsf-bench-debug backtrace --board b1 [--project <name>] [--elf <path>] <board_dir>
    vsf-bench-debug regs --board b1 [--project <name>] <board_dir>
    vsf-bench-debug read --board b1 --addr 0x2000FF00 --len 256 <board_dir>
"""

from __future__ import annotations

import argparse
import fnmatch
import sys
from pathlib import Path

from vsf_bench.config.map import load as load_hardware_map
from vsf_bench.config.map import load_project as load_project_from_map


def parse_args():
    parser = argparse.ArgumentParser(prog="vsf-bench-debug")
    sub = parser.add_subparsers(dest="cmd", required=True)

    crash_p = sub.add_parser("crash-dump", help="Halt CPU, capture crash context, resume")
    _add_elf_args(crash_p)

    bt_p = sub.add_parser("backtrace", help="Halt CPU, read current call stack")
    _add_elf_args(bt_p)

    regs_p = sub.add_parser("regs", help="Halt CPU, read R0-R12/SP/LR/PC/xPSR")
    regs_p.add_argument("--project", type=str, default=None, help="Project name for ELF auto-discovery")

    read_p = sub.add_parser("read", help="Read memory block")
    read_p.add_argument("--addr", type=str, required=True, help="Start address (hex)")
    read_p.add_argument("--len", type=int, default=256, help="Length in bytes")

    vars_p = sub.add_parser("vars", help="Read global/static variables by name (requires ELF)")
    vars_p.add_argument("--name", type=str, action="append", default=None,
                        help="Variable name (repeatable, supports fnmatch patterns like 'vsf_*')")
    vars_p.add_argument("--project", type=str, default=None,
                        help="Project name for ELF and debug_vars auto-discovery")
    vars_p.add_argument("--elf", type=str, default=None,
                        help="Path to ELF/.out file (overrides --project auto-discovery)")

    parser.add_argument("--board", type=str, default=None)
    parser.add_argument("board_dir")
    return parser.parse_args()


def _add_elf_args(subparser):
    subparser.add_argument("--elf", type=str, default=None, help="Path to ELF/.out file for symbol resolution")
    subparser.add_argument("--project", type=str, default=None, help="Project name for ELF auto-discovery from hardware-map")


def _resolve_board(hardware_map_path: str, board_name: str | None):
    board = load_hardware_map(hardware_map_path, board_name=board_name)
    if board.debug_probe is None:
        print(
            "[vsf-bench-debug] Error: board has no debug_probe in hardware-map.yml",
            file=sys.stderr,
        )
        sys.exit(2)
    return board


def _find_elf(args, hardware_map_path: str, board) -> str | None:
    """Find ELF path from build artifacts. Resolution order:

      1. Explicit --elf argument
      2. --project → project.build.artifacts (format: "elf" or "out"), resolved relative to build_dir
      3. Board's embedded build config artifacts (same lookup)
      4. None (no ELF, no symbol resolution)
    """
    # 1. Explicit --elf
    elf = getattr(args, "elf", None)
    if elf:
        p = Path(elf)
        if p.exists():
            return str(p.resolve())
        print(f"[vsf-bench-debug] Warning: --elf path not found: {elf}", file=sys.stderr)

    # 2. --project → find ELF artifact in project config
    project_name = getattr(args, "project", None)
    if project_name:
        try:
            project = load_project_from_map(hardware_map_path, project_name)
            elf = _find_elf_in_artifacts(project.build)
            if elf:
                return elf
        except Exception as e:
            print(f"[vsf-bench-debug] Warning: cannot load project '{project_name}': {e}", file=sys.stderr)

    # 3. Board's embedded build config (flat-format)
    if board.build:
        elf = _find_elf_in_artifacts(board.build)
        if elf:
            return elf

    return None


def _find_elf_in_artifacts(build) -> str | None:
    """Search build.artifacts for an ELF artifact (format: "elf" or "out").

    The artifact name is resolved relative to build.build_dir.
    Returns the first match or None.
    """
    build_dir = Path(build.build_dir)
    if not build_dir.exists():
        return None
    for a in build.artifacts:
        if a.format in ("elf", "out"):
            p = (build_dir / a.name).resolve()
            if p.exists():
                return str(p)
    return None


def cmd_crash_dump(board, args):
    """Halt CPU, capture crash context, output JSON."""
    from vsf_bench.utils.debug import DebugSession

    elf_path = _find_elf(args, str(Path(args.board_dir).resolve()), board)

    probe_cfg = board.debug_probe
    target = probe_cfg.get("target", "cortex_m")
    probe_id = probe_cfg.get("probe")

    with DebugSession(target=target, probe=probe_id, elf_path=elf_path) as dbg:
        dump = dbg.crash_dump()

    if elf_path:
        print(f"[vsf-bench-debug] ELF: {elf_path}")

    print(dump.to_json())


def cmd_backtrace(board, args):
    """Halt CPU, read current call stack."""
    from vsf_bench.utils.debug import DebugSession

    elf_path = _find_elf(args, str(Path(args.board_dir).resolve()), board)

    probe_cfg = board.debug_probe
    target = probe_cfg.get("target", "cortex_m")

    with DebugSession(target=target, elf_path=elf_path) as dbg:
        dbg.halt()
        try:
            frames = dbg.stack_backtrace()
            if elf_path:
                print(f"[vsf-bench-debug] ELF: {elf_path}")
            for i, f in enumerate(frames):
                # Raw values always shown — function/file annotations are supplements
                func = f"  <{f.function}>" if f.function else ""
                src = f"  // {f.file}:{f.line}" if (f.file and f.line) else ""
                if f.file and not f.line:
                    src = f"  // {f.file}"

                if i == 0:
                    # Frame #0 from live registers: full PC/SP/LR context
                    print(f"  #{i}: PC=0x{f.pc:08X}  SP=0x{f.sp:08X}  LR=0x{f.lr:08X}{func}{src}")
                elif f.lr:
                    # Frames #1+: PC decoded from LR on stack, show raw LR value
                    print(f"  #{i}: PC=0x{f.pc:08X}  LR=0x{f.lr:08X}{func}{src}")
                else:
                    print(f"  #{i}: PC=0x{f.pc:08X}{func}{src}")
        finally:
            dbg.resume()


def cmd_regs(board, args):
    """Halt CPU, dump all core registers with optional symbol resolution."""
    from vsf_bench.utils.debug import DebugSession

    elf_path = _find_elf(args, str(Path(args.board_dir).resolve()), board)

    probe_cfg = board.debug_probe
    target = probe_cfg.get("target", "cortex_m")

    with DebugSession(target=target, elf_path=elf_path) as dbg:
        dbg.halt()
        try:
            regs = dbg.read_core_regs()
            pc = regs.get("PC", 0)
            lr = regs.get("LR", 0)

            if elf_path and dbg._elf:
                pc_func = dbg._elf.get_function(pc)
                lr_func = dbg._elf.get_function(lr)
                print(f"[vsf-bench-debug] ELF: {elf_path}")
            else:
                pc_func = lr_func = ""

            for name in ["R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7",
                         "R8", "R9", "R10", "R11", "R12", "SP", "LR", "PC", "XPSR"]:
                val = regs[name]
                note = ""
                if name == "PC" and pc_func:
                    note = f"  <{pc_func}>"
                elif name == "LR" and lr_func:
                    note = f"  <{lr_func}>"
                print(f"  {name:4s} = 0x{val:08X}{note}")
        finally:
            dbg.resume()


def cmd_read(board, args):
    """Read a memory block."""
    from vsf_bench.utils.debug import DebugSession

    addr = int(args.addr, 0)
    length = args.len

    probe_cfg = board.debug_probe
    target = probe_cfg.get("target", "cortex_m")

    with DebugSession(target=target) as dbg:
        data = dbg.read_mem(addr, length)
    for i in range(0, len(data), 16):
        chunk = data[i:i + 16]
        hex_str = " ".join(f"{b:02X}" for b in chunk)
        ascii_str = "".join(chr(b) if 32 <= b < 127 else "." for b in chunk)
        print(f"  0x{addr + i:08X}: {hex_str:<48s} {ascii_str}")


def cmd_vars(board, args):
    """Read global/static variables by name using ELF symbol table.

    --name supports both exact names and fnmatch patterns (*, ?).
    When a name contains wildcards, all matching variables are dumped.
    """
    from vsf_bench.utils.debug import DebugSession

    elf_path = _find_elf(args, str(Path(args.board_dir).resolve()), board)
    if not elf_path:
        print("[vsf-bench-debug] Error: ELF file required for variable lookup. "
              "Use --project or --elf.", file=sys.stderr)
        sys.exit(2)

    probe_cfg = board.debug_probe
    target = probe_cfg.get("target", "cortex_m")

    # Gather names from CLI --name + project.debug_vars
    names = list(getattr(args, "name", None) or [])
    project_name = getattr(args, "project", None)
    if project_name:
        try:
            project = load_project_from_map(str(Path(args.board_dir).resolve()), project_name)
            for v in project.debug_vars:
                if v not in names:
                    names.append(v)
        except Exception:
            pass

    if not names:
        print("[vsf-bench-debug] Error: no variable names specified. "
              "Use --name <var> or configure debug_vars in project.", file=sys.stderr)
        sys.exit(2)

    with DebugSession(target=target, elf_path=elf_path) as dbg:
        print(f"[vsf-bench-debug] ELF: {elf_path}")

        for name in names:
            # Check if name contains wildcards
            if any(c in name for c in '*?[]'):
                results = dbg.dump_variables_by_pattern(name)
                if not results:
                    print(f"  (no variables matching '{name}')")
                    print()
                for r in results:
                    _print_variable(r)
            else:
                r = dbg.read_variable(name)
                if r is None:
                    print(f"  {name}: <not found>")
                    print()
                else:
                    _print_variable(r)


def _print_variable(r: dict) -> None:
    """Pretty-print a single variable dump."""
    if "error" in r:
        print(f"  {r['name']}: <{r['error']}>")
        return

    print(f"  {r['name']} @ {r['addr']}  ({r['size']} bytes)")
    print(f"        raw: {r['bytes']}")

    if "uint32" in r:
        print(f"      uint8: {r.get('uint8', '')}  uint16: {r.get('uint16', '')}  "
              f"uint32: {r.get('uint32', '')}  uint64: {r.get('uint64', '')}")
    if "pointer" in r:
        print(f"    pointer: {r['pointer']}")
    if "string" in r:
        s = r["string"]
        if len(s) > 80:
            s = s[:80] + "..."
        print(f"     string: \"{s}\"")

    print()


def main():
    args = parse_args()

    hardware_map_path = str(Path(args.board_dir).resolve())
    try:
        board = _resolve_board(hardware_map_path, args.board)
    except Exception as e:
        print(f"[vsf-bench-debug] Config error: {e}", file=sys.stderr)
        sys.exit(2)

    # Validate --project if provided
    if hasattr(args, "project") and args.project:
        try:
            load_project_from_map(hardware_map_path, args.project)
        except Exception as e:
            print(f"[vsf-bench-debug] Error: project '{args.project}' — {e}", file=sys.stderr)
            sys.exit(2)

    handlers = {
        "crash-dump": cmd_crash_dump,
        "backtrace": cmd_backtrace,
        "regs": cmd_regs,
        "read": cmd_read,
        "vars": cmd_vars,
    }
    try:
        handlers[args.cmd](board, args)
    except ImportError as e:
        print(f"[vsf-bench-debug] Import error: {e}", file=sys.stderr)
        sys.exit(3)
    except Exception as e:
        print(f"[vsf-bench-debug] Error: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
