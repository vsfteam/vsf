#!/usr/bin/env python3
"""
Generate device.h peripheral macro block from a YAML instance map.

Usage:
    generate-device-peripheral-macros.py <input.yaml>
    generate-device-peripheral-macros.py --in-place <device.h> <input.yaml>

Exit codes:
    0 = success
    1 = file not found or unreadable
    2 = YAML parse error
"""

import argparse
import sys
from pathlib import Path

# Bootstrap: ensure scripts/ is on sys.path so _lib is importable
_SCRIPTS_DIR = Path(__file__).parent.parent.resolve()
if str(_SCRIPTS_DIR) not in sys.path:
    sys.path.insert(0, str(_SCRIPTS_DIR))

try:
    import yaml  # type: ignore[reportMissingModuleSource]
except ImportError:
    print("Error: pyyaml required. Install with: pip install pyyaml", file=sys.stderr)
    sys.exit(1)

from _lib.checker import replace_zone
from _lib.scaffold import build_peripheral_macros


def generate_macros(yaml_path: Path) -> str:
    """Load a YAML peripheral map and delegate macro generation to the shared
    implementation in _scaffold_common."""
    with yaml_path.open(encoding="utf-8") as f:
        data = yaml.safe_load(f)

    if not isinstance(data, dict) or "peripherals" not in data:
        raise ValueError("YAML must contain a 'peripherals' key")

    peripherals = data["peripherals"]
    if not isinstance(peripherals, dict):
        raise ValueError("'peripherals' must be a mapping")

    return build_peripheral_macros(peripherals)


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate device.h peripheral macro block from YAML.")
    parser.add_argument("--in-place", metavar="DEVICE_H", help="Replace // peripheral defines zone in the given device.h")
    parser.add_argument("yaml", help="Path to YAML peripheral instance map")
    args = parser.parse_args()

    yaml_path = Path(args.yaml)
    if not yaml_path.is_file():
        print(f"Error: file not found: {yaml_path}", file=sys.stderr)
        return 1

    try:
        output = generate_macros(yaml_path)
    except (yaml.YAMLError, ValueError) as err:
        print(f"Error: {err}", file=sys.stderr)
        return 2

    if args.in_place:
        device_path = Path(args.in_place)
        if not device_path.is_file():
            print(f"Error: file not found: {device_path}", file=sys.stderr)
            return 1
        content = device_path.read_text(encoding="utf-8")
        new_content = replace_zone(content, "peripheral defines", output)
        if new_content == content:
            print("Warning: // peripheral defines zone not found; file unchanged", file=sys.stderr)
        device_path.write_text(new_content, encoding="utf-8")
        print(f"Updated {device_path}")
    else:
        print(output)

    return 0


if __name__ == "__main__":
    sys.exit(main())
