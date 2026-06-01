#!/usr/bin/env python3
"""fini() NVIC ordering rule."""

import re
from pathlib import Path

from ..checker import Finding
from ._helpers import line_in_func


def check_fini_nvic_order(funcs: list[dict], path: Path) -> tuple[list[Finding], list[Finding]]:
    """fini() must call NVIC_DisableIRQ before clearing peripheral IRQ enable bits."""
    errors: list[Finding] = []
    for func in funcs:
        if "_fini" not in func["name"]:
            continue
        disable = line_in_func(func, "NVIC_DisableIRQ")
        if disable < 0:
            continue
        peri_clear = -1
        for i, line in enumerate(func["lines"]):
            if re.search(r'reg->\w+\s*&=\s*~', line):
                peri_clear = i
                break
        if peri_clear >= 0 and disable > peri_clear:
            errors.append(Finding(
                path, func["start_line"] + disable, "fini-nvic-order",
                f"NVIC_DisableIRQ appears after peripheral IRQ clear in {func['name']} — "
                f"disable NVIC first to prevent racing IRQ pends",
                reference="IRQ disable in fini()",
            ))
    return errors, []
