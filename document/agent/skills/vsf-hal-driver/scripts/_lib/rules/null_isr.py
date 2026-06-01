#!/usr/bin/env python3
"""init() null ISR guard rule."""

import re
from pathlib import Path

from ..checker import Finding
from ._helpers import func_has_any


def check_init_null_isr(funcs: list[dict], path: Path) -> tuple[list[Finding], list[Finding]]:
    """init() with NVIC_EnableIRQ should handle cfg_ptr->isr.handler_fn == NULL."""
    warnings: list[Finding] = []
    for func in funcs:
        if "_init" not in func["name"]:
            continue
        if not func_has_any(func, "NVIC_EnableIRQ"):
            continue
        has_null_check = bool(re.search(
            r'handler_fn\s*[!=]=\s*NULL|NULL\s*[!=]=\s*handler_fn',
            func["body"]
        ))
        if not has_null_check:
            warnings.append(Finding(
                path, func["start_line"], "init-null-isr-no-disable",
                f"{func['name']} enables NVIC unconditionally — "
                f"add a handler_fn != NULL guard; when NULL call NVIC_DisableIRQ(irqn)",
                reference="Init without ISR handler",
                severity="warn",
            ))
    return [], warnings
