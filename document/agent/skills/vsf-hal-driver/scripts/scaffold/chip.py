#!/usr/bin/env python3
"""
Generate VSF HAL driver skeleton for a new chip.

Reads a YAML config describing the chip's hardware and scaffolds the driver
directory from vsf/source/hal/driver/template/.
"""

import argparse
import sys
from pathlib import Path

# Bootstrap: ensure scripts/ is on sys.path so _lib is importable
_SCRIPTS_DIR = Path(__file__).parent.parent.resolve()
if str(_SCRIPTS_DIR) not in sys.path:
    sys.path.insert(0, str(_SCRIPTS_DIR))

from _lib.scaffold import (
    ChipConfig,
    Peripheral,
    TemplateRenderer,
    write_peripheral,
)


def scaffold(config_path: str, driver_dir: str, template_dir: str | None = None) -> None:
    try:
        cfg = ChipConfig.load(Path(config_path))
    except ValueError as err:
        print(f"Error: {err}", file=sys.stderr)
        sys.exit(1)

    errors = cfg.validate()
    if errors:
        print("Validation errors:", file=sys.stderr)
        for error in errors:
            print(f"  - {error}", file=sys.stderr)
        sys.exit(1)

    try:
        Scaffolder(cfg, Path(driver_dir), Path(template_dir) if template_dir else None).run()
    except FileNotFoundError as err:
        print(f"Error: {err}", file=sys.stderr)
        sys.exit(1)


