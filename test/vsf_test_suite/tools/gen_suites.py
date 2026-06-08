"""vs告-test suite codegen — manifest-driven suite list generator.

Reads a per-project ``test_manifest.yml``, scans the shared suite directories,
applies exclude/include rules, and generates:

    * ``vsf_test_suites.h``  — periph driver includes + data union + externs
    * ``vsf_test_suites.c``  — suite list array + count
    * ``vsf_test_suite_enables.h`` — #define ENABLED for each enabled suite

Usage::

    python gen_suites.py <manifest.yml> <output_dir>
"""

from __future__ import annotations

import re
import sys
from fnmatch import fnmatch
from pathlib import Path

# Force UTF-8 output on Windows
if sys.platform == "win32":
    import io
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
from textwrap import dedent


# ---------------------------------------------------------------------------
# Discovery
# ---------------------------------------------------------------------------

_SUITE_DIR = Path(__file__).resolve().parent.parent  # vsf_test_suite/

# Map: "periph" → relative-from-suite-dir path to periph driver *.h
def _scan_suite_c_files() -> dict[str, list[Path]]:
    """Walk suite dirs and return ``{periph: [suite_c_path, ...]}``.

    Periph names are normalised to lower-case (e.g. ADC→adc, ARCH→arch).
    """
    periphs: dict[str, list[Path]] = {}

    # Driver peripherals: hal/driver/<periph>/suite/vsf_test_*.c
    driver_dir = _SUITE_DIR / "hal" / "driver"
    if driver_dir.is_dir():
        for entry in sorted(driver_dir.iterdir()):
            if not entry.is_dir():
                continue
            periph = entry.name.lower()
            suite_dir = entry / "suite"
            if not suite_dir.is_dir():
                continue
            c_files = sorted(suite_dir.glob("vsf_test_*.c"))
            if c_files:
                periphs[periph] = list(c_files)

    # Arch: hal/arch/suite/vsf_test_*.c
    arch_suite = _SUITE_DIR / "hal" / "arch" / "suite"
    if arch_suite.is_dir():
        c_files = sorted(arch_suite.glob("vsf_test_*.c"))
        if c_files:
            periphs["arch"] = list(c_files)

    return periphs


def _parse_suite_name(c_path: Path, periph: str) -> str | None:
    """Extract suite short-name from filename.

    ``vsf_test_adc_oneshot.c`` → ``adc_oneshot``  (adc is periph)
    ``vsf_test_arch_preempt.c`` → ``arch_preempt``
    ``vsf_test_gpio_toggle_stress.c`` → ``gpio_toggle_stress``
    """
    stem = c_path.stem  # vsf_test_<periph>_<suite>
    if not stem.startswith("vsf_test_"):
        return None
    suffix = stem[len("vsf_test_"):]
    # For arch: vsf_test_arch_preempt → suffix = arch_preempt, suite = preempt
    # For driver: vsf_test_gpio_toggle → suffix = gpio_toggle, suite = toggle
    if suffix.startswith(periph + "_"):
        return suffix  # gpio_toggle
    return suffix  # arch_preempt


def _suite_to_enable_macro(suite_full: str) -> str:
    """``gpio_toggle`` → ``VSF_TEST_GPIO_TOGGLE_ENABLE``"""
    return f"VSF_TEST_{suite_full.upper()}_ENABLE"


def _periph_to_enable_macro(periph: str) -> str:
    """``gpio`` → ``VSF_TEST_GPIO_ENABLE``"""
    return f"VSF_TEST_{periph.upper()}_ENABLE"


def _suite_to_struct_name(c_path: Path) -> str:
    """Extract C struct name from file path.

    ``vsf_test_adc_oneshot.c`` → struct ``vsf_test_adc_oneshot``
    """
    return c_path.stem  # vsf_test_gpio_toggle


def _find_periph_header(periph: str) -> Path | None:
    """Find the periph driver header for a given peripheral."""
    candidates = [
        _SUITE_DIR / "hal" / "driver" / periph / f"vsf_test_{periph}.h",
        _SUITE_DIR / "hal" / "arch" / f"vsf_test_{periph}.h",
    ]
    for cand in candidates:
        if cand.exists():
            return cand
    return None


