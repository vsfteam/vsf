#!/usr/bin/env python3
"""Shared helpers for function-level quality rules."""

import re


def line_in_func(func: dict, needle: str) -> int:
    """Return 0-based line index of first occurrence of *needle* in function body,
    or -1 if not found."""
    for i, line in enumerate(func["lines"]):
        if needle in line:
            return i
    return -1


def func_has_any(func: dict, *needles: str) -> bool:
    """Return True if any needle appears anywhere in the function body."""
    body = func["body"]
    return any(n in body for n in needles)


# Patterns that exempt an init() from clock/reset requirements.
# Example:  // no clock gate: RP2040 timer is always-on
CLOCK_EXEMPT_RE = re.compile(
    r"//\s*no[-\s]clock[-\s]?gate\b|/\*\s*no[-\s]clock[-\s]?gate\b",
    re.IGNORECASE,
)
RESET_EXEMPT_RE = re.compile(
    r"//\s*no[-\s]reset\b|/\*\s*no[-\s]reset\b",
    re.IGNORECASE,
)


def func_has_exemption(func: dict, exempt_re: re.Pattern) -> bool:
    """Return True if the function body contains an exemption comment."""
    for line in func["lines"]:
        if exempt_re.search(line):
            return True
    return False
