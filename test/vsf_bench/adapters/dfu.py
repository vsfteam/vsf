"""DFUAdapter — DFU flash via dfu-util + optional GPIO for DFU key.

Uses a ``GPIO`` capable adapter (FT232H, CH347, etc.) to hold the DFU key
during power-on.  Without GPIO, relies on the bootloader auto-entering DFU
mode when no valid application is present.

Lifecycle (managed by flash_phase context manager)::

    adapter = DFUAdapter(config)
    with adapter:                  # prepare(): press DFU key via GPIO
        power_cycle(board)         # board boots with DFU key held → DFU mode
        adapter.flash(build_dir)   # wait for DFU device → dfu-util → release key
    # close(): release GPIO
"""

import subprocess
import time
from pathlib import Path

from vsf_bench.capabilities.program import ProgramCapability
from vsf_bench.capabilities.gpio import GPIO
from vsf_bench.config import RunnerConfig


class DFUAdapter(ProgramCapability):
    """Flash firmware via DFU (Device Firmware Upgrade).

    Required params:
      * ``vid``: DFU device USB vendor ID (hex)
      * ``pid``: DFU device USB product ID

    Optional GPIO params (for DFU key):
      * ``gpio_adapter``: adapter class path (e.g. "vsf_bench.adapters.ft232h.FT232HAdapter")
      * ``gpio_serial``: FT232H/CH347 USB serial
      * ``gpio_pin``: pin number (default 0)
      * ``gpio_active_low``: True if active-low (default True)
    """

    REQUIRED_PARAMS = ["vid", "pid"]

    def __init__(self, config: RunnerConfig):
        super().__init__(config)
        self._vid = int(config.params["vid"], 0)
        self._pid = int(config.params["pid"], 0)
        self._gpio = self._create_gpio(config.params)

    @staticmethod
    def _create_gpio(params: dict) -> GPIO | None:
        gpio_adapter = params.get("gpio_adapter")
        if not gpio_adapter:
            return None
        # Dynamic import: "vsf_bench.adapters.ft232h.FT232HAdapter"
        parts = gpio_adapter.rsplit(".", 1)
        if len(parts) != 2:
            raise RuntimeError(f"Invalid gpio_adapter: {gpio_adapter}")
        import importlib
        mod = importlib.import_module(parts[0])
        cls = getattr(mod, parts[1])
        return cls(
            serial=params["gpio_serial"],
            pin=int(params.get("gpio_pin", 0)),
            port=params.get("gpio_port", "AD"),
            active_low=params.get("gpio_active_low", "true").lower() != "false",
        )

    # ── DFU key (GPIO) ────────────────────────────────────

    def prepare(self) -> None:
        if self._gpio:
            self._gpio.open()
            self._gpio.set(True)

    def close(self) -> None:
        if self._gpio:
            self._gpio.close()

    def _release_key(self) -> None:
        if self._gpio:
            self._gpio.set(False)

    # ── DFU flash ─────────────────────────────────────────

    def flash(self, build_dir: Path) -> None:
        artifact = self._config.artifact
        if artifact is None:
            raise RuntimeError("DFUAdapter: no artifact configured")

        dfu_file = build_dir / artifact.name
        if not dfu_file.exists():
            raise FileNotFoundError(f"DFU file not found: {dfu_file}")

        if not self._wait_dfu(timeout=10):
            raise RuntimeError(
                f"DFU device not found "
                f"(vid=0x{self._vid:04X} pid=0x{self._pid:04X})"
            )

        self._release_key()
        self._dfu_download(str(dfu_file), timeout=120)

    def _wait_dfu(self, timeout: float = 10) -> bool:
        vid_s = f"{self._vid:04x}"
        pid_s = f"{self._pid:04x}"
        start = time.time()
        while time.time() - start < timeout:
            try:
                result = subprocess.run(
                    ["dfu-util", "-l", "-d", f"{vid_s}:{pid_s}"],
                    capture_output=True, text=True, timeout=5,
                )
                if result.returncode == 0 and "Found DFU" in result.stdout:
                    return True
            except FileNotFoundError:
                raise RuntimeError("dfu-util not found in PATH")
            except subprocess.TimeoutExpired:
                pass
            time.sleep(0.3)
        return False

    def _dfu_download(self, dfu_file: str, timeout: float = 120) -> None:
        vid_s = f"{self._vid:04x}"
        pid_s = f"{self._pid:04x}"
        cmd = ["dfu-util", "-d", f"{vid_s}:{pid_s}", "-a", "0", "-D", dfu_file]
        result = subprocess.run(
            cmd, capture_output=True, text=True, timeout=timeout,
        )
        if result.returncode != 0:
            raise RuntimeError(
                f"dfu-util failed (exit {result.returncode}): {result.stderr}"
            )
