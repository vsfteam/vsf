"""vsf-bench-test — standalone test entry point.

Runs the test phase against firmware that is already flashed and running.
Does not build or flash. This is the fastest iteration loop when only
the host-side test scripts are changing.

Exit codes:
  0  all tests passed
  1  one or more failures
  2  config / argument error
  3  board lock busy
"""

import argparse
import sys
from pathlib import Path

from vsf_bench import pipeline
from vsf_bench.cli._args import add_shared_test_args, resolve_shuffle_seed
from vsf_bench.lock import LockBusyError


def parse_args():
    parser = argparse.ArgumentParser(prog="vsf-bench-test")
    add_shared_test_args(parser)
    return parser.parse_args()


def main():
    args = parse_args()
    hardware_map_path = (Path(args.board_dir) / "hardware-map.yml").resolve()
    test_params_yml = hardware_map_path.parent / "test_params.yml"
    if not test_params_yml.exists():
        test_params_yml = None

    try:
        board = pipeline.load_board(hardware_map_path, board_name=args.board)
    except Exception as e:
        print(f"[vsf-bench-test] Config error: {e}", file=sys.stderr)
        sys.exit(2)

    case_specs: list[str] = []
    if args.case:
        case_specs.extend(args.case)
    if args.case_index:
        case_specs.extend(str(i) for i in args.case_index)

    shuffle_seed = resolve_shuffle_seed(args, case_specs)

    log_dir = Path(args.log_dir) if args.log_dir else None

    try:
        lock = pipeline.acquire_board_lock(board, args.wait)
    except LockBusyError as e:
        print(f"[vsf-bench-test] {e}", file=sys.stderr)
        sys.exit(3)

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
    except (ValueError, KeyError) as e:
        print(f"[vsf-bench-test] Config error: {e}", file=sys.stderr)
        sys.exit(2)
    except Exception as e:
        print(f"[vsf-bench-test] Test phase error: {e}", file=sys.stderr)
        sys.exit(1)
    finally:
        if lock is not None:
            lock.release()

    if not overall_pass:
        sys.exit(1)


if __name__ == "__main__":
    main()
