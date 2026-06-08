---
name: vsf-bench
description: Build, program, and test VSF firmware on target hardware. Requires board for program/test; build-only offline. Supports multi-stage pipelines, multi-board parallel test, and capabilities/adapters architecture.
metadata:
  version: "2.0"
  license: Apache-2.0
---

**UTILITY SKILL** — used standalone or after vsf-hal-driver changes.

USE FOR:
- Build-program-test loop
- Build-only without hardware
- Program and test on hardware
- Multi-project pipelines (bootloader → app → test)
- Multi-board parallel testing
- Hardware-interaction tests: GPIO trigger + serial send/expect + LA capture + post-process. Declarative YAML — no Python required.
- Stress/loop testing with `--repeat N` and `--set` overrides

DO NOT USE FOR:
- HAL driver porting (use vsf-hal-driver)
- Driver debugging (use vsf-hal-driver)

## Concepts

- **Orchestrator flow:** `build → program → for each suite: send trigger → run script`. Trigger is `vsf-test run <suite>` over serial.
- **Hardware map:** `hardware-map.yml` defines `build.source_dir`, serial port (descriptive matching by VID/PID/serial), power config, pipelines, board-level properties (dfu_key, debug_probe), and `gpio_adapters` for FT232H/CH347 IO control.
- **Program abstraction:** `ProgramCapability` ABC replaces "flash" — DFU, SWD, and UART HCI are all programming methods. The old `FlashRunner` name is preserved as a backward-compat alias.
- **Pipeline:** Multi-stage build/program/test sequence defined in hardware-map.yml `pipelines:` section. Each stage references a project and can override the runner and flash parameters. Supports `scenario` action for inline scenario execution.
- **Pipeline（流水线）：** The one and only orchestration concept. A pipeline is a `steps:` list — every step type is equal. Supports `timeout` (global), `matrix` (parameter combinations), step-level `continue-on-error` / `max-retries` / `on-failure`.
- **Step `id`:** Optional named reference on any step, enabling `--set id.key=value` targeted overrides (e.g. `--set pair_wait.delay=3.0`).
- **`loop` block:** Repeat a block of steps N times. `--repeat N` CLI flag overrides all loop.repeat values.
- **`matrix`:** Cartesian product of parameter values — one run per combination. `--matrix key=val` filters. `${{ matrix.key }}` syntax in step params.
- **`continue-on-error`:** Step failure doesn't stop pipeline. Combined with `on-failure` for diagnostic dump steps.
- **`max-retries`:** Auto-retry a failed step N times before declaring failure.
- **`--set key=value`:** Override any step parameter from CLI without editing YAML. Global (`--set delay=2.0`) or id-targeted (`--set pair_wait.delay=3.0`).
- **Capabilities/Adapters:** `capabilities/` defines abstract interfaces (ProgramCapability, GPIO, LogicAnalyzer). `adapters/` implements them per hardware entity (DFUAdapter, FT232HAdapter, DSViewAdapter). One file per hardware device.
- **Script signature:** `def run(serial, la=None)`. Scripts validate only — orchestrator sends triggers. Exception = FAIL, normal return = PASS.
- **Output path convention:** All run artifacts go under `logs/<ts>-<board>-<pipeline>/`. Loop iterations get `runs/01/`, `runs/02/` subdirectories. Files prefixed by category: `run.log` (combined log), `test-*` (test artifacts), `la-*` (LA artifacts). No `--log-dir` or `log_dir` config — `logs/` is hardcoded.
- **Path model:** All paths are cwd-relative or absolute. No `project_root` parameter.

## Quickstart