def _make_include_path(header_path: Path) -> str:
    """Convert absolute header path to a relative include path.

    e.g. ``<vsf_test_suite>/hal/driver/gpio/vsf_test_gpio.h``
    """
    try:
        rel = header_path.relative_to(_SUITE_DIR)
    except ValueError:
        return str(header_path)
    return str(rel).replace("\\", "/")


# ---------------------------------------------------------------------------
# Manifest
# ---------------------------------------------------------------------------

def _load_manifest(manifest_path: Path) -> dict:
    import yaml
    if not manifest_path.exists():
        return {"exclude": [], "include": []}
    with open(manifest_path, "r", encoding="utf-8") as f:
        return yaml.safe_load(f) or {}


def _matches_glob(suite_tag: str, pattern: str) -> bool:
    """Check if ``<periph>/<suite>`` matches a glob pattern.

    Patterns support ``**/`` prefix, ``*`` wildcard, etc.
    """
    # Normalise pattern: "gpio/**" → matches all gpio suites
    # "**/systimer_freq" → matches any periph with that suite
    return fnmatch(suite_tag, pattern)


# ---------------------------------------------------------------------------
# Code generation
# ---------------------------------------------------------------------------

_DISCLAIMER = "/* AUTO-GENERATED by gen_suites.py — DO NOT EDIT */"


