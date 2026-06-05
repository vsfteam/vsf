"""vsf-bench — unified build / flash / test pipeline.

Composes `pipeline.build_phase`, `pipeline.program_phase`, and
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
from vsf_bench.hwctrl.tee_logger import init_logger as _init_logger
from vsf_bench.lock import LockBusyError


def parse_args():
    parser = argparse.ArgumentParser(prog="vsf-bench")
    add_shared_test_args(parser)
    parser.add_argument("--build", action="store_true")
    parser.add_argument("--program", action="store_true")
    parser.add_argument("--test", action="store_true")
    parser.add_argument("--all", action="store_true")
    parser.add_argument(
        "--pipeline", type=str, default=None,
        help="Run a named multi-stage pipeline (from hardware-map.yml pipelines section)",
    )
    parser.add_argument(
        "--list-pipelines", action="store_true",
        help="List available pipelines in hardware-map.yml",
    )
    return parser.parse_args()


def main():
    args = parse_args()

    # Load hardware-map defaults (log_dir etc.)
    hw_path = Path(args.hardware_map).resolve()
    from vsf_bench.hardware_map import load_defaults
    _defaults = load_defaults(str(hw_path)) if hw_path.exists() else {}
    log_dir = args.log_dir or _defaults.get("log_dir")

    # ── --list-pipelines ──
    if args.list_pipelines:
        from vsf_bench.hardware_map import list_pipelines as _list_pipelines
        try:
            pipelines = _list_pipelines(str(hw_path))
        except Exception as e:
            print(f"[vsf-bench] Error: {e}", file=sys.stderr)
            sys.exit(2)
        if not pipelines:
            print("No pipelines defined in hardware-map.yml")
        else:
            print("Available pipelines:")
            for p in pipelines:
                desc = f" — {p.description}" if p.description else ""
                print(f"  {p.name}{desc}")
        return

    # ── --pipeline ──
    if args.pipeline:
        if not args.board:
            print("[vsf-bench] Error: --pipeline requires --board", file=sys.stderr)
            sys.exit(2)
        from vsf_bench.hardware_map import load_pipeline, resolve_pipeline_projects
        board_name = args.board[0] if args.board else None
        try:
            pipeline_obj = load_pipeline(str(hw_path), args.pipeline)
            board, _first_project = pipeline.load_board(
                hw_path,
                board_name=board_name,
                project_name=pipeline_obj.stages[0].project,
            )
            project_map = resolve_pipeline_projects(
                pipeline_obj, str(hw_path), board_name=board_name,
            )
            _init_logger(log_dir)
        except Exception as e:
            print(f"[vsf-bench] Pipeline error: {e}", file=sys.stderr)
            sys.exit(2)
        try:
            pipeline.run_pipeline(pipeline_obj, board, project_map)
        except Exception as e:
            print(f"[vsf-bench] Pipeline failed: {e}", file=sys.stderr)
            sys.exit(1)
        return

    # Initialize TeeLogger — use --log-dir, or hardware-map defaults.log_dir
    _init_logger(log_dir)

    hardware_map_path = Path(args.hardware_map).resolve()
    do_build = args.build or args.all
    do_program = args.program or args.all
    do_test = args.test or args.all

    if not args.project:
        print("[vsf-bench] Error: --project is required", file=sys.stderr)
        sys.exit(2)

    # Load project
    from vsf_bench.hardware_map import load_project as _load_project
    try:
        project = _load_project(str(hardware_map_path), args.project)
    except Exception as e:
        print(f"[vsf-bench] Config error: {e}", file=sys.stderr)
        sys.exit(2)

    # Load board (only needed for program/test)
    board = None
    if do_program or do_test:
        try:
            _board_result = pipeline.load_board(
                hardware_map_path,
                project_name=args.project,
                board_name=(args.board[0] if args.board else None),
            )
            # load_board returns (board, project) tuple when project_name is given
            board = _board_result[0] if isinstance(_board_result, tuple) else _board_result
        except Exception as e:
            print(f"[vsf-bench] Config error: {e}", file=sys.stderr)
            sys.exit(2)
        for _rcfg in project.runners.values():
            p = _rcfg.params
            p.setdefault("program_port", board.program_uart)
            p.setdefault("debug_port", board.debug_uart)
            p.setdefault("debug_baudrate", board.debug_baudrate)

    build_config = project.build

    # Apply CLI overrides for source/build directories
    if args.source_dir:
        build_config.source_dir = args.source_dir
        if not args.build_dir:
            build_config.build_dir = str(Path(args.source_dir) / "build")
    if args.build_dir:
        build_config.build_dir = args.build_dir

    build_dir = Path(build_config.build_dir)

    if do_build:
        try:
            build_dir = pipeline.build_phase(project)
        except Exception as e:
            print(f"[vsf-bench] Build failed: {e}", file=sys.stderr)
            sys.exit(1)

    if do_program or do_test:
        try:
            lock = pipeline.acquire_board_lock(board, args.wait)
        except LockBusyError as e:
            print(f"[vsf-bench] {e}", file=sys.stderr)
            sys.exit(3)
    else:
        lock = None

    try:
        if do_program:
            if not build_dir.exists():
                print(f"[vsf-bench] Build directory missing: {build_dir}", file=sys.stderr)
                print(f"[vsf-bench] Run with --build first.", file=sys.stderr)
                sys.exit(2)
            try:
                pipeline.program_phase(board, build_dir, project=project)
            except Exception as e:
                print(f"[vsf-bench] Program failed: {e}", file=sys.stderr)
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
