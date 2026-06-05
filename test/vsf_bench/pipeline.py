"""Shared phase functions used by all vsf-bench CLI scripts.

Each phase function does one thing and is called by exactly one CLI entry:
  * `load_board()`   — YAML → BoardConfig (used by all)
  * `build_phase()`  — cmake build (vsf-bench-build)
  * `program_phase()`  — runner flash (vsf-bench-flash)
  * `run_test_phase()` — multi-suite LA-aware test orchestration (vsf-bench-test)

The unified `vsf-bench` entry (`cli/run.py`) composes these in order based on
the `--build/--flash/--test/--all` flags.
"""

import inspect
import json
import shutil
import sys
import time
from datetime import datetime
from pathlib import Path

from vsf_bench.hardware_map import load as load_hardware_map, validate_runners
from vsf_bench.hardware_map import load_board_and_project as _load_board_and_project
from vsf_bench.builders.registry import get_builder_class
from vsf_bench.runners.registry import get_runner_class
from vsf_bench.instruments.serial_instrument import SerialInstrument
from vsf_bench.instruments.logic_analyzer_instrument import LogicAnalyzerInstrument
from vsf_bench.vsf_test_shell import VsfTestShellProtocol
from vsf_bench.lock import BoardLock, LockBusyError
from vsf_bench.suite import discover_suites, load_script_module, script_needs_la, resolve_suites
from vsf_bench.test_params_loader import load_test_params
from vsf_bench.hwctrl.tee_logger import get_logger as _get_logger


def __log_event(message):
    """Log a vsf-bench event. Uses TeeLogger when available, else print."""
    try:
        _get_logger().event(message)
    except RuntimeError:
        from datetime import datetime as _dt
        ts = _dt.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]
        print(f"[{ts}] [vsf-bench] {message}")


def load_board(hardware_map_path: Path, board_name: str | None = None,
               project_name: str | None = None):
    """Read hardware-map.yml and return (BoardConfig, ProjectConfig) or BoardConfig.

    *project_name=None* (legacy) → returns BoardConfig with embedded build/runner.
    *project_name=<name>* → returns ``(board, project)`` tuple.
    """
    if project_name is not None:
        board, project = _load_board_and_project(
            str(hardware_map_path), board_name=board_name,
            project_name=project_name,
        )
        validate_runners(project, build_artifacts=project.build.artifacts)
        return board, project

    # Legacy path
    board = load_hardware_map(str(hardware_map_path), board_name=board_name)
    validate_runners(board)
    return board


def acquire_board_lock(board, wait_spec=None) -> BoardLock | None:
    """Acquire the hardware-exclusion lock for *board*.

    *wait_spec=None* → no lock (backward compatible, single-agent mode).
    *wait_spec=-1.0* → wait indefinitely.
    *wait_spec>=0* → wait that many seconds, then raise LockBusyError.
    """
    if wait_spec is None:
        return None
    lock = BoardLock(board.name)
    if wait_spec < 0:
        lock.acquire(wait=True)
    else:
        lock.acquire(wait=True, timeout=wait_spec)
    __log_event(f"Lock acquired: {board.name}")
    return lock


def build_phase(build_src) -> Path:
    """Run the configured build tool → return build_dir. Raises on error.

    *build_src* can be a BoardConfig (legacy, ``.build`` attr) or a
    ProjectConfig / BuildConfig directly.
    """
    build = getattr(build_src, "build", build_src)
    builder_cls = get_builder_class(build.tool)
    if builder_cls is None:
        raise RuntimeError(f"Unknown build tool: {build.tool}")
    __log_event(f"Building ({build.source_dir}) via {build.tool}...")
    builder = builder_cls(build)
    build_dir = builder.build()
    __log_event(f"Build complete: {build_dir}")
    return build_dir


def _find_hub_by_addr(hub_addr: int) -> str | None:
    """Find SmartUSBHub COM port by querying device address."""
    import serial.tools.list_ports
    from smartusbhub import SmartUSBHub
    for port in serial.tools.list_ports.comports():
        if port.vid != 0x1A86 or port.pid != 0xFE0C:
            continue
        try:
            hub = SmartUSBHub(port.device)
            addr = hub.get_device_address()
            hub.disconnect()
            if addr == hub_addr:
                return port.device
        except Exception:
            continue
    return None


