# I2C

Template in `template/__series_name_a__/common/i2c/`. Copy, rename placeholders, implement APIs via register access. Common inc: `i2c_template.inc`, `i2c_interface.c`, `i2c_common.c`.

Reference: `driver/ST/STM32H7RSXX/common/` for multi-peripheral working examples.

## Mode bits (vsf_i2c_mode_t)

| Field | Bits | Mandatory |
|-------|------|-----------|
| Clock source | 0-1 | — |
| Clock ratio | 2-6 | — |
| Address mode | 7 | 7_BITS, 10_BITS |

## IRQ mask (vsf_i2c_irq_mask_t)

MASTER_TRANSFER_COMPLETE(0), MASTER_ADDRESS_NACK(1), MASTER_START_OR_RESTART_DETECT(2) — template defaults.

## API

Core: init, fini, enable, disable, capability, get_configuration.
IRQ: irq_enable/disable/clear, status.
Master FIFO: master_fifo_transfer, master_request, master_get_transferred_count.
Slave FIFO: slave_fifo_transfer, slave_request, slave_get_transferred_count.
Control: ctrl.

## Pin wiring verification (pre-driver)

Before writing the hardware I2C driver, verify the physical wiring with a **GPIO bit-bang scan**. This catches the most common board-level mistake — SCL/SDA swapped — without debugging a non-working hardware driver.

### Why do this first

- A non-responsive I2C slave can be: swapped wires, missing pull-ups, unpowered slave, wrong address, or a driver bug.
- GPIO bit-bang gives you deterministic, single-step control over every edge. You can see exactly where the bus stalls.
- If the scan works but the hardware driver doesn't, you have isolated the bug to the driver.

### How to scan

VSF provides a ready-made test scenario `i2c_bus_scan` in `test/vsf_test_suite/hal/driver/i2c/suite/vsf_test_i2c_bus_scan.c`. It uses the `vsf_gpio_i2c` software-I2C module (no hardware I2C controller needed) to scan the bus and automatically retries with swapped SCL/SDA if the first attempt finds nothing.

**Board setup:** For each I2C bus you want to verify, declare a `describe_gpio_i2c` instance in your board file and wire it into `vsf_board_t` (e.g. `gpio_i2c0` for the first bus, `gpio_i2c1` for the second). Set the `port` field to your GPIO bank before the test runs.

**Run:**
```
vsf-test run i2c_bus_scan
```

**What you should see:**
- If wiring is correct: `Devices found: N` (N > 0), test passes.
- If SCL/SDA are swapped: scan finds nothing on first attempt, retries with swapped pins, reports `[I2C] SCL/SDA appear swapped.`, test passes.
- If no device at all: `No device found on either pin order. Check pull-ups and power.`, test fails.

After the scan, the scenario restores pins to their hardware I2C alternate function so the real driver can take over.

### Post-scan cleanup

`vsf_gpio_i2c_init()` reconfigures the pins to GPIO open-drain + pull-up. The `i2c_bus_scan` scenario restores them to the hardware I2C alternate function after scanning:

```c
vsf_gpio_port_config_pins(gpio, (1u << scl_pin) | (1u << sda_pin),
    &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_AF,
        .alternate_function = <chip-specific I2C FUNCSEL>,
    });
```