"""Suite discovery, script loading, and resolution.

Pure functions — no hardware interaction. Testable without a board.
"""

import importlib.util
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


# ---------------------------------------------------------------------------
# PeripheralType auto-gen from vsf_test.h
# ---------------------------------------------------------------------------

_PERIPHERAL_ENUM_RE = r"^\s*VSF_PERIPHERAL_TYPE_(\w+)\s*(?:=\s*(\d+))?,?"


def _find_vsf_test_header() -> Path | None:
    """Locate vsf_test.h relative to vsf-bench package."""
    candidates = [
        # vsf submodule layout
        Path(__file__).resolve().parent / ".." / ".." / ".." / ".."
        / "source" / "component" / "test" / "vsf_test" / "vsf_test.h",
    ]
    for p in candidates:
        if p.exists():
            return p.resolve()
    return None


def generate_peripheral_type_enum() -> type:
    """Parse ``vsf_test.h`` and return a Python ``PeripheralType`` IntEnum.

    Returns a dynamically created enum whose members match the C enum
    ``vsf_peripheral_type_t``.  Cached after first call.
    """
    import re
    from enum import IntEnum

    header = _find_vsf_test_header()
    if header is None:
        # Fallback: empty enum so import doesn't fail
        return IntEnum("PeripheralType", {"NONE": 0})

    content = header.read_text(encoding="utf-8", errors="replace")
    # Find the enum block
    m = re.search(
        r"typedef\s+enum\s+vsf_peripheral_type_t\s*\{(.*?)\}\s*vsf_peripheral_type_t",
        content, re.DOTALL,
    )
    if not m:
        return IntEnum("PeripheralType", {"NONE": 0})

    members: dict[str, int] = {}
    auto_val = 0
    for line in m.group(1).splitlines():
        line = line.strip()
        if not line or line.startswith("//") or line.startswith("/*"):
            continue
        # Match VSF_PERIPHERAL_TYPE_XXX or VSF_PERIPHERAL_TYPE_XXX = N
        mm = re.match(r"VSF_PERIPHERAL_TYPE_(\w+)\s*(?:=\s*(-?\d+))?\s*,?", line)
        if mm:
            name = mm.group(1)
            if mm.group(2) is not None:
                auto_val = int(mm.group(2))
            members[name] = auto_val
            auto_val += 1

    return IntEnum("PeripheralType", members)  # type: ignore[arg-type]


# Module-level cache
PeripheralType = generate_peripheral_type_enum()