def _board_power_cycle(board, delay_off_s: float = 0.5) -> None:
    """Power-cycle *board* with randomised exponential backoff.

    Encapsulates SmartUSBHub resolution and retry — callers don't need
    to know about hub COM ports or contention handling.
    """
    if not board.power:
        raise RuntimeError(f"Board '{board.name}' has no power config")
    if board.power.type != "smartusbhub":
        raise RuntimeError(f"Unsupported power type: {board.power.type}")

    # Resolve SmartUSBHub COM port by querying device address
    hub_com = _find_hub_by_addr(board.power.hub_addr)
    if not hub_com:
        raise RuntimeError(
            f"SmartUSBHub with addr 0x{board.power.hub_addr:04X} not found"
        )

    import random as _random
    try:
        from smartusbhub import SmartUSBHub
    except ImportError:
        raise RuntimeError("smartusbhub not installed — cannot power-cycle")

    def _cmd(state, label):
        last_exc = None
        for attempt in range(1, 11):
            try:
                hub = SmartUSBHub(hub_com)
                try:
                    hub.set_channel_power(board.power.port, state=state)
                finally:
                    hub.disconnect()
                __log_event(f"power {label} port {board.power.port}")
                return
            except Exception as e:
                last_exc = e
                if attempt < 10:
                    delay = min(0.02 * (2 ** (attempt - 1)), 5.0)
                    time.sleep(delay + delay * _random.uniform(-0.5, 0.5))
        raise last_exc

    _cmd(0, "off")
    if delay_off_s > 0:
        time.sleep(delay_off_s)
    _cmd(1, "on")


def program_phase(board_or_project, build_dir: Path, project=None) -> None:
    """Power-cycle board (if configured), then run flash runner."""
    board = board_or_project

    if project is not None:
        active_runner = project.active_runner
        runners = project.runners
    else:
        active_runner = board.active_runner
        runners = board.runners

    runner_cfg = runners[active_runner]

    # Power-cycle before flash.
    runner_cls = get_runner_class(runner_cfg.type)
    if runner_cls is None:
        raise RuntimeError(f"Unknown runner type: {runner_cfg.type}")
    runner = runner_cls(runner_cfg)

    with runner:
        _board_power_cycle(board)

        __log_event(f"Programming via {active_runner}...")
        runner.flash(build_dir)

    # close() called automatically on exit
    __log_event("Program complete")


# ---------------------------------------------------------------------------
# Test phase
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# Helper: scenario lookup (used by _resolve_case_value and _run_script_phase1)
# ---------------------------------------------------------------------------

def _find_scenario_for_suite(params: dict, suite_name: str) -> tuple[str, dict] | None:
    """Find YAML scenario key and data for a given suite name.

    Tries direct key match first, then common peripheral-prefix patterns
    ({peripheral}_{name}, {peripheral}_rx_{name}, {peripheral}_tx_{name}).
    """
    for key, value in params.items():
        if key == "marker" or not isinstance(value, dict):
            continue
        if key == suite_name:
            return key, value
        scenario_name = value.get("name", "")
        for prefix in (
            "usart", "gpio", "i2c", "spi", "adc", "pwm",
            "timer", "rtc", "flash", "wdt", "dma",
        ):
            if suite_name == f"{prefix}_{scenario_name}":
                return key, value
            if suite_name == f"{prefix}_rx_{scenario_name}":
                return key, value
            if suite_name == f"{prefix}_tx_{scenario_name}":
                return key, value
    return None


def _resolve_case_value(params: dict, suite_name: str, case_value: str) -> int:
    """Convert a case parameter value (e.g. '921600') to its case index.

    Iterates the matched scenario's cases and checks every field (except
    'idx', 'host', 'la') for an exact string match.
    """
    result = _find_scenario_for_suite(params, suite_name)
    if result is None:
        raise ValueError(f"Suite '{suite_name}' not found in test params")

    _, scenario = result
    cases = scenario.get("cases", [])
    for case in cases:
        for field, val in case.items():
            if field in ("idx", "host", "la"):
                continue
            if str(val) == case_value:
                return case["idx"]

    raise ValueError(
        f"Case value '{case_value}' not found in suite '{suite_name}'"
    )


