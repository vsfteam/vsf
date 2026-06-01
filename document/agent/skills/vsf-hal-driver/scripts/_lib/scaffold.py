"""Shared scaffolding logic for scaffold_chip.py and scaffold_peripheral.py."""

import re
from dataclasses import dataclass
from pathlib import Path
from typing import Any, TypeAlias, cast

from _lib.checker import replace_zone

try:
    import yaml  # type: ignore[reportMissingModuleSource]
except ImportError:
    raise ImportError("pyyaml required. Install with: pip install pyyaml")

RawMap: TypeAlias = dict[str, Any]

REQUIRED_TOP = ("vendor", "series", "device", "cpu", "arch_pri_num", "arch_pri_bit")

_REGISTRY: dict[str, dict] | None = None


def _load_registry() -> dict[str, dict]:
    global _REGISTRY
    if _REGISTRY is None:
        reg_file = Path(__file__).parent.parent / "peripheral-registry.yml"
        if reg_file.is_file():
            _REGISTRY = yaml.safe_load(reg_file.read_text(encoding="utf-8")) or {}
        else:
            _REGISTRY = {}
    return _REGISTRY


def api_prefix(name: str) -> str:
    reg = _load_registry()
    return reg.get(name, {}).get("template_dir", name)


def api_upper(name: str) -> str:
    reg = _load_registry()
    return reg.get(name, {}).get("macro_suffix", name.upper())


def text(value: Any, default: str = "") -> str:
    return default if value is None else str(value)


@dataclass(frozen=True)
class Instance:
    index: int
    irqn: str
    irq_handler: str
    reg: str
    rst_bit: str = ""
    clk_bit: str = ""

    @classmethod
    def from_raw(cls, raw: Any) -> "Instance":
        return cls(
            index=int(raw["index"]),
            irqn=text(raw["irqn"]),
            irq_handler=text(raw["irq_handler"]),
            reg=text(raw["reg"]),
            rst_bit=text(raw.get("rst_bit")),
            clk_bit=text(raw.get("clk_bit")),
        )


@dataclass(frozen=True)
class Peripheral:
    name: str
    raw: RawMap | int | None

    @property
    def data(self) -> RawMap:
        return self.raw if isinstance(self.raw, dict) else {}

    @property
    def is_usb(self) -> bool:
        return self.name.startswith("usb")

    @property
    def api_prefix(self) -> str:
        return api_prefix(self.name)

    @property
    def api_upper(self) -> str:
        return api_upper(self.name)

    @property
    def ipcore(self) -> str:
        return text(self.data.get("ipcore"))

    @property
    def usb_mode(self) -> str:
        return text(self.data.get("mode"), "nonip")

    @property
    def count(self) -> int | None:
        if isinstance(self.raw, int):
            return self.raw
        if "count" in self.data and "instances" not in self.data:
            return int(self.data["count"])
        return None

    @property
    def instances(self) -> list[Instance]:
        return [Instance.from_raw(item) for item in self.data.get("instances", []) if isinstance(item, dict)]

    def validate(self) -> list[str]:
        errors: list[str] = []
        if self.is_usb:
            if self.usb_mode not in ("nonip", "ip"):
                errors.append("USB peripheral requires mode: nonip or ip")
            return errors

        if "instances" in self.data:
            for inst in self.data.get("instances", []):
                if not isinstance(inst, dict):
                    errors.append(f"{self.name} instance must be a mapping")
                    continue
                for field in ("index", "irqn", "irq_handler", "reg"):
                    if field not in inst:
                        errors.append(f"{self.name} instance missing '{field}'")

        if self.name == "gpio":
            if "port_count" not in self.data:
                errors.append("gpio requires port_count")
            if "pin_count" not in self.data:
                errors.append("gpio requires pin_count")
        return errors

    def ipcore_replacements(self, device: str) -> dict[str, str]:
        if not self.ipcore:
            return {}
        parts = self.ipcore.split("/")
        ip_name = parts[1] if len(parts) > 1 else parts[0]
        ip_lower = ip_name.lower()
        ip_upper = ip_name.upper()
        prefix_upper = self.api_prefix.upper()
        return {
            f"${{{prefix_upper}_IP}}": ip_upper,
            f"${{{self.api_prefix}_ip}}": ip_lower,
            f"${{SERIES/{prefix_upper}_IP}}": f"{device.upper()}_{prefix_upper}",
            "${SERIES/GPIO_IP}": f"{device.upper()}_GPIO",
        }


