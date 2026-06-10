"""Pipeline step execution engine — unified steps model.

build, program, power_cycle, delay, wait_for, serial_send, gpio_set,
la_start/stop/decode, run, loop are all equal step types.
"""

from __future__ import annotations

import subprocess
import time
from pathlib import Path
from typing import TYPE_CHECKING, Any

if TYPE_CHECKING:
    from vsf_bench.config.models import (
        BoardConfig, PipelineConfig, ProjectConfig, StepConfig,
    )

from vsf_bench.utils.tee_logger import get_logger as _get_logger


def _log(message: str) -> None:
    try:
        _get_logger().event(message)
    except RuntimeError:
        print(f"[vsf-bench] {message}")


def execute_pipeline(
    pipeline: PipelineConfig,
    board: BoardConfig,
    run_dir: Path,
    project_map: dict[str, ProjectConfig],
    overrides: dict[str, Any] | None = None,
    repeat_override: int | None = None,
) -> bool:
    """Execute all steps. Returns True if all pass."""
    overrides = overrides or {}
    try:
        return _execute_steps(
            pipeline.steps, board, run_dir, project_map,
            overrides, repeat_override, pipeline.timeout,
        )
    finally:
        _stop_debug_stream_capture(run_dir)


def _execute_steps(
    steps: list[StepConfig],
    board: BoardConfig,
    run_dir: Path,
    project_map: dict[str, ProjectConfig],
    overrides: dict[str, Any],
    repeat_override: int | None,
    timeout: float | None,
) -> bool:
    t0 = time.monotonic()
    all_ok = True

    for step in steps:
        # ── timeout gate ──
        if timeout and (time.monotonic() - t0) > timeout:
            _log(f"Skipping remaining {len(steps)} steps — pipeline timeout ({timeout}s)")
            break

        # ── repeat override ──
        if repeat_override is not None and step.type.value == "loop":
            step.params["repeat"] = repeat_override

        # ── overrides ──
        _apply_overrides(step, overrides)

        # ── execute with retries ──
        ok = _execute_step_with_retry(step, board, run_dir, project_map, overrides)

        if not ok:
            all_ok = False
            if step.on_failure:
                _log(f"Step '{step.id or step.type.value}' failed — executing on_failure")
                _execute_steps(step.on_failure, board, run_dir, project_map, overrides, None, None)
            if not step.continue_on_error:
                _log(f"Pipeline stopped at {step.id or step.type.value}")
                return False

    return all_ok


def _apply_overrides(step: StepConfig, overrides: dict[str, Any]) -> None:
    if not overrides:
        return
    st = step.type.value
    for key, val in overrides.items():
        # id.key match
        if step.id:
            prefix = f"{step.id}."
            if key.startswith(prefix):
                param_key = key[len(prefix):]
                step.params[param_key] = val
                return
        # type.key match (e.g. loop.repeat=1)
        prefix = f"{st}."
        if key.startswith(prefix):
            param_key = key[len(prefix):]
            step.params[param_key] = val
            return
        # bare key match
        if "." not in key and key in step.params:
            step.params[key] = val


def _execute_step_with_retry(
    step: StepConfig, board: BoardConfig, run_dir: Path,
    project_map: dict[str, ProjectConfig], overrides: dict[str, Any],
) -> bool:
    max_tries = step.max_retries
    for attempt in range(1, max_tries + 1):
        try:
            _execute_one_step(step, board, run_dir, project_map, overrides)
            return True
        except Exception as e:
            _log(f"Step '{step.id or step.type.value}' attempt {attempt}/{max_tries} failed: {e}")
            if attempt < max_tries:
                time.sleep(0.5)
    return False