def generate(
    manifest_path: Path,
    output_dir: Path,
) -> None:
    manifest = _load_manifest(manifest_path)
    periphs = _scan_suite_c_files()

    # Build suite list: [(periph, suite_full, c_path, struct_name)]
    all_suites: list[tuple[str, str, Path, str]] = []
    for periph, c_files in sorted(periphs.items()):
        for c_path in c_files:
            suite_full = _parse_suite_name(c_path, periph)
            if suite_full is None:
                continue
            struct_name = _suite_to_struct_name(c_path)
            all_suites.append((periph, suite_full, c_path, struct_name))

    # Apply exclude
    exclude_patterns: list[str] = manifest.get("exclude", []) or []
    enabled = []
    for periph, suite_full, c_path, struct_name in all_suites:
        tag = f"{periph}/{suite_full}"
        excluded = any(_matches_glob(tag, pat) for pat in exclude_patterns)
        if not excluded:
            enabled.append((periph, suite_full, c_path, struct_name))

    # Apply include (custom suites)
    include_entries: list[dict] = manifest.get("include", []) or []
    custom_suites = []
    for entry in include_entries:
        c_path = (manifest_path.parent / entry["path"]).resolve()
        if not c_path.exists():
            print(f"Warning: custom suite not found: {c_path}", file=sys.stderr)
            continue
        # Derive struct name from filename
        struct_name = c_path.stem
        h_path = c_path.with_suffix(".h")
        custom_suites.append((c_path, struct_name, h_path))

    # Collect periphs that have ≥1 enabled suite
    periph_set: set[str] = set()
    for periph, _, _, _ in enabled:
        periph_set.add(periph)

    # Find periph driver headers
    periph_headers: dict[str, str] = {}  # periph → include path
    for periph in sorted(periph_set):
        h = _find_periph_header(periph)
        if h:
            periph_headers[periph] = _make_include_path(h)

    # Periph data type names
    periph_data_types: dict[str, str] = {}  # periph → data type
    for periph in sorted(periph_set):
        periph_data_types[periph] = f"vsf_test_{periph}_data_t"

    # ── Generate vsf_test_suites.h ──
    h_lines = [
        _DISCLAIMER,
        "#ifndef __VSF_TEST_SUITES_H__",
        "#define __VSF_TEST_SUITES_H__",
        "",
        '#include "vsf_board.h"',
        "",
    ]
    # Enable macros header
    h_lines.append('#include "vsf_test_suite_enables.h"')
    h_lines.append("")
    # Platform stubs (unconditional — headers have include guards)
    stub_files: list[str] = manifest.get("stubs", []) or []
    for stub in stub_files:
        h_lines.append(f'#include "{stub}"')
    if stub_files:
        h_lines.append("")
    # Periph driver includes
    for periph in sorted(periph_set):
        inc = periph_headers.get(periph)
        if inc:
            h_lines.append(f'#include "{inc}"')
    # Custom suite includes
    for _, _, h_path in custom_suites:
        try:
            rel = h_path.relative_to(_SUITE_DIR)
            inc = str(rel).replace("\\", "/")
        except ValueError:
            inc = str(h_path).replace("\\", "/")
        h_lines.append(f'#include "{inc}"')
    h_lines.append("")
    # Data union
    h_lines.append("typedef union {")
    for periph in sorted(periph_set):
        macro = _periph_to_enable_macro(periph)
        dtype = periph_data_types[periph]
        h_lines.append(f"#if {macro} == ENABLED")
        h_lines.append(f"    {dtype} {periph};")
        h_lines.append("#endif")
    h_lines.append("} vsf_test_suite_data_t;")
    h_lines.append("")
    # Externs
    h_lines.extend([
        "extern vsf_test_suite_data_t vsf_test_suite_data;",
        "extern const vsf_test_suite_t *vsf_test_suite_list[];",
        "extern uint8_t vsf_test_suite_count;",
        "",
        "#endif /* __VSF_TEST_SUITES_H__ */",
        "",
    ])

    # ── Generate vsf_test_suites.c ──
    c_lines = [
        _DISCLAIMER,
        "",
        '#include "vsf_test_suites.h"',
        '#include "vsf_board.h"',
        "",
        "vsf_test_suite_data_t vsf_test_suite_data;",
        "",
        "const vsf_test_suite_t *vsf_test_suite_list[] = {",
    ]
    for periph, suite_full, c_path, struct_name in enabled:
        c_lines.append(f"    (const vsf_test_suite_t *)&{struct_name},")
    for _, struct_name, _ in custom_suites:
        c_lines.append(f"    (const vsf_test_suite_t *)&{struct_name},")
    c_lines.extend([
        "};",
        "uint8_t vsf_test_suite_count = "
        f"(uint8_t)(sizeof(vsf_test_suite_list) / sizeof(vsf_test_suite_list[0]));",
        "",
    ])

    # ── Generate vsf_test_suite_enables.h ──
    e_lines = [
        _DISCLAIMER,
        "#ifndef __VSF_TEST_SUITE_ENABLES_H__",
        "#define __VSF_TEST_SUITE_ENABLES_H__",
        "",
        "/* Per-suite enable macros — generated from test_manifest.yml */",
        "/* These override the periph driver header defaults (DISABLED). */",
        "",
    ]
    # Per-periph enable
    for periph in sorted(periph_set):
        macro = _periph_to_enable_macro(periph)
        e_lines.append(f"#define {macro}    ENABLED")
    e_lines.append("")
    # Per-suite enable
    for periph, suite_full, _, _ in enabled:
        macro = _suite_to_enable_macro(suite_full)
        e_lines.append(f"#define {macro}    ENABLED")
    e_lines.append("")
    e_lines.append("#endif /* __VSF_TEST_SUITE_ENABLES_H__ */")
    e_lines.append("")

    # ── Write ──
    output_dir.mkdir(parents=True, exist_ok=True)
    (output_dir / "vsf_test_suites.h").write_text("\n".join(h_lines), encoding="utf-8")
    (output_dir / "vsf_test_suites.c").write_text("\n".join(c_lines), encoding="utf-8")
    (output_dir / "vsf_test_suite_enables.h").write_text("\n".join(e_lines), encoding="utf-8")

    # ── Print summary ──
    print(f"Generated 3 files → {output_dir.resolve()}")
    print(f"  Suites: {len(enabled)} enabled")
    if exclude_patterns:
        excluded_count = len(all_suites) - len(enabled)
        print(f"         {excluded_count} excluded by {len(exclude_patterns)} patterns")
    if custom_suites:
        print(f"         {len(custom_suites)} custom")
    for periph, suite_full, _, _ in enabled:
        print(f"    [OK] {periph}/{suite_full}")
    for _, struct_name, _ in custom_suites:
        print(f"    [+] {struct_name} (custom)")


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def main():
    if len(sys.argv) < 3:
        print("Usage: python gen_suites.py <manifest.yml> <output_dir>")
        sys.exit(2)
    manifest_path = Path(sys.argv[1]).resolve()
    output_dir = Path(sys.argv[2]).resolve()
    generate(manifest_path, output_dir)


if __name__ == "__main__":
    main()
