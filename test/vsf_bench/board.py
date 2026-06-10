"""Board operations — load, lock, power-cycle, hub discovery."""

from __future__ import annotations

import time
from pathlib import Path

from vsf_bench.config.map import load as load_hardware_map, validate_runners
from vsf_bench.config.map import load_board_and_project as _load_board_and_project
from vsf_bench.utils.lock import BoardLock


def _log_event(message: str) -> None:
    """Log an event. Uses TeeLogger when available, else print."""
    try:
        from vsf_bench.utils.tee_logger import get_logger
        get_logger().event(message)
    except RuntimeError:
        from datetime import datetime as _dt
        ts = _dt.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]
        print(f"[{ts}] [vsf-bench] {message}")


# ── Board loading ───────────────────────────────────────────

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

    board = load_hardware_map(str(hardware_map_path), board_name=board_name)
    validate_runners(board)
    return board


def load_all_connected_boards(hardware_map_path: str) -> list:
    import yaml
    with open(hardware_map_path, "r", encoding="utf-8") as f:
        raw = yaml.safe_load(f) or {}
    board_entries = raw.get("boards", [])
    boards = []
    for entry in board_entries:
        if entry.get("connected", True):
            board = load_hardware_map(hardware_map_path, board_name=entry.get("name"))
            boards.append(board)
    return boards


# ── Board locking ───────────────────────────────────────────

def acquire_board_lock(board, wait_spec=None) -> BoardLock | None:
    from vsf_bench.utils.lock import LockBusyError
    lock = BoardLock(board.name)
    if wait_spec is None:
        return None
    if wait_spec < 0:
        lock.acquire(wait=True)
    else:
        lock.acquire(wait=True, timeout=wait_spec)
    _log_event(f"Lock acquired: {board.name}")
    return lock


# ── Power control ───────────────────────────────────────────

def find_hub_by_addr(hub_addr: int) -> str | None:
    import serial.tools.list_ports
    from smartusbhub import SmartUSBHub
    for port in serial.tools.list_ports.comports():
        if port.vid == 0x1A86 and port.pid == 0xFE0C:
            try:
                hub = SmartUSBHub(port.device)
                addr = hub.get_device_address()
                hub.disconnect()
                if addr == hub_addr:
                    return port.device
            except Exception:
                continue
    return None


def power_cycle(board, delay_off_s: float = 0.5) -> None:
    if not board.power:
        raise RuntimeError(f"Board '{board.name}' has no power config")
    if board.power.type != "smartusbhub":
        raise RuntimeError(f"Unsupported power type: {board.power.type}")

    hub_com = find_hub_by_addr(board.power.hub_addr)
    if not hub_com:
        raise RuntimeError(f"SmartUSBHub with addr 0x{board.power.hub_addr:04X} not found")

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
                _log_event(f"power {label} port {board.power.port}")
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
