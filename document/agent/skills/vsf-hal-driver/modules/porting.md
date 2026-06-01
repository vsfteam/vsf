# Porting a New Chip — Step-by-Step Ladder

A linear path from blank vendor SDK to fully-tested VSF HAL on a new chip. Each rung has a concrete pass criterion; do not advance until the rung's verification is observed. References to mechanical detail point to `REFERENCE.md`.

Read sequentially. Do not skim — verification gates exist because every later rung assumes earlier rungs hold.

---

## R0 — Vendor serial I/O

**Goal.** Get the chip's vendor SDK to send and receive bytes on a UART pin pair you can wire to your host.

**Prerequisites.** None.

**How to achieve.**

1. Build and flash the vendor SDK's UART example (often `hello_uart`, `uart_echo`, or similar).
2. Wire the chip's TX/RX pins to a USB-serial adapter on the host (typically GP0/GP1 for RP2040-class chips; see vendor reference for your chip).
3. Open the host serial port at the example's baudrate. Confirm the example's banner prints.
4. If the example only does TX, modify it to echo back received bytes — you need bidirectional flow for later rungs.

**Verification.** Host sends one printable byte. Chip echoes that byte back. Pass = byte returns within ~100 ms.

**Unlocks.** Hardware is alive. UART pins are wired correctly. Clocks at vendor defaults work.

**Pitfalls.** A common failure here is wrong UART instance (chip has 2; pinmux selects the other one) or wrong baudrate divisor formula. If the vendor example itself fails, stop and fix the vendor SDK before moving on. Do not bring VSF code into a non-working board.

---

## R1 — VSF skeleton compiles

**Goal.** Generate the chip's VSF directory tree from the template, set up `device.h` per-instance macros, and add system clock + watchdog tick initialization in `driver.c`.

**Prerequisites.** R0.

**How to achieve.**

1. Run `scaffold/chip.py --driver-dir source/hal/driver --config chip.yaml` to copy the template skeleton into `source/hal/driver/<Vendor>/<Chip>/`.
2. Write a YAML instance map and run `scaffold/macros.py --in-place source/hal/driver/<Vendor>/<Chip>/device.h instances.yaml`. Per REFERENCE.md "Per-instance parameterization in device.h", the generated macros are `VSF_HW_<PERIPH>_COUNT` and `VSF_HW_<PERIPH><N>_REG / _IRQN / _IRQHandler` for every instance.
3. In `driver.c`'s `vsf_driver_init()`, port the vendor SDK's clock setup (PLL bring-up, peripheral clock gates). If your chip has a watchdog tick required for the system timer (see RP2040 example), enable it here.
4. Add the chip's CMake fragment so `vsf_demo.elf` links.

**Verification.** `cmake --build` succeeds. The resulting firmware boots and `vsf_driver_init()` returns `true` (verify with a single `printf` immediately after).

**Unlocks.** VSF compilation works. Clock tree is configured. You can now add peripheral drivers one at a time.

**Pitfalls.** Forgetting the watchdog tick (or its equivalent on your chip) breaks the system timer silently — `vsf_systimer_get_us()` returns 0 or drifts. If your chip's clock setup is unfamiliar, R4 will catch the symptom; finding it later is cheap because the bug is contained to `driver.c`.

---

## R2 — Test framework attach

**Goal.** Wire the `vsf-test` shell to the R0 serial path so you can drive scenarios from the host.

**Prerequisites.** R1.

**How to achieve.**

1. In `board/<your_board>/vsf_board.c`, route the debug stream output to the UART you wired in R0. Use the same UART instance.
2. Enable `VSF_USE_TEST = ENABLED` in `vsf_usr_cfg.h`.
3. Flash. Connect host serial at 115200.

**Verification.** Send `vsf-test scene --list` over the host serial. Firmware responds with a (possibly empty) scene list and a `>` prompt. Pass = the response arrives, even if no scenes are registered.

