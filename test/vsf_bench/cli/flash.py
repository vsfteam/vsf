"""vsf-bench-flash — standalone flash entry point.

Flashes the artifact produced by a prior build to the board via the
active runner declared in hardware-map.yml.

Exit codes:
  0  flash complete
  1  runner error (OpenOCD failure, missing UF2 mount, …)
  2  config / argument error
  3  board lock busy
"""

import argparse
import sys
from pathlib import Path

from vsf_bench import pipeline
from vsf_bench.cli._args import add_shared_test_args
from vsf_bench.lock import LockBusyError


def main():
    parser = argparse.ArgumentParser(prog="vsf-bench-flash")
    add_shared_test_args(parser)
    args = parser.parse_args()

    hardware_map_path = Path(args.board_dir) / "hardware-map.yml"

    try:
        board = pipeline.load_board(hardware_map_path, board_name=args.board)
    except Exception as e:
        print(f"[vsf-bench-flash] Config error: {e}", file=sys.stderr)
        sys.exit(2)

    build_dir = Path(board.build.build_dir)
    if not build_dir.exists():
        print(f"[vsf-bench-flash] Build directory missing: {build_dir}", file=sys.stderr)
        print(f"[vsf-bench-flash] Run vsf-bench-build first.", file=sys.stderr)
        sys.exit(2)

    try:
        lock = pipeline.acquire_board_lock(board, args.wait)
    except LockBusyError as e:
        print(f"[vsf-bench-flash] {e}", file=sys.stderr)
        sys.exit(3)

    try:
        pipeline.flash_phase(board, build_dir)
    except Exception as e:
        print(f"[vsf-bench-flash] Flash failed: {e}", file=sys.stderr)
        sys.exit(1)
    finally:
        if lock is not None:
            lock.release()


if __name__ == "__main__":
    main()
