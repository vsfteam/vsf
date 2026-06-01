#!/usr/bin/env python3
"""Silent frequency default substitution rule."""

import re
from pathlib import Path

from ..checker import Finding


def check_silent_freq_default(funcs: list[dict], path: Path) -> tuple[list[Finding], list[Finding]]:
    """cfg_ptr->clock_hz (or freq) being 0 must return VSF_ERR_INVALID_PARAMETER."""
    errors: list[Finding] = []
    freq_default_re = re.compile(
        r'\bif\s*\(\s*(?:freq|clock_hz)\s*==\s*0\s*\)\s*\{?\s*\b(?:freq|clock_hz)\s*=',
        re.IGNORECASE | re.DOTALL,
    )
    for func in funcs:
        m = freq_default_re.search(func["body"])
        if m:
            line_offset = func["body"][:m.start()].count("\n")
            errors.append(Finding(
                path, func["start_line"] + line_offset, "silent-freq-default",
                f"{func['name']} silently substitutes default frequency for 0 — "
                f"return VSF_ERR_INVALID_PARAMETER instead",
                reference="Invalid frequency",
            ))
    return errors, []
