"""UF2Runner — flash via USB mass storage (BOOTSEL mode)."""

import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

from vsf_bench.config import RunnerConfig
from vsf_bench.runners.base import FlashRunner


class UF2Runner(FlashRunner):
    REQUIRED_PARAMS = []

    def __init__(self, config: RunnerConfig):
        super().__init__(config)
        self.device: str | None = config.params.get("device")
        self.mount_point: str | None = config.params.get("mount_point")

    @classmethod
    def validate_params(cls, params: dict) -> list[str]:
        has_device = "device" in params
        has_mount = "mount_point" in params
        if has_device == has_mount:
            return ["UF2 runner requires exactly one of 'device' or 'mount_point'"]
        return []

    def flash(self, build_dir: Path) -> None:
        """Flash firmware via UF2 mass storage copy."""
        assert self._config.artifact is not None
        uf2 = build_dir / self._config.artifact.name
        if not uf2.exists():
            raise FileNotFoundError(f"UF2 not found: {uf2}")

        if self.device:
            self._flash_manual(uf2)
        else:
            self._flash_auto_mounted(uf2)

    def _flash_manual(self, uf2: Path) -> None:
        assert self.device is not None
        tmpdir = tempfile.mkdtemp(prefix="vsf-bench-uf2-")
        try:
            subprocess.run(["mount", self.device, tmpdir], check=True, capture_output=True)
            shutil.copy2(str(uf2), tmpdir)
            subprocess.run(["sync"], check=True)
        except subprocess.CalledProcessError as e:
            print(f"UF2 manual mount failed: {e.stderr.decode() if e.stderr else e}", file=sys.stderr)
            raise
        finally:
            subprocess.run(["umount", tmpdir], capture_output=True)
            os.rmdir(tmpdir)

    def _flash_auto_mounted(self, uf2: Path) -> None:
        assert self.mount_point is not None
        mount = Path(self.mount_point)
        if not mount.is_dir():
            raise FileNotFoundError(
                f"UF2 mount point {self.mount_point} not found (board not in BOOTSEL mode?)"
            )
        shutil.copy2(str(uf2), str(mount))
        subprocess.run(["sync"], check=True)
