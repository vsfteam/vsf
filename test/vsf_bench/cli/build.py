"""vsf-bench-build — standalone build entry point.

Invokes cmake on the source/build directories declared in hardware-map.yml.
Does not flash or test.

Exit codes:
  0  build complete
  1  cmake error
  2  config / argument error
"""

import argparse
import sys
from pathlib import Path

from vsf_bench import pipeline


def main():
    parser = argparse.ArgumentParser(prog="vsf-bench-build")
    parser.add_argument("--source-dir", type=str, default=None)
    parser.add_argument("--build-dir", type=str, default=None)
    parser.add_argument("--board", type=str, default=None,
                        help="Select target board by name when multiple are connected")
    parser.add_argument("board_dir")
    args = parser.parse_args()

    hardware_map_path = Path(args.hardware_map)

    try:
        board_name = args.board[0] if args.board else None
        board = pipeline.load_board(hardware_map_path, board_name=board_name)
    except Exception as e:
        print(f"[vsf-bench-build] Config error: {e}", file=sys.stderr)
        sys.exit(2)

    if args.source_dir:
        board.build.source_dir = args.source_dir
        if not args.build_dir:
            board.build.build_dir = str(Path(args.source_dir) / "build")
    if args.build_dir:
        board.build.build_dir = args.build_dir

    try:
        pipeline.build_phase(board)
    except Exception as e:
        print(f"[vsf-bench-build] Build failed: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