def _execute_one_step(
    step: StepConfig, board: BoardConfig, run_dir: Path,
    project_map: dict[str, ProjectConfig], overrides: dict[str, Any],
) -> None:
    st = step.type.value
    p = step.params
    _log(f"  [{st}] {step.id or ''} {p}")

    if st == "build":
        _step_build(p, project_map, run_dir)
    elif st == "program":
        _step_program(p, board, project_map, run_dir)
    elif st == "power_cycle":
        _step_power_cycle(board, p)
    elif st == "power_off":
        _step_power(board, state=0)
    elif st == "power_on":
        _step_power(board, state=1)
    elif st == "delay":
        dur = float(p.get("duration", p.get("delay", 1.0)))
        _log(f"  delay {dur}s")
        time.sleep(dur)
    elif st == "wait_for":
        _step_wait_for(p, board)
    elif st == "serial_send":
        _step_serial_send(p, board)
    elif st == "gpio_set":
        _step_gpio_set(p, board)
    elif st == "la_start":
        _step_la_start(p, board, run_dir)
    elif st == "la_stop":
        _step_la_stop(board, run_dir)
    elif st == "la_decode":
        _step_la_decode(p, board, run_dir)
    elif st == "debug_stream_start":
        _start_debug_stream_capture(board)
    elif st == "debug_stream_stop":
        _stop_debug_stream_capture(run_dir)
    elif st == "run":
        _step_run(p, run_dir)
    elif st == "loop":
        loop_repeat = p.get("repeat", 1)
        loop_steps = step.steps or []
        _log(f"  loop x{loop_repeat}")
        for i in range(loop_repeat):
            iter_dir = run_dir / "runs" / f"{i+1:02d}"
            iter_dir.mkdir(parents=True, exist_ok=True)
            _log(f"  loop iteration {i+1}/{loop_repeat} → {iter_dir}")
            _execute_steps(loop_steps, board, iter_dir, project_map, overrides, None, None)
    else:
        raise RuntimeError(f"Unknown step type: {st}")


# ------------------------------------------------------------------ step impls

# Global state for LA capture session
_la_adapter = None


def _step_build(params, project_map, run_dir):
    project_name = params.get("build") or params.get("project") or next(iter(project_map))
    _ensure_project_loaded(project_name, project_map)
    project = project_map[project_name]
    from vsf_bench.pipeline import build_phase
    build_phase(project)


def _ensure_project_loaded(project_name: str, project_map: dict) -> None:
    if project_name not in project_map:
        from vsf_bench.config.map import load_project
        hw_path = Path("board/hardware-map.yml")  # fallback
        project_map[project_name] = load_project(str(hw_path), project_name)


def _step_program(params, board, project_map, run_dir):
    project_name = params.get("program") or params.get("project") or next(iter(project_map))
    _ensure_project_loaded(project_name, project_map)
    project = project_map[project_name]
    for _rcfg in project.runners.values():
        p = _rcfg.params
        p.setdefault("program_port", board.program_uart)
        p.setdefault("debug_port", board.debug_uart)
        p.setdefault("debug_baudrate", board.debug_baudrate)
    from vsf_bench.pipeline import program_phase
    build_dir = Path(project.build.build_dir)
    program_phase(board, build_dir, project=project)


def _step_power_cycle(board, params=None):
    from vsf_bench.pipeline import _board_power_cycle
    delay = float((params or {}).get("delay", 0.5))
    _board_power_cycle(board, delay_off_s=delay)


def _step_power(board, state):
    from vsf_bench.pipeline import _find_hub_by_addr
    from smartusbhub import SmartUSBHub
    hub_com = _find_hub_by_addr(board.power.hub_addr)
    if hub_com:
        hub = SmartUSBHub(hub_com)
        hub.set_channel_power(board.power.port, state=state)
        hub.disconnect()


def _step_wait_for(params, board):
    from vsf_bench.utils.serial import SerialInstrument
    timeout = float(params.get("timeout", 10))
    patterns = params.get("expect", params.get("patterns", []))
    ser = SerialInstrument(board.debug_uart, board.debug_baudrate)
    ser.open()
    try:
        result = ser.expect_any(patterns, timeout=timeout)
        _log(f"  wait_for matched: {result}")
    finally:
        ser.close()


def _step_serial_send(params, board):
    from vsf_bench.utils.serial import SerialInstrument
    data = params.get("data") or params.get("text") or ""
    ser = SerialInstrument(board.debug_uart, board.debug_baudrate)
    ser.open()
    try:
        ser.send(data)
    finally:
        ser.close()


def _step_gpio_set(params, board):
    from vsf_bench.adapters.ft232h import FT232HAdapter
    adapter_name = params.get("adapter", "")
    pin = int(params.get("pin", 0))
    level = params.get("level", "low")
    duration = float(params.get("duration", 0))
    active = level == "low"  # active_low=true by default

    # Resolve adapter from hardware-map
    gpio_cfg = _resolve_gpio_adapter(adapter_name, board)
    adapter = FT232HAdapter(
        serial=gpio_cfg.get("adapter_serial", ""),
        pin=pin, port=gpio_cfg.get("port", "AD"),
        active_low=gpio_cfg.get("active_low", True),
    )
    adapter.open()
    try:
        adapter.set(active)
        if duration > 0:
            time.sleep(duration)
        adapter.set(not active)
    finally:
        adapter.close()