```bash
# Full regression (build + program + ALL suites) — SLOW, final gate only
vsf-bench --all --hardware-map board/hardware-map.yml

# Development: build + program + single suite (fast)
vsf-bench --all --hardware-map board/hardware-map.yml --suite <name>

# Fastest iteration: test-only, no rebuild (firmware already programmed)
vsf-bench --test --hardware-map board/hardware-map.yml --suite <name>

# Individual steps (for isolating failures)
vsf-bench --build   --hardware-map board/hardware-map.yml
vsf-bench --program --hardware-map board/hardware-map.yml --board <board>
vsf-bench --test    --hardware-map board/hardware-map.yml --board <board>

# Pipeline (build + program multiple projects in sequence)
vsf-bench --pipeline <name> --board <board> --hardware-map board/hardware-map.yml

# Multi-board parallel test
vsf-bench test --all-boards --project <project> --hardware-map board/hardware-map.yml

# List available pipelines
vsf-bench --list-pipelines --hardware-map board/hardware-map.yml

# Pipeline with overrides
vsf-bench --pipeline bt_stress --board b1 --repeat 1000 --set pair_wait.delay=3.0
```

## CLI changes (v2.0)

| Old (v1.x) | New (v2.0) | Reason |
|-----------|-----------|--------|
| `--flash` | `--program` | "flash" reserved for flash memory hardware; program covers DFU/SWD/UART |
| `vsf-bench-flash` | `vsf-bench-program` | Same |
| `FlashRunner` | `ProgramCapability` (FlashRunner kept as compat alias) | capability abstraction |
| — | `--repeat N` | Override all `loop.repeat` values from CLI |
| — | `--set key=value` | Override step params without editing YAML; supports `id.key=value` |
| — | `--matrix key=val` | Filter matrix combinations from CLI |

## Example: BH1098 pipeline (build + flash)

```bash
# Build bootloader (UART) → flash → build app → program via UART
vsf-bench --pipeline uart_flash_regression --board b1 --hardware-map board/hardware-map.yml
```

## Example: BT pairing stress test pipeline

```yaml
# hardware-map.yml
pipelines:
  bt_stress:
    description: "BT 配对压力测试 — build/flash 一次 + 配对循环 100 次"
    steps:
      - build: application-standalone
      - program: application-standalone
      - power_cycle
      - wait_for:
          expect:
            - pattern: "bt_stack_ready"
              verdict: pass
            - pattern: "BT_ERROR"
              verdict: fail
          timeout: 10
      - loop:
          repeat: 100
          steps:
            - power_cycle
            - id: boot_wait
              delay: 1.0
            - wait_for:
                expect:
                  - pattern: "bt_stack_ready"
                    verdict: pass
                  - pattern: "BT_ERROR"
                    verdict: fail
                timeout: 10
            - id: trigger_pair
              gpio_set:
                level: low
                duration: 0.5
            - serial_send: "bt scan\r\n"
            - la_start: {}
            - id: pair_wait
              delay: 2.0
            - wait_for:
                expect:
                  - pattern: "PAIRED"
                    verdict: pass
                  - pattern: "DISCONNECT"
                    verdict: fail
                timeout: 30
            - la_stop: {}
            - la_decode: { baudrate: 2000000 }
            - run: "python collect.py $RUN_DIR/la-decode-*.csv"
```

```bash
# 标准运行
vsf-bench --pipeline bt_stress --board b1

# 调试：跑 5 次，配对等待时间改 3 秒
vsf-bench --pipeline bt_stress --board b1 --repeat 5 --set pair_wait.delay=3.0
```

### Step primitives reference

