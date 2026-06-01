#!/usr/bin/env python3
"""
Toggle VSF_HAL_USE_* macros in vsf_usr_cfg.h between ENABLED and DISABLED.

Usage:
    enable-periph.py --enable usart,gpio,spi --disable i2c,pwm <vsf_usr_cfg.h>
    enable-periph.py --enable usart --disable i2c <vsf_usr_cfg.h>

Exit codes:
    0 = all toggles applied successfully
    1 = one or more peripheral names unrecognized (no matching #define line)
    2 = file error (not found, not readable, not writable)
"""

import argparse
import re
import sys
from pathlib import Path


def parse_list(value: str | None) -> list[str]:
    if not value:
        return []
    return [p.strip().lower() for p in value.split(",") if p.strip()]


def toggle(cfg_path: Path, enable: list[str], disable: list[str]) -> int:
    if not cfg_path.is_file():
        print(f"Error: file not found: {cfg_path}", file=sys.stderr)
        return 2

    text = cfg_path.read_text(encoding="utf-8")
    original = text
    unrecognized: list[str] = []

    for periph in enable:
        pattern = re.compile(
            rf"^(\s*#\s*define\s+VSF_HAL_USE_{periph.upper()}\s+)(DISABLED|ENABLED)",
            re.MULTILINE | re.IGNORECASE,
        )
        if not pattern.search(text):
            unrecognized.append(periph)
            continue
        text = pattern.sub(rf"\1ENABLED", text)

    for periph in disable:
        pattern = re.compile(
            rf"^(\s*#\s*define\s+VSF_HAL_USE_{periph.upper()}\s+)(DISABLED|ENABLED)",
            re.MULTILINE | re.IGNORECASE,
        )
        if not pattern.search(text):
            unrecognized.append(periph)
            continue
        text = pattern.sub(rf"\1DISABLED", text)

    if unrecognized:
        print(f"Error: unrecognized peripheral(s): {', '.join(sorted(set(unrecognized)))}", file=sys.stderr)
        return 1

    if text != original:
        cfg_path.write_text(text, encoding="utf-8")
        changed = []
        for p in enable:
            changed.append(f"{p}: ENABLED")
        for p in disable:
            changed.append(f"{p}: DISABLED")
        print(f"Updated {cfg_path}: {', '.join(changed)}")
    else:
        print(f"No changes needed in {cfg_path}")

    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description="Toggle VSF_HAL_USE_* in vsf_usr_cfg.h.")
    parser.add_argument("--enable", default="", help="Comma-separated list of peripherals to enable")
    parser.add_argument("--disable", default="", help="Comma-separated list of peripherals to disable")
    parser.add_argument("cfg", help="Path to vsf_usr_cfg.h")
    args = parser.parse_args()

    enable = parse_list(args.enable)
    disable = parse_list(args.disable)

    if not enable and not disable:
        print("Error: nothing to do — specify --enable and/or --disable", file=sys.stderr)
        return 2

    return toggle(Path(args.cfg), enable, disable)


if __name__ == "__main__":
    sys.exit(main())
