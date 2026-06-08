# Concepts

- **Pipeline（流水线）：** The one and only orchestration concept. A pipeline is a `steps:` list — every step type (build, program, power_cycle, delay, serial_send, wait_for, gpio_set, la_start, la_decode, run, loop) is equal. No separate "scenario" or "actions vs steps" distinction. Defined in `hardware-map.yml` `pipelines:` section.
- **Step `id`:** Optional named reference on any step, enabling `--set id.key=value` targeted overrides. Useful for adjusting timeouts, delays, etc. without editing YAML.
- **`loop` block:** Repeat a block of steps N times. CLI `--repeat N` overrides all loop.repeat values. Each iteration writes to `runs/01/`, `runs/02/` subdirectories.
- **`--set key=value`:** CLI override. Global (`--set delay=2.0`) changes all matching keys; id-targeted (`--set pair_wait.delay=3.0`) changes a specific step.
- **Orchestrator flow (test suites):** `build → program → for each suite: send trigger → run script`. Trigger is `vsf-test run <suite>` over serial.
- **Script signature:** `def run(serial: SerialInstrument, la: LogicAnalyzer | None = None) -> None`. Scripts are validation-only — they do NOT send triggers; the orchestrator handles that. Exception = FAIL, normal return = PASS.
- **Hardware map:** `hardware-map.yml` defines `build.source_dir`, serial port (descriptive matching by VID/PID/serial), power config, pipelines, `gpio_adapters`, and board-level properties (dfu_key, debug_probe, logic_analyzer).
- **Program abstraction:** `ProgramCapability` ABC — DFU, SWD, and UART HCI are all programming methods. Runner selected by `active_runner` in project config.
- **Capabilities/Adapters:** `capabilities/` defines abstract interfaces (ProgramCapability, GPIO, LogicAnalyzer). `adapters/` implements them per hardware entity (DFUAdapter, FT232HAdapter, DSViewAdapter). One file per hardware device.
- **GPIO adapter:** GPIO adapters (FT232H, CH347) defined in `hardware-map.yml` `gpio_adapters` section. Pipeline steps reference them by name, specifying only `pin` and `active_low`.
- **LA parameter inheritance:** `la_start` / `la_decode` inherit device/samplerate/channels from `board.logic_analyzer` in hardware-map.yml. Step only overrides what differs.
- **wait_for multi-pattern:** `expect` list is ordered — first matching pattern wins. Each pattern has `verdict: pass` or `verdict: fail`. Timeout always equals fail.
- **Output path convention:** All run artifacts under `logs/<ts>-<board>-<pipeline>/`. Files prefixed by category: `run.log` (combined host+firmware log), `test-events.jsonl` (audit log + verdict), `test-report.junit.xml` (JUnit), `la-*` (LA artifacts). `logs/` is hardcoded — no `--log-dir`.
