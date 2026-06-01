#!/usr/bin/env python3
"""irq_disable NVIC leak rule."""

from pathlib import Path

from ..checker import Finding


def check_irq_disable_nvic_leak(funcs: list[dict], path: Path) -> tuple[list[Finding], list[Finding]]:
    """irq_disable() must only clear peripheral-level IRQ bits; never call NVIC_DisableIRQ."""
    errors: list[Finding] = []
    for func in funcs:
        if "_irq_disable" not in func["name"]:
            continue
        for i, line in enumerate(func["lines"]):
            if "NVIC_DisableIRQ" in line:
                errors.append(Finding(
                    path, func["start_line"] + i, "irq-disable-nvic-leak",
                    f"{func['name']} calls NVIC_DisableIRQ — "
                    f"peripheral irq_disable must only clear reg->IER bits; "
                    f"NVIC_DisableIRQ belongs in fini()",
                    reference="NVIC and peripheral IRQ separation",
                ))
                break
    return errors, []