**Unlocks.** You can now use `vsf-bench` and `vsf-bench-test` against the new board. Every later rung's verification is run through this channel.

**Substitutions.** If your chip lacks UART entirely (USB-CDC only), wire the debug stream to the USB-CDC endpoint instead. The shell protocol is byte-oriented and indifferent to transport.

---

## R2.5 — IO wiring verification

**Goal.** Confirm the physical pins on the board are wired correctly before spending time debugging driver register logic.

**Prerequisites.** R2.

**How to achieve.**

1. Run the `gpio_io_check` suite. This drives each test pin high and low and (optionally) verifies the level with a logic analyzer or loopback.
2. If any pin fails to toggle, fix the hardware (broken trace, wrong pin in hardware-map.yml, missing solder bridge) before proceeding.

```bash
vsf-bench --all board/<board>/hardware-map.yml --suite gpio_io_check
```

**Verification.** All pins declared in the hardware-map fixture toggle and are read back at the expected level. Pass = no mismatches.

**Unlocks.** You can trust the physical layer for all later peripherals. A failing UART or SPI test at R3a/R5 is almost certainly a driver bug, not a wiring problem.

**Pitfalls.** Skipping this step and jumping straight to UART debugging is common — but a swapped TX/RX pair or a missing loopback jumper produces exactly the same symptoms as a broken baudrate divisor. `gpio_io_check` separates wiring bugs from code bugs in under 10 seconds.

---

## R3a — First VSF UART driver

**Goal.** A second UART instance (or the same one in VSF mode) passes the `usart_baud` scenario.

**Prerequisites.** R2.

**How to achieve.**

1. Run `scaffold/peripheral.py --driver-dir source/hal/driver --chip <Vendor>/<Chip> --periph uart` to copy the template into your chip's tree. See REFERENCE.md "Style migration".
2. Replace template placeholders with your chip's IP details (PL011, DW APB UART, USART, etc.). Use `VSF_MCONNECT` for instance prefixing — never write `vsf_hw_uart0` directly in the `.c` file.
3. Add `IMP_LV0` invocations for each UART instance. The struct's per-instance fields (`reg`, `irq`, `rst_bit`, etc.) come from the macros you defined in R1.
4. Add the UART pinmux to `board/<your_board>/vsf_board.c`. Use `vsf_gpio_port_config_pins()` — **not** raw vendor register writes. See [[pico-board-init-cleanup]] for the canonical pattern.
5. Run `check/structure.py --periph usart --side header <your_uart.h>`, `check/structure.py --periph usart --side source <your_uart.c>`, and `check/quality.py <your_uart.c>`. All three must exit 0 (or 2 with only known-acceptable warnings) before moving on.

**Verification.** `vsf-bench-test <hardware-map.yml> --suite usart_baud` reports all cases pass.

**Unlocks.** R3b can run, or you can advance straight to R4. Other peripherals can follow this template.

**Pitfalls.** Reset deassert, clock gate, NVIC enable — all three must happen in `init()`. Missing any one produces a driver that compiles and looks correct but never produces a TX edge. See REFERENCE.md "Driver init() responsibilities".

---

## R3b — First VSF GPIO driver

**Goal.** A GPIO pin can be configured, set, cleared, and read; a logic analyzer confirms the edges.

**Prerequisites.** R2.

**How to achieve.**

1. Run `scaffold/peripheral.py --driver-dir source/hal/driver --chip <Vendor>/<Chip> --periph gpio` to copy the template into your chip's tree.
2. Implement set, clear, read, and `port_config_pins`. The `alternate_function` field of `vsf_gpio_cfg_t` writes the chip's pin-function selector.
3. Add the GPIO instance via `IMP_LV0`. Per-port base addresses go in `device.h`.
4. Run `check/structure.py --periph gpio --side header` and `check/structure.py --periph gpio --side source` on the generated files. Exit 0 (or 2 with known-acceptable warnings) before moving on.