@dataclass(frozen=True)
class ChipConfig:
    raw: RawMap

    @classmethod
    def load(cls, path: Path) -> "ChipConfig":
        with path.open(encoding="utf-8") as f:
            loaded: Any = yaml.safe_load(f)
        if not isinstance(loaded, dict):
            raise ValueError("YAML config must be a mapping")
        return cls(cast(RawMap, loaded))

    @property
    def vendor(self) -> str:
        return text(self.raw["vendor"])

    @property
    def series(self) -> str:
        return text(self.raw["series"])

    @property
    def device(self) -> str:
        return text(self.raw["device"])

    @property
    def cpu(self) -> str:
        return text(self.raw["cpu"]).upper()

    @property
    def arch_pri_num(self) -> int:
        return int(self.raw["arch_pri_num"])

    @property
    def arch_pri_bit(self) -> int:
        return int(self.raw["arch_pri_bit"])

    @property
    def peripherals(self) -> list[Peripheral]:
        raw_peripherals = self.raw.get("peripherals", {})
        if not isinstance(raw_peripherals, dict):
            return []
        return [Peripheral(name, value) for name, value in cast(RawMap, raw_peripherals).items()]

    @property
    def arch_macro(self) -> str:
        return {
            "ARM": "__CPU_ARM__",
            "RV": "__CPU_RV__",
            "X86": "__CPU_X86__",
            "MCS51": "__CPU_MCS51__",
        }.get(self.cpu, f"__CPU_{self.cpu}__")

    @property
    def replacements(self) -> dict[str, str]:
        return {
            "${VENDOR}": self.vendor,
            "${SERIES}": self.series,
            "${DEVICE}": self.device,
            "__SERIES_NAME_A__": self.series,
            "__DEVICE_NAME_A__": self.device,
            "__series_name_a__": self.series.lower(),
            "__device_name_a__": self.device.lower(),
            "${ARCH_MACRO}": self.arch_macro,
            "vendor_header.h": f"{self.device}.h",
        }

    def validate(self) -> list[str]:
        errors: list[str] = []
        for key in REQUIRED_TOP:
            if key not in self.raw:
                errors.append(f"Missing required field: {key}")
        for peripheral in self.peripherals:
            errors.extend(peripheral.validate())
        return errors

    def peripheral_by_name(self, name: str) -> Peripheral | None:
        for p in self.peripherals:
            if p.name == name:
                return p
        return None


