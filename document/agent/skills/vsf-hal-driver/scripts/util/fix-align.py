#!/usr/bin/env python3
"""
Align multi-line #define continuation backslashes using clang-format.

Two-pass approach:
  1. clang-format normalises spacing and line breaks inside each macro block.
  2. All continuation backslashes are then aligned to column 81 (the VSF
     convention), regardless of how clang-format chose to right-align them.

Only #define blocks are touched — the rest of the file stays byte-for-byte
identical to the original.

Usage:
    fix-macro-align.py <file.c> [file2.c ...]

Requires: clang-format on PATH
"""

import subprocess
import sys
import tempfile
from pathlib import Path

TARGET_COL = 81


def _clang_format_cfg() -> Path:
    cfg = tempfile.NamedTemporaryFile(
        mode="w", suffix=".yml", prefix="clangfmt_", delete=False
    )
    cfg.write(
        "BasedOnStyle: LLVM\n"
        "IndentWidth: 4\n"
        "UseTab: Never\n"
        "AlignEscapedNewlines: Right\n"
        "ColumnLimit: 80\n"
    )
    cfg.close()
    return Path(cfg.name)


def _find_macro_blocks(lines: list[str]) -> list[tuple[int, int]]:
    """Return (start, end) line-index pairs (0-based, inclusive) for each
    multi-line #define block."""
    blocks: list[tuple[int, int]] = []
    i = 0
    while i < len(lines):
        stripped = lines[i].lstrip()
        if stripped.startswith("#") and "define" in stripped:
            start = i
            has_continuation = lines[i].rstrip("\n\r").rstrip().endswith("\\")
            while i < len(lines) and lines[i].rstrip("\n\r").rstrip().endswith("\\"):
                i += 1
            if i < len(lines):
                i += 1
            if has_continuation:
                blocks.append((start, i - 1))
        else:
            i += 1
    return blocks


def _format_block(block_lines: list[str], cfg_path: Path) -> list[str]:
    """Pass 1: normalise macro content with clang-format."""
    block_text = "".join(block_lines)
    result = subprocess.run(
        ["clang-format", f"--style=file:{cfg_path}",
         "--assume-filename=/tmp/macro.c"],
        input=block_text,
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        sys.stderr.write(f"clang-format error: {result.stderr}\n")
        return block_lines

    formatted = result.stdout.splitlines(keepends=True)
    if formatted and not formatted[-1].endswith("\n"):
        formatted[-1] += "\n"
    return formatted


def _align_backslashes(lines: list[str]) -> list[str]:
    """Pass 2: align continuation backslashes to TARGET_COL (81)."""
    out: list[str] = []
    for line in lines:
        stripped = line.rstrip("\n\r")
        if not stripped.rstrip().endswith("\\"):
            out.append(line)
            continue

        content = stripped.rstrip().rstrip("\\").rstrip()
        if len(content) >= TARGET_COL - 1:
            # Content already reaches or exceeds target — backslash goes right after
            out_line = content + " \\\n"
        else:
            padding = TARGET_COL - len(content) - 1  # -1 for the backslash itself
            out_line = content + " " * padding + "\\\n"

        # Preserve original line ending
        if line.endswith("\r\n"):
            out_line = out_line.rstrip("\n") + "\r\n"
        out.append(out_line)
    return out


def fix_file(path: Path, cfg_path: Path) -> int:
    text = path.read_text()
    lines = text.splitlines(keepends=True)
    blocks = _find_macro_blocks(lines)

    if not blocks:
        return 0

    fixed = 0
    for start, end in reversed(blocks):
        original = lines[start:end + 1]
        formatted = _format_block(original, cfg_path)
        aligned = _align_backslashes(formatted)
        if aligned != original:
            lines[start:end + 1] = aligned
            fixed += 1

    if fixed:
        path.write_text("".join(lines))
    return fixed


def main() -> None:
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <file.c> [file2.c ...]")
        sys.exit(1)

    cfg_path = _clang_format_cfg()
    try:
        total_fixed = 0
        for raw in sys.argv[1:]:
            p = Path(raw)
            if not p.is_file():
                print(f"error: not a file: {raw}", file=sys.stderr)
                sys.exit(2)
            fixed = fix_file(p, cfg_path)
            total_fixed += fixed
            if fixed:
                print(f"  {p.name}: fixed {fixed} macro block(s)")
            else:
                print(f"  {p.name}: OK")

        if total_fixed:
            print(f"\nFixed {total_fixed} macro block(s) total.")
        else:
            print("\nAll macro blocks already aligned.")
    finally:
        cfg_path.unlink(missing_ok=True)


if __name__ == "__main__":
    main()
