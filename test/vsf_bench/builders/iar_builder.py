"""IAR Embedded Workbench builder — directly calls IarBuild.exe.

No external script dependency. Locates IarBuild.exe via:
1. ``IAR_EWARM_DIR`` environment variable
2. Default install path: ``C:/iar/ewarm-*.*.*/common/bin/IarBuild.exe``
"""

import glob
import os
import shutil
import subprocess
from pathlib import Path

from vsf_bench.builders.base import BuildRunner


def _find_iarbuild() -> str:
    """Locate IarBuild.exe. Raises FileNotFoundError if not found."""
    # 1. Environment variable
    env_dir = os.environ.get("IAR_EWARM_DIR")
    if env_dir:
        candidate = Path(env_dir) / "common" / "bin" / "IarBuild.exe"
        if candidate.exists():
            return str(candidate)

    # 2. shutil.which (PATH lookup)
    which = shutil.which("IarBuild.exe")
    if which:
        return which

    # 3. Search IAR install roots across all drives and known patterns
    search_roots = _collect_iar_roots()
    for root in search_roots:
        candidate = root / "common" / "bin" / "IarBuild.exe"
        if candidate.exists():
            return str(candidate)

    raise FileNotFoundError(
        "IarBuild.exe not found. Set IAR_EWARM_DIR environment variable "
        "to your IAR EWARM installation root."
    )


def _collect_iar_roots() -> list[Path]:
    """Collect candidate IAR EWARM install directories."""
    roots: list[Path] = []

    # Common base directories
    progfiles_x86 = os.environ.get("ProgramFiles(x86)", "C:/Program Files (x86)")
    progfiles = os.environ.get("ProgramFiles", "C:/Program Files")

    base_dirs = [
        Path(progfiles_x86) / "IAR Systems",
        Path(progfiles) / "IAR Systems",
        Path("C:/iar"),
        Path("/c/iar"),
    ]

    # Also try other drive roots (D:, E:)
    import string
    for drive in string.ascii_uppercase[3:6]:  # D, E, F
        for pf in ("Program Files (x86)", "Program Files"):
            p = Path(f"{drive}:/{pf}/IAR Systems")
            if p.exists():
                base_dirs.append(p)

    for base in base_dirs:
        if not base.exists():
            continue
        # Pattern 1: ewarm-* (e.g. C:/iar/ewarm-9.60.3)
        for name in ("ewarm-*", "arm-*"):
            for d in sorted(base.glob(name), reverse=True):
                if (d / "common" / "bin" / "IarBuild.exe").exists():
                    roots.append(d)
                    break  # newest first

        # Pattern 2: Embedded Workbench * (e.g. Embedded Workbench 8.3)
        for d in sorted(base.glob("Embedded Workbench *"), reverse=True):
            arm_dir = d / "arm"
            if arm_dir.exists():
                roots.append(arm_dir)
            else:
                roots.append(d)

    return roots


class IARBuilder(BuildRunner):
    """Build an IAR .ewp project directly via IarBuild.exe."""

    def __init__(self, build_cfg):
        self._build_cfg = build_cfg

    def build(self) -> Path:
        cfg = self._build_cfg
        source_dir = Path(cfg.source_dir)
        build_dir = Path(cfg.build_dir)
        params = cfg.params

        ewp_file = params.get("project")
        if not ewp_file:
            raise RuntimeError("IARBuilder: missing 'project' param (ewp filename)")

        config = params.get("config", "")
        if not config:
            raise RuntimeError("IARBuilder: missing 'config' param")

        ewp_path = source_dir / ewp_file
        if not ewp_path.exists():
            raise FileNotFoundError(f"IARBuilder: .ewp not found: {ewp_path}")

        iarbuild = _find_iarbuild()
        jobs = os.cpu_count() or 8

        cmd = [iarbuild, str(ewp_path), "-make", config, "-parallel", str(jobs)]
        print(f"[vsf-bench] IAR: {' '.join(cmd)}")
        result = subprocess.run(cmd, capture_output=False, cwd=str(source_dir))
        if result.returncode != 0:
            raise RuntimeError(
                f"IAR build failed (exit {result.returncode})"
            )

        return build_dir