**Verification.** Wire a GPIO pin to a logic analyzer channel. Run `vsf-bench-test --suite gpio_toggle`. LA confirms the configured-rate toggle.

**Unlocks.** Peripherals that need pin alternate-function selection (UART, I2C, SPI, PWM) can now be wired through the GPIO HAL. R3a's pinmux step depends on this.

**Substitutions.** If R3a is already complete with pinmux via raw register writes, R3b lets you retire those writes in favor of `vsf_gpio_port_config_pins()` — the canonical pattern.

---

## R4 — System clock verified end-to-end

**Goal.** Confirm the system timer and CPU clock are configured at the expected rate by measuring an externally observable timing.

**Prerequisites.** R3a or R3b (whichever produces a measurable edge).

**How to achieve.**

1. Build a scenario that toggles a GPIO at a known systimer-derived rate (e.g., 100 ms intervals). The `gpio_systimer_health` scenario in [[systimer-health-check]] is the canonical implementation.
2. Wire the GPIO to the logic analyzer.
3. Run the scenario through `vsf-bench-test`.

**Verification.** Logic analyzer measures the actual edge interval. Every measured gap falls within ±5% of 100 ms. Pass = all 10 (or more) gaps within tolerance.

**Unlocks.** All time-sensitive peripherals can be trusted: UART baudrate, PWM frequency, SPI clock, DMA throughput. Without this rung, a working UART may be working at the wrong baudrate (which "just" produces silent corruption you'll chase for hours).

**Pitfalls.** A timer running at 10 kHz instead of 1 MHz produces gaps 100× longer than expected — symptom is unambiguous. A timer running at 10× nominal (e.g., 100 kHz when expecting 10 kHz) is more subtle; the LA tolerance check catches it as out-of-band.

---

## R5 — Routine peripheral expansion

**Goal.** Add each remaining peripheral by template copy, one at a time. Each peripheral is its own mini-port.

**Prerequisites.** R4.

**How to achieve.** For each peripheral (I2C, SPI, ADC, PWM, DMA, RTC, Flash, WDT, Timer):

1. Read `peripherals/<periph>.md` in this skill.
2. Run `scaffold/peripheral.py --driver-dir source/hal/driver --chip <Vendor>/<Chip> --periph <periph>` to copy the matching template directory.
3. If the peripheral requires new instance macros in `device.h`, write a YAML instance map and run `scaffold/macros.py --in-place device.h instances.yaml`.
4. Fill in IP-specific register manipulation. Use `IMP_LV0` and `VSF_MCONNECT` per R3a.
5. Add the peripheral's pinmux to `vsf_board.c` if it needs alternate-function pins.
6. Run `util/enable.py --enable <periph> <vsf_usr_cfg.h>` to enable the peripheral.
7. Run `check/structure.py --periph <periph> --side header`, `check/structure.py --periph <periph> --side source`, and `check/quality.py`. All three must exit 0 (or 2 with only known-acceptable warnings).
8. Run `check/audit.py --chip <Vendor>/<Chip>` to verify cross-file consistency. Must exit 0 (or 2 with only known-acceptable warnings).
9. Run the peripheral's `vsf-bench-test` scenario.

**Verification.** Each peripheral's scenario passes.

**Unlocks.** Full peripheral suite. Port is complete.

**Pitfalls.** Order matters: peripherals with strict timing (UART baudrate, PWM frequency) must be ported after R4. Peripherals with optional features (DMA, IRQ) can land in stages — implement the polling path first, add IRQ in a second PR.

---

## Reading order for the rest of the skill

After a successful R5, the supporting documents are:

- `SKILL.md` — the operations cheat sheet for ongoing work
- `REFERENCE.md` — mechanical recipes referenced from each rung
- `peripherals/<periph>.md` — per-peripheral specifics

Each rung above is one screen. If you find yourself reading further down before achieving the current rung's verification, stop and finish the current rung first.