def build_peripheral_macros(peripherals: dict[str, Any]) -> str:
    """Generate the device.h peripheral macro block from a peripheral map.

    *peripherals* is a dict mapping peripheral name -> config dict
    (as loaded from YAML).

    Returns the macro block as a single string (no trailing blank line).
    """
    lines: list[str] = []

    for name, cfg in peripherals.items():
        if not isinstance(cfg, dict):
            continue

        if name.startswith("usb"):
            continue

        upper = api_upper(name)

        if name == "gpio":
            port_count = int(cfg.get("port_count", 0))
            pin_count = int(cfg.get("pin_count", 32))
            lines.extend([
                "// GPIO",
                f"#define VSF_HW_GPIO_PORT_COUNT                  {port_count}",
                f"#define VSF_HW_GPIO_PIN_COUNT                   {pin_count}",
                "",
            ])
        elif "instances" in cfg:
            instances = cfg["instances"]
            if not isinstance(instances, list):
                continue

            indices: list[int] = []
            for inst in instances:
                if isinstance(inst, dict):
                    indices.append(int(inst.get("index", 0)))

            if not indices:
                continue

            contiguous = indices == list(range(indices[0], indices[-1] + 1))
            lines.append(f"// {upper}")
            lines.append(f"#define VSF_HW_{upper}_COUNT                    {len(indices)}")

            if not (contiguous and indices[0] == 0):
                mask = sum(1 << idx for idx in indices)
                lines.append(f"#define VSF_HW_{upper}_MASK                     0x{mask:02X}")

            for inst in instances:
                if not isinstance(inst, dict):
                    continue
                idx = int(inst["index"])
                lines.append(f"#define VSF_HW_{upper}{idx}_IRQN                    {inst['irqn']}")
                lines.append(f"#define VSF_HW_{upper}{idx}_IRQHandler              {inst['irq_handler']}")
                lines.append(f"#define VSF_HW_{upper}{idx}_REG                     {inst['reg']}")
                if inst.get("rst_bit"):
                    lines.append(f"#define VSF_HW_{upper}{idx}_RST_BIT                 {inst['rst_bit']}")
                if inst.get("clk_bit"):
                    lines.append(f"#define VSF_HW_{upper}{idx}_CLK_BIT                 {inst['clk_bit']}")
            lines.append("")
        elif "count" in cfg:
            count = int(cfg["count"])
            lines.extend([f"// {upper}", f"#define VSF_HW_{upper}_COUNT                     {count}", ""])

    return "\n".join(lines)


