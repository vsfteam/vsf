"""vsf-bench — unified build / flash / test pipeline.

Composes `pipeline.build_phase`, `pipeline.flash_phase`, and
`pipeline.run_test_phase` based on the requested flags. Standalone
scripts (`vsf-bench-build`, `vsf-bench-flash`, `vsf-bench-test`) call
the same underlying functions, so behavior stays consistent.

LA capture supports two modes (see `--la-mode`):
  * `shared` (default): a single LA capture for the entire run; uses
    --decode-start / --decode-end so each suite's decode only scans its
    own window.
  * `per-suite`: one LA capture per suite; small files decode fast.
"""

import argparse
import sys
from pathlib import Path

from vsf_bench import pipeline
from vsf_bench.cli._args import add_shared_test_args, resolve_shuffle_seed
from vsf_bench.lock import LockBusyError


def parse_args():
    parser = argparse.ArgumentParser(prog="vsf-bench")
    add_shared_test_args(parser)
    parser.add_argument("--build", action="store_true")
    parser.add_argument("--flash", action="store_true")
    parser.add_argument("--test", action="store_true")
    parser.add_argument("--all", action="store_true")
    return parser.parse_args()


def main():
    args = parse_args()

    do_build = args.build or args.all
    do_flash = args.flash or args.all
    do_test = args.test or args.all

    if not (do_build or do_flash or do_test):
        print("[vsf-bench] Error: at least one of --build, --flash, --test, --all is required")
        sys.exit(2)

    hardware_map_path = (Path(args.board_dir) / "hardware-map.yml").resolve()
    test_params_yml = hardware_map_path.parent / "test_params.yml"
    if not test_params_yml.exists():
        test_params_yml = None
    try:
        board = pipeline.load_board(hardware_map_path, board_name=args.board)
    except Exception as e:
        print(f"[vsf-bench] Config error: {e}", file=sys.stderr)
        sys.exit(2)

    # Apply CLI overrides for source/build directories
    if args.source_dir:
        board.build.source_dir = args.source_dir
        if not args.build_dir:
            board.build.build_dir = str(Path(args.source_dir) / "build")
    if args.build_dir:
        board.build.build_dir = args.build_dir

    build_dir = Path(board.build.build_dir)

    if do_build:
        try:
            build_dir = pipeline.build_phase(board)
        except Exception as e:
            print(f"[vsf-bench] Build failed: {e}", file=sys.stderr)
            sys.exit(1)

    if do_flash or do_test:
        try:
            lock = pipeline.acquire_board_lock(board, args.wait)
        except LockBusyError as e:
            print(f"[vsf-bench] {e}", file=sys.stderr)
            sys.exit(3)
    else:
        lock = None

    try:
        if do_flash:
            if not build_dir.exists():
                print(f"[vsf-bench] Build directory missing: {build_dir}", file=sys.stderr)
                print(f"[vsf-bench] Run with --build first.", file=sys.stderr)
                sys.exit(2)
            try:
                pipeline.flash_phase(board, build_dir)
            except Exception as e:
                print(f"[vsf-bench] Flash failed: {e}", file=sys.stderr)
                sys.exit(1)

        if not do_test:
            return

        case_specs: list[str] = []
        if args.case:
            case_specs.extend(args.case)
        if args.case_index:
            case_specs.extend(str(i) for i in args.case_index)

        shuffle_seed = resolve_shuffle_seed(args, case_specs)

        log_dir = Path(args.log_dir) if args.log_dir else None

        try:
            overall_pass = pipeline.run_test_phase(
                board=board,
                suite_names=args.suite,
                script_override=Path(args.script) if args.script else None,
                case_specs=case_specs,
                la_mode=args.la_mode,
                log_dir=log_dir,
                shuffle_seed=shuffle_seed,
                test_params_yml=test_params_yml,
                trace_level=args.trace_level,
            )
        except Exception as e:
            print(f"[vsf-bench] Test phase error: {e}", file=sys.stderr)
            sys.exit(1)

        if not overall_pass:
            sys.exit(1)
    finally:
        if lock is not None:
            lock.release()


if __name__ == "__main__":
    main()
