"""Test execution — suite discovery, shell interaction, LA orchestration, JUnit XML."""

from __future__ import annotations

import inspect
import json
import shutil
import time
from pathlib import Path

from vsf_bench.board import power_cycle as _board_power_cycle, acquire_board_lock, load_all_connected_boards
from vsf_bench.utils.serial import SerialInstrument
from vsf_bench.utils.tee_logger import get_logger as _get_logger
from vsf_bench.vsf_test.shell import VsfTestShellProtocol
from vsf_bench.vsf_test.suite import discover_suites, load_script_module, script_needs_la, resolve_suites
from vsf_bench.vsf_test.params_loader import load_test_params


_PHASE1_ACK_TIMEOUT = 2.0


# ── Scenario / case resolution ──────────────────────────────

def _find_scenario_for_suite(params: dict, suite_name: str) -> tuple[str, dict] | None:
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
    raise ValueError(f"Case value '{case_value}' not found in suite '{suite_name}'")


# ── Phase 1 shell test ──────────────────────────────────────

def _run_script_phase1(
    suite_name: str,
    case: str | None,
    script_module,
    ser: SerialInstrument,
    test_params_yml: Path | None = None,
) -> bool:
    shell = VsfTestShellProtocol(ser)
    case_tag = f".{case}" if case else ""
    cmd = shell.build_run_cmd(suite_name, case)
    shell.drain_repl()
    ser.send(cmd)
    _get_logger().event(f"Triggered: {cmd.strip()}")
    t0 = time.perf_counter()

    ack = None
    for attempt, tmo in ((1, 1.0), (2, _PHASE1_ACK_TIMEOUT)):
        ack = shell.expect_suite_ack(timeout=tmo)
        if ack is not None:
            break
        if attempt == 1:
            shell.drain_repl()
            ser.send(cmd)
            _get_logger().event(f"Retrying: {cmd.strip()}")
        else:
            _get_logger().event(f"FAIL: {suite_name}{case_tag}: no shell ack within 1s")
            return False
    assert ack is not None
    if "not found" in ack:
        _get_logger().event(f"FAIL: {suite_name}{case_tag}: {ack.strip()}")
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
        _get_logger().event(f"PASS phase1: {suite_name}{case_tag} ({elapsed:.3f} s)")
        return True
    except (TimeoutError, AssertionError, RuntimeError, KeyError, AttributeError) as e:
        elapsed = time.perf_counter() - t0
        _get_logger().event(f"FAIL: {suite_name}{case_tag}: {e} ({elapsed:.3f} s)")
        return False


# ── LA decode callback ──────────────────────────────────────

def _call_decode(mod, adapter, channels: dict, capture_path: Path,
                 start_ns: int | None, end_ns: int | None,
                 marker_baud: int = 115200,
                 test_params_yml: Path | None = None) -> None:
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
    if "capture_path" in params:
        kwargs["capture_path"] = capture_path
    if "channels" in params:
        kwargs["channels"] = channels
    mod.decode(adapter, **kwargs)


# ── Test phase ──────────────────────────────────────────────

