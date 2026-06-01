#!/usr/bin/env python3
"""
Deterministic check: VSF HAL driver anti-pattern detector.

Scans one or more driver .c / .h files for code that branches on hardware
instance, hardcodes per-instance register/IRQ/clock/reset values, or otherwise
violates the parameterize-everything-in-device.h convention.

Each rule fires independently. Lines containing `// quality: allow-<rule-id>`
suppress that specific rule for that line.

Usage:
    check-driver-quality.py <file> [<file> ...]

Exit codes:
    0 = clean
    1 = at least one finding
    2 = script error (bad arguments, missing file)
"""

from __future__ import annotations

import sys
from pathlib import Path

# Bootstrap: ensure scripts/ is on sys.path so _lib is importable
_SCRIPTS_DIR = Path(__file__).parent.parent.resolve()
if str(_SCRIPTS_DIR) not in sys.path:
    sys.path.insert(0, str(_SCRIPTS_DIR))

from _lib.checker import (
    EXIT_PASS,
    EXIT_ERROR,
    EXIT_WARNING,
    Finding,
    preprocess,
    load_pattern_rules,
    check_pattern_rules,
    check_ast_pattern_rules,
)
from _lib.rules import ALL_PATTERN_RULES, ALL_FUNC_RULES

_SCRIPT_DIR = Path(__file__).parent.resolve()
_PATTERN_RULES = load_pattern_rules(_SCRIPT_DIR.parent / "quality-rules.yml")


# Some files are not driver implementations and should be skipped to avoid
# noise: device.h is the *destination* of per-instance literals; a board
# file is the *destination* of pinmux. The script accepts these but applies
# rules narrowly.
def filename_skip_rules(path: Path) -> set[str]:
    name = path.name.lower()
    skipped: set[str] = set()
    if name == "device.h":
        skipped |= {"hardcoded-instance-name", "hardcoded-irq",
                    "hardcoded-reset", "hardcoded-clock",
                    "chip-prefixed-define"}
    if name.startswith("vsf_board") or name == "board.c":
        skipped |= {"pinmux-in-driver", "chip-prefixed-define"}
    if name == "driver.c":
        skipped |= {"hardcoded-reset"}
    if "/gpio/" in str(path).replace("\\", "/") or path.name.startswith("gpio."):
        skipped |= {"pinmux-in-driver"}
    # Startup files contain the interrupt vector table; bare IRQ names and
    # reset/clock literals are legitimate there.
    if path.name.lower().startswith("startup_"):
        skipped |= {"hardcoded-irq", "hardcoded-instance-name",
                    "hardcoded-reset", "hardcoded-clock"}
    return skipped


def check_file(path: Path) -> tuple[list[Finding], list[Finding]]:
    text = path.read_text()
    lines = preprocess(text)
    skip = filename_skip_rules(path)
    error_findings: list[Finding] = []
    warn_findings: list[Finding] = []

    # YAML pattern rules (split by AST vs line-based)
    ast_rules = [r for r in _PATTERN_RULES if r.get("node_type") or r.get("node_types")]
    line_rules = [r for r in _PATTERN_RULES if not (r.get("node_type") or r.get("node_types"))]

    for f in check_ast_pattern_rules(text, ast_rules, path):
        if f.rule_id not in skip:
            error_findings.append(f)

    for f in check_pattern_rules(lines, line_rules, path):
        if f.rule_id not in skip:
            error_findings.append(f)

    # Python pattern rules
    for rule in ALL_PATTERN_RULES:
        for f in rule(lines, path):
            if f.rule_id in skip:
                continue
            if f.severity == "warn":
                warn_findings.append(f)
            else:
                error_findings.append(f)

    # Function-level semantic rules
    from checker_base import extract_functions
    funcs = extract_functions(text)
    for checker in ALL_FUNC_RULES:
        errs, warns = checker(funcs, path)
        error_findings.extend(errs)
        warn_findings.extend(warns)

    return error_findings, warn_findings


def main(argv: list[str]) -> int:
    if len(argv) < 2:
        print(f"Usage: {argv[0]} <file> [<file> ...]", file=sys.stderr)
        return EXIT_ERROR

    paths: list[Path] = []
    for raw in argv[1:]:
        p = Path(raw)
        if not p.is_file():
            print(f"error: not a file: {raw}", file=sys.stderr)
            return EXIT_ERROR
        paths.append(p)

    all_errors: list[Finding] = []
    all_warns: list[Finding] = []
    for p in paths:
        errs, warns = check_file(p)
        all_errors.extend(errs)
        all_warns.extend(warns)

    for f in all_errors + all_warns:
        print(f.render())

    if all_errors:
        print(f"\nFAIL: {len(all_errors)} error(s), {len(all_warns)} warning(s)")
        return EXIT_ERROR
    elif all_warns:
        print(f"\nPASS: {len(all_warns)} warning(s) (review and proceed)")
        return EXIT_WARNING
    print(f"PASS: {len(paths)} file(s) clean")
    return EXIT_PASS


if __name__ == "__main__":
    sys.exit(main(sys.argv))
