#!/usr/bin/env python3
"""
Cross-file port completeness auditor.

Scans a chip driver directory and reports structural gaps across:
- device.h (declarations)
- driver .h/.c files (existence)
- driver.h (template blocks)
- vsf_usr_cfg.h (enable flags)

Usage:
    audit-port.py --chip RaspberryPi/RP2040 [--driver-dir source/hal/driver] \
                  [--board-dir board/pico] [--vsf-usr-cfg path/to/vsf_usr_cfg.h]

Exit codes:
    0 = clean
    1 = errors (gaps that prevent compilation)
    2 = warnings (suspicious but compilable)
    3 = script error
"""

import argparse
import re
import sys
from pathlib import Path

try:
    import yaml  # type: ignore[reportMissingModuleSource]
except ImportError:
    print("Error: pyyaml required. Install with: pip install pyyaml", file=sys.stderr)
    sys.exit(3)

_SCRIPT_DIR = Path(__file__).parent.resolve()


def _load_registry() -> dict[str, dict]:
    reg_file = _SCRIPT_DIR.parent / "peripheral-registry.yml"
    if reg_file.is_file():
        return yaml.safe_load(reg_file.read_text(encoding="utf-8")) or {}
    return {}


_REGISTRY = _load_registry()


def _macro_suffix(short: str) -> str:
    return _REGISTRY.get(short, {}).get("macro_suffix", short.upper())


def _short_from_upper(upper: str) -> str | None:
    for short, info in _REGISTRY.items():
        if info.get("macro_suffix") == upper:
            return short
    return None


def scan_device_h(device_h: Path) -> dict[str, dict]:
    """Parse device.h for VSF_HW_*_COUNT declarations and IRQN macro values."""
    text = device_h.read_text(encoding="utf-8")
    results: dict[str, dict] = {}

    # Match: #define VSF_HW_GPIO_PORT_COUNT  N   or   #define VSF_HW_ADC_COUNT  N
    count_re = re.compile(
        r'^\s*#\s*define\s+VSF_HW_([A-Z0-9]+)_(COUNT|PORT_COUNT)\s+(\d+)',
        re.MULTILINE,
    )
    for m in count_re.finditer(text):
        upper = m.group(1)
        kind = m.group(2)
        count = int(m.group(3))
        short = _short_from_upper(upper) or upper.lower()
        results[short] = {"count": count, "kind": kind, "upper": upper}

    # Also look for instance macros to detect peripherals without COUNT
    inst_re = re.compile(
        r'^\s*#\s*define\s+VSF_HW_([A-Z0-9]+)\d+_IRQN\s+',
        re.MULTILINE,
    )
    for m in inst_re.finditer(text):
        upper = m.group(1)
        short = _short_from_upper(upper) or upper.lower()
        if short not in results:
            results[short] = {"count": 1, "kind": "instance", "upper": upper}

    return results


def scan_device_h_irqn_values(device_h: Path) -> dict[str, list[str]]:
    """Parse device.h for VSF_HW_<P><N>_IRQN macro values.

    Returns a mapping: peripheral_short -> list of bare IRQ names
    (e.g. {"timer": ["TIMER_IRQ_0_IRQn", "TIMER_IRQ_1_IRQn", ...]}).
    """
    text = device_h.read_text(encoding="utf-8")
    irqn_re = re.compile(
        r'^\s*#\s*define\s+VSF_HW_([A-Z0-9]+)\d+_IRQN\s+([A-Za-z0-9_]+)',
        re.MULTILINE,
    )
    periph_irqns: dict[str, list[str]] = {}
    for m in irqn_re.finditer(text):
        upper = m.group(1)
        bare_name = m.group(2)
        short = _short_from_upper(upper) or upper.lower()
        periph_irqns.setdefault(short, []).append(bare_name)
    return periph_irqns


