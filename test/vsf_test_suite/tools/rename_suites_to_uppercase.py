#!/usr/bin/env python3
"""Rename all VSF test suite names from lowercase to UPPERCASE.

Performs deterministic string replacements across C, Python, and YAML files,
then renames files with git mv.

Usage:
    python3 rename_suites_to_uppercase.py --dry-run   # Preview only
    python3 rename_suites_to_uppercase.py --apply     # Apply changes
"""

import argparse
import os
import re
import subprocess
import sys
from pathlib import Path
from collections import OrderedDict


def find_vsf_root() -> Path:
    """Find the vsf.demo repo root from this script's location."""
    script_dir = Path(__file__).resolve().parent
    # Go up from tools/ to vsf_test/, then up to vsf/
    return script_dir.parent.parent.parent.parent


def discover_suite_names(vsf_root: Path) -> list[str]:
    """Extract all suite names from .name = "..." in C suite files.

    Returns list sorted by length descending to avoid partial-match issues.
    """
    suite_dir = vsf_root / "vsf" / "test" / "vsf_test_suite"
    names = set()

    for c_file in suite_dir.glob("**/suite/vsf_test_*.c"):
        content = c_file.read_text(encoding="utf-8")
        m = re.search(r'\.name\s*=\s*"([^"]+)"', content)
        if m:
            names.add(m.group(1))

    # Also check Python-only suites (no .c file)
    for py_file in suite_dir.glob("**/suite/vsf_test_*.py"):
        stem = py_file.stem  # e.g., vsf_test_usart_rx_break_error
        if stem.startswith("vsf_test_"):
            name = stem[len("vsf_test_"):]  # e.g., usart_rx_break_error
            names.add(name)

    # Sort by length descending so longer names are replaced first
    return sorted(names, key=len, reverse=True)


def discover_yaml_name_map(vsf_root: Path) -> dict[str, str]:
    """Build a map of all 'name:' values to their uppercase versions from YAML files.

    Returns {lowercase_name: uppercase_name}
    """
    params_dir = vsf_root / "vsf" / "test" / "vsf_test_suite" / "params"
    name_map = {}

    for yml_file in params_dir.glob("**/*.yml"):
        content = yml_file.read_text(encoding="utf-8")
        for m in re.finditer(r'^\s+name:\s+(\S+)', content, re.MULTILINE):
            val = m.group(1)
            name_map[val] = val.upper()

    return name_map


# ---------------------------------------------------------------------------
# C file replacements
# ---------------------------------------------------------------------------

def replace_in_c_file(filepath: Path, suite_lower: str, suite_upper: str, dry_run: bool) -> int:
    """Apply C identifier and string replacements. Returns number of changes."""
    if not filepath.exists():
        return 0

    content = original = filepath.read_text(encoding="utf-8")
    changes = 0

    # 1. Replace vsf_test_<suite_lower> in identifiers
    old_id = f"vsf_test_{suite_lower}"
    new_id = f"vsf_test_{suite_upper}"
    if old_id in content:
        content = content.replace(old_id, new_id)
        changes += 1

    # 2. Replace .name = "<suite_lower>" string
    old_str = f'.name = "{suite_lower}"'
    new_str = f'.name = "{suite_upper}"'
    if old_str in content:
        content = content.replace(old_str, new_str)
        changes += 1

    # 3. Replace .<suite_lower> struct member access (but NOT part of vsf_test_<suite_lower>)
    #    Only in patterns like: ).suite_lower. or .suite_lower. or data.suite_lower
    old_member = f".{suite_lower}"
    new_member = f".{suite_upper}"
    if old_member in content:
        # Be careful: .suite_lower could be part of vsf_test_suite_lower (false match)
        # We already replaced vsf_test_{suite_lower} so this should be safe
        content = content.replace(old_member, new_member)
        # Don't double-count if the only .suite_lower occurrences were in vsf_test_suite_lower
        if old_member in original and content != original:
            pass  # changes already counted

    if content != original:
        if not dry_run:
            filepath.write_text(content, encoding="utf-8")
        return 1
    return 0


