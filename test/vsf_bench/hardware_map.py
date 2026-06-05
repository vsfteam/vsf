"""Parse hardware-map.yml and return typed board descriptors."""

import sys
import yaml
from pathlib import Path

from vsf_bench.config import (
    ArtifactConfig,
    BoardConfig,
    BuildConfig,
    LogicAnalyzerConfig,
    PipelineConfig,
    PowerConfig,
    ProjectConfig,
    RunnerConfig,
    StageConfig,
)
from vsf_bench.hwctrl.match_serial import match_serial_port


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


def _parse_build_config(build_cfg: dict, yaml_dir: Path) -> BuildConfig:
    """Parse the ``build`` section.  Known keys go to BuildConfig fields;
    everything else becomes builder params.
    """
    known = {"source_dir", "build_dir", "tool", "artifacts"}
    params = {k: v for k, v in build_cfg.items() if k not in known}

    return BuildConfig(
        source_dir=_resolve_yaml_path(build_cfg.get("source_dir", ""), yaml_dir),
        build_dir=_resolve_yaml_path(build_cfg.get("build_dir", ""), yaml_dir),
        tool=build_cfg.get("tool", "cmake"),
        params=params,
        artifacts=[
            ArtifactConfig(name=a["name"], format=a["format"])
            for a in build_cfg.get("artifacts", [])
        ],
    )


def _parse_runners(runners_raw: dict) -> dict[str, RunnerConfig]:
    """Parse the ``runners`` section shared by board entries and project entries."""
    runners = {}
    for name, cfg in runners_raw.items():
        artifact_raw = cfg.get("artifact")
        artifact = None
        if artifact_raw:
            artifact = ArtifactConfig(
                name=artifact_raw["name"],
                format=artifact_raw["format"],
            )
        if "params" in cfg and isinstance(cfg["params"], dict):
            params = dict(cfg["params"])
        else:
            params = {k: v for k, v in cfg.items()
                      if k not in ("type", "artifact")}
        runners[name] = RunnerConfig(
            type=cfg["type"],
            artifact=artifact,
            params=params,
        )
    return runners


def _entry_to_board(entry: dict, yaml_dir: Path, hubs: dict | None = None, gpio_adapters: dict | None = None) -> BoardConfig:
    """Convert one YAML board entry to a BoardConfig (no name filtering)."""
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

    # ── UART ports ──
    log_raw = entry.get("debug_uart", entry.get("serial", ""))
    if isinstance(log_raw, dict):
        debug_uart = match_serial_port(log_raw, label=f"board {_derive_board_name(entry)} log UART")
        debug_baudrate = log_raw.get("baudrate", entry.get("baud", 0))
    else:
        debug_uart = log_raw
        debug_baudrate = entry.get("baud", 0)

    dl_raw = entry.get("program_uart", entry.get("download_serial", ""))
    if isinstance(dl_raw, dict):
        program_uart = match_serial_port(dl_raw, label=f"board {_derive_board_name(entry)} download UART")
    else:
        program_uart = dl_raw

    # ── Power (resolve hub reference) ──
    power = None
    power_raw = entry.get("power")
    if power_raw:
        hub_name = power_raw.get("hub", "")
        hub_cfg = hubs.get(hub_name, {}) if hubs else {}
        power = PowerConfig(
            hub_addr=hub_cfg.get("hub_addr", power_raw.get("hub_addr")),
            port=power_raw.get("port", 3),
        )

    # ── DFU key (resolve gpio reference) ──
    dfu_key = entry.get("dfu_key")
    gpio_adapter_serial = ""
    if dfu_key and gpio_adapters:
        gpio_name = dfu_key.get("gpio", "")
        gpio_cfg = gpio_adapters.get(gpio_name, {})
        gpio_adapter_serial = gpio_cfg.get("adapter_serial", "")
    if not dfu_key:
        dfu_key = None

    board = BoardConfig(
        platform=entry.get("platform", ""),
        connected=entry.get("connected", True),
        debug_uart=debug_uart,
        debug_baudrate=debug_baudrate,
        program_uart=program_uart,
        power=power,
        fixtures=entry.get("fixtures", []),
        logic_analyzer=la_cfg,
        debug_probe=entry.get("debug_probe"),
        dfu_key=dfu_key,
        gpio_adapter_serial=gpio_adapter_serial,
        board_pins=_resolve_yaml_path(entry.get("board_pins", ""), yaml_dir),
        name=_derive_board_name(entry),
    )
    board.validate()
    return board


def _entry_to_project(project_name: str, entry: dict, yaml_dir: Path) -> ProjectConfig:
    """Convert one YAML project entry to a ProjectConfig."""
    build_cfg = entry.get("build", {})
    build = _parse_build_config(build_cfg, yaml_dir)

    runners = _parse_runners(entry.get("runners", {}))

    project = ProjectConfig(
        build=build,
        active_runner=entry.get("active_runner", ""),
        runners=runners,
        name=project_name,
    )
    project.validate()
    return project


# ---------------------------------------------------------------------------
# Backward-compatible board-centric loaders (used by existing test codepaths)
# ---------------------------------------------------------------------------