def _run_script_phase1(
    suite_name: str,
    case: str | None,
    script_module,
    ser: SerialInstrument,
    test_params_yml: Path | None = None,
) -> bool:
    """Send trigger, run script.run(). Returns True on PASS."""
    shell = VsfTestShellProtocol(ser)
    case_tag = f".{case}" if case else ""
    cmd = shell.build_run_cmd(suite_name, case)
    shell.drain_repl()
    ser.send(cmd)
    __log_event(f"Triggered: {cmd.strip()}")
    t0 = time.perf_counter()

    # vsf-test-shell emits "suite ack: <name>" on successful lookup,
    # "suite not found: <name>" / "case not found: <case>" otherwise.
    # Retry once on timeout: stale output from previous suite can delay ack.
    ack = None
    for attempt, tmo in ((1, 1.0), (2, 2.0)):
        ack = shell.expect_suite_ack(timeout=tmo)
        if ack is not None:
            break
        if attempt == 1:
            shell.drain_repl()
            ser.send(cmd)
            __log_event(f"Retrying: {cmd.strip()}")
        else:
            __log_event(f"FAIL: {suite_name}{case_tag}: no shell ack within 1s")
            return False
    assert ack is not None
    if "not found" in ack:
        __log_event(f"FAIL: {suite_name}{case_tag}: {ack.strip()}")
        return False

    try:
        if script_module is not None:
            sig = inspect.signature(script_module.run)
            if "test_params_yml" in sig.parameters:
                script_module.run(ser, test_params_yml=test_params_yml)
            else:
                script_module.run(ser)
        else:
            ser.expect_test_summary(suite_name, timeout=1.5)
        elapsed = time.perf_counter() - t0
        __log_event(f"PASS phase1: {suite_name}{case_tag} ({elapsed:.3f} s)")
        return True
    except (TimeoutError, AssertionError, RuntimeError, KeyError, AttributeError) as e:
        elapsed = time.perf_counter() - t0
        __log_event(f"FAIL: {suite_name}{case_tag}: {e} ({elapsed:.3f} s)")
        return False


def _call_decode(mod, la: LogicAnalyzerInstrument,
                 start_ns: int | None, end_ns: int | None,
                 marker_baud: int = 115200,
                 test_params_yml: Path | None = None) -> None:
    """Invoke a script's decode() with whichever signature it accepts."""
    sig = inspect.signature(mod.decode)
    params = sig.parameters
    kwargs = {}
    if "decode_start_ns" in params:
        kwargs["decode_start_ns"] = start_ns
    if "decode_end_ns" in params:
        kwargs["decode_end_ns"] = end_ns
    if "marker_baud" in params:
        kwargs["marker_baud"] = marker_baud
    if "test_params_yml" in params:
        kwargs["test_params_yml"] = test_params_yml
    mod.decode(la, **kwargs)


def _new_la(la_cfg, cli_path: Path, capture_path: Path) -> LogicAnalyzerInstrument:
    return LogicAnalyzerInstrument(
        cli_path=cli_path,
        device=la_cfg.device,
        samplerate=la_cfg.samplerate,
        channels=la_cfg.channels,
        capture_path=capture_path,
    )


def _mk_log_dir(log_dir: Path | None, ordered_suites: list[tuple[str, Path | None]]) -> Path:
    if log_dir:
        run_dir = log_dir.resolve()
    else:
        timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
        tag = ordered_suites[0][0] if len(ordered_suites) == 1 else "vsf_test_suite"
        run_dir = Path(f"logs/{timestamp}-{tag}")
    run_dir.mkdir(parents=True, exist_ok=True)
    return run_dir


