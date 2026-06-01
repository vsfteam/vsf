# Concepts

- **Orchestrator flow:** `build → flash → for each scene: send trigger → run script`. The trigger is `vsf-test run <scene>` sent over serial.
- **Script signature:** `def run(serial: SerialInstrument, la: LogicAnalyzerInstrument | None = None) -> None`. Scripts are validation-only — they do NOT send triggers; the orchestrator handles that. If a script raises an exception, the orchestrator records a FAIL verdict and continues to the next scene. If a script returns normally (no exception), the verdict is PASS.
- **Hardware map:** `hardware-map.yml` defines `build.source_dir`, `serial.port`/`baudrate`, `flash.runner` (swd or uf2), and optional `logic_analyzer.channels`. Only hardware listed in the map is supported.
- **Flash runners:** swd (OpenOCD + CMSIS-DAP) is the active runner. Fallback uf2 flashes via `/dev/sdb1`. Runner is selected by `flash.runner` in hardware-map.yml.
