#!/usr/bin/env python3
"""NVIC priority ordering rule."""

from pathlib import Path

from ..checker import Finding
from ._helpers import line_in_func


def check_nvic_priority_order(funcs: list[dict], path: Path) -> tuple[list[Finding], list[Finding]]:
    """In init(), NVIC_SetPriority must come before NVIC_EnableIRQ."""
    errors: list[Finding] = []
    for func in funcs:
        if "_init" not in func["name"]:
            continue
        setprio = line_in_func(func, "NVIC_SetPriority")
        enable = line_in_func(func, "NVIC_EnableIRQ")
        if enable < 0:
            continue
        if setprio < 0:
            errors.append(Finding(
                path, func["start_line"] + enable, "nvic-priority-order",
                f"{func['name']} calls NVIC_EnableIRQ without preceding NVIC_SetPriority",
                reference="IRQ enable in init()",
            ))
        elif setprio >= enable:
            errors.append(Finding(
                path, func["start_line"] + enable, "nvic-priority-order",
                f"NVIC_EnableIRQ appears before NVIC_SetPriority in {func['name']}",
                reference="IRQ enable in init()",
            ))
    return errors, []