| Primitive | YAML | Purpose |
|-----------|------|---------|
| `build` | `- build: <project>` | Compile firmware |
| `program` | `- program: <project>` | Flash firmware |
| `power_cycle` | `- power_cycle` | Power off→0.5s→on (SmartUSBHub) |
| `power_off` | `- power_off` | Power off only |
| `power_on` | `- power_on` | Power on only |
| `delay` | `- delay: 1.0` | Sleep N seconds |
| `serial_send` | `- serial_send: "cmd\r\n"` | Send data over serial to firmware |
| `wait_for` | `- wait_for: { expect: [{pattern, verdict}], timeout: 10 }` | Multi-pattern serial match (first hit wins) |
| `gpio_set` | `- gpio_set: { level: low, duration: 0.5 }` | FT232H IO toggle |
| `la_start` / `la_stop` | `- la_start: {}` | LA capture start/stop (params inherit from board.la) |
| `la_decode` | `- la_decode: { baudrate: 2000000 }` | UART decode from .dsl |
| `run` | `- run: "python analyze.py $RUN_DIR/..."` | Shell command ($RUN_DIR env var) |
| `loop` | `- loop: { repeat: 100, steps: [...] }` | Repeat steps N times |

### Step-level control flow (any step can have these)

| Property | YAML | Purpose |
|----------|------|---------|
| `continue-on-error` | `continue-on-error: true` | Don't stop pipeline on failure |
| `max-retries` | `max-retries: 3` | Retry up to N times before declaring fail |
| `on-failure` | `on-failure: <steps>` | Steps to run only if this step fails (diagnostic dump) |

### Pipeline-level properties

| Property | YAML | Purpose |
|----------|------|---------|
| `timeout` | `timeout: 600` | Global timeout in seconds |
| `matrix` | `matrix: { board: [b1,b3], baud: [115200,2M] }` | Cartesian product — one run per combo |

## Error handling

| Failure | Cause | Action |
|---------|-------|--------|
| Build fails | cmake/IAR/SDK missing, wrong `build.source_dir` | verify path and build tool installation |
| Program fails | board disconnected, wrong runner, handshake timeout | check USB; verify runner config; check serial port matching |
| Test timeout | baudrate mismatch, firmware not responding | verify serial port/baudrate match firmware |
| Suite not found | suite disabled in `vsf_usr_cfg.h` | check `VSF_USE_TEST_<SUITE>` is ENABLED |
| No suites listed | test framework not enabled | verify `VSF_USE_TEST = ENABLED` |
| Hardware unresponsive | USB/connection issue | verify connection and runner config |
| Multi-device DFU conflict | multiple boards in DFU mode with same VID/PID | power-cycle only target board with DFU key; assign unique serial numbers |
| Pipeline wait_for timeout | pattern not matched within timeout | check firmware log in `run.log`; increase timeout or adjust pattern regex |
| Pipeline gpio_set fails | FT232H not connected or pyftdi missing | check USB; `pip install pyftdi` |
| Pipeline la_start fails | DSLogic not connected or dsview-cli not in PATH | check USB; verify dsview-cli installation |
| Pipeline run exits non-zero | shell command returned error | check script path and arguments; verify `$RUN_DIR` is set |
| Pipeline serial_send no effect | serial port not open or firmware not listening | verify step serial_send precedes wait_for to read response |

If vsf-bench not installed: `pip install -e vsf/test/vsf_bench`.

## Regression failure isolation

When a full `--all` regression fails, **always isolate to a single suite** before blaming the driver:

```bash
# 1. Reprogram to ensure a clean device state
vsf-bench --program --hardware-map board/hardware-map.yml --board <board>

# 2. Run only the failing suite
vsf-bench --test --hardware-map board/hardware-map.yml --board <board> --suite <failed_suite>

# 3. If it passes alone, the failure is caused by suite interaction
#    (state left behind by a previous suite). Check the suite just before it.
# 4. If it still fails alone, the driver has a genuine bug — use vsf-hal-driver.
```

**Rule:** a suite that passes in isolation but fails in a full run is NOT a driver bug; it is a test-sequence/state-cleanup bug.

## Reference (optional supplementary reading)

- [concepts](modules/concepts.md) — Orchestrator flow, script signature, hardware map, capabilities/adapters
- [examples](modules/examples.md) — IO verification, single scenario, BH1098 pipeline
- [troubleshooting](modules/troubleshooting.md) — Detailed failure analysis
- [reference](modules/reference.md) — Complete CLI reference