def run_test_phase(
    board,
    suite_names: list[str] | None,
    script_override: Path | None,
    case_specs: list[str],
    la_mode: str,
    log_dir: Path | None,
    shuffle_seed: int | None = None,
    test_params_yml: Path | None = None,
    trace_level: str | None = None,
) -> bool:
    """Run suites against firmware that is already flashed and running.

    Returns True if every suite/case passed (phase1 + decode), False otherwise.
    Does NOT build or flash — that is the caller's responsibility.
    """
    discovered = discover_suites()
    ordered_suites = resolve_suites(suite_names, script_override, discovered)
    if not ordered_suites:
        raise RuntimeError("No suites discovered")

    __log_event(f"Suites: {[s for s, _ in ordered_suites]}")
    __log_event(f"LA mode: {la_mode}")
    t0_overall = time.perf_counter()

    if case_specs and len(ordered_suites) > 1:
        raise ValueError("--case/--case-index requires exactly one --suite")

    # Resolve non-numeric --case values to indices via YAML lookup
    if case_specs and len(ordered_suites) == 1:
        suite_name = ordered_suites[0][0]
        if test_params_yml is None:
            raise ValueError(
                "Non-numeric --case values require --test-params to locate test_params.yml"
            )
        params = load_test_params(
            test_params_yml=test_params_yml,
            board_pins_path=board.board_pins,
        )
        resolved: list[str] = []
        for spec in case_specs:
            if spec.isdigit():
                resolved.append(spec)
            else:
                idx = _resolve_case_value(params, suite_name, spec)
                resolved.append(str(idx))
                __log_event(f"Resolved --case {spec} -> index {idx}")
        case_specs = resolved

    run_dir = _mk_log_dir(log_dir, ordered_suites)
    log_path = run_dir / "vsf-bench.jsonl"

    ser = SerialInstrument(board.debug_uart, board.debug_baudrate, audit_log=log_path)
    ser.open()

    la_cfg = board.logic_analyzer
    cli_path = None
    if la_cfg is not None:
        if la_cfg.cli:
            cli_path = Path(la_cfg.cli)
        else:
            cli_path = shutil.which("dsview-cli")
            if cli_path:
                cli_path = Path(cli_path)

    shell = VsfTestShellProtocol(ser)

    # Drain stale boot output, then wait for the shell prompt.
    shell_ready = shell.wait_for_shell_ready()
    if not shell_ready:
        __log_event("Warning: shell not responding, proceeding anyway")

    if trace_level and shell_ready:
        shell.set_trace_level(trace_level)
        __log_event(f"Trace level: {trace_level}")

    # When no explicit --suite filter, intersect with what the firmware reports.
    if not suite_names:
        fw_suites = shell.query_firmware_suites()
        if fw_suites:
            skipped = [s for s, _ in ordered_suites if s not in fw_suites]
            ordered_suites = [(s, p) for s, p in ordered_suites if s in fw_suites]
            if skipped:
                __log_event(f"Skipped (not in firmware): {skipped}")
        __log_event(f"Effective suites: {[s for s, _ in ordered_suites]}")

    loaded: list[tuple[str, Path | None, object | None, bool]] = []
    for suite_name, script_path in ordered_suites:
        if script_path is None:
            loaded.append((suite_name, None, None, False))
            continue
        mod = load_script_module(script_path)
        loaded.append((suite_name, script_path, mod, script_needs_la(script_path, mod)))

    any_needs_la = any(needs for _, _, _, needs in loaded) and la_cfg is not None and cli_path is not None
    overall_pass = True

    if la_mode == "shared" and any_needs_la:
        overall_pass = _test_loop_shared_la(
            loaded, ser, la_cfg, cli_path, run_dir, case_specs,
            board,
            shuffle_seed=shuffle_seed,
            test_params_yml=test_params_yml,
        )
    else:
        overall_pass = _test_loop_per_suite(
            loaded, ser, la_cfg, cli_path, run_dir, case_specs,
            shuffle_seed=shuffle_seed,
            test_params_yml=test_params_yml,
        )

    ser.close()

    with open(log_path, "a") as f:
        verdict = "pass" if overall_pass else "fail"
        f.write(json.dumps({"verdict": verdict}) + "\n")
    elapsed_overall = time.perf_counter() - t0_overall
    print()
    suite_tag = ordered_suites[0][0] if len(ordered_suites) == 1 else f"{len(ordered_suites)} suites"
    __log_event(f"{'PASS' if overall_pass else 'FAIL'} ({elapsed_overall:.3f} s)")
    __log_event(f"Test: {suite_tag} | Log: {log_path}")

    # JUnit XML report
    _write_junit_xml(run_dir, overall_pass, ordered_suites)

    return overall_pass


