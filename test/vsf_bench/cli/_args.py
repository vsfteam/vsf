"""Shared CLI arguments for vsf-bench entry points.

Both `vsf-bench` (cli/run.py) and `vsf-bench-test` (cli/test.py) use the
same test-phase flags. This module extracts the common definitions so a
new flag only needs to be added once.
"""

import argparse
import sys


def add_shared_test_args(parser: argparse.ArgumentParser) -> None:
    """Add test-phase arguments that are shared across all CLI entry points.

    Callers should add their own entry-point-specific flags *after* calling
    this function (e.g. `--build/--flash/--test/--all` for the unified runner).
    """
    parser.add_argument("--source-dir", type=str, default=None,
                        help="Override build source_dir from hardware-map.yml")
    parser.add_argument("--build-dir", type=str, default=None,
                        help="Override build build_dir from hardware-map.yml")
    parser.add_argument("--suite", action="append", default=None)
    parser.add_argument("--case", action="append", default=None)
    parser.add_argument("--case-index", action="append", type=int, default=None)
    parser.add_argument("--script", type=argparse.FileType(), default=None)
    parser.add_argument(
        "--la-mode",
        choices=["per-suite", "shared"],
        default="shared",
        help="LA capture lifetime",
    )
    parser.add_argument(
        "--trace-level",
        choices=["none", "error", "warning", "info", "debug"],
        default="debug",
        help="Set vsf_trace level on the device before testing (default: debug)",
    )
    parser.add_argument(
        "--random",
        action="store_true",
        help="Shuffle case execution order within each suite",
    )
    parser.add_argument(
        "--seed",
        type=int,
        default=None,
        help="Fixed shuffle seed for reproducibility (implies --random)",
    )
    parser.add_argument(
        "--board",
        action="append",
        default=None,
        help="Select target board(s) by name. Repeat for multiple boards, or use --all-boards",
    )
    parser.add_argument(
        "--all-boards",
        action="store_true",
        help="Run on all connected boards (from hardware-map.yml)",
    )
    parser.add_argument(
        "--project",
        type=str,
        default=None,
        help="Select project by name from hardware-map.yml projects section",
    )
    parser.add_argument(
        "--wait",
        nargs="?",
        type=float,
        const=-1.0,
        default=None,
        help="Wait for board lock if busy (optional timeout in seconds)",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Validate test_params.yml schema only, do not execute tests",
    )
    parser.add_argument("--hardware-map", type=str, required=True,
                        help="Path to hardware-map.yml")


def resolve_shuffle_seed(args, case_specs: list[str]) -> int | None:
    """Validate --random/--seed against --case/--case-index and return seed.

    * If neither --random nor --seed is given, returns ``None``.
    * If --seed is given, returns that value.
    * If --random is given without --seed, generates a random 32-bit seed.
    * If --case/--case-index is also given, prints an error and calls
      ``sys.exit(2)``.
    """
    if (args.random or args.seed is not None) and case_specs:
        print(
            "[vsf-bench] Config error: --case/--case-index cannot combine with --random/--seed",
            file=sys.stderr,
        )
        sys.exit(2)

    if args.random or args.seed is not None:
        import os
        return (
            args.seed
            if args.seed is not None
            else int.from_bytes(os.urandom(4), "little") or 1
        )
    return None
