"""SWDRunner — flash via OpenOCD."""

import subprocess
import sys
from pathlib import Path

from vsf_bench.config.models import RunnerConfig
from vsf_bench.runners.base import FlashRunner


class SWDRunner(FlashRunner):
    REQUIRED_PARAMS = ["interface", "target"]

    def __init__(self, config: RunnerConfig):
        super().__init__(config)
        self.interface = config.params["interface"]
        self.target = config.params["target"]
        self.speed = config.params.get("speed", 5000)

    def flash(self, build_dir: Path) -> None:
        """Flash firmware via OpenOCD SWD."""
        assert self._config.artifact is not None
        elf = build_dir / self._config.artifact.name
        if not elf.exists():
            raise FileNotFoundError(f"ELF not found: {elf}")

        cmd = [
            "openocd",
            "-f", f"interface/{self.interface}",
            "-f", f"target/{self.target}",
            "-c", f"adapter speed {self.speed}",
            "-c", f"program {elf} verify reset exit",
        ]
        try:
            subprocess.run(cmd, check=True, capture_output=True)
        except subprocess.CalledProcessError as e:
            print(f"OpenOCD failed (exit {e.returncode}):\n{e.stderr.decode()}", file=sys.stderr)
            raise
