#!/usr/bin/env python3
"""Generate C test parameter header from YAML configuration.

Usage:
    python gen_test_params.py <input.yml> <output.h>

The input YAML defines test scenarios; the output is a C header with
static const arrays and macros consumed by the firmware.

YAML structure:
    marker:        # ignored by this script (host-side only)
    include:       # optional: list (or single string) of YAML files to merge
        - usart.yml
        - gpio.yml
    <scenario_key>:
        name: <c_identifier_stem>     # required: drives generated names
        defaults:                      # optional: scenario-level default params → #define <NAME>_DEFAULT_<KEY>
            <key>: <scalar|list>
        cases:                         # required: per-case varying params → struct array
            - {<field>: <value>, ..., host: {...}}

Each case dict becomes a C struct initializer; the `host` key is reserved
for host-only data and is excluded from C generation.

The `include:` directive supports recursive composition. Included files are
loaded in order; later files override earlier ones on top-level key collision.
The entry file's own keys are merged on top of all includes. Relative paths
in `include:` resolve against the directory of the YAML file declaring them.

Generated identifiers, given `name: foo`:
    struct: vsf_test_usart_foo_case_t
    array:  __foo_cases
    defaults: VSF_TEST_FOO_DEFAULT_<KEY>
"""

import argparse
import sys
from pathlib import Path

import importlib.util
if importlib.util.find_spec("yaml") is None:
    print("Error: PyYAML is required. Install with: pip install pyyaml", file=sys.stderr)
    raise SystemExit(1)

from vsf_bench.test.params_loader import load_yaml_with_includes


def _format_value(value) -> str:
    """Format a YAML value as a C literal/expression."""
    if isinstance(value, bool):
        return "true" if value else "false"
    if isinstance(value, list):
        return "(" + " | ".join(str(v) for v in value) + ")"
    if isinstance(value, str):
        return value
    return str(value)


def _format_case(case: dict, defaults_keys: set | None = None) -> str:
    """Format one case dict as a C designated initializer.

    Skips reserved keys (`host`, `la`) that hold host-side only data.
    Also skips keys inherited from scenario-level `defaults` (they are
    emitted as #define macros instead), except `expect_pass` which is
    present in every case struct.
    """
    skip = {"host", "la"}
    if defaults_keys:
        skip |= defaults_keys
        skip.discard("expect_pass")

    parts = []
    for key, value in case.items():
        if key in skip:
            continue
        parts.append(f".{key} = {_format_value(value)}")
    return "{ " + ", ".join(parts) + " }"


def _emit_scenario(lines: list[str], scenario_key: str, sc: dict) -> None:
    name = sc.get("name")
    if not name:
        return
    cases = sc.get("cases")
    if not cases:
        return

    upper = name.upper()
    lines.append(f"/* === {scenario_key} ({name}) === */")
    lines.append("")

    # timeout_s → #define VSF_TEST_<NAME>_TIMEOUT_MS (firmware-side opt-in)
    timeout_s = sc.get("timeout_s")
    if timeout_s is not None:
        lines.append(f"#define VSF_TEST_{upper}_TIMEOUT_MS  ((uint32_t)({timeout_s} * 1000))")
        lines.append("")

    # suite_timeout_s → #define VSF_TEST_<NAME>_SUITE_TIMEOUT_MS (firmware-side opt-in)
    suite_timeout_s = sc.get("suite_timeout_s")
    if suite_timeout_s is not None:
        lines.append(f"#define VSF_TEST_{upper}_SUITE_TIMEOUT_MS  ((uint32_t)({suite_timeout_s} * 1000))")
        lines.append("")

    # defaults params → #define VSF_TEST_<NAME>_DEFAULT_<KEY>
    defaults = sc.get("defaults") or {}
    defaults_keys: set[str] = set()
    if defaults:
        for key, value in defaults.items():
            if key in ("host", "la"):
                continue
            macro = f"VSF_TEST_{upper}_DEFAULT_{key.upper()}"
            lines.append(f"#define {macro}  {_format_value(value)}")
            defaults_keys.add(key)
        lines.append("")

    # PARAMS_INIT macro for static parameter initialization (no suite back-pointer)
    params_init_macro = f"VSF_TEST_{upper}_PARAMS_INIT"
    lines.append(f"#define {params_init_macro}  \\")
    for i, case in enumerate(cases):
        comma = "," if i < len(cases) - 1 else ""
        suffix = "  \\" if i < len(cases) - 1 else ""
        init = _format_case(case, defaults_keys)
        lines.append(f"    {init}{comma}{suffix}")

    lines.append(f"#define VSF_TEST_{upper}_CASE_COUNT  {len(cases)}")
    lines.append("")

    lines.append(f"#ifndef VSF_TEST_{upper}_ENABLE")
    lines.append(f"#   define VSF_TEST_{upper}_ENABLE  ENABLED")
    lines.append("#endif")
    lines.append("")

def _load_yaml_with_includes(yml_path: Path, stack: list[Path] | None = None) -> dict:
    """Backward-compatible alias for the shared loader with global_base."""
    global_base = (Path(__file__).resolve().parent / ".." / "vsf_test_suite" / "params").resolve()
    return load_yaml_with_includes(yml_path, stack, global_base=global_base)


def generate_header(yml_path: Path, out_path: Path) -> None:
    params = _load_yaml_with_includes(yml_path)

    lines = [
        "/* Auto-generated from test_params.yml — do not edit manually */",
        "#ifndef __TEST_PARAMS_GENERATED_H__",
        "#define __TEST_PARAMS_GENERATED_H__",
        "",
    ]

    # Global firmware params from marker section (still needed by scenario code)
    marker = params.get("marker") or {}
    if "delay_ms" in marker:
        lines.append(f"#define VSF_TEST_MARKER_DELAY_MS  {marker['delay_ms']}")

    for scenario_key, sc in params.items():
        if scenario_key == "marker":
            continue
        if not isinstance(sc, dict):
            continue
        # Per-scenario payload + drain (currently shared values across scenarios,
        # but emit per-scenario to keep gen script generic).
        if "payload" in sc or "payload_drain_ms" in sc:
            payload = sc.get("payload", "Hello VSF\r\n")
            drain_ms = sc.get("payload_drain_ms", 500)
            c_payload = (
                payload.replace('\\', '\\\\')
                       .replace('"', '\\"')
                       .replace('\r', '\\r')
                       .replace('\n', '\\n')
            )
            name = sc.get("name", scenario_key).upper()
            lines.append(f'#define VSF_TEST_{name}_PAYLOAD          "{c_payload}"')
            lines.append(f"#define VSF_TEST_{name}_PAYLOAD_DRAIN_MS {drain_ms}")
    lines.append("")

    for scenario_key, sc in params.items():
        if scenario_key == "marker":
            continue
        if not isinstance(sc, dict):
            continue
        _emit_scenario(lines, scenario_key, sc)

    lines.extend([
        "#endif /* __TEST_PARAMS_GENERATED_H__ */",
        "",
    ])

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text("\n".join(lines))
    print(f"Generated: {out_path}")


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate C header from test params YAML")
    parser.add_argument("yml", type=Path, help="Input YAML file")
    parser.add_argument("out", type=Path, help="Output C header file")
    args = parser.parse_args()

    if not args.yml.exists():
        print(f"Error: {args.yml} not found", file=sys.stderr)
        return 1

    generate_header(args.yml, args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