class Scaffolder:
    def __init__(self, cfg: ChipConfig, driver_dir: Path, template_dir: Path | None = None) -> None:
        self.cfg = cfg
        self.driver_dir = driver_dir.resolve()
        self.template_dir = (template_dir or driver_dir / "template").resolve()
        self.renderer = TemplateRenderer(cfg)

    @property
    def vendor_path(self) -> Path:
        return self.driver_dir / self.cfg.vendor

    @property
    def device_path(self) -> Path:
        return self.vendor_path / self.cfg.device

    def run(self) -> list[str]:
        if not self.template_dir.exists():
            raise FileNotFoundError(f"template directory not found at {self.template_dir}")

        is_new_vendor = not (self.vendor_path / "driver.h").exists()
        self.device_path.mkdir(parents=True, exist_ok=True)
        (self.vendor_path / self.cfg.series / "common").mkdir(parents=True, exist_ok=True)

        self.write_device_files()
        generated_stubs = self.write_peripherals()
        self.write_series_common()
        self.register_vendor(is_new_vendor)
        print(f"Scaffold complete: {self.cfg.vendor}/{self.cfg.device} -> {self.device_path}")
        print(f"Peripherals: {', '.join(generated_stubs) or '(none)'}")
        return generated_stubs

    def write_device_files(self) -> None:
        self.write("device.h", self.renderer.device_h(self.template_dir))
        self.write("__device.h", self.renderer.device_dot_h(self.template_dir))
        self.write("driver.h", self.renderer.driver_h(self.template_dir))

        self.copy_template_file("__device_name_a__/driver.c", "driver.c")
        startup = self.template_dir / "__series_name_a__" / "__device_name_a__" / "startup.c"
        if startup.exists():
            content = self.renderer.replace(startup.read_text(encoding="utf-8"))
            handlers, vectors = self.renderer.startup_entries()
            content = content.replace("{{INTERRUPT_HANDLERS}}", handlers)
            content = content.replace("{{INTERRUPT_VECTORS}}", vectors)
            self.write(f"startup_{self.cfg.device}.c", content)
        self.copy_template_file("__device_name_a__/CMakeLists.txt", "CMakeLists.txt")

    def write_peripherals(self) -> list[str]:
        generated: list[str] = []
        for peripheral in self.cfg.peripherals:
            if peripheral.is_usb:
                self.write_usb(peripheral)
                generated.append("usb")
                continue
            ok, msg = write_peripheral(peripheral, self.template_dir, self.device_path, self.renderer, force=True)
            if ok:
                generated.append(peripheral.name)
            else:
                print(f"  warn: {msg}")
        return generated

    def write_usb(self, peripheral: Peripheral) -> None:
        dst_dir = self.device_path / ("usb" if peripheral.usb_mode == "ip" else "usb_otg.nonip")
        dst_dir.mkdir(parents=True, exist_ok=True)
        src_dir = self.template_dir / "__series_name_a__" / "common" / "usb_otg.nonip"
        if not src_dir.exists():
            return
        for src in src_dir.iterdir():
            if src.is_file():
                content = self.renderer.replace(src.read_text(encoding="utf-8"))
                (dst_dir / src.name).write_text(content, encoding="utf-8")

    def write_series_common(self) -> None:
        src = self.template_dir / "__series_name_a__" / "common" / "CMakeLists.txt"
        if src.exists():
            dst = self.vendor_path / self.cfg.series / "common" / "CMakeLists.txt"
            dst.write_text(self.renderer.replace(src.read_text(encoding="utf-8")), encoding="utf-8")

    def register_vendor(self, is_new_vendor: bool) -> None:
        if is_new_vendor:
            self.register_new_vendor()
        else:
            self.append_device_to_vendor()

    def register_new_vendor(self) -> None:
        top_driver = self.driver_dir / "driver.h"
        if not top_driver.exists():
            top_driver.write_text(
                "#include \"hal/vsf_hal_cfg.h\"\n\n"
                "#undef VSF_HAL_DRIVER_HEADER\n\n"
                "#if 0\n"
                "#else\n"
                "#   error No supported device found.\n"
                "#endif\n\n"
                "#ifdef VSF_HAL_DRIVER_HEADER\n"
                "#   include VSF_HAL_DRIVER_HEADER\n"
                "#endif\n",
                encoding="utf-8",
            )
        if f"defined(__{self.cfg.vendor}__)" in top_driver.read_text(encoding="utf-8"):
            return

        self.vendor_path.mkdir(parents=True, exist_ok=True)
        content = self.renderer.replace((self.template_dir / "driver.h").read_text(encoding="utf-8"))
        content = content.replace("defined(__DEVICE_NAME_A__)", f"defined(__{self.cfg.device}__)")
        content = content.replace("__DEVICE_NAME_A__", self.cfg.device)
        content = content.replace("__VSF_HAL_DRIVER_${VENDOR}_H__", f"__HAL_DRIVER_{self.cfg.vendor.upper()}_H__")
        content = content.replace("./__SERIES_NAME_A__/", f"./{self.cfg.device}/")
        (self.vendor_path / "driver.h").write_text(content, encoding="utf-8")
        self.insert_before_else(top_driver, [f"#elif defined(__{self.cfg.vendor}__)", f'#   include "./{self.cfg.vendor}/driver.h"'])

    def append_device_to_vendor(self) -> None:
        vendor_driver = self.vendor_path / "driver.h"
        content = vendor_driver.read_text(encoding="utf-8")
        if f"defined(__{self.cfg.device}__)" in content:
            return
        branch = [
            f"#elif   defined(__{self.cfg.device}__)",
            f'#   define  VSF_{self.cfg.vendor.upper()}_DRIVER_HEADER           "./{self.cfg.device}/driver.h"',
        ]
        self.insert_before_else(vendor_driver, branch)

    def insert_before_else(self, path: Path, new_lines: list[str]) -> None:
        lines = path.read_text(encoding="utf-8").split("\n")
        result: list[str] = []
        inserted = False
        for line in lines:
            if not inserted and "".join(line.split()).startswith("#else"):
                result.extend(new_lines)
                inserted = True
            result.append(line)
        path.write_text("\n".join(result) + "\n", encoding="utf-8")

    def copy_template_file(self, template_rel: str, output_name: str) -> None:
        src = self.template_dir / "__series_name_a__" / template_rel
        if src.exists():
            self.write(output_name, self.renderer.replace(src.read_text(encoding="utf-8")))

    def write(self, relative: str, content: str) -> None:
        (self.device_path / relative).write_text(content, encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate VSF HAL driver skeleton for a new chip.")
    parser.add_argument("--driver-dir", required=True, help="Path to output driver directory")
    parser.add_argument("--config", required=True, help="Path to YAML chip configuration file")
    parser.add_argument("--template-dir", help="Path to VSF template directory (default: <driver-dir>/template)")
    args = parser.parse_args()
    scaffold(args.config, args.driver_dir, args.template_dir)


if __name__ == "__main__":
    main()
