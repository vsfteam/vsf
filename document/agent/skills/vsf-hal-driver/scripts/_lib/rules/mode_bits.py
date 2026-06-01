#!/usr/bin/env python3
"""Mode bits translation via if/else rule."""

import re
from pathlib import Path

from ..checker import Finding


def check_mode_bits_translation(funcs: list[dict], path: Path) -> tuple[list[Finding], list[Finding]]:
    """Detect mode bits translated via if/else instead of direct register mapping."""
    warnings: list[Finding] = []
    mode_branch_re = re.compile(
        r'if\s*\([^)]*mode[^)]*&[^)]*MODE_[^)]*\)\s*\{[^{}]*?\w+\s*\|?=\s*[^;]+;[^{}]*?\}',
        re.IGNORECASE | re.DOTALL,
    )
    for func in funcs:
        if "_init" not in func["name"]:
            continue
        m = mode_branch_re.search(func["body"])
        if m:
            line_offset = func["body"][:m.start()].count("\n")
            warnings.append(Finding(
                path, func["start_line"] + line_offset, "mode-bits-translation",
                f"{func['name']}: mode bits appear to be translated via if/else — "
                f"consider reimplementing the enum to encode register bits directly "
                f"(convention 8: Mode/config bits map hardware registers)",
                reference="Conventions",
                severity="warn",
            ))
    return [], warnings