def _entry_to_board_legacy(entry: dict, yaml_dir: Path) -> BoardConfig:
    """Convert one YAML entry (old flat format) to a BoardConfig with embedded
    build/runner fields for backward compatibility."""
    build_cfg = entry.get("build", {})
    build = _parse_build_config(build_cfg, yaml_dir)
    runners = _parse_runners(entry.get("runners", {}))

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

    # Legacy BoardConfig expects active_runner, runners, build
    # Use setattr to work around the fact that these fields no longer exist
    # on the canonical BoardConfig.
    from dataclasses import fields as dc_fields
    legacy_field_names = {f.name for f in dc_fields(BoardConfig)}
    kwargs = dict(
        platform=entry.get("platform", ""),
        connected=entry.get("connected", True),
        debug_uart=entry.get("serial", ""),
        debug_baudrate=entry.get("baud", 0),
        fixtures=entry.get("fixtures", []),
        logic_analyzer=la_cfg,
        board_pins=_resolve_yaml_path(entry.get("board_pins", ""), yaml_dir),
        name=_derive_board_name(entry),
    )
    board = BoardConfig(**kwargs)

    # Attach legacy fields dynamically for backward compat
    board.active_runner = entry.get("active_runner", "")          # type: ignore[attr-defined]
    board.runners = runners                                        # type: ignore[attr-defined]
    board.build = build                                            # type: ignore[attr-defined]

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
    with open(p, encoding="utf-8") as f:
        entries = yaml.safe_load(f)

    if not entries:
        raise RuntimeError(f"No entries in {p}")

    connected = [e for e in entries if e.get("connected", False)]
    if not connected:
        raise RuntimeError("No connected board found in hardware-map.yml")

    if board_name:
        for entry in connected:
            if _derive_board_name(entry) == board_name:
                return _entry_to_board(entry, yaml_dir, hubs, gpio_adapters)
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

    return _entry_to_board_legacy(connected[0], yaml_dir)


def load_all(path: str | Path) -> list[BoardConfig]:
    """Return all connected boards from a hardware-map.yml file (legacy format)."""
    p = Path(path)
    yaml_dir = p.parent.resolve()
    with open(p, encoding="utf-8") as f:
        entries = yaml.safe_load(f)

    return [
        _entry_to_board_legacy(e, yaml_dir)
        for e in entries
        if e.get("connected", False)
    ]


# ---------------------------------------------------------------------------
# Board+project loader
# ---------------------------------------------------------------------------

def load_board_and_project(
    path: str | Path,
    board_name: str | None = None,
    project_name: str | None = None,
) -> tuple[BoardConfig, ProjectConfig]:
    """Load a board and project from a hardware-map.yml file.

    The YAML must have top-level ``boards`` and ``projects`` sections (the
    hardware_map format).  *board_name* and *project_name* select specific entries;
    when omitted the first connected board / first project is used.

    Returns ``(BoardConfig, ProjectConfig)``.
    """
    p = Path(path)
    yaml_dir = p.parent.resolve()
    with open(p, encoding="utf-8") as f:
        doc = yaml.safe_load(f)

    if not isinstance(doc, dict):
        raise RuntimeError(
            f"Expected a mapping with 'boards' and 'projects' keys, got {type(doc).__name__}"
        )

    boards_raw = doc.get("boards", [])
    projects_raw = doc.get("projects", {})
    hubs = doc.get("smartusb_hubs", {})
    gpio_adapters = doc.get("gpio_adapters", {})

    if not boards_raw:
        raise RuntimeError("No 'boards' section in hardware-map.yml")
    if not projects_raw:
        raise RuntimeError("No 'projects' section in hardware-map.yml")

    # --- Board selection ---
    connected = [e for e in boards_raw if e.get("connected", False)]
    if not connected:
        raise RuntimeError("No connected board found in hardware-map.yml")

    if board_name:
        entry = None
        for e in connected:
            if _derive_board_name(e) == board_name:
                entry = e
                break
        if entry is None:
            names = [_derive_board_name(e) for e in connected]
            raise RuntimeError(
                f"Board '{board_name}' not found. Available: {names}"
            )
    else:
        if len(connected) > 1:
            names = [_derive_board_name(e) for e in connected]
            print(
                f"[vsf-bench] Multiple connected boards: {names}. "
                f"Using '{names[0]}'. Use --board to select.",
                file=sys.stderr,
            )
        entry = connected[0]

    board = _entry_to_board(entry, yaml_dir, hubs, gpio_adapters)

    # --- Project selection ---
    if not project_name:
        project_name = next(iter(projects_raw.keys()))
        if len(projects_raw) > 1:
            print(
                f"[vsf-bench] Multiple projects: {list(projects_raw.keys())}. "
                f"Using '{project_name}'. Use --project to select.",
                file=sys.stderr,
            )

    proj_entry = projects_raw.get(project_name)
    if proj_entry is None:
        raise RuntimeError(
            f"Project '{project_name}' not found. "
            f"Available: {list(projects_raw.keys())}"
        )

    project = _entry_to_project(project_name, proj_entry, yaml_dir)
    return board, project


