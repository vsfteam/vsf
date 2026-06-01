---
name: vsf-hal-driver
description: Create, implement, audit, and debug VSF HAL LV0 drivers. Handles register-level bugs in `.c` files (clock gates, IRQ handlers, register writes). Delegates build/flash/test to vsf-bench.
metadata:
  version: "1.0"
  license: Apache-2.0
---

**UTILITY SKILL** — delegates build/flash/test to vsf-bench skill. Read [concepts](modules/concepts.md) first if unfamiliar with VSF terms (LV0, IMP_LV0, VSF_MCONNECT).

USE FOR:
- Full chip port: vendor UART echo → VSF skeleton → test framework → peripheral → clock verify → full suite
- Adding peripherals to existing chip ports (register-level `.c`/`.h` driver code, not just pinmux wiring)
- Debugging LV0 driver register/IRQ/clock/DMA bugs

DO NOT USE FOR:
- Build, flash, or test only (use vsf-bench)
- Pinmux-only changes with no `.c`/`.h` driver code written
- LV1/LV2 layers or framework wrappers (use diagnose skill)
- Crashes where a bug ABOVE LV0 passes bad data to a driver — if the crash reproduces with a known-good driver, it is LV1/LV2; use diagnose skill

## Quickstart

**Scope rule:** If the bug is inside a `vsf_hw_*.c` file, it is LV0 and in scope. If the bug is in glue code that calls the driver incorrectly, it is LV1/LV2 and out of scope.

### New chip port (6 rungs)

| Rung | Goal | Verify |
|------|------|--------|
| R0 | Vendor UART echo on wired pins | Host byte echoes within ~100ms |
| R1 | VSF skeleton compiles | `cmake --build` passes; printf after `vsf_driver_init()` works |
| R2 | Test framework shell over serial | `vsf-test scene --list` responds over serial |
| R3 | First VSF peripheral | vsf-bench scenario passes |
| R4 | System clock verified | Logic analyzer measures gaps within ±5% of expected |
| R5 | Remaining peripherals one-by-one | Each peripheral's vsf-bench scenario passes |

Do not skip rungs. Each rung assumes earlier rungs hold. Detailed per-rung steps: [porting](modules/porting.md).

**Done when:** All peripherals in `vsf_usr_cfg.h` pass `vsf-bench --all hardware-map.yml`. This is the **final regression gate** — `--all` without `--suite` runs every enabled suite and is time-consuming. During development, use `--suite` to run only the peripheral you're working on (see Verify step below).

**If no hardware:** All static checks (skeleton + structure + quality + audit) exit 0 or 2 = structurally sound; flag hardware testing as pending.

**Handoffs:** vsf-bench for build/flash; diagnose skill for LV1/LV2 crashes. If a task requires both LV0 driver changes and LV1 glue, complete LV0 first then hand off LV1.

### Add peripheral (enter at R3 — assumes R0-R2 already pass)

**Implement:**
1. Scaffold: `scripts/scaffold/peripheral.py --driver-dir source/hal/driver --chip Vendor/Chip --periph <name>` — if scripts/ don't exist yet, copy template from `vsf/demo/template/hal/driver/<periph>/`
2. Implement register operations in .c/.h; use `VSF_MCONNECT(..., __IDX)` for instance prefixing — `VSF_MCONNECT` is a token-paste macro that builds per-instance names from `device.h` macros; never hardcode instance names like `vsf_hw_uart0`
3. Add `IMP_LV0` invocation per instance — `IMP_LV0` expands per-instance structs and IRQ handlers from `device.h` macros (reg base, IRQn, rst_bit, clk_bit). For new ports, derive these macros from vendor SDK headers (register base addresses, IRQn enums, reset/clock bit positions)
4. Add pinmux in `board/<board>/vsf_board.c` using `vsf_gpio_port_config_pins()` — not raw register writes
5. Enable: `scripts/util/enable.py --enable <periph> <vsf_usr_cfg.h>` — sets `#define VSF_HAL_USE_<PERIPH> ENABLED` in the config header. For a brand new chip, copy an existing `vsf_usr_cfg.h` from a similar chip port as starting template

**Check:**
6. Skeleton: `scripts/check/skeleton.py <template.c> <driver.c>` — verify signatures and structs match template
7. Static: `scripts/check/structure.py --periph <name> --side header <file.h>` and `--side source <file.c>`; `scripts/check/quality.py <file.c>`
8. Audit: `scripts/check/audit.py --chip Vendor/Chip` — cross-file consistency