def _test_loop_shared_la(
    loaded, ser, la_cfg, cli_path, run_dir, case_specs,
    board,
    shuffle_seed: int | None = None,
    test_params_yml: Path | None = None,
) -> bool:
    """Shared LA mode: one capture spans the contiguous LA-needing block.

    `la_start_t` is recorded AFTER `wait_until_started()` returns, so it
    aligns tightly with the dsview-cli capture file's t=0. Each suite's
    decode window is padded by SHARED_WINDOW_PAD_NS on both sides as a
    small safety margin for residual scheduler / USB jitter.
    """
    SHARED_WINDOW_PAD_NS = 500_000_000  # 500 ms each side

    la_indices = [i for i, (_, _, _, n) in enumerate(loaded) if n]
    first_la_idx = la_indices[0]
    last_la_idx = la_indices[-1]

    shared_capture = run_dir / "shared-capture.dsl"
    shared_la: LogicAnalyzerInstrument | None = None
    la_start_t: float | None = None
    suite_windows: list[tuple[str, object, int, int]] = []
    overall_pass = True

    for i, (suite_name, _script_path, mod, _needs) in enumerate(loaded):
        if i == first_la_idx:
            shared_la = _new_la(la_cfg, cli_path, shared_capture)
            shared_la.start(300.0)
            shared_la.wait_until_started(timeout=5.0)
            la_start_t = time.monotonic()
            __log_event(f"Shared LA started -> {shared_capture}")

        cases_to_run = case_specs if case_specs else [None]
        for case in cases_to_run:
            if shuffle_seed is not None and case is None:
                # Shuffle is host-side only; firmware shell no longer accepts seed
                __log_event(f"{suite_name}: shuffle seed={shuffle_seed} (host-side only)")
            case_tag = f".{case}" if case else ""
            __log_event(f"Suite: {suite_name}{case_tag}")
            t_start = int((time.monotonic() - la_start_t) * 1e9) if la_start_t is not None else 0
            ok = _run_script_phase1(suite_name, case, mod, ser, test_params_yml)
            t_end = int((time.monotonic() - la_start_t) * 1e9) if la_start_t is not None else 0
            if not ok:
                overall_pass = False
            if mod is not None and hasattr(mod, "decode") and shared_la is not None:
                suite_windows.append((suite_name, mod, t_start, t_end))

        if i == last_la_idx and shared_la is not None:
            __log_event("Stopping shared LA...")
            shared_la.stop()
            try:
                shared_la.wait(timeout=30.0)
            except (TimeoutError, RuntimeError) as e:
                __log_event(f"LA wait warning: {e}")

    for suite_name, mod, t_start, t_end in suite_windows:
        decode_start = max(0, t_start - SHARED_WINDOW_PAD_NS)
        decode_end = t_end + SHARED_WINDOW_PAD_NS
        __log_event(f"Decoding (shared): {suite_name}  window=[{decode_start/1e9:.2f}s,{decode_end/1e9:.2f}s]")
        t0_decode = time.perf_counter()
        try:
            _call_decode(mod, shared_la, decode_start, decode_end, board.debug_baudrate, test_params_yml)
            elapsed = time.perf_counter() - t0_decode
            __log_event(f"PASS decode: {suite_name} ({elapsed:.3f} s)")
        except (AssertionError, RuntimeError, KeyError, AttributeError, FileNotFoundError) as e:
            elapsed = time.perf_counter() - t0_decode
            __log_event(f"FAIL decode: {suite_name}: {e} ({elapsed:.3f} s)")
            overall_pass = False

    return overall_pass