def scan_driver_h(driver_h: Path) -> set[str]:
    """Find which peripherals have template blocks in driver.h."""
    text = driver_h.read_text(encoding="utf-8")
    found: set[str] = set()
    for m in re.finditer(r'#if\s+VSF_HAL_USE_([A-Z0-9_]+)\s*==\s*ENABLED', text):
        upper = m.group(1)
        short = _short_from_upper(upper) or upper.lower()
        found.add(short)
    return found


def scan_vsf_usr_cfg(cfg_path: Path) -> set[str]:
    """Find which VSF_HAL_USE_* are ENABLED in vsf_usr_cfg.h."""
    text = cfg_path.read_text(encoding="utf-8")
    enabled: set[str] = set()
    for m in re.finditer(r'^\s*#\s*define\s+VSF_HAL_USE_([A-Z0-9_]+)\s+ENABLED', text, re.MULTILINE):
        upper = m.group(1)
        short = _short_from_upper(upper) or upper.lower()
        enabled.add(short)
    return enabled


def scan_peripheral_files(chip_dir: Path) -> set[str]:
    """Find which peripheral subdirectories exist with .h/.c files."""
    found: set[str] = set()
    if not chip_dir.exists():
        return found
    for subdir in chip_dir.iterdir():
        if subdir.is_dir() and any(f.suffix in (".h", ".c") for f in subdir.iterdir() if f.is_file()):
            found.add(subdir.name)
    return found


def check_init_wiring(
    chip_dir: Path,
    declarations: dict[str, dict],
    board_dir: Path | None,
) -> list[tuple[str, str]]:
    """Soft check: each declared peripheral should have at least one reference
    in board code or application code (heuristic: instance name or type used).
    """
    findings: list[tuple[str, str]] = []
    scan_files: list[Path] = []

    if board_dir and board_dir.exists():
        scan_files.extend(board_dir.rglob("*.c"))
        scan_files.extend(board_dir.rglob("*.h"))

    # Also scan chip driver .h files (instance extern declarations are callsites)
    if chip_dir.exists():
        for hfile in chip_dir.rglob("*.h"):
            scan_files.append(hfile)

    if not scan_files:
        return findings

    # Peripherals that typically don't need a direct board-level reference
    # (system services, test-only, or called indirectly via application code)
    _no_init_wiring_check = frozenset({"rtc", "wdt", "adc", "pwm", "timer", "spi", "dma"})

    for short, info in declarations.items():
        if info["count"] == 0:
            continue
        if short in _no_init_wiring_check:
            continue

        # Collect all names that share the same macro_suffix (e.g. uart/usart)
        api_names = [short]
        my_info = _REGISTRY.get(short, {})
        my_tpl = my_info.get("template_dir", short)
        if my_tpl != short:
            api_names.append(my_tpl)
        my_suffix = _macro_suffix(short)
        for other_short, info in _REGISTRY.items():
            if other_short != short and info.get("macro_suffix") == my_suffix:
                api_names.append(other_short)
                other_tpl = info.get("template_dir", other_short)
                if other_tpl != other_short:
                    api_names.append(other_tpl)

        candidates = []
        for name in api_names:
            candidates.extend([
                rf"vsf_hw_{name}\d+",
                rf"vsf_hw_{name}_t",
            ])

        found = False
        for cfile in scan_files:
            text = cfile.read_text(encoding="utf-8")
            for pat in candidates:
                if re.search(pat, text):
                    found = True
                    break
            if found:
                break

        if not found:
            findings.append((short, f"no call site for vsf_hw_{short}* in scanned board/driver code"))

    return findings