class TemplateRenderer:
    def __init__(self, cfg: ChipConfig) -> None:
        self.cfg = cfg

    def replace(self, content: str, extra: dict[str, str] | None = None) -> str:
        replacements = dict(self.cfg.replacements)
        if extra is not None:
            replacements.update(extra)
        for old, new in replacements.items():
            content = content.replace(old, new)
        return content

    def _replace_zone(self, content: str, zone_name: str, replacement: str) -> str:
        return replace_zone(content, zone_name, replacement)

    def strip_role_blocks(self, content: str) -> str:
        lines: list[str] = []
        skipping_ipcore = False
        for line in content.splitlines():
            marker = line.strip()
            if marker == "// IPCore":
                skipping_ipcore = True
                continue
            if marker == "// IPCore end":
                skipping_ipcore = False
                continue
            if skipping_ipcore:
                continue
            if marker == "// HW" or marker.startswith("// HW "):
                continue
            if marker == "// HW end":
                continue
            if marker in ("// HW/IPCore", "// HW/IPCore end"):
                continue
            lines.append(line)
        return "\n".join(lines) + "\n"

    def device_h(self, template_dir: Path) -> str:
        content = self.replace((template_dir / "__series_name_a__" / "__device_name_a__" / "device.h").read_text(encoding="utf-8"))
        content = re.sub(r"(#\s*define\s+VSF_ARCH_PRI_NUM\s+)\d+", rf"\g<1>{self.cfg.arch_pri_num}", content)
        content = re.sub(r"(#\s*define\s+VSF_ARCH_PRI_BIT\s+)\d+", rf"\g<1>{self.cfg.arch_pri_bit}", content)
        return self._replace_zone(content, "peripheral defines", self.peripheral_defines())

    def device_dot_h(self, template_dir: Path) -> str:
        return self.replace((template_dir / "__series_name_a__" / "__device.h").read_text(encoding="utf-8"))

    def driver_h(self, template_dir: Path) -> str:
        content = self.replace((template_dir / "__series_name_a__" / "__device_name_a__" / "driver.h").read_text(encoding="utf-8"))
        content = self._replace_zone(content, "peripheral includes", self.peripheral_includes())
        return self._replace_zone(content, "template blocks", self.template_blocks())

    def peripheral_defines(self) -> str:
        lines = [
            "// Software interrupt provided by the device",
            "#define VSF_DEV_SWI_NUM                             0",
            "",
        ]
        peripherals_dict: dict[str, Any] = {}
        for p in self.cfg.peripherals:
            peripherals_dict[p.name] = p.data
        return "\n".join(lines) + build_peripheral_macros(peripherals_dict)

    def peripheral_includes(self) -> str:
        lines: list[str] = []
        for peripheral in self.cfg.peripherals:
            if peripheral.is_usb:
                path = "usb/usb.h" if peripheral.usb_mode == "ip" else "usb_otg.nonip/usb_otg.nonip.h"
                lines.extend([f"// USB OTG ({peripheral.usb_mode})", f'#include "{path}"'])
                continue
            lines.extend([
                f"#if VSF_HAL_USE_{peripheral.api_upper} == ENABLED",
                f"#   include \"{peripheral.name}/{peripheral.name}.h\"",
                "#endif",
            ])
        return "\n".join(lines)

    def template_blocks(self) -> str:
        blocks: list[str] = []
        for peripheral in self.cfg.peripherals:
            if peripheral.is_usb:
                continue
            blocks.append(f"#if VSF_HAL_USE_{peripheral.api_upper} == ENABLED")
            if peripheral.name == "gpio":
                blocks.append("#   define VSF_GPIO_USE_IO_MODE_TYPE                        ENABLED")
            blocks.extend([
                f"#   include \"hal/driver/common/template/vsf_template_{peripheral.api_prefix}.h\"",
                "",
                f"#   define VSF_{peripheral.api_upper}_CFG_DEC_PREFIX                            vsf_hw",
                f"#   define VSF_{peripheral.api_upper}_CFG_DEC_UPCASE_PREFIX                     VSF_HW",
                f"#   include \"hal/driver/common/{peripheral.api_prefix}/{peripheral.api_prefix}_template.h\"",
                "#endif",
                "",
            ])
        return "\n".join(blocks)

    def startup_entries(self) -> tuple[str, str]:
        handlers: list[str] = []
        vectors: list[str] = []
        for peripheral in self.cfg.peripherals:
            if peripheral.is_usb:
                continue
            for inst in peripheral.instances:
                handlers.append(f"WEAK_ISR({inst.irq_handler})")
                vectors.append(f"    {inst.irq_handler},")
        return "\n".join(handlers), "\n".join(vectors)


def write_peripheral(
    peripheral: Peripheral,
    template_dir: Path,
    device_path: Path,
    renderer: TemplateRenderer,
    force: bool = False,
) -> tuple[bool, str]:
    """Copy template files for a single peripheral to the device directory.

    Returns (success, message). On success=True, files were written.
    On success=False, message explains why (e.g. destination exists).
    """
    src_dir = template_dir / "__series_name_a__" / "common" / peripheral.api_prefix
    dst_dir = device_path / peripheral.name

    if not (src_dir.exists() and any(src_dir.iterdir())):
        return False, f"no template for peripheral '{peripheral.name}'"

    if dst_dir.exists() and any(dst_dir.iterdir()) and not force:
        return False, f"destination exists: {dst_dir} (use --force to overwrite)"

    dst_dir.mkdir(parents=True, exist_ok=True)

    extra = peripheral.ipcore_replacements(renderer.cfg.device)
    for src in src_dir.iterdir():
        if not (src.is_file() and src.suffix in (".h", ".c")):
            continue
        content = renderer.strip_role_blocks(src.read_text(encoding="utf-8"))
        content = renderer.replace(content, extra)
        dst_name = src.name
        if peripheral.api_prefix != peripheral.name and src.stem == peripheral.api_prefix:
            dst_name = peripheral.name + src.suffix
        (dst_dir / dst_name).write_text(content, encoding="utf-8")

    return True, f"scaffolded {peripheral.name} -> {dst_dir}"