def _resolve_gpio_adapter(name: str, board) -> dict:
    """Resolve GPIO adapter config from board's gpio_adapter_serial or hardware-map."""
    return {
        "adapter_serial": board.gpio_adapter_serial or "FT96OF9L",
        "port": "AD",
        "active_low": True,
    }


def _step_la_start(params, board, run_dir):
    global _la_adapter
    from vsf_bench.adapters.dsview import DSViewAdapter
    la_cfg = board.logic_analyzer
    if la_cfg is None:
        raise RuntimeError("No logic_analyzer config for board")
    cli = _resolve_la_cli(la_cfg)
    channel = params.get("channel", "CH8")
    duration = float(params.get("duration", 30))
    _la_adapter = DSViewAdapter(cli, la_cfg.device, la_cfg.samplerate, {"capture": channel})
    capture_path = run_dir / f"la-{channel}.dsl"
    _la_adapter.start(capture_path, duration)
    _la_adapter.wait_until_started()
    _log(f"  la_start: {channel} for {duration}s")


def _step_la_stop(board, run_dir):
    global _la_adapter
    if _la_adapter:
        _la_adapter.wait(timeout=60)
        _la_adapter = None


def _step_la_decode(params, board, run_dir):
    from vsf_bench.adapters.dsview import DSViewAdapter
    from vsf_bench.config.models import UARTConfig
    from vsf_bench.utils.core import parse_uart_csv
    la_cfg = board.logic_analyzer
    if la_cfg is None:
        raise RuntimeError("No logic_analyzer config for board")
    cli = _resolve_la_cli(la_cfg)
    channel = params.get("channel", "CH8")
    baudrate = int(params.get("baudrate", 2000000))
    cfg = UARTConfig(baudrate=baudrate)
    adapter = DSViewAdapter(cli, la_cfg.device, la_cfg.samplerate, {})
    capture_path = run_dir / f"la-{channel}.dsl"
    csv = adapter.decode_uart(capture_path, channel, cfg)
    data = parse_uart_csv(csv)
    text = data.decode("utf-8", errors="replace")
    output = run_dir / f"la-decode-{channel}-{baudrate}.txt"
    output.write_text(text, encoding="utf-8")
    _log(f"  la_decode: {len(data)} bytes -> {output}")


def _step_run(params, run_dir):
    import os
    cmd = params.get("cmd") or params.get("command") or ""
    env = {**os.environ, "RUN_DIR": str(run_dir)}
    _log(f"  run: {cmd}")
    subprocess.run(cmd, shell=True, cwd=str(run_dir), env=env)


# Global debug stream capture state
_debug_buf = ""
_debug_thread = None
_debug_ser = None


def _start_debug_stream_capture(board):
    global _debug_buf, _debug_thread, _debug_ser
    import threading, time
    from vsf_bench.utils.serial import SerialInstrument
    _debug_buf = ""
    # Retry — COM port may be briefly held by flasher
    for attempt in range(5):
        try:
            _debug_ser = SerialInstrument(board.debug_uart, board.debug_baudrate)
            _debug_ser.open()
            break
        except Exception:
            if attempt < 4:
                time.sleep(0.2)
            else:
                raise

    def _read_loop():
        global _debug_buf
        while _debug_ser._ser and _debug_ser._ser.is_open:
            try:
                chunk = _debug_ser.read_all(timeout=0.5)
                if chunk:
                    _debug_buf += chunk
                    _get_logger().device(chunk.rstrip())
            except Exception:
                break

    _debug_thread = threading.Thread(target=_read_loop, daemon=True)
    _debug_thread.start()


def _stop_debug_stream_capture(run_dir=None):
    global _debug_buf, _debug_thread, _debug_ser
    if _debug_ser:
        _debug_ser.close()
        _debug_ser = None
    if _debug_buf and run_dir:
        normalized = _debug_buf.replace("\r\n", "\n").replace("\r", "\n")
        output = run_dir / "debug_stream.log"
        # Append mode — multiple stop/start cycles accumulate
        mode = "ab" if output.exists() else "wb"
        with open(output, mode) as f:
            f.write(normalized.encode("utf-8"))
        _log(f"  debug_stream: {len(normalized)} bytes -> {output}")
        _debug_buf = ""


def _resolve_la_cli(la_cfg):
    import shutil
    from pathlib import Path
    cli = Path(la_cfg.cli) if la_cfg.cli else Path("dsview-cli")
    if not la_cfg.cli:
        resolved = shutil.which("dsview-cli")
        if resolved:
            cli = Path(resolved)
    return cli
