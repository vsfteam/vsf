"""CMakeBuilder — cmake configure + build."""

import os
import subprocess
from pathlib import Path

from vsf_bench.builders.base import BuildRunner
from vsf_bench.config.models import BuildConfig


class CMakeBuilder(BuildRunner):
    def __init__(self, build_config: BuildConfig):
        self.source_dir = Path(build_config.source_dir)
        self.build_dir = Path(build_config.build_dir)

    def build(self) -> Path:
        """Run cmake configure (if needed) + build. Returns build directory.

        Paths are resolved against cwd so the caller controls the anchor
        directory via os.chdir or by using absolute paths in hardware-map.yml.
        """
        self.build_dir.mkdir(parents=True, exist_ok=True)

        if not (self.build_dir / "CMakeCache.txt").exists():
            subprocess.run(
                [
                    "cmake",
                    "-B", str(self.build_dir),
                    "-S", str(self.source_dir),
                ],
                check=True,
            )

        subprocess.run(
            ["cmake", "--build", str(self.build_dir), "-j", str(os.cpu_count() or 1)],
            check=True,
        )

        return self.build_dir