def _test_loop_per_suite(
    loaded, ser, la_cfg, cli_path, run_dir, case_specs,
    shuffle_seed: int | None = None,
    test_params_yml: Path | None = None,
) -> bool:
    """Per-suite LA mode: one capture per suite (or no LA at all)."""
    overall_pass = True

    for suite_name, _script_path, mod, needs_la in loaded:
        cases_to_run = case_specs if case_specs else [None]
        for case in cases_to_run:
            case_tag = f".{case}" if case else ""
            __log_event(f"Suite: {suite_name}{case_tag}")
            if shuffle_seed is not None and case is None:
                # Shuffle is host-side only; firmware shell no longer accepts seed
                __log_event(f"{suite_name}: shuffle seed={shuffle_seed} (host-side only)")

            scene_la: LogicAnalyzerInstrument | None = None
            if needs_la and la_cfg is not None and cli_path is not None:
                label = f"{suite_name}{('_' + case) if case else ''}"
                capture_path = run_dir / f"{label}-capture.dsl"
                scene_la = _new_la(la_cfg, cli_path, capture_path)
                scene_la.start(180.0)
                scene_la.wait_until_started(timeout=5.0)

            ok = _run_script_phase1(suite_name, case, mod, ser, test_params_yml)
            if not ok:
                overall_pass = False

            if scene_la is not None:
                scene_la.stop()
                try:
                    scene_la.wait(timeout=15.0)
                except (TimeoutError, RuntimeError) as e:
                    __log_event(f"LA wait warning: {e}")

            if ok and mod is not None and hasattr(mod, "decode") and scene_la is not None:
                t0_decode = time.perf_counter()
                try:
                    _call_decode(mod, scene_la, None, None, board.debug_baudrate, test_params_yml)
                    elapsed = time.perf_counter() - t0_decode
                    __log_event(f"PASS decode: {suite_name}{case_tag} ({elapsed:.3f} s)")
                except (AssertionError, RuntimeError, KeyError, AttributeError, FileNotFoundError) as e:
                    elapsed = time.perf_counter() - t0_decode
                    __log_event(f"FAIL decode: {suite_name}{case_tag}: {e} ({elapsed:.3f} s)")
                    overall_pass = False

    return overall_pass


# ---------------------------------------------------------------------------
# JUnit XML report
# ---------------------------------------------------------------------------

def _write_junit_xml(
    run_dir: Path, overall_pass: bool, suites: list[tuple[str, Path | None]]
) -> None:
    """Generate a JUnit XML report at ``run_dir/report.junit.xml``."""
    import xml.etree.ElementTree as ET
    from xml.dom import minidom

    n_suites = len(suites)
    testsuite = ET.Element("testsuite", {
        "name": "vsf-bench",
        "tests": str(n_suites),
        "failures": str(0 if overall_pass else 1),
        "errors": "0",
        "time": "0",
    })
    for suite_name, _ in suites:
        tc = ET.SubElement(testsuite, "testcase", {
            "name": suite_name,
            "classname": "vsf.test." + suite_name,
        })
        if not overall_pass:
            ET.SubElement(tc, "failure", {
                "message": f"Suite {suite_name}: FAIL",
                "type": "AssertionError",
            })

    xml_str = minidom.parseString(ET.tostring(testsuite)).toprettyxml(indent="  ")
    report_path = run_dir / "report.junit.xml"
    report_path.write_text(xml_str)
    __log_event(f"JUnit XML: {report_path}")


# ---------------------------------------------------------------------------
# Hang recovery
# ---------------------------------------------------------------------------

_HANG_CONSECUTIVE_THRESHOLD = 2