def check_include_convention(chip_dir: Path) -> list[tuple[Path, str]]:
    """Check that .c files include hal/driver/vendor_driver.h, not bare chip headers."""
    findings: list[tuple[Path, str]] = []
    if not chip_dir.exists():
        return findings

    # Pattern: include of a bare vendor chip header like "RP2040.h" or "stm32h7xx.h"
    # Exempt: device.h and driver.c (chip-level integration)
    bare_chip_re = re.compile(r'#\s*include\s+"[A-Z][A-Za-z0-9_]*\.h"')
    exempt_names = {"device.h", "driver.c", "driver.h", "__device.h"}
    exempt_prefixes = ("startup_",)

    for cfile in chip_dir.rglob("*.c"):
        if cfile.name in exempt_names or cfile.name.startswith(exempt_prefixes):
            continue
        text = cfile.read_text(encoding="utf-8")
        for m in bare_chip_re.finditer(text):
            inc = m.group(0)
            # Skip if it's a known peripheral header from device.h centralized block
            if "hardware/structs/" in inc or "hardware/regs/" in inc:
                continue
            # Skip if it's vendor_driver.h or vsf_hal.h
            if "vendor_driver.h" in inc or "vsf_hal.h" in inc or "vsf_hal_cfg.h" in inc:
                continue
            findings.append((cfile, inc))

    return findings


def check_irqn_usage(
    chip_dir: Path,
    periph_irqns: dict[str, list[str]],
) -> list[tuple[str, str]]:
    """For each peripheral with VSF_HW_*_IRQN macros in device.h, verify that
    the driver .c file references those macros (via VSF_MCONNECT or directly)
    rather than baking the bare vendor IRQ names into the driver body.
    """
    findings: list[tuple[str, str]] = []
    if not chip_dir.exists():
        return findings

    for short, bare_names in periph_irqns.items():
        c_file = chip_dir / short / f"{short}.c"
        if not c_file.is_file():
            continue

        text = c_file.read_text(encoding="utf-8")

        # Does the driver reference its device.h IRQN macros?
        uses_vsf_macros = bool(
            re.search(r"VSF_HW_" + re.escape(short.upper()) + r"\d*_IRQN", text)
            or re.search(r"VSF_MCONNECT.*_IRQN", text, re.DOTALL)
        )
        # Also accept the correct pattern: an irqn struct field populated in IMP_LV0
        has_irqn_field = bool(re.search(r"\.irqn\b", text))
        if uses_vsf_macros or has_irqn_field:
            continue

        # Does the driver contain NVIC calls (signs it uses IRQs)?
        uses_nvic = bool(re.search(r"\bNVIC_(SetPriority|EnableIRQ|DisableIRQ)\b", text))
        if not uses_nvic:
            continue

        # Does the driver bake in the bare vendor IRQ names?
        for bare in bare_names:
            if re.search(r"\b" + re.escape(bare) + r"\b", text):
                findings.append(
                    (short, f"{c_file.name} uses bare IRQ name `{bare}` — "
                             f"should reference VSF_HW_{short.upper()}*_IRQN via "
                             f"VSF_MCONNECT in IMP_LV0 and store in an `irqn` field")
                )
                break  # one finding per peripheral is enough

    return findings


def check_driver_h_includes(driver_h: Path, files: set[str]) -> list[tuple[str, str]]:
    """Check that driver.h includes each peripheral header *before* the
    corresponding #if VSF_HAL_USE_<PERIPH> template block.
    """
    findings: list[tuple[str, str]] = []
    if not driver_h.is_file():
        return findings

    text = driver_h.read_text(encoding="utf-8")
    for short in sorted(files):
        h_file = f"{short}/{short}.h"
        # Find the template block for this peripheral
        block_re = re.compile(
            rf'#if\s+VSF_HAL_USE_{_macro_suffix(short)}\s*==\s*ENABLED',
            re.MULTILINE,
        )
        for m in block_re.finditer(text):
            block_start = m.start()
            # Look for the include in the text *before* this block
            before = text[:block_start]
            if h_file not in before:
                findings.append(
                    (short, f"{h_file} not included before #if VSF_HAL_USE_{_macro_suffix(short)} in {driver_h.name}")
                )
            break   # one check per peripheral
    return findings


