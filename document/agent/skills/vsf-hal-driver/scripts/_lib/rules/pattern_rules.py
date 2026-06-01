#!/usr/bin/env python3
"""Python-based pattern rules (regex / text matching on ScanLines)."""

import re
from pathlib import Path

from ..checker import Finding, ScanLine, emit

_LITERAL_ADDR_RE = re.compile(r"\b0x([0-9A-Fa-f]{8,})\b")
_BACKSLASH_TARGET_COL = 81
_SPIN_WAIT_RE = re.compile(r"\bwhile\s*\([^;{]*\)\s*;")
_SPIN_WAIT_KEYWORDS = frozenset({
    "spin-wait", "spinwait", "busy-wait", "busywait",
    "wait", "poll", "polling",
    "abort", "reset", "ready", "done", "complete", "completion",
    "busy", "idle",
    "cycle", "us", "μs", "microsecond",
    "delay", "timeout",
})
_CHIP_CONSTANT_SUFFIXES_EXTENDED = frozenset({
    "SIZE", "SECTOR_SIZE", "PAGE_SIZE", "BLOCK_SIZE",
    "XIP_BASE", "SECTOR_NUM", "CHANNEL_NUM", "CHANNEL_COUNT",
    "PER_INSTANCE",
})


def _looks_like_mask(hex_digits: str) -> bool:
    s = hex_digits.upper()
    chars = set(s)
    if chars.issubset(set("F0")):
        return True
    nonzero = chars - {"0"}
    return len(nonzero) == 1


def _extract_chip_prefix(path: Path) -> str | None:
    parts = list(path.parts)
    for i, p in enumerate(parts):
        if p.lower() == 'driver':
            if i + 4 < len(parts):
                return parts[i + 2].upper()
            break
    return None


def rule_hardcoded_address(lines: list[ScanLine], path: Path) -> list[Finding]:
    def predicate(sl: ScanLine) -> bool:
        if sl.in_imp_lv0:
            return False
        if sl.text.lstrip().startswith("#"):
            return False
        m = _LITERAL_ADDR_RE.search(sl.text)
        return m is not None and not _looks_like_mask(m.group(1))
    return emit(lines, "hardcoded-address",
                 "literal hex address in driver body — wire it through device.h instead",
                 predicate, reference="Per-instance parameterization in device.h")


def rule_macro_backslash_align(lines: list[ScanLine], path: Path) -> list[Finding]:
    def predicate(sl: ScanLine) -> bool:
        stripped = sl.text.rstrip()
        if not stripped.rstrip().endswith("\\"):
            return False
        content = stripped.rstrip().rstrip("\\").rstrip()
        if len(content) > _BACKSLASH_TARGET_COL - 1:
            return True
        return len(stripped) != _BACKSLASH_TARGET_COL
    return emit(lines, "macro-backslash-align",
                 "multi-line #define continuation backslash not at column 81 — "
                 "run fix-macro-align.py to auto-fix",
                 predicate, reference="Macro formatting convention")


def rule_spin_wait_comment(lines: list[ScanLine], path: Path) -> list[Finding]:
    def _has_explanation(idx: int) -> bool:
        for j in range(max(0, idx - 3), idx):
            sl = lines[j]
            txt = sl.text
            comment_part = ""
            if sl.in_comment:
                comment_part = txt.strip().lstrip("*").strip()
            elif "//" in txt:
                comment_part = txt.split("//", 1)[1]
            elif "/*" in txt:
                start = txt.find("/*")
                end = txt.find("*/", start)
                if end == -1:
                    comment_part = txt[start:]
                else:
                    comment_part = txt[start:end + 2]
            if comment_part:
                if any(kw in comment_part.lower() for kw in _SPIN_WAIT_KEYWORDS):
                    return True
        return False

    findings: list[Finding] = []
    for i, sl in enumerate(lines):
        if sl.in_comment or sl.in_string_literal:
            continue
        m = _SPIN_WAIT_RE.search(sl.text)
        if not m:
            continue
        inner = m.group(0).split("(", 1)[1].rsplit(")", 1)[0].strip()
        if inner in ("1", "0", "true", "false", "TRUE", "FALSE", "ENABLED", "DISABLED"):
            continue
        if not _has_explanation(i):
            findings.append(Finding(
                path, sl.lineno, "spin-wait-no-comment",
                "bare spin-wait loop without explanatory comment — "
                "add a comment explaining why and expected duration (< X us)",
                reference="Spin-wait on hardware state"))
    return findings


def rule_chip_prefixed_define(lines: list[ScanLine], path: Path) -> list[Finding]:
    if path.name.lower() == "device.h":
        return []
    if path.name.lower().startswith("vsf_board") or path.name.lower() == "board.c":
        return []

    chip_prefix = _extract_chip_prefix(path)
    if not chip_prefix:
        return []

    findings: list[Finding] = []
    define_re = re.compile(r'#\s*define\s+([A-Z][A-Z0-9]*)_([A-Z][A-Z0-9_]*)')

    for sl in lines:
        if sl.in_comment or sl.in_string_literal:
            continue
        m = define_re.search(sl.text)
        if not m:
            continue
        prefix = m.group(1)
        rest = m.group(2)
        if prefix.startswith("VSF") or prefix.startswith("__VSF"):
            continue
        if prefix == chip_prefix:
            if any(s in rest for s in _CHIP_CONSTANT_SUFFIXES_EXTENDED):
                findings.append(Finding(
                    path, sl.lineno, "chip-prefixed-define",
                    f"chip-prefixed constant '#define {prefix}_{rest}' in driver file — "
                    f"move to device.h as VSF_HW_<PERIPH>_...",
                    reference="Convention 13: No magic numbers",
                ))
    return findings