def run_test_phase(
    board,
    suite_names: list[str] | None,
    script_override: Path | None,
    case_specs: list[str],
    la_mode: str,
    run_dir: Path,
    shuffle_seed: int | None = None,
    test_params_yml: Path | None = None,
    trace_level: str | None = None,
) -> bool:
    discovered = discover_suites()
    ordered_suites = resolve_suites(suite_names, script_override, discovered)
    if not ordered_suites:
        raise RuntimeError("No suites discovered")

    _get_logger().event(f"Suites: {[s for s, _ in ordered_suites]}")
    _get_logger().event(f"LA mode: {la_mode}")
    t0_overall = time.perf_counter()

    if case_specs and len(ordered_suites) > 1:
        raise ValueError("--case/--case-index requires exactly one --suite")

    if case_specs and len(ordered_suites) == 1:
        suite_name = ordered_suites[0][0]
        if test_params_yml is None:
            raise ValueError("Non-numeric --case values require --test-params")
        params = load_test_params(test_params_yml=test_params_yml, board_pins_path=board.board_pins)
        resolved: list[str] = []
        for spec in case_specs:
            if spec.isdigit():
                resolved.append(spec)
            else:
                idx = _resolve_case_value(params, suite_name, spec)
                resolved.append(str(idx))
                _get_logger().event(f"Resolved --case {spec} -> index {idx}")
        case_specs = resolved

    log_path = run_dir / "test-events.jsonl"

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
    time.sleep(4.0)  # wait for firmware boot + shell init (thread adds delay)
    shell_ready = shell.wait_for_shell_ready(timeout=5.0)
    if not shell_ready:
        _get_logger().event("Warning: shell not responding, proceeding anyway")

    if trace_level and shell_ready:
        shell.set_trace_level(trace_level)
        _get_logger().event(f"Trace level: {trace_level}")

    if not suite_names:
        fw_suites = shell.query_firmware_suites()
        if fw_suites:
            skipped = [s for s, _ in ordered_suites if s not in fw_suites]
            ordered_suites = [(s, p) for s, p in ordered_suites if s in fw_suites]
            if skipped:
                _get_logger().event(f"Skipped (not in firmware): {skipped}")
        _get_logger().event(f"Effective suites: {[s for s, _ in ordered_suites]}")

    loaded: list[tuple[str, Path | None, object | None, bool]] = []
    for suite_name, script_path in ordered_suites:
        if script_path is None:
            loaded.append((suite_name, None, None, False))
            continue
        mod = load_script_module(script_path)
        loaded.append((suite_name, script_path, mod, script_needs_la(script_path, mod)))

    any_needs_la = any(needs for _, _, _, needs in loaded) and la_cfg is not None and cli_path is not None
    overall_pass = True

    if any_needs_la:
        marker_baudrate = la_cfg.marker_baudrate
        from vsf_bench.vsf_test.scene import CaptureScene
        scene = CaptureScene(la_cfg, cli_path, run_dir, marker_baudrate=marker_baudrate)
        _get_logger().event(f"LA marker baudrate: {marker_baudrate}")

        if la_mode == "shared":
            overall_pass = scene.run_shared(
                loaded, ser, case_specs,
                run_phase1=_run_script_phase1, call_decode=_call_decode,
                log=_get_logger().event, shuffle_seed=shuffle_seed,
                test_params_yml=test_params_yml,
            )
        else:
            overall_pass = scene.run_per_suite(
                loaded, ser, case_specs,
                run_phase1=_run_script_phase1, call_decode=_call_decode,
                log=_get_logger().event, shuffle_seed=shuffle_seed,
                test_params_yml=test_params_yml,
            )
    else:
        for suite_name, _script_path, mod, _needs in loaded:
            cases_to_run = case_specs if case_specs else [None]
            for case in cases_to_run:
                if shuffle_seed is not None and case is None:
                    _get_logger().event(f"{suite_name}: shuffle seed={shuffle_seed} (host-side only)")
                case_tag = f".{case}" if case else ""
                _get_logger().event(f"Suite: {suite_name}{case_tag}")
                ok = _run_script_phase1(suite_name, case, mod, ser, test_params_yml)
                if not ok:
                    overall_pass = False

    ser.close()

    with open(log_path, "a") as f:
        verdict = "pass" if overall_pass else "fail"
        f.write(json.dumps({"verdict": verdict}) + "\n")
    elapsed_overall = time.perf_counter() - t0_overall
    print()
    suite_tag = ordered_suites[0][0] if len(ordered_suites) == 1 else f"{len(ordered_suites)} suites"
    _get_logger().event(f"{'PASS' if overall_pass else 'FAIL'} ({elapsed_overall:.3f} s)")
    _get_logger().event(f"Test: {suite_tag} | Log: {log_path}")

    _write_junit_xml(run_dir, overall_pass, ordered_suites)
    return overall_pass


# ── JUnit XML ───────────────────────────────────────────────