# ---------------------------------------------------------------------------
# Runner validation
# ---------------------------------------------------------------------------

def validate_runners(target, build_artifacts=None) -> None:
    """Validate runner params against their class definitions.

    *target* can be a BoardConfig (legacy — runners/build attached dynamically),
    a ProjectConfig, or any object with ``.runners`` and ``.build`` attributes.

    Checks REQUIRED_PARAMS, class-specific validate_params(), and artifact
    cross-reference against build.artifacts.
    """
    from vsf_bench.runners.registry import get_runner_class

    errors = []
    runners = getattr(target, "runners", {})
    build = getattr(target, "build", None)
    artifacts = build_artifacts
    if artifacts is None and build is not None:
        artifacts = getattr(build, "artifacts", [])

    for name, runner_cfg in runners.items():
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
        elif artifacts:
            build_names = {a.name for a in artifacts}
            if runner_cfg.artifact.name not in build_names:
                errors.append(
                    f"runners.{name}.artifact '{runner_cfg.artifact.name}' "
                    f"not found in build.artifacts"
                )

    for name, runner_cfg in runners.items():
        if runner_cfg.artifact and runner_cfg.artifact.format not in (
            "elf", "uf2", "bin", "hex",
        ):
            errors.append(
                f"runners.{name}.artifact.format '{runner_cfg.artifact.format}' unknown"
            )

    if errors:
        raise ValueError("Runner validation failed:\n  " + "\n  ".join(errors))


# ---------------------------------------------------------------------------
# Pipeline loading
# ---------------------------------------------------------------------------

def load_pipeline(
    hardware_map_path: str,
    pipeline_name: str,
) -> PipelineConfig:
    """Parse a named pipeline from hardware-map.yml ``pipelines`` section.

    Returns a ``PipelineConfig`` whose ``stages`` each have a ``project``
    *name* (str).  Call ``resolve_pipeline_projects()`` afterwards to
    replace those strings with ``ProjectConfig`` instances.
    """
    with open(hardware_map_path, "r", encoding="utf-8") as f:
        raw = yaml.safe_load(f) or {}

    pipelines_raw = raw.get("pipelines", {})
    if not pipelines_raw:
        raise ValueError(
            f"No 'pipelines' section found in {hardware_map_path}"
        )

    if pipeline_name not in pipelines_raw:
        raise ValueError(
            f"Pipeline '{pipeline_name}' not found in {hardware_map_path}. "
            f"Available: {list(pipelines_raw.keys())}"
        )

    entry = pipelines_raw[pipeline_name]
    stages = []
    for i, s in enumerate(entry.get("stages", [])):
        actions = s.get("actions", [])
        if not actions:
            raise ValueError(
                f"Pipeline '{pipeline_name}' stage {i}: 'actions' is required"
            )
        stages.append(StageConfig(
            project=s["project"],
            actions=actions,
            flash_overrides=s.get("flash_overrides"),
            wait_for=s.get("wait_for"),
            power_cycle=s.get("power_cycle", False),
        ))

    return PipelineConfig(
        name=pipeline_name,
        description=entry.get("description", ""),
        stages=stages,
    )


def list_pipelines(hardware_map_path: str) -> list[PipelineConfig]:
    """Return all pipelines defined in hardware-map.yml (names only, no resolution)."""
    with open(hardware_map_path, "r", encoding="utf-8") as f:
        raw = yaml.safe_load(f) or {}
    pipelines_raw = raw.get("pipelines", {})
    result = []
    for name, entry in pipelines_raw.items():
        result.append(PipelineConfig(
            name=name,
            description=entry.get("description", ""),
            stages=[],  # not resolved here
        ))
    return result


def resolve_pipeline_projects(
    pipeline: PipelineConfig,
    hardware_map_path: str,
    board_name: str | None = None,
) -> dict[str, ProjectConfig]:
    """Load all ProjectConfig instances referenced by pipeline stages.

    *board_name* is forwarded to avoid the "Multiple connected boards"
    warning when the caller has already selected a board.
    """
    project_names = {s.project for s in pipeline.stages}
    project_map: dict[str, ProjectConfig] = {}

    for name in project_names:
        _, project = load_board_and_project(
            hardware_map_path, project_name=name, board_name=board_name,
        )
        project_map[name] = project

    return project_map


def load_project(hardware_map_path: str, project_name: str) -> ProjectConfig:
    """Load a single project from hardware-map.yml without resolving a board."""
    p = Path(hardware_map_path)
    yaml_dir = p.parent.resolve()
    with open(p, encoding="utf-8") as f:
        doc = yaml.safe_load(f) or {}
    projects_raw = doc.get("projects", {})
    if project_name not in projects_raw:
        raise ValueError(
            f"Project '{project_name}' not found. Available: {list(projects_raw.keys())}"
        )
    return _entry_to_project(project_name, projects_raw[project_name], yaml_dir)


def load_defaults(hardware_map_path: str) -> dict:
    """Return the ``defaults`` section from hardware-map.yml, or {}."""
    with open(hardware_map_path, "r", encoding="utf-8") as f:
        raw = yaml.safe_load(f) or {}
    return raw.get("defaults", {})