def _try_hang_recovery(board, hang_count: int) -> bool:
    """Attempt recovery after *hang_count* consecutive suite timeouts.

    Returns True if recovery was attempted, False if unavailable.
    """
    if hang_count < _HANG_CONSECUTIVE_THRESHOLD:
        return False
    if not board.power or not board.debug_probe:
        return False

    __log_event(
        f"Hang detected ({hang_count} consecutive timeouts) — "
        f"attempting recovery"
    )
    try:
        _board_power_cycle(board, delay_off_s=1.0)
        __log_event("Recovery: power-cycle OK")
    except Exception as e:
        __log_event(f"Recovery: power-cycle failed — {e}")
        return False

    # Attempt crash dump
    try:
        from vsf_bench.debug import DebugSession

        probe_cfg = board.debug_probe
        with DebugSession(
            target=probe_cfg.get("target", "cortex_m"),
            probe=probe_cfg.get("probe"),
        ) as dbg:
            dump = dbg.crash_dump()
        __log_event(f"Recovery: crash-dump — {dump.fault_type}")
    except Exception as e:
        __log_event(f"Recovery: crash-dump unavailable — {e}")

    return True


# ---------------------------------------------------------------------------
# Multi-stage pipeline
# ---------------------------------------------------------------------------

def run_pipeline(
    pipeline: "PipelineConfig",
    board: "BoardConfig",
    project_map: dict,
    test_params_yml: "Path | None" = None,
) -> bool:
    """Execute a multi-stage build/flash/test pipeline on *board*.

    Returns True when all stages pass. Raises RuntimeError on first failure.
    Each stage's build_dir is cached so later stages can reuse build output.
    """
    build_cache: dict[str, Path] = {}   # project_name → build_dir
    n_stages = len(pipeline.stages)

    # Populate board-level params that runners need
    for _proj in project_map.values():
        for _rcfg in _proj.runners.values():
            p = _rcfg.params
            p.setdefault("program_port", board.program_uart)
            p.setdefault("debug_port", board.debug_uart)
            p.setdefault("debug_baudrate", board.debug_baudrate)

    __log_event(f"Pipeline: {pipeline.name} ({pipeline.description})")
    __log_event(f"Board: {board.name}  Stages: {n_stages}")

    for i, stage in enumerate(pipeline.stages, start=1):
        project = project_map[stage.project]
        __log_event(
            f"[Stage {i}/{n_stages}] {stage.project}: {' + '.join(stage.actions)}"
        )

        # ── power-cycle before stage ──
        if stage.power_cycle and board.power:
            _board_power_cycle(board)

        # ── build ──
        if "build" in stage.actions:
            build_dir = build_phase(project)
            build_cache[stage.project] = build_dir

        # ── flash ──
        if "flash" in stage.actions:
            build_dir = build_cache.get(stage.project)
            if build_dir is None:
                build_dir = Path(project.build.build_dir)
                if not build_dir.exists():
                    raise RuntimeError(
                        f"[Stage {i}] build dir missing for "
                        f"'{stage.project}': {build_dir}"
                    )
            _pipeline_stage_flash(board, project, build_dir, stage)

        # ── test (placeholder) ──
        if "test" in stage.actions:
            __log_event(
                f"[Stage {i}] test: skipping "
                f"(test-in-pipeline not yet implemented)"
            )

        __log_event(f"[Stage {i}/{n_stages}] {stage.project}: OK")

    __log_event(f"Pipeline {pipeline.name}: PASS")
    return True