def replace_in_peripheral_header(filepath: Path, suite_lower: str, suite_upper: str, dry_run: bool) -> int:
    """Apply replacements in peripheral header files (include paths, type refs, member names)."""
    if not filepath.exists():
        return 0

    content = original = filepath.read_text(encoding="utf-8")

    # 1. Include path: #include "suite/vsf_test_<suite_lower>.h"
    old_inc = f'"suite/vsf_test_{suite_lower}.h"'
    new_inc = f'"suite/vsf_test_{suite_upper}.h"'
    content = content.replace(old_inc, new_inc)

    # 2. Identifier replacement
    old_id = f"vsf_test_{suite_lower}"
    new_id = f"vsf_test_{suite_upper}"
    content = content.replace(old_id, new_id)

    # 3. Member name: .<suite_lower> (union member)
    old_member = f".{suite_lower}"
    new_member = f".{suite_upper}"
    content = content.replace(old_member, new_member)

    if content != original:
        if not dry_run:
            filepath.write_text(content, encoding="utf-8")
        return 1
    return 0


# ---------------------------------------------------------------------------
# Python file replacements
# ---------------------------------------------------------------------------

def replace_in_python_file(filepath: Path, suite_lower: str, suite_upper: str, dry_run: bool) -> int:
    """Replace suite name string literals in Python scenario and utility files."""
    if not filepath.exists():
        return 0

    content = original = filepath.read_text(encoding="utf-8")
    changes = 0

    # Replace "<suite_lower>" with "<suite_upper>"
    old_str = f'"{suite_lower}"'
    new_str = f'"{suite_upper}"'
    if old_str in content:
        content = content.replace(old_str, new_str)
        changes += 1

    # Also handle f-string patterns: f"...{suite_lower}CASE..."
    # Just do a plain string replace for the suite name part in f-strings
    # e.g., f"{suite_name}:CASE:" uses variable, but literal references matter
    old_fstr = f"{suite_lower}:"
    new_fstr = f"{suite_upper}:"
    # Only replace in non-quoted contexts (inside f-strings or variables)
    # This is tricky; skip for now and handle manually if needed

    if content != original:
        if not dry_run:
            filepath.write_text(content, encoding="utf-8")
        return 1
    return 0


# ---------------------------------------------------------------------------
# YAML file replacements
# ---------------------------------------------------------------------------

def replace_in_yaml_file(filepath: Path, suite_lower: str, suite_upper: str, dry_run: bool) -> int:
    """Replace suite name in YAML keys and name: values."""
    if not filepath.exists():
        return 0

    content = original = filepath.read_text(encoding="utf-8")
    changes = 0

    # 1. Replace YAML key: "suite_lower:" at line start (with possible indentation)
    #    Match: ^<indent>suite_lower:
    old_key = re.compile(rf'^(\s*){re.escape(suite_lower)}(\s*):', re.MULTILINE)
    new_key = rf'\g<1>{suite_upper}\g<2>:'
    new_content = old_key.sub(new_key, content)
    if new_content != content:
        content = new_content
        changes += 1

    # 2. Replace "name: suite_lower" value
    old_name = f"name: {suite_lower}"
    new_name = f"name: {suite_upper}"
    if old_name in content:
        content = content.replace(old_name, new_name)
        changes += 1

    if content != original:
        if not dry_run:
            filepath.write_text(content, encoding="utf-8")
        return 1
    return 0


# ---------------------------------------------------------------------------
# File renaming
# ---------------------------------------------------------------------------

def rename_files(vsf_root: Path, suite_lower: str, suite_upper: str, dry_run: bool) -> int:
    """Git mv suite files from lowercase to uppercase names."""
    suite_dir = vsf_root / "vsf" / "test" / "vsf_test_suite"
    count = 0

    suffixes = [".c", ".h", ".py"]
    for suffix in suffixes:
        old_name = f"vsf_test_{suite_lower}{suffix}"
        new_name = f"vsf_test_{suite_upper}{suffix}"

        # Find the file
        for found in suite_dir.glob(f"**/suite/{old_name}"):
            old_path = found
            new_path = found.parent / new_name
            if not dry_run:
                subprocess.run(
                    ["git", "mv", str(old_path), str(new_path)],
                    cwd=vsf_root,
                    capture_output=True,
                )
            print(f"  git mv {old_path.relative_to(vsf_root)} -> {new_path.relative_to(vsf_root)}")
            count += 1

    return count


# ---------------------------------------------------------------------------
# Pipeline.py specific
# ---------------------------------------------------------------------------

