"""CaptureScene — LA capture + decode orchestration for test suites.

Encapsulates shared-mode and per-suite-mode capture lifecycle so
``pipeline.py`` doesn't need to manage LA threads, timestamps, or
window padding.
"""

from __future__ import annotations

import time
from pathlib import Path
from typing import TYPE_CHECKING, Callable

if TYPE_CHECKING:
    from vsf_bench.config import LogicAnalyzerConfig
    from vsf_bench.capabilities.logic_analyzer import LogicAnalyzer
    from vsf_bench.serial import SerialInstrument

SHARED_WINDOW_PAD_NS = 500_000_000  # 500 ms each side


class CaptureScene:
    """Run a batch of test suites with LA capture + decode.

    Two strategies:
      * ``shared`` — one capture spans all LA-needing suites
      * ``per_suite`` — each LA-needing suite gets its own capture
    """

    def __init__(
        self,
        la_cfg: LogicAnalyzerConfig,
        cli_path: Path,
        run_dir: Path,
        marker_baudrate: int = 115200,
    ):
        self._la_cfg = la_cfg
        self._cli_path = cli_path
        self._run_dir = run_dir
        self._marker_baudrate = marker_baudrate

    # ------------------------------------------------------------------ shared

    def run_shared(
        self,
        loaded: list,
        ser: SerialInstrument,
        case_specs: list[str],
        run_phase1: Callable,
        call_decode: Callable,
        log: Callable[[str], None],
        shuffle_seed: int | None = None,
        test_params_yml: Path | None = None,
    ) -> bool:
        la_indices = [i for i, (_, _, _, n) in enumerate(loaded) if n]
        first_la_idx = la_indices[0]
        last_la_idx = la_indices[-1]

        shared_capture = self._run_dir / "shared-capture.dsl"
        shared_adapter = None
        la_start_t: float | None = None
        suite_windows: list[tuple[str, object, int, int]] = []
        overall_pass = True

        for i, (suite_name, _script_path, mod, _needs) in enumerate(loaded):
            if i == first_la_idx:
                shared_adapter = self._new_adapter()
                shared_adapter.start(shared_capture, 300.0)
                shared_adapter.wait_until_started(timeout=5.0)
                la_start_t = time.monotonic()
                log(f"Shared LA started -> {shared_capture}")

            cases_to_run = case_specs if case_specs else [None]
            for case in cases_to_run:
                if shuffle_seed is not None and case is None:
                    log(f"{suite_name}: shuffle seed={shuffle_seed} (host-side only)")
                case_tag = f".{case}" if case else ""
                log(f"Suite: {suite_name}{case_tag}")
                t_start = int((time.monotonic() - la_start_t) * 1e9) if la_start_t is not None else 0
                ok = run_phase1(suite_name, case, mod, ser, test_params_yml)
                t_end = int((time.monotonic() - la_start_t) * 1e9) if la_start_t is not None else 0
                if not ok:
                    overall_pass = False
                if mod is not None and hasattr(mod, "decode") and shared_adapter is not None:
                    suite_windows.append((suite_name, mod, t_start, t_end))

            if i == last_la_idx and shared_adapter is not None:
                log("Stopping shared LA...")
                shared_adapter.stop()
                try:
                    shared_adapter.wait(timeout=30.0)
                except (TimeoutError, RuntimeError) as e:
                    log(f"LA wait warning: {e}")

        for suite_name, mod, t_start, t_end in suite_windows:
            decode_start = max(0, t_start - SHARED_WINDOW_PAD_NS)
            decode_end = t_end + SHARED_WINDOW_PAD_NS
            log(f"Decoding (shared): {suite_name}  window=[{decode_start/1e9:.2f}s,{decode_end/1e9:.2f}s]")
            t0_decode = time.perf_counter()
            try:
                call_decode(mod, shared_adapter, self._la_cfg.channels,
                           shared_capture, decode_start, decode_end,
                           self._marker_baudrate, test_params_yml)
                elapsed = time.perf_counter() - t0_decode
                log(f"PASS decode: {suite_name} ({elapsed:.3f} s)")
            except (AssertionError, RuntimeError, KeyError, AttributeError, FileNotFoundError) as e:
                elapsed = time.perf_counter() - t0_decode
                log(f"FAIL decode: {suite_name}: {e} ({elapsed:.3f} s)")
                overall_pass = False

        return overall_pass

    # ------------------------------------------------------------------ per_suite

    def run_per_suite(
        self,
        loaded: list,
        ser: SerialInstrument,
        case_specs: list[str],
        run_phase1: Callable,
        call_decode: Callable,
        log: Callable[[str], None],
        shuffle_seed: int | None = None,
        test_params_yml: Path | None = None,
    ) -> bool:
        overall_pass = True

        for suite_name, _script_path, mod, needs_la in loaded:
            cases_to_run = case_specs if case_specs else [None]
            for case in cases_to_run:
                case_tag = f".{case}" if case else ""
                log(f"Suite: {suite_name}{case_tag}")
                if shuffle_seed is not None and case is None:
                    log(f"{suite_name}: shuffle seed={shuffle_seed} (host-side only)")

                adapter = None
                capture_path = None
                if needs_la:
                    label = f"{suite_name}{('_' + case) if case else ''}"
                    capture_path = self._run_dir / f"{label}-capture.dsl"
                    adapter = self._new_adapter()
                    adapter.start(capture_path, 180.0)
                    adapter.wait_until_started(timeout=5.0)

                ok = run_phase1(suite_name, case, mod, ser, test_params_yml)
                if not ok:
                    overall_pass = False

                if adapter is not None:
                    adapter.stop()
                    try:
                        adapter.wait(timeout=15.0)
                    except (TimeoutError, RuntimeError) as e:
                        log(f"LA wait warning: {e}")

                if ok and mod is not None and hasattr(mod, "decode") and adapter is not None:
                    t0_decode = time.perf_counter()
                    try:
                        call_decode(mod, adapter, self._la_cfg.channels,
                                   capture_path, None, None,
                                   self._marker_baudrate, test_params_yml)
                        elapsed = time.perf_counter() - t0_decode
                        log(f"PASS decode: {suite_name}{case_tag} ({elapsed:.3f} s)")
                    except (AssertionError, RuntimeError, KeyError, AttributeError, FileNotFoundError) as e:
                        elapsed = time.perf_counter() - t0_decode
                        log(f"FAIL decode: {suite_name}{case_tag}: {e} ({elapsed:.3f} s)")
                        overall_pass = False

        return overall_pass

    # ------------------------------------------------------------------ internal

    def _new_adapter(self) -> LogicAnalyzer:
        from vsf_bench.adapters.dsview import DSViewAdapter
        return DSViewAdapter(
            cli_path=self._cli_path,
            device=self._la_cfg.device,
            samplerate=self._la_cfg.samplerate,
            channels=self._la_cfg.channels,
        )
