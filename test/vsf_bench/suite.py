"""Suite discovery, script loading, and resolution.

Pure functions — no hardware interaction. Testable without a board.
"""

import importlib.util
import sys
from pathlib import Path


def discover_suites() -> dict[str, Path]:
    """Walk the test tree and return {suite_name: script_path}.

    Discovers `vsf_test_<suite>.py` files under `vsf/test/vsf_test_suite/`.
    Uses the script's own location (vsf/test/vsf_bench/) to find the
    sibling `vsf_test_suite/` directory, so discovery works regardless of
    project layout (root repo, vsf.demo directly, or a private HAL).
    """
    suites: dict[str, Path] = {}
    # vsf_bench and vsf_test_suite are siblings under vsf/test/.
    base = (Path(__file__).resolve().parent / ".." / "vsf_test_suite").resolve()
    if not base.exists():
        return suites

    def _scan_dir(scan_dir: Path) -> None:
        if not scan_dir.is_dir():
            return
        # Each sub-directory may contain its own suite/ folder (driver pattern).
        for entry in scan_dir.iterdir():
            suite_dir = entry / "suite"
            if suite_dir.is_dir():
                for f in suite_dir.glob("vsf_test_*.py"):
                    stem = f.stem
                    if stem.startswith("vsf_test_"):
                        suite_name = stem[len("vsf_test_"):]
                        suites[suite_name] = f
        # The scan_dir itself may contain a suite/ folder directly (arch pattern).
        direct_suite = scan_dir / "suite"
        if direct_suite.is_dir():
            for f in direct_suite.glob("vsf_test_*.py"):
                stem = f.stem
                if stem.startswith("vsf_test_"):
                    suite_name = stem[len("vsf_test_"):]
                    suites[suite_name] = f

    # Scan HAL driver and arch sub-trees
    _scan_dir(base / "hal" / "driver")
    _scan_dir(base / "hal" / "arch")
    return suites


def load_script_module(path: Path):
    """Import a test script `.py` file as a module."""
    if not path.exists():
        raise FileNotFoundError(f"Test script not found: {path}")
    spec = importlib.util.spec_from_file_location("test_script", path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"Cannot create module spec for {path}")
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    if not hasattr(mod, "run"):
        raise AttributeError(f"Test script {path} must define run(serial)")
    return mod


def script_needs_la(script_path: Path | None, mod=None) -> bool:
    """A script needs LA iff it defines `decode()`."""
    if mod is not None:
        return hasattr(mod, "decode")
    if script_path is None:
        return False
    try:
        source = script_path.read_text()
        return "def decode(" in source
    except Exception:
        return False


def resolve_suites(
    requested: list[str] | None,
    script_override: Path | None,
    discovered: dict[str, Path],
) -> list[tuple[str, Path | None]]:
    """Apply --suite / --script arguments to the discovered suite map.

    * No --suite → run every discovered suite in alphabetical order.
    * One or more --suite names → run those, in given order.
    * --script overrides the script path for a single --suite.
    """
    if requested:
        if script_override and len(requested) > 1:
            raise ValueError("--script can only be used with a single --suite")
        ordered: list[tuple[str, Path | None]] = []
        for name in requested:
            if script_override:
                ordered.append((name, script_override.resolve()))
            elif name in discovered:
                ordered.append((name, discovered[name]))
            else:
                raise KeyError(
                    f"Suite not found: {name}. Discovered: {sorted(discovered.keys())}"
                )
        return ordered
    return [(name, discovered[name]) for name in sorted(discovered.keys())]
