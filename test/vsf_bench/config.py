"""Shared configuration dataclasses for vsf-bench.

Separated from hardware_map.py to avoid circular imports between
hardware_map, runners, and registry.
"""

from dataclasses import dataclass, field


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
class BuilderConfig:
    type: str
    params: dict = field(default_factory=dict)


@dataclass
class BuildConfig:
    source_dir: str
    build_dir: str
    build_tool: str = "cmake"
    builders: dict[str, BuilderConfig] = field(default_factory=dict)
    artifacts: list[ArtifactConfig] = field(default_factory=list)


@dataclass
class LogicAnalyzerConfig:
    cli: str | None = None
    device: str = "DSLogic"
    samplerate: str = "10M"
    capture_duration: float = 120.0
    channels: dict[str, str] = field(default_factory=dict)


@dataclass
class BoardConfig:
    platform: str
    active_runner: str
    runners: dict[str, RunnerConfig]
    serial: str
    baud: int
    build: BuildConfig
    connected: bool = True
    fixtures: list[str] = field(default_factory=list)
    logic_analyzer: LogicAnalyzerConfig | None = None
    board_pins: str = ""
    name: str = ""

    def validate(self) -> None:
        missing = []
        if not self.platform:
            missing.append("platform")
        if not self.active_runner:
            missing.append("active_runner")
        if self.active_runner not in self.runners:
            missing.append(f"runners.{self.active_runner} (active runner not in runners map)")
        if not self.runners:
            missing.append("runners")
        if not self.serial:
            missing.append("serial")
        if not self.baud:
            missing.append("baud")
        if not self.build.source_dir:
            missing.append("build.source_dir")
        if not self.build.build_dir:
            missing.append("build.build_dir")
        if not self.build.artifacts:
            missing.append("build.artifacts")
        if missing:
            raise ValueError(f"Missing required fields: {', '.join(missing)}")