def update_pipeline(vsf_root: Path, dry_run: bool) -> bool:
    """Update _find_scenario_for_suite prefix list to uppercase."""
    pipeline_path = vsf_root / "vsf" / "test" / "vsf_bench" / "pipeline.py"
    if not pipeline_path.exists():
        print("  WARNING: pipeline.py not found")
        return False

    content = original = pipeline_path.read_text(encoding="utf-8")

    # The prefix tuple currently uses lowercase
    # Replace each lowercase prefix with uppercase (multi-line format)
    prefixes_lower = ["usart", "gpio", "i2c", "spi", "adc", "pwm", "timer", "rtc", "flash", "wdt", "dma"]
    prefixes_upper = [p.upper() for p in prefixes_lower]
    changed = False
    for lo, hi in zip(prefixes_lower, prefixes_upper):
        old_str = f'"{lo}"'
        new_str = f'"{hi}"'
        if old_str in content:
            content = content.replace(old_str, new_str)
            changed = True

    if changed:
        if not dry_run:
            pipeline_path.write_text(content, encoding="utf-8")
        print(f"  Updated prefix list in {pipeline_path.relative_to(vsf_root)}")
        return True
    else:
        print("  WARNING: prefix tuple pattern not found in pipeline.py (may already be updated)")
        return False


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description="Rename VSF test suites to UPPERCASE")
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--dry-run", action="store_true", help="Preview changes only")
    group.add_argument("--apply", action="store_true", help="Apply all changes")
    args = parser.parse_args()

    dry_run = args.dry_run
    vsf_root = find_vsf_root()
    print(f"VSF root: {vsf_root}")

    suite_names = discover_suite_names(vsf_root)
    print(f"\nDiscovered {len(suite_names)} suite names:")
    for n in suite_names:
        print(f"  {n} -> {n.upper()}")

    if dry_run:
        print("\n=== DRY RUN ===\n")

    # Track changes
    c_files_changed = 0
    h_files_changed = 0
    py_files_changed = 0
    yaml_files_changed = 0
    periph_h_changed = 0
    files_renamed = 0

    suite_dir = vsf_root / "vsf" / "test" / "vsf_test_suite"
    params_dir = vsf_root / "vsf" / "test" / "vsf_test_suite" / "params"

    for suite_lower in suite_names:
        suite_upper = suite_lower.upper()
        print(f"\n--- {suite_lower} -> {suite_upper} ---")

        # C suite file
        for c_file in suite_dir.glob(f"**/suite/vsf_test_{suite_lower}.c"):
            if replace_in_c_file(c_file, suite_lower, suite_upper, dry_run):
                c_files_changed += 1
                print(f"  C: {c_file.relative_to(vsf_root)}")

        # C header file
        for h_file in suite_dir.glob(f"**/suite/vsf_test_{suite_lower}.h"):
            if replace_in_c_file(h_file, suite_lower, suite_upper, dry_run):
                h_files_changed += 1
                print(f"  H: {h_file.relative_to(vsf_root)}")

        # Python scenario file
        for py_file in suite_dir.glob(f"**/suite/vsf_test_{suite_lower}.py"):
            if replace_in_python_file(py_file, suite_lower, suite_upper, dry_run):
                py_files_changed += 1
                print(f"  PY: {py_file.relative_to(vsf_root)}")

        # YAML files
        for yml_file in params_dir.glob("**/*.yml"):
            if replace_in_yaml_file(yml_file, suite_lower, suite_upper, dry_run):
                yaml_files_changed += 1
                print(f"  YAML: {yml_file.relative_to(vsf_root)}")

        # Peripheral header files
        for periph_h in suite_dir.glob("hal/driver/*/vsf_test_*.h"):
            if f"/suite/vsf_test_{suite_lower}.h" in periph_h.read_text(encoding="utf-8") or \
               f"vsf_test_{suite_lower}" in periph_h.read_text(encoding="utf-8"):
                if replace_in_peripheral_header(periph_h, suite_lower, suite_upper, dry_run):
                    periph_h_changed += 1
                    print(f"  PH: {periph_h.relative_to(vsf_root)}")

        # File renames
        renamed = rename_files(vsf_root, suite_lower, suite_upper, dry_run)
        files_renamed += renamed

    # Update pipeline.py
    print("\n--- pipeline.py ---")
    update_pipeline(vsf_root, dry_run)

    # Summary
    print(f"\n{'='*60}")
    print(f"Summary ({'DRY RUN' if dry_run else 'APPLIED'}):")
    print(f"  C files changed:  {c_files_changed}")
    print(f"  H files changed:  {h_files_changed}")
    print(f"  PY files changed: {py_files_changed}")
    print(f"  YAML files changed: {yaml_files_changed}")
    print(f"  Peripheral H changed: {periph_h_changed}")
    print(f"  Files renamed:    {files_renamed}")
    print(f"  Total: {c_files_changed + h_files_changed + py_files_changed + yaml_files_changed + periph_h_changed}")


if __name__ == "__main__":
    main()
