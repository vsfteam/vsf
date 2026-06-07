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
- Multi-stage pipelines (bootloader → app → test)
- Multi-board parallel testing

DO NOT USE FOR:
- HAL driver porting (use vsf-hal-driver)
- Driver debugging (use vsf-hal-driver)

## Concepts

- **Orchestrator flow:** `build → program → for each suite: send trigger → run script`. Trigger is `vsf-test run <suite>` over serial.
- **Hardware map:** `hardware-map.yml` defines `build.source_dir`, serial port (descriptive matching by VID/PID/serial), power config, pipelines, and board-level properties (dfu_key, debug_probe).
- **Program abstraction:** `ProgramCapability` ABC replaces "flash" — DFU, SWD, and UART HCI are all programming methods. The old `FlashRunner` name is preserved as a backward-compat alias.
- **Pipeline:** Multi-stage build/program/test sequence defined in hardware-map.yml `pipelines:` section. Each stage references a project and can override the runner and flash parameters.
- **Capabilities/Adapters:** `capabilities/` defines abstract interfaces (ProgramCapability, GPIO). `adapters/` implements them per hardware entity (DFUAdapter, FT232HAdapter). One file per hardware device.
- **Script signature:** `def run(serial, la=None)`. Scripts validate only — orchestrator sends triggers. Exception = FAIL, normal return = PASS.
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
```

## CLI changes (v2.0)

| Old (v1.x) | New (v2.0) | Reason |
|-----------|-----------|--------|
| `--flash` | `--program` | "flash" reserved for flash memory hardware; program covers DFU/SWD/UART |
| `vsf-bench-flash` | `vsf-bench-program` | Same |
| `FlashRunner` | `ProgramCapability` (FlashRunner kept as compat alias) | capability abstraction |

## Example: BH1098 pipeline

```bash
# Build bootloader (UART) → flash → build app → program via UART
vsf-bench --pipeline uart_flash_regression --board b1 --hardware-map board/hardware-map.yml
```

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
