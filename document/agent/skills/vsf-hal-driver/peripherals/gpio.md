# GPIO

Always Direct mode. Template: `template/.../gpio/gpio.{h,c}`. Common inc: `gpio_template.inc`.

## API (VSF_GPIO_APIS)

mandatory: init, fini, capability, set/clear, read, config_pins, get_pin_configuration, set_input/output, exti_irq_enable/disable/clear.
optional: toggle, output_and_set/clear, switch_direction, read_output_register.

## Capability macros

Define these **before** including `gpio_template.inc` to control optional API support:

| Macro | Effect | Default |
|---|---|---|
| `VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_SET` | Enable `output_and_set` API | 0 (asserts if called) |
| `VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR` | Enable `output_and_clear` API | 0 (asserts if called) |
| `VSF_GPIO_CFG_CHANGE_DIR_FIRST` | Direction-change order: `ENABLED` = set_output then clear; `DISABLED` = clear then set_output | ENABLED |

```c
#define VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_SET      1
#define VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR    1
#define VSF_GPIO_CFG_CHANGE_DIR_FIRST                       DISABLED
#define VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY             ENABLED
#include "hal/driver/common/gpio/gpio_template.inc"
```

Reference: `driver/GigaDevice/GD32H7XX/common/gpio/gpio.c`.

## Design tips

- **Verify register reset defaults.** Picking a wrong base value for pin configuration registers can cause subtle bugs (e.g. bus-keep vs pull-up). Read the datasheet carefully for reset values.
- **Check output-disable bits.** Setting a global output-disable flag (like `PADS.OD`) for input pins can block later atomic output transitions. Prefer per-pin control via the output-enable register.
- **Self-loopback.** If the chip supports reading output level while driving, set `capability.can_read_in_gpio_output_mode=1`. Enables same-pin testing without external wiring.
- **OD emulation.** If HW lacks open-drain mode, emulate: track OD pins in driver-side mask, keep output register at 0, toggle output-enable to drive/float.
- **`get_pin_configuration`** should read real HW registers and re-derive the configured mode — not return template defaults.
- **AF detection without reimplementing mode enum:** check mode base OR `alternate_function != 0`.
- **EXTI: level vs edge.** Level bits auto-track HW status (read-only). Edge bits are write-1-clear. Never clear level bits in the clear function.
- **Single-port IMP_LV0** can hardcode instance name.

## IO wiring verification (`gpio_io_check`)

The `gpio_io_check` suite is a wiring-first diagnostic. Before debugging any peripheral test failure, run it to confirm the physical pins declared in `hardware-map.yml` are actually toggling:

```bash
vsf-bench --all board/<board>/hardware-map.yml --suite gpio_io_check
```

This catches common hardware problems (swapped TX/RX, missing loopback, broken traces) that produce the same symptoms as driver bugs.

## Reference

- RP2040: `driver/RaspberryPi/RP2040/gpio/gpio.c`
