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


class StepType(Enum):
    BUILD = "build"
    PROGRAM = "program"
    POWER_CYCLE = "power_cycle"
    POWER_OFF = "power_off"
    POWER_ON = "power_on"
    DELAY = "delay"
    WAIT_FOR = "wait_for"
    SERIAL_SEND = "serial_send"
    GPIO_SET = "gpio_set"
    LA_START = "la_start"
    LA_STOP = "la_stop"
    LA_DECODE = "la_decode"
    DEBUG_STREAM_START = "debug_stream_start"
    DEBUG_STREAM_STOP = "debug_stream_stop"
    RUN = "run"
    LOOP = "loop"


@dataclass
class StepConfig:
    type: StepType
    id: str | None = None
    params: dict = field(default_factory=dict)
    continue_on_error: bool = False
    max_retries: int = 1
    on_failure: list["StepConfig"] | None = None
    steps: list["StepConfig"] | None = None  # for LOOP type


@dataclass
class UARTConfig:
    """UART decode parameters."""
    baudrate: int = 115200
    parity_type: str = "none"
    num_data_bits: int = 8
    num_stop_bits: float = 1.0


@dataclass
class ArtifactConfig:
    name: str
    format: str
    kind: str = ""       # variant discriminator: "raw", "crc", "encrypted", "debug", ...


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
    marker_baudrate: int = 115200       # marker UART baud rate
    marker_uart_params: UARTConfig = field(default_factory=UARTConfig)


@dataclass
class ProjectConfig:
    """Firmware project: build, artifacts, and how to flash."""
    build: BuildConfig
    active_runner: str = ""
    runners: dict[str, RunnerConfig] = field(default_factory=dict)
    name: str = ""
    log_dir: str = ""                        # project-specific default log directory
    debug_vars: list[str] = field(default_factory=list)  # variables to dump with vsf-bench-debug vars

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
    """Multi-stage pipeline: named sequence of build/flash/test stages.

    Supports two formats:
      - New: ``steps`` — list of StepConfig (unified steps model)
      - Legacy: ``stages`` — list of StageConfig (old project-action format)
    """
    name: str
    description: str = ""
    timeout: float | None = None
    matrix: dict[str, list] = field(default_factory=dict)
    stages: list[StageConfig] = field(default_factory=list)    # legacy
    steps: list[StepConfig] = field(default_factory=list)      # new unified


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
    # Flat-format entries may embed project info directly on the board
    build: BuildConfig | None = None
    active_runner: str = ""
    runners: dict[str, RunnerConfig] = field(default_factory=dict)

    def validate(self) -> None:
        missing = []
        if not self.platform:
            missing.append("platform")
        if not self.debug_uart:
            missing.append("debug_uart")
        if missing:
            raise ValueError(f"BoardConfig missing: {', '.join(missing)}")
