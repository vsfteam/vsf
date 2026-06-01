"""Parse hardware-map.yml and return typed board descriptors."""

import sys
import yaml
from pathlib import Path

from vsf_bench.config import (
    ArtifactConfig,
    BoardConfig,
    BuildConfig,
    BuilderConfig,
    LogicAnalyzerConfig,
    RunnerConfig,
)


def _resolve_yaml_path(value: str, yaml_dir: Path) -> str:
    """Convert a path relative to the YAML file into an absolute path."""
    if not value:
        return value
    p = Path(value)
    return str(p if p.is_absolute() else yaml_dir / p)


def _derive_board_name(entry: dict) -> str:
    """Derive a stable board name from YAML.

    Precedence: explicit ``name`` field → serial port basename.
    """
    name = entry.get("name", "")
    if name:
        return name
    serial = entry.get("serial", "")
    if serial:
        return Path(serial).name  # e.g. "ttyUSB1"
    return entry.get("platform", "unknown")


def _entry_to_board(entry: dict, yaml_dir: Path) -> BoardConfig:
    """Convert one YAML entry to a BoardConfig (no name filtering)."""
    build_cfg = entry.get("build", {})
    build_artifacts = [
        ArtifactConfig(name=a["name"], format=a["format"])
        for a in build_cfg.get("artifacts", [])
    ]

    builders = {}
    for name, cfg in build_cfg.get("builders", {}).items():
        params = {k: v for k, v in cfg.items() if k != "type"}
        builders[name] = BuilderConfig(
            type=cfg["type"],
            params=params,
        )

    build_tool = build_cfg.get("build_tool", "cmake")
    if not builders and build_tool:
        builders[build_tool] = BuilderConfig(type=build_tool)

    build = BuildConfig(
        source_dir=_resolve_yaml_path(build_cfg.get("source_dir", ""), yaml_dir),
        build_dir=_resolve_yaml_path(build_cfg.get("build_dir", ""), yaml_dir),
        build_tool=build_tool,
        builders=builders,
        artifacts=build_artifacts,
    )

    runners = {}
    for name, cfg in entry.get("runners", {}).items():
        artifact_raw = cfg.get("artifact")
        artifact = None
        if artifact_raw:
            artifact = ArtifactConfig(
                name=artifact_raw["name"],
                format=artifact_raw["format"],
            )
        params = {k: v for k, v in cfg.items()
                  if k not in ("type", "artifact")}
        runners[name] = RunnerConfig(
            type=cfg["type"],
            artifact=artifact,
            params=params,
        )

    la_cfg = None
    la_raw = entry.get("logic_analyzer")
    if la_raw:
        cli_raw = la_raw.get("cli")
        la_cfg = LogicAnalyzerConfig(
            cli=_resolve_yaml_path(cli_raw, yaml_dir) if cli_raw else None,
            device=la_raw.get("device", "DSLogic"),
            samplerate=la_raw.get("samplerate", "10M"),
            capture_duration=float(la_raw.get("capture_duration", 120)),
            channels=la_raw.get("channels", {}),
        )

    board = BoardConfig(
        platform=entry.get("platform", ""),
        connected=entry.get("connected", True),
        active_runner=entry.get("active_runner", ""),
        runners=runners,
        serial=entry.get("serial", ""),
        baud=entry.get("baud", 0),
        build=build,
        fixtures=entry.get("fixtures", []),
        logic_analyzer=la_cfg,
        board_pins=_resolve_yaml_path(entry.get("board_pins", ""), yaml_dir),
        name=_derive_board_name(entry),
    )
    board.validate()
    return board


def load(path: str | Path, board_name: str | None = None) -> BoardConfig:
    """Load a connected board from a hardware-map.yml file.

    *board_name=None* → return the first connected board (backward compatible).
    *board_name=<name>* → return the board whose ``name`` / serial port matches.

    All relative paths inside the YAML are resolved against the YAML file's
    parent directory so the hardware-map is self-contained regardless of cwd.
    """
    p = Path(path)
    yaml_dir = p.parent.resolve()
    with open(p) as f:
        entries = yaml.safe_load(f)

    if not entries:
        raise RuntimeError(f"No entries in {p}")

    connected = [e for e in entries if e.get("connected", False)]
    if not connected:
        raise RuntimeError("No connected board found in hardware-map.yml")

    if board_name:
        for entry in connected:
            if _derive_board_name(entry) == board_name:
                return _entry_to_board(entry, yaml_dir)
        names = [_derive_board_name(e) for e in connected]
        raise RuntimeError(
            f"Board '{board_name}' not found in hardware-map.yml. "
            f"Available: {names}"
        )

    if len(connected) > 1:
        names = [_derive_board_name(e) for e in connected]
        print(
            f"[vsf-bench] Multiple connected boards: {names}. "
            f"Using '{names[0]}'. Use --board to select.",
            file=sys.stderr,
        )

    return _entry_to_board(connected[0], yaml_dir)


def load_all(path: str | Path) -> list[BoardConfig]:
    """Return all connected boards from a hardware-map.yml file."""
    p = Path(path)
    yaml_dir = p.parent.resolve()
    with open(p) as f:
        entries = yaml.safe_load(f)

    return [
        _entry_to_board(e, yaml_dir)
        for e in entries
        if e.get("connected", False)
    ]


def validate_runners(board: BoardConfig) -> None:
    """Validate runner params against their class definitions.

    Checks REQUIRED_PARAMS, class-specific validate_params(), and artifact
    cross-reference against build.artifacts. Uses inline import to avoid
    circular dependency with runner modules.
    """
    from vsf_bench.runners.registry import get_runner_class

    errors = []

    for name, runner_cfg in board.runners.items():
        cls = get_runner_class(runner_cfg.type)
        if cls is None:
            continue  # custom runner (post-MVP), skip validation

        for param in cls.REQUIRED_PARAMS:
            if param not in runner_cfg.params:
                errors.append(f"runners.{name}: missing required param '{param}'")

        for err in cls.validate_params(runner_cfg.params):
            errors.append(f"runners.{name}: {err}")

        if runner_cfg.artifact is None:
            errors.append(f"runners.{name}: missing required 'artifact' field")
        else:
            build_names = {a.name for a in board.build.artifacts}
            if runner_cfg.artifact.name not in build_names:
                errors.append(
                    f"runners.{name}.artifact '{runner_cfg.artifact.name}' "
                    f"not found in build.artifacts"
                )

    for name, runner_cfg in board.runners.items():
        if runner_cfg.artifact and runner_cfg.artifact.format not in (
            "elf", "uf2", "bin", "hex",
        ):
            errors.append(
                f"runners.{name}.artifact.format '{runner_cfg.artifact.format}' unknown"
            )

    if errors:
        raise ValueError("Runner validation failed:\n  " + "\n  ".join(errors))