def _pipeline_stage_flash(board, project, build_dir: Path, stage) -> None:
    """Flash one pipeline stage, applying flash_overrides + wait_for.

    flash_overrides can override: runner name, artifact, params.
    wait_for: after flash, open serial and wait for regex match.
    """
    from copy import deepcopy
    from vsf_bench.config import ArtifactConfig

    # Determine runner name (stage override or project default)
    runner_name = stage.flash_overrides.get("runner") if stage.flash_overrides else None
    if runner_name is None:
        runner_name = project.active_runner

    runner_cfg = project.runners.get(runner_name)
    if runner_cfg is None:
        avail = list(project.runners.keys())
        raise RuntimeError(
            f"Runner '{runner_name}' not found in project "
            f"'{project.name or stage.project}'. Available: {avail}"
        )

    # Patch runner config if flash_overrides modify artifact or params
    if stage.flash_overrides:
        if "artifact" in stage.flash_overrides or "params" in stage.flash_overrides:
            runner_cfg = deepcopy(runner_cfg)
            if "artifact" in stage.flash_overrides:
                art = stage.flash_overrides["artifact"]
                runner_cfg.artifact = ArtifactConfig(
                    name=art["name"],
                    format=art.get("format", "bin"),
                )
            if "params" in stage.flash_overrides:
                runner_cfg.params.update(stage.flash_overrides["params"])

    # Execute flash via the existing program_phase
    runner_cls = get_runner_class(runner_cfg.type)
    if runner_cls is None:
        raise RuntimeError(f"Unknown runner type: {runner_cfg.type}")
    runner = runner_cls(runner_cfg)
    with runner:
        __log_event(f"Programming via {runner_name}...")
        runner.flash(build_dir)
    __log_event("Program complete")

    # ── wait_for: open serial, wait for log pattern ──
    if stage.wait_for:
        __log_event(f"Waiting for log pattern: /{stage.wait_for}/")
        ser = SerialInstrument(board.debug_uart, board.debug_baudrate)
        try:
            ser.open()
            ser.expect(stage.wait_for, timeout=60.0)
            __log_event(f"Log pattern matched: {stage.wait_for}")
        except TimeoutError:
            raise RuntimeError(
                f"Timeout waiting for /{stage.wait_for}/ after flash "
                f"(board {board.name}, serial {board.debug_uart})"
            )
        finally:
            ser.close()


# ---------------------------------------------------------------------------
# Multi-board test (Phase 12)
# ---------------------------------------------------------------------------

def _load_all_connected_boards(hardware_map_path: str) -> list:
    """Return all connected boards from hardware-map.yml."""
    from vsf_bench.hardware_map import load as _load_hw
    import yaml
    with open(hardware_map_path, "r", encoding="utf-8") as f:
        raw = yaml.safe_load(f) or {}
    board_entries = raw.get("boards", [])
    boards = []
    for entry in board_entries:
        if entry.get("connected", True):
            board = _load_hw(hardware_map_path, board_name=entry.get("name"))
            boards.append(board)
    return boards


def run_test_phase_all(
    hardware_map_path: str,
    project_name: str,
    suite_names: list[str] | None = None,
    la_mode: str = "shared",
    log_dir: str | None = None,
    trace_level: str = "debug",
    timeout_per_board: float = 600.0,
) -> dict[str, bool]:
    """Run test phase on all connected boards in parallel.

    Returns ``{board_name: passed}`` dict.
    """
    from concurrent.futures import ThreadPoolExecutor, as_completed
    from vsf_bench.hardware_map import load_board_and_project

    boards = _load_all_connected_boards(hardware_map_path)
    if not boards:
        raise RuntimeError("No connected boards found")

    __log_event(f"Multi-board test: {len(boards)} boards, project={project_name}")

    def _run_one(board):
        board_dir = str(Path(hardware_map_path).parent)
        try:
            lock = acquire_board_lock(board, wait_spec=-1.0)  # wait indefinitely
        except LockBusyError:
            __log_event(f"[{board.name}] SKIP: board locked")
            return board.name, False
        try:
            _, project = load_board_and_project(
                hardware_map_path, board_name=board.name, project_name=project_name,
            )
            build_dir = build_phase(project)
            program_phase(board, build_dir, project=project)
            ok = run_test_phase(
                board=board,
                suite_names=suite_names,
                script_override=None,
                case_specs=[],
                la_mode=la_mode,
                log_dir=Path(log_dir) / board.name if log_dir else None,
                shuffle_seed=None,
                test_params_yml=None,
                trace_level=trace_level,
            )
            return board.name, ok
        except Exception as e:
            __log_event(f"[{board.name}] ERROR: {e}")
            return board.name, False
        finally:
            if lock:
                lock.release()

    results: dict[str, bool] = {}
    with ThreadPoolExecutor(max_workers=len(boards)) as executor:
        futures = {executor.submit(_run_one, b): b for b in boards}
        for future in as_completed(futures):
            name, ok = future.result()
            results[name] = ok
            __log_event(f"[{name}] {'PASS' if ok else 'FAIL'}")

    passed = sum(1 for v in results.values() if v)
    __log_event(f"Multi-board: {passed}/{len(results)} boards PASS")
    return results