def _write_junit_xml(run_dir: Path, overall_pass: bool, suites: list[tuple[str, Path | None]]) -> None:
    import xml.etree.ElementTree as ET
    from xml.dom import minidom

    n_suites = len(suites)
    testsuite = ET.Element("testsuite", {
        "name": "vsf-bench", "tests": str(n_suites),
        "failures": str(0 if overall_pass else 1), "errors": "0", "time": "0",
    })
    for suite_name, _ in suites:
        tc = ET.SubElement(testsuite, "testcase", {
            "name": suite_name, "classname": "vsf.test." + suite_name,
        })
        if not overall_pass:
            ET.SubElement(tc, "failure", {
                "message": f"Suite {suite_name}: FAIL", "type": "AssertionError",
            })

    xml_str = minidom.parseString(ET.tostring(testsuite)).toprettyxml(indent="  ")
    (run_dir / "report.junit.xml").write_text(xml_str)
    _get_logger().event(f"JUnit XML: {run_dir / 'report.junit.xml'}")


# ── Hang recovery ───────────────────────────────────────────

_HANG_CONSECUTIVE_THRESHOLD = 2


def try_hang_recovery(board, hang_count: int) -> bool:
    if hang_count < _HANG_CONSECUTIVE_THRESHOLD:
        return False
    if not board.power or not board.debug_probe:
        return False

    _get_logger().event(f"Hang detected ({hang_count} consecutive timeouts) — attempting recovery")
    try:
        _board_power_cycle(board, delay_off_s=1.0)
        _get_logger().event("Recovery: power-cycle OK")
    except Exception as e:
        _get_logger().event(f"Recovery: power-cycle failed — {e}")
        return False

    try:
        from vsf_bench.utils.debug import DebugSession
        probe_cfg = board.debug_probe
        with DebugSession(target=probe_cfg.get("target", "cortex_m"), probe=probe_cfg.get("probe")) as dbg:
            dump = dbg.crash_dump()
        _get_logger().event(f"Recovery: crash-dump — {dump.fault_type}")
    except Exception as e:
        _get_logger().event(f"Recovery: crash-dump unavailable — {e}")

    return True


# ── Multi-board test ────────────────────────────────────────

def run_test_phase_all(
    hardware_map_path: str,
    project_name: str,
    suite_names: list[str] | None = None,
    la_mode: str = "shared",
    log_dir: str | None = None,
    trace_level: str = "debug",
    timeout_per_board: float = 600.0,
) -> dict[str, bool]:
    from concurrent.futures import ThreadPoolExecutor, as_completed
    from vsf_bench.config.map import load_board_and_project
    from vsf_bench.board import load_board
    from vsf_bench.phases.build import build_phase
    from vsf_bench.phases.program import program_phase
    from vsf_bench.board import acquire_board_lock
    from vsf_bench.utils.lock import LockBusyError

    boards = load_all_connected_boards(hardware_map_path)
    if not boards:
        raise RuntimeError("No connected boards found")

    _get_logger().event(f"Multi-board test: {len(boards)} boards, project={project_name}")

    def _run_one(board):
        try:
            lock = acquire_board_lock(board, wait_spec=-1.0)
        except LockBusyError:
            _get_logger().event(f"[{board.name}] SKIP: board locked")
            return board.name, False
        try:
            _, project = load_board_and_project(
                hardware_map_path, board_name=board.name, project_name=project_name,
            )
            build_dir = build_phase(project)
            program_phase(board, build_dir, project=project)
            ok = run_test_phase(
                board=board, suite_names=suite_names, script_override=None,
                case_specs=[], la_mode=la_mode,
                run_dir=Path(log_dir) / board.name if log_dir else None,
                shuffle_seed=None, test_params_yml=None, trace_level=trace_level,
            )
            return board.name, ok
        except Exception as e:
            _get_logger().event(f"[{board.name}] ERROR: {e}")
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
            _get_logger().event(f"[{name}] {'PASS' if ok else 'FAIL'}")

    passed = sum(1 for v in results.values() if v)
    _get_logger().event(f"Multi-board: {passed}/{len(results)} boards PASS")
    return results