**Verify:**
9. `vsf-bench --all hardware-map.yml --suite <periph>_<scenario>`
   - **Testing strategy:** `--suite <name>` limits the run to one peripheral's tests — fast, targeted feedback during development. `--all` without `--suite` runs every enabled suite (build + flash + all tests), which is slow. **Only use `--all` without `--suite` for final regression** after all peripherals are done.
   - If firmware is already flashed and you only need to re-run tests (no code changed): `vsf-bench --test hardware-map.yml --suite <periph>_<scenario>` skips build and flash.
   - If vsf-bench unavailable: run static checks (steps 6-8) only; flag to user that hardware testing is pending
   - If serial not responding: check hardware-map serial config and re-flash; run `gpio_io_check` first to rule out wiring

### Debug driver

When a driver compiles but produces no I/O (TX, clock, data — nothing on logic analyzer):

**Most common:**
1. Check `init()` for the three required steps:
   - Reset deassert: `reset_hw->reset &= ~rst_bit;`
   - Clock gate: `clock_hw->enable |= clk_bit;`
   - NVIC enable: `NVIC_EnableIRQ(irqn);` with priority from `cfg_ptr->prio`
2. Check IRQ handler name matches the IMP_LV0 expansion — mismatch = ISR never fires
3. Check DMA: DMA clock enabled in RCC/clock controller; channel mapped to peripheral per reference manual
4. Run `gpio_io_check` suite to rule out wiring before suspecting driver logic

**Less common:**
5. Check `volatile` on register pointers; spin-wait loops need `// < X us` annotation
6. Check `watchdog_hw->tick = N | WATCHDOG_TICK_ENABLE_BITS` in `driver.c` — missing this makes systimer run at wrong frequency, causing 100× timing errors caught by logic analyzer

**Stuck after checking all above?** (Markers = serial test markers sent by vsf-bench framework over UART)
- **Markers arrive but tests fail:** driver logic is wrong — run `scripts/check/audit.py --chip Vendor/Chip` to catch cross-file mismatches, then review register writes against reference manual
- **No markers at all:** firmware crashed before reaching test code — check `vsf_driver_init()` return value, add printf after each init step
- **vsf-bench exits non-zero:** distinguish infrastructure failure (serial/wiring) from test failure (driver bug)
- **Tests pass but hardware still misbehaves intermittently:** add `volatile` to register pointers, check spin-wait annotations, verify DMA channel arbitration
- **Scripts produce unexpected errors (Python import, wrong version):** check Python 3.11+ and `pip install -e vsf.demo/vsf/test/vsf_bench`

### When scripts fail

| Script | Exit ≠ 0 | Fix |
|--------|----------|-----|
| `skeleton.py` | Signature changed or struct removed | Restore from `hal/driver/common/template/vsf_template_<periph>.h`; implement inside functions, don't alter signatures |
| `structure.py` | Missing API or wrong prototype | Add missing function/struct; check IMP_LV0 block present |
| `quality.py` | Convention violation | Fix violation; suppress with `// quality: allow-<id>` only if confirmed false positive |
| `audit.py` | Cross-file mismatch | Fix declared-but-not-defined IRQ handlers or missing `VSF_HW_<PERIPH>_MASK` in `device.h` |

If `scripts/` don't exist yet (fresh port): skip checks 6-8; verify manually that `device.h` macros match vendor SDK headers, then proceed to step 9. Add scripts/ later.

Full troubleshooting: [troubleshooting](modules/troubleshooting.md).

## Example: Mode bits via if/else

**Symptom:** `init()` uses verbose `if/else` chains to translate VSF mode bits into hardware register bits.

**Fix:** Redefine the mode enum in chip-specific `.h` so values directly encode register bits (convention 8), then extract with a single mask in `.c`. See [examples](modules/examples.md) for before/after code.

## Reference

Modules marked **(required)** are blocking — skill cannot proceed without them.

- **[concepts](modules/concepts.md)** (required) — LV0, IMP_LV0, VSF_MCONNECT, reimplement-type macros, exit codes
- **[conventions](modules/conventions.md)** (required) — 15 conventions enforced by `scripts/check/quality.py`
- [examples](modules/examples.md) — Unsupported config, unused fields, mode bits via if/else
- [troubleshooting](modules/troubleshooting.md) — Script failures, runtime failures, iteration loop
- [porting](modules/porting.md) — Detailed per-rung steps for new chip port
- [reference](modules/reference.md) — Common patterns, script reference, peripheral specs