def check_device_h_mask(device_h: Path, declarations: dict[str, dict]) -> list[tuple[str, str]]:
    """Check that every instance-declared peripheral has enough macros for
    the template to derive MASK (from COUNT) or COUNT (from MASK).

    GPIO is special: it uses VSF_HW_GPIO_PORT_MASK / VSF_HW_GPIO_PORT_COUNT,
    and vsf_template_gpio.h derives VSF_HW_GPIO_MASK from VSF_HW_GPIO_PORT_MASK.
    For all other peripherals the template header derives one from the other
    via VSF_HAL_COUNT_TO_MASK / VSF_HAL_MASK_TO_COUNT, so either macro is
    sufficient.
    """
    findings: list[tuple[str, str]] = []
    if not device_h.is_file():
        return findings

    text = device_h.read_text(encoding="utf-8")
    for short, info in sorted(declarations.items()):
        if info["count"] == 0:
            continue
        upper = info["upper"]

        # GPIO special case: PORT_MASK or PORT_COUNT is enough.
        if upper == "GPIO":
            has_port_mask = re.search(
                r'^\s*#\s*define\s+VSF_HW_GPIO_PORT_MASK\b', text, re.MULTILINE
            )
            has_port_count = re.search(
                r'^\s*#\s*define\s+VSF_HW_GPIO_PORT_COUNT\b', text, re.MULTILINE
            )
            if not has_port_mask and not has_port_count:
                findings.append(
                    (short, "VSF_HW_GPIO_PORT_COUNT declared but neither VSF_HW_GPIO_PORT_MASK nor VSF_HW_GPIO_PORT_COUNT found")
                )
            continue

        # Non-GPIO: MASK and COUNT are mutually sufficient (template derives
        # one from the other).  scan_device_h already confirmed COUNT exists,
        # so MASK is optional.  Only flag if neither is present.
        has_mask = re.search(
            rf'^\s*#\s*define\s+VSF_HW_{re.escape(upper)}_MASK\b', text, re.MULTILINE
        )
        has_count = re.search(
            rf'^\s*#\s*define\s+VSF_HW_{re.escape(upper)}_COUNT\b', text, re.MULTILINE
        )
        if not has_mask and not has_count:
            findings.append(
                (short, f"VSF_HW_{upper}_COUNT={info['count']} but neither VSF_HW_{upper}_MASK nor VSF_HW_{upper}_COUNT found")
            )
    return findings


