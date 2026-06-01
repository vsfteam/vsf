#!/usr/bin/env python3
"""
Scaffold a single peripheral for an existing chip from template.

Usage:
    scaffold_peripheral.py --driver-dir <dir> --chip Vendor/Chip --periph spi
    scaffold_peripheral.py --driver-dir <dir> --chip Vendor/Chip --periph spi --force

Exit codes:
    0 = scaffolded successfully
    1 = destination exists (use --force)
    2 = unknown peripheral (no template)
    3 = script error (bad args, missing dirs)
"""

import argparse
import sys
from pathlib import Path

# Bootstrap: ensure scripts/ is on sys.path so _lib is importable
_SCRIPTS_DIR = Path(__file__).parent.parent.resolve()
if str(_SCRIPTS_DIR) not in sys.path:
    sys.path.insert(0, str(_SCRIPTS_DIR))

from _lib.scaffold import ChipConfig, Peripheral, TemplateRenderer, write_peripheral


def scaffold(
    driver_dir: str,
    chip: str,
    periph: str,
    force: bool = False,
    template_dir: str | None = None,
) -> int:
    parts = chip.split("/")
    if len(parts) != 2:
        print(f"Error: --chip must be Vendor/Chip, got: {chip}", file=sys.stderr)
        return 3

    vendor, device = parts
    driver_path = Path(driver_dir).resolve()
    tpl_dir = (Path(template_dir) if template_dir else driver_path / "template").resolve()
    device_path = driver_path / vendor / device

    if not tpl_dir.exists():
        print(f"Error: template directory not found at {tpl_dir}", file=sys.stderr)
        return 3

    if not device_path.exists():
        print(f"Error: chip directory not found at {device_path}", file=sys.stderr)
        return 3

    # Build a minimal ChipConfig from the chip path so the renderer has
    # the right series / device replacements.
    cfg = ChipConfig(
        {
            "vendor": vendor,
            "series": device,
            "device": device,
            "cpu": "ARM",
            "arch_pri_num": 4,
            "arch_pri_bit": 2,
            "peripherals": {},
        }
    )
    renderer = TemplateRenderer(cfg)

    peripheral = Peripheral(periph, {})
    ok, msg = write_peripheral(peripheral, tpl_dir, device_path, renderer, force=force)

    if ok:
        print(msg)
        return 0

    if "destination exists" in msg:
        print(f"Error: {msg}", file=sys.stderr)
        return 1

    print(f"Error: {msg}", file=sys.stderr)
    return 2


def main() -> int:
    parser = argparse.ArgumentParser(description="Scaffold a single peripheral from template.")
    parser.add_argument("--driver-dir", required=True, help="Path to driver directory (e.g. source/hal/driver)")
    parser.add_argument("--chip", required=True, help="Chip path under driver dir, e.g. RaspberryPi/RP2040")
    parser.add_argument("--periph", required=True, help="Peripheral name (e.g. spi, i2c, gpio, uart)")
    parser.add_argument("--force", action="store_true", help="Overwrite existing destination")
    parser.add_argument("--template-dir", help="Path to VSF template directory (default: <driver-dir>/template)")
    args = parser.parse_args()

    return scaffold(args.driver_dir, args.chip, args.periph, args.force, args.template_dir)


if __name__ == "__main__":
    sys.exit(main())
