"""Shared configuration dataclasses for vsf-bench.

Separated from hardware_map.py to avoid circular imports between
hardware_map, runners, and registry.

Board/Project separation: BoardConfig describes a physical board (serial
port, baud, power, fixtures).  ProjectConfig describes what firmware to
build and how to flash it (build tool, runner, artifacts).  One board can
run multiple projects.
"""

from dataclasses import dataclass, field
from enum import Enum


class TestResult(str, Enum):
    """Precise test verdict."""
    PASS = "PASS"
    FAIL_TIMEOUT = "FAIL_TIMEOUT"
    FAIL_ASSERT = "FAIL_ASSERT"
    FAIL_CRASH = "FAIL_CRASH"
    FAIL_DECODE = "FAIL_DECODE"
    SKIP = "SKIP"


@dataclass
class ArtifactConfig:
    name: str
    format: str


@dataclass
class RunnerConfig:
    type: str
    artifact: ArtifactConfig | None
    params: dict = field(default_factory=dict)


@dataclass
class BuildConfig:
    source_dir: str
    build_dir: str
    tool: str = "cmake"                       # builder name → registry lookup
    params: dict = field(default_factory=dict)  # builder-specific parameters
    artifacts: list[ArtifactConfig] = field(default_factory=list)


@dataclass
class LogicAnalyzerConfig:
    cli: str | None = None
    device: str = "DSLogic"
    samplerate: str = "10M"
    capture_duration: float = 120.0
    channels: dict[str, str] = field(default_factory=dict)
    marker_channel: str | None = None  # per-board marker UART channel; None falls back to caller default


@dataclass
class ProjectConfig:
    """Firmware project: build, artifacts, and how to flash."""
    build: BuildConfig
    active_runner: str = ""
    runners: dict[str, RunnerConfig] = field(default_factory=dict)
    name: str = ""

    def validate(self) -> None:
        missing = []
        if not self.build.source_dir:
            missing.append("build.source_dir")
        if not self.build.build_dir:
            missing.append("build.build_dir")
        if not self.build.artifacts:
            missing.append("build.artifacts")
        if missing:
            raise ValueError(f"ProjectConfig missing: {', '.join(missing)}")


@dataclass
class StageConfig:
    """One stage in a multi-project pipeline.

    Each stage references a project and specifies which actions to run.
    ``flash_overrides`` can override runner, artifact, or params per-stage.
    ``wait_for`` specifies a regex to match in serial log after flash.
    """
    project: str                          # project name (resolved later)
    actions: list[str]                    # "build" / "flash" / "test"
    flash_overrides: dict | None = None   # e.g. {"runner": "dfu", "artifact": {...}}
    wait_for: str | None = None           # log pattern to wait for after flash
    power_cycle: bool = False             # power-cycle before this stage


@dataclass
class PipelineConfig:
    """Multi-stage pipeline: named sequence of build/flash/test stages."""
    name: str
    description: str = ""
    stages: list[StageConfig] = field(default_factory=list)


@dataclass
class PowerConfig:
    type: str = "smartusbhub"
    hub_addr: int | None = None
    port: int = 3


@dataclass
class BoardConfig:
    platform: str
    debug_uart: str = ""          # log UART COM port (resolved)
    debug_baudrate: int = 0
    program_uart: str = ""     # download/flash UART COM port (resolved)
    connected: bool = True
    power: PowerConfig | None = None
    fixtures: list[str] = field(default_factory=list)
    logic_analyzer: LogicAnalyzerConfig | None = None
    debug_probe: dict | None = None
    dfu_key: dict | None = None    # {gpio, pin, active_low}
    gpio_adapter_serial: str = ""  # resolved from gpio_adapters section
    board_pins: str = ""
    name: str = ""

    def validate(self) -> None:
        missing = []
        if not self.platform:
            missing.append("platform")
        if not self.debug_uart:
            missing.append("debug_uart")
        if missing:
            raise ValueError(f"BoardConfig missing: {', '.join(missing)}")
