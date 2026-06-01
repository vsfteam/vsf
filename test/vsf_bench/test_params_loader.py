"""Test parameter YAML loading with `include:` directive resolution.

Used by both host-side test scripts and gen_test_params.py (CMake).

Environment variable:
    VSF_TEST_GLOBAL_PARAMS_DIR  —  absolute or project-relative path to the
                                    global test-params directory.  If unset,
                                    the loader defaults to
                                    the script-relative vsf_test_suite/params directory.
"""

from __future__ import annotations

import os
import re
import sys
from pathlib import Path

import yaml

GLOBAL_PARAMS_ENV = "VSF_TEST_GLOBAL_PARAMS_DIR"


def _get_global_base() -> Path | None:
    env = os.environ.get(GLOBAL_PARAMS_ENV)
    return Path(env) if env else None


def _deep_merge(base: dict, overlay: dict) -> dict:
    result = {}
    for key, value in base.items():
        if key in overlay:
            if isinstance(value, dict) and isinstance(overlay[key], dict):
                result[key] = _deep_merge(value, overlay[key])
            else:
                result[key] = overlay[key]
        else:
            result[key] = value
    for key, value in overlay.items():
        if key not in base:
            result[key] = value
    return result


PIN_MACRO_RE = re.compile(r"^#define\s+(VSF_TEST_[A-Z0-9_]*PIN[A-Z0-9_]*)\s+(\d+)\s*(?:/\*.*\*/)?$")


def _load_pinmap(board_pins_path: str | Path | None) -> dict[str, int]:
    """Parse VSF_TEST_*PIN* macros from a board-specific C header.

    If `board_pins_path` is given and exists, parse it directly.
    Returns a dict mapping macro name → integer value.
    """
    pinmap: dict[str, int] = {}
    if not board_pins_path:
        return pinmap
    header_path = Path(board_pins_path)
    if not header_path.exists():
        return pinmap
    with open(header_path) as f:
        for line in f:
            m = PIN_MACRO_RE.match(line.strip())
            if m:
                pinmap[m.group(1)] = int(m.group(2))
    return pinmap


def _resolve_pinmap_in_value(value, pinmap: dict[str, int]):
    """Resolve a single YAML value through the pinmap."""
    if isinstance(value, str) and value in pinmap:
        return pinmap[value]
    return value


def _resolve_pinmap(params: dict, pinmap: dict[str, int]) -> dict:
    """Recursively resolve pin-macro strings in all case dicts."""
    if not pinmap:
        return params
    for key, value in params.items():
        if key == "marker" or not isinstance(value, dict):
            continue
        cases = value.get("cases")
        if not isinstance(cases, list):
            continue
        for case in cases:
            if not isinstance(case, dict):
                continue
            for field, field_value in case.items():
                if field in ("host", "la"):
                    continue
                case[field] = _resolve_pinmap_in_value(field_value, pinmap)
    return params


def _apply_defaults(params: dict) -> dict:
    for key, value in params.items():
        if not isinstance(value, dict):
            continue
        defaults = value.get("defaults")
        if defaults is None:
            continue
        cases = value.get("cases")
        if not isinstance(cases, list):
            continue
        for i, case in enumerate(cases):
            if isinstance(case, dict):
                cases[i] = _deep_merge(defaults, case)
    return params


def _resolve_include(inc: str, local_dir: Path, global_base: Path | None) -> Path:
    """Resolve an include path: local first, then global fallback."""
    inc_path = Path(inc)
    if inc_path.is_absolute():
        return inc_path

    # Try local first
    local_path = local_dir / inc_path
    if local_path.exists():
        return local_path

    # Fallback to global base
    if global_base is not None:
        global_path = global_base / inc_path
        if global_path.exists():
            return global_path

    # Return local path so the existing error message is accurate
    return local_path


def load_yaml_with_includes(
    yml_path: Path,
    stack: list[Path] | None = None,
    global_base: Path | None = None,
) -> dict:
    yml_path = Path(yml_path).resolve()
    stack = stack or []
    if yml_path in stack:
        cycle = " -> ".join(str(p) for p in stack + [yml_path])
        print(f"Error: include cycle detected: {cycle}", file=sys.stderr)
        raise SystemExit(1)

    with open(yml_path) as f:
        params = yaml.safe_load(f) or {}

    if not isinstance(params, dict):
        return params

    includes = params.pop("include", None)
    if includes is None:
        return _apply_defaults(params)

    if isinstance(includes, str):
        includes = [includes]
    if not isinstance(includes, list):
        print(f"Error: 'include' in {yml_path} must be a string or list", file=sys.stderr)
        raise SystemExit(1)

    global_base = global_base or _get_global_base()

    merged: dict = {}
    for inc in includes:
        inc_path = _resolve_include(inc, yml_path.parent, global_base)
        if not inc_path.exists():
            print(f"Error: include file not found: {inc_path} (from {yml_path})", file=sys.stderr)
            raise SystemExit(1)
        sub = load_yaml_with_includes(inc_path, stack + [yml_path], global_base)
        for key, value in sub.items():
            if key in merged:
                print(f"Warning: duplicate key '{key}' in {inc_path} overrides earlier include", file=sys.stderr)
            merged[key] = value

    for key, value in params.items():
        if key in merged:
            print(f"Warning: duplicate key '{key}' in {yml_path} overrides include", file=sys.stderr)
        merged[key] = value

    return _apply_defaults(merged)


def load_test_params(
    test_params_yml: str | Path,
    board_pins_path: str | Path | None = None,
    global_base: str | Path | None = None,
) -> dict:
    """Load aggregated test params.

    Args:
        test_params_yml: Path to the root test_params.yml (required).
        board_pins_path: Optional path to vsf_test_board_pins.h for pin macro resolution.
        global_base: Optional global params directory (absolute or cwd-relative).
                     Falls back to VSF_TEST_GLOBAL_PARAMS_DIR env var,
                     then to the script-relative vsf_test_suite/params directory.
    """
    yml_path = Path(test_params_yml).resolve()

    if global_base is not None:
        gb = Path(global_base)
        if not gb.is_absolute():
            gb = Path.cwd() / gb
    elif os.environ.get(GLOBAL_PARAMS_ENV):
        gb = Path(os.environ[GLOBAL_PARAMS_ENV])
    else:
        # script-relative: vsf_bench/../vsf_test_suite/params
        gb = (Path(__file__).resolve().parent / ".." / "vsf_test_suite" / "params").resolve()

    params = load_yaml_with_includes(yml_path, global_base=gb)
    pinmap = _load_pinmap(board_pins_path)
    return _resolve_pinmap(params, pinmap)