def audit(
    chip: str,
    driver_dir: Path,
    board_dir: Path | None,
    cfg_path: Path | None,
) -> int:
    parts = chip.split("/")
    if len(parts) != 2:
        print("Error: --chip must be Vendor/Chip", file=sys.stderr)
        return 3

    vendor, device = parts
    chip_dir = driver_dir / vendor / device
    device_h = chip_dir / "device.h"
    driver_h = chip_dir / "driver.h"

    if not chip_dir.exists():
        print(f"Error: chip directory not found: {chip_dir}", file=sys.stderr)
        return 3

    if not device_h.is_file():
        print(f"Error: device.h not found: {device_h}", file=sys.stderr)
        return 3

    errors = 0
    warnings = 0

    declarations = scan_device_h(device_h)
    periph_irqns = scan_device_h_irqn_values(device_h)
    files = scan_peripheral_files(chip_dir)
    template_blocks = scan_driver_h(driver_h) if driver_h.is_file() else set()
    enabled = set()
    if cfg_path and cfg_path.is_file():
        enabled = scan_vsf_usr_cfg(cfg_path)

    print(f"=== Auditing {chip} ===\n")

    # 1. Declaration gaps: declared in device.h but no files
    for short, info in sorted(declarations.items()):
        if info["count"] > 0 and short not in files:
            print(f"[declaration-gap] {short}: VSF_HW_{info['upper']}_COUNT={info['count']} but no {short}/ directory")
            errors += 1

    # 2. Enable gaps: declared but not enabled
    if cfg_path and cfg_path.is_file():
        for short, info in sorted(declarations.items()):
            if info["count"] > 0:
                mapped = _macro_suffix(short)
                # Check if enabled in cfg
                cfg_text = cfg_path.read_text(encoding="utf-8")
                cfg_enabled = bool(
                    re.search(rf'^\s*#\s*define\s+VSF_HAL_USE_{mapped}\s+ENABLED', cfg_text, re.MULTILINE)
                )
                if not cfg_enabled:
                    print(f"[enable-gap] {short}: declared in device.h but VSF_HAL_USE_{mapped} not ENABLED in {cfg_path}")
                    warnings += 1

    # 3. Template block gaps: enabled but no template block in driver.h
    if driver_h.is_file() and cfg_path and cfg_path.is_file():
        for short in sorted(enabled):
            if short not in template_blocks:
                # Check if there are files for this peripheral
                has_files = short in files
                if has_files:
                    print(f"[template-block-gap] {short}: enabled in vsf_usr_cfg.h but no template block in driver.h")
                    errors += 1

    # 4. Stale declarations: COUNT=0 but files exist
    for short in sorted(files):
        if short in declarations and declarations[short]["count"] == 0:
            print(f"[stale-declaration] {short}: files exist but VSF_HW_{declarations[short]['upper']}_COUNT=0")
            warnings += 1
        elif short not in declarations:
            # Files exist but no declaration at all
            print(f"[stale-declaration] {short}: files exist but no VSF_HW_* declaration in device.h")
            warnings += 1

    # 5. Init wiring (soft warning)
    init_findings = check_init_wiring(chip_dir, declarations, board_dir)
    for short, msg in init_findings:
        print(f"[init-wiring] {short}: {msg}")
        warnings += 1

    # 6. Include convention
    include_findings = check_include_convention(chip_dir)
    for cfile, inc in include_findings:
        print(f"[include-convention] {cfile}: {inc} — use hal/driver/vendor_driver.h instead")
        warnings += 1

    # 7. IRQN macro usage: device.h declares VSF_HW_*_IRQN but driver .c
    #    bakes in bare vendor IRQ names instead of referencing those macros.
    irqn_findings = check_irqn_usage(chip_dir, periph_irqns)
    for short, msg in irqn_findings:
        print(f"[hardcoded-irq] {short}: {msg}")
        errors += 1

    # 8. driver.h include order: peripheral headers before template blocks
    driver_h_include_findings = check_driver_h_includes(driver_h, files)
    for short, msg in driver_h_include_findings:
        print(f"[driver-h-include] {short}: {msg}")
        errors += 1

    # 9. device.h mask macros: every COUNT > 0 needs a MASK macro
    mask_findings = check_device_h_mask(device_h, declarations)
    for short, msg in mask_findings:
        print(f"[missing-mask] {short}: {msg}")
        errors += 1

    print()
    if errors:
        print(f"FAIL: {errors} error(s), {warnings} warning(s)")
        return 1
    elif warnings:
        print(f"PASS: {warnings} warning(s)")
        return 2
    else:
        print("PASS: all checks passed")
        return 0


def main() -> int:
    parser = argparse.ArgumentParser(description="Cross-file port completeness auditor.")
    parser.add_argument("--chip", required=True, help="Chip path, e.g. RaspberryPi/RP2040")
    parser.add_argument("--driver-dir", default="source/hal/driver", help="Path to driver directory")
    parser.add_argument("--board-dir", help="Path to board directory (e.g. board/pico)")
    parser.add_argument("--vsf-usr-cfg", help="Path to vsf_usr_cfg.h (default: auto-discover from board-dir)")
    args = parser.parse_args()

    driver_dir = Path(args.driver_dir).resolve()

    cfg_path: Path | None = None
    if args.vsf_usr_cfg:
        cfg_path = Path(args.vsf_usr_cfg).resolve()
    elif args.board_dir:
        board_cfg = Path(args.board_dir) / "vsf_usr_cfg.h"
        if board_cfg.is_file():
            cfg_path = board_cfg.resolve()

    return audit(args.chip, driver_dir, Path(args.board_dir) if args.board_dir else None, cfg_path)


if __name__ == "__main__":
    sys.exit(main())
