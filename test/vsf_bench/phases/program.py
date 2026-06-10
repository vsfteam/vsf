"""Program phase — flash firmware via registered runner."""

from __future__ import annotations

from pathlib import Path

from vsf_bench.board import power_cycle
from vsf_bench.runners.registry import get_runner_class
from vsf_bench.utils.tee_logger import get_logger as _get_logger


def program_phase(board_or_project, build_dir: Path, project=None) -> None:
    board = board_or_project

    if project is not None:
        active_runner = project.active_runner
        runners = project.runners
    else:
        active_runner = board.active_runner
        runners = board.runners

    runner_cfg = runners[active_runner]
    runner_cls = get_runner_class(runner_cfg.type)
    if runner_cls is None:
        raise RuntimeError(f"Unknown runner type: {runner_cfg.type}")
    runner = runner_cls(runner_cfg)

    with runner:
        power_cycle(board)
        _get_logger().event(f"Programming via {active_runner}...")
        runner.flash(build_dir)

    _get_logger().event("Program complete")
