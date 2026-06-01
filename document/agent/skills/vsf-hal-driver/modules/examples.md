# Examples

## Silent peripheral — init() compiles but no I/O on any pin

**Symptom:** `vsf_hw_<periph>_init()` returns `VSF_ERR_NONE` and the firmware boots, but the peripheral produces no output (TX edge, clock, data — nothing). Logic analyzer shows pins staying at idle level.

**Diagnosis:** Check `init()` for all three required steps:
- Reset deassert: `reset_hw->reset &= ~rst_bit;` (deassert the peripheral reset line)
- Clock gate: `clock_hw->enable |= clk_bit;` (enable the peripheral clock)
- IRQ enable: `NVIC_EnableIRQ(irqn);` and set priority from `cfg_ptr->prio` if configurable

Missing any one = no I/O. This is the most common LV0 driver bug across all peripherals.

## Adding a new peripheral to an existing chip port

Follow the "Add peripheral" flow in Quickstart. This example focuses on what can go wrong and how to catch it early:

- **Step 1-2 (scaffold/implement):** verify the template copied into `source/hal/driver/<Vendor>/<Chip>/<periph>/`. If the directory already exists, scaffold fails — edit directly.
- **Step 5 (static checks):** structure check catches missing API functions; quality check catches convention violations. Fix these before audit.
- **Step 6 (audit):** cross-file check catches IRQ handler declared in `device.h` but not defined in `.c`, or vice versa.
- **Step 7 (vsf-bench):** only run after all static checks + audit pass. If vsf-bench fails, re-run `gpio_io_check` to rule out wiring before suspecting driver logic.

## New chip port — common pitfalls per rung

Follow the 6-rung ladder in Quickstart. Typical failures:
- R1: forgetting watchdog tick or PLL config → `vsf_systimer_get_us()` returns 0 or drifts
- R2: debug stream routed to wrong serial instance → no shell prompt
- R3: pinmux via raw vendor registers instead of `vsf_port_config_pins()` → driver works only by accident
- R4: timer running at wrong frequency → 100× timing errors caught by LA tolerance check

## Unsupported config silently accepted — init() returns NONE but feature never works

**Symptom:** `init()` returns `VSF_ERR_NONE`, the application later waits for a callback or event that never arrives. The user files a bug: "IRQ never fires" or "DMA never completes." Root cause is the hardware does not support the feature, but `init()` accepted the configuration silently.

**Fix:** In `init()`, validate every field that expresses a feature the hardware might not support. If the field requests an unsupported capability, return `VSF_ERR_NOT_SUPPORT` immediately. Example pattern:

```c
/* Hardware has no interrupt line for this peripheral.
 * If user requests interrupt mode, reject at init(). */
if (cfg_ptr->isr.handler_fn != NULL) {
    return VSF_ERR_NOT_SUPPORT;
}
```

Applies to any peripheral where the chip lacks an IRQ line, DMA channel, configurable priority, or other optional feature that the VSF API allows the user to request.

## Unused config struct fields must be documented

**Symptom:** `vsf_<periph>_cfg_t` has a field (e.g., `prio`) that the chip hardware cannot configure. The `init()` function stores the whole struct but never reads that field — with no comment explaining why. A future maintainer cannot tell whether the omission is a driver bug or intentional.

**Fix:** Document each unused field directly above the struct store in `init()`:
```c
// cfg_ptr->prio intentionally unused: <chip> IRQ priority is fixed in hardware
dma_ptr->cfg = *cfg_ptr;
```
Applies to every peripheral type. For every config field: either use it in the driver, or document why the chip hardware doesn't support it.

## Mode bits translated via if/else instead of direct register mapping

**Symptom:** `init()` has a chain of `if (mode & MODE_X) { reg |= BIT; }` to translate VSF mode bits into hardware register bits. The code works but is verbose and brittle — every new mode requires another branch.

**Diagnosis:** Look for `if` blocks inside `init()` that test `cfg_ptr->mode` against individual `VSF_*_MODE_*` constants and then OR bits into a register variable. Example (PL022 SPI):
```c
/* Before — translation via if/else */
if (cfg_ptr->mode & VSF_SPI_MODE_2) { cr0 |= (1u << 6); }  /* CPOL */
if (cfg_ptr->mode & VSF_SPI_MODE_1) { cr0 |= (1u << 7); }  /* CPHA */
```

**Fix:** Redefine the mode enum in the chip-specific `.h` so that mode values **directly encode** the hardware register bits. Then extract them with a single mask operation in `init()`. Example:

```c
/* spi.h — redefine VSF_SPI_CPOL/CPHA to match PL022 CR0 bits [7:6] */
#define VSF_SPI_CFG_REIMPLEMENT_TYPE_MODE   ENABLED
typedef enum vsf_spi_mode_t {
    /* ... other modes unchanged ... */
    VSF_SPI_CPOL_LOW  = 0x00,
    VSF_SPI_CPOL_HIGH = (1u << 6),   /* PL022 SPO */
    VSF_SPI_CPHA_LOW  = 0x00,
    VSF_SPI_CPHA_HIGH = (1u << 7),   /* PL022 SPH */
    VSF_SPI_MODE_0 = VSF_SPI_CPOL_LOW  | VSF_SPI_CPHA_LOW,
    VSF_SPI_MODE_1 = VSF_SPI_CPOL_LOW  | VSF_SPI_CPHA_HIGH,
    VSF_SPI_MODE_2 = VSF_SPI_CPOL_HIGH | VSF_SPI_CPHA_LOW,
    VSF_SPI_MODE_3 = VSF_SPI_CPOL_HIGH | VSF_SPI_CPHA_HIGH,
    /* ... */
} vsf_spi_mode_t;
```

```c
/* spi.c — single-line extraction, no if/else */
cr0 |= (cfg_ptr->mode & ((1u << 6) | (1u << 7)));
```

Applies to any peripheral where VSF config bits can be aligned with hardware register fields: GPIO MODER/PUPDR, USART baud/mode, I2C speed, SPI frame format, etc. When hardware layout prevents perfect alignment, minimize conversion to the unavoidable cases only. See convention 8.
