---
name: vsf-bench
description: Build, flash, and run automated test suites for VSF firmware on target hardware. Requires board for flash/test; build-only offline. Delegates driver changes to vsf-hal-driver.
metadata:
  version: "1.0"
  license: Apache-2.0
---

**UTILITY SKILL** — used standalone or after vsf-hal-driver changes.

USE FOR:
- Build-flash-test loop
- Build-only without hardware
- Flash and test on hardware

DO NOT USE FOR:
- HAL driver porting (use vsf-hal-driver)
- Driver debugging (use vsf-hal-driver)
- Modifying test scripts (use write-a-skill)

## Concepts

- **Orchestrator flow:** `build → flash → for each scene: send trigger → run script`. Trigger is `vsf-test run <scene>` over serial.
- **Hardware map:** `hardware-map.yml` defines `build.source_dir`, `serial.port`/`baudrate`, `flash.runner` (swd or uf2).
- **Script signature:** `def run(serial, la=None)`. Scripts validate only — orchestrator sends triggers. Exception = FAIL, normal return = PASS.
- **Path model:** All paths are cwd-relative or absolute. No `project_root` parameter.

## Testing strategy

**`--all` without `--suite` runs every enabled suite — slow. Only use for final regression.** During development, always limit with `--suite` to the peripheral you're working on. If the firmware is already flashed, `--test` skips build+flash for even faster iteration.

## Quickstart

```bash
# Full regression (build + flash + ALL suites) — SLOW, final gate only
vsf-bench --all vsf.demo/board/<board>/hardware-map.yml

# Development: build + flash + single suite (fast)
vsf-bench --all vsf.demo/board/<board>/hardware-map.yml --suite <name>

# Fastest iteration: test-only, no rebuild (firmware already flashed)
vsf-bench --test vsf.demo/board/<board>/hardware-map.yml --suite <name>

# Individual steps (for isolating failures)
vsf-bench --build  vsf.demo/board/<board>/hardware-map.yml
vsf-bench --flash  vsf.demo/board/<board>/hardware-map.yml
vsf-bench --test   vsf.demo/board/<board>/hardware-map.yml
```

## Example: Full loop after driver changes

```bash
# Build, flash, and run all test suites:
vsf-bench --all vsf.demo/board/rp2040/hardware-map.yml

# If build succeeds but flash fails: stop — do not test
# If flash succeeds but test fails: check logs for timeout, LA, or driver bug
#   - Timeout/LA → check hardware-map.yml serial/config
#   - Driver bug → run `Skill("vsf-hal-driver")` with test logs attached
```

## Error handling

| Failure | Cause | Action |
|---------|-------|--------|
| Build fails | cmake/SDK missing, wrong `build.source_dir` | verify path and cmake installation |
| Flash fails | board disconnected, wrong runner | check USB; verify `flash.runner` (swd vs uf2) |
| Test timeout | baudrate mismatch, firmware not responding | verify `serial.port`/`baudrate` match firmware |
| Suite not found | suite disabled in `vsf_usr_cfg.h` | check `VSF_USE_TEST_<SUITE>` is ENABLED |
| No scenes listed | test framework not enabled | verify `VSF_USE_TEST = ENABLED` |
| Hardware unresponsive | USB/connection issue | verify connection and `flash.runner` |

If vsf-bench not installed: `pip install -e vsf.demo/vsf/test/vsf_bench`.

## Regression failure isolation

When a full `--all` regression fails, **always isolate to a single suite** before blaming the driver:

```bash
# 1. Reflash to ensure a clean device state
vsf-bench --flash vsf.demo/board/<board>/hardware-map.yml

# 2. Run only the failing suite
vsf-bench --test vsf.demo/board/<board>/hardware-map.yml --suite <failed_suite>

# 3. If it passes alone, the failure is caused by suite interaction
#    (state left behind by a previous suite). Check the suite just before it.
# 4. If it still fails alone, the driver has a genuine bug — use vsf-hal-driver.
```

**Rule:** a suite that passes in isolation but fails in a full run is NOT a driver bug; it is a test-sequence/state-cleanup bug.

## Limitations

- **Hardware required for flash/test:** `--flash` and `--test` need a connected board. Use `--build` only if no hardware.
- **Test assumes firmware running:** `--test` alone does not flash first. For cold start, use `--all`.
- **hardware-map.yml required:** tool cannot infer board config. If missing, copy from `vsf.demo/board/rp2040/hardware-map.yml` as template and edit for your board.
- **No incremental builds:** always clean-rebuilds.

## Reference (optional supplementary reading)

- [concepts](modules/concepts.md) — Orchestrator flow, script signature, hardware map, flash runners
- [examples](modules/examples.md) — IO verification, single scenario
- [troubleshooting](modules/troubleshooting.md) — Detailed failure analysis
- [reference](modules/reference.md) — Complete reference docs
