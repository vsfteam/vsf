"""vsf-bench-debug — pyOCD crash dump and backtrace.

Usage::

    vsf-bench-debug crash-dump --board b1 <board_dir>
    vsf-bench-debug backtrace --board b1 <board_dir>
    vsf-bench-debug regs --board b1 <board_dir>
    vsf-bench-debug read --board b1 --addr 0x2000FF00 --len 256 <board_dir>
"""

import argparse
import json
import sys
from pathlib import Path

from vsf_bench.hardware_map import load as load_hardware_map


def parse_args():
    parser = argparse.ArgumentParser(prog="vsf-bench-debug")
    sub = parser.add_subparsers(dest="cmd", required=True)

    sub.add_parser("crash-dump", help="Halt CPU, capture crash context, resume")
    sub.add_parser("backtrace", help="Halt CPU, read current call stack")
    sub.add_parser("regs", help="Halt CPU, read R0-R12/SP/LR/PC/xPSR")

    read_p = sub.add_parser("read", help="Read memory block")
    read_p.add_argument("--addr", type=str, required=True, help="Start address (hex)")
    read_p.add_argument("--len", type=int, default=256, help="Length in bytes")

    parser.add_argument("--board", type=str, default=None)
    parser.add_argument("board_dir")
    return parser.parse_args()


def _resolve_board(hardware_map_path: str, board_name: str | None):
    board = load_hardware_map(hardware_map_path, board_name=board_name)
    if board.debug_probe is None:
        print(
            "[vsf-bench-debug] Error: board has no debug_probe in hardware-map.yml",
            file=sys.stderr,
        )
        sys.exit(2)
    return board


def cmd_crash_dump(board, args):
    """Halt CPU, capture crash context, output JSON."""
    from vsf_bench.debug import DebugSession

    probe_cfg = board.debug_probe
    target = probe_cfg.get("target", "cortex_m")
    probe_id = probe_cfg.get("probe")

    with DebugSession(target=target, probe=probe_id) as dbg:
        dump = dbg.crash_dump()
    print(dump.to_json())


def cmd_backtrace(board, args):
    """Halt CPU, read current call stack."""
    from vsf_bench.debug import DebugSession

    probe_cfg = board.debug_probe
    target = probe_cfg.get("target", "cortex_m")

    with DebugSession(target=target) as dbg:
        dbg.halt()
        try:
            frames = dbg.stack_backtrace()
            print(f"PC = 0x{frames[0].pc:08X}")
            print(f"SP = 0x{frames[0].sp:08X}")
            print(f"LR = 0x{frames[0].lr:08X}")
        finally:
            dbg.resume()


def cmd_regs(board, args):
    """Halt CPU, dump all core registers."""
    from vsf_bench.debug import DebugSession

    probe_cfg = board.debug_probe
    target = probe_cfg.get("target", "cortex_m")

    with DebugSession(target=target) as dbg:
        dbg.halt()
        try:
            regs = dbg.read_core_regs()
            for name in ["R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7",
                         "R8", "R9", "R10", "R11", "R12", "SP", "LR", "PC", "XPSR"]:
                print(f"  {name:4s} = 0x{regs[name]:08X}")
        finally:
            dbg.resume()


def cmd_read(board, args):
    """Read a memory block."""
    from vsf_bench.debug import DebugSession

    addr = int(args.addr, 0)
    length = args.len

    probe_cfg = board.debug_probe
    target = probe_cfg.get("target", "cortex_m")

    with DebugSession(target=target) as dbg:
        data = dbg.read_mem(addr, length)
    for i in range(0, len(data), 16):
        chunk = data[i:i + 16]
        hex_str = " ".join(f"{b:02X}" for b in chunk)
        print(f"  0x{addr + i:08X}: {hex_str}")


def main():
    args = parse_args()

    hardware_map_path = Path(args.hardware_map).resolve()
    try:
        board = _resolve_board(str(hardware_map_path), args.board)
    except Exception as e:
        print(f"[vsf-bench-debug] Config error: {e}", file=sys.stderr)
        sys.exit(2)

    handlers = {
        "crash-dump": cmd_crash_dump,
        "backtrace": cmd_backtrace,
        "regs": cmd_regs,
        "read": cmd_read,
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
