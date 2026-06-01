#!/usr/bin/env python3
"""init() clock and reset rules."""

from pathlib import Path

from ..checker import Finding
from ._helpers import func_has_any, func_has_exemption, CLOCK_EXEMPT_RE, RESET_EXEMPT_RE


def check_init_has_reset(funcs: list[dict], path: Path) -> tuple[list[Finding], list[Finding]]:
    """init() that uses NVIC should also deassert the peripheral reset."""
    warnings: list[Finding] = []
    reset_markers = ("reset_hw", "resets_hw", "RST_BIT", "rst_bit", "reset &=", "reset |=")
    for func in funcs:
        if "_init" not in func["name"]:
            continue
        if not func_has_any(func, "NVIC_EnableIRQ"):
            continue
        if func_has_exemption(func, RESET_EXEMPT_RE):
            continue
        if not func_has_any(func, *reset_markers):
            warnings.append(Finding(
                path, func["start_line"], "init-has-reset",
                f"{func['name']} has NVIC_EnableIRQ but no reset deassert — "
                f"add reset_hw->reset &= ~rst_bit",
                reference="Clock and reset",
                severity="warn",
            ))
    return [], warnings


def check_init_has_clock(funcs: list[dict], path: Path) -> tuple[list[Finding], list[Finding]]:
    """init() that uses NVIC should also enable the peripheral clock gate."""
    warnings: list[Finding] = []
    clock_markers = ("clk_bit", "CLK_BIT", "clock_hw", "clock_get_hz",
                     "AHBENR", "APBENR", "AHB1ENR", "APB1ENR",
                     "vsf_hw_peripheral_enable", "peripheral_enable")
    for func in funcs:
        if "_init" not in func["name"]:
            continue
        if not func_has_any(func, "NVIC_EnableIRQ"):
            continue
        if func_has_exemption(func, CLOCK_EXEMPT_RE):
            continue
        if not func_has_any(func, *clock_markers):
            warnings.append(Finding(
                path, func["start_line"], "init-has-clock",
                f"{func['name']} has NVIC_EnableIRQ but no clock gate enable — "
                f"add clock enable before register access",
                reference="Clock and reset",
                severity="warn",
            ))
    return [], warnings
