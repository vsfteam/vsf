# Examples

## Full build-flash-test loop after driver changes

```bash
vsf-bench --all vsf.demo/board/rp2040/hardware-map.yml
```

## Pipeline failure handling

- **Build fails:** fix source/config, then retry
- **Build succeeds but flash fails:** report error and stop — do not proceed to test
- **Flash succeeds but test fails:** capture verbose output with `vsf-bench --all ... --verbose`, then invoke `Skill("vsf-hal-driver")` for driver debugging

## Debugging a failing peripheral with IO verification first

```bash
# Before debugging any peripheral, rule out wiring issues:
vsf-bench --all vsf.demo/board/<board>/hardware-map.yml --suite gpio_io_check
# If gpio_io_check passes but usart_baud fails, the issue is in the driver, not wiring.
```

## Running a single test scenario during driver development

```bash
vsf-bench --all vsf.demo/board/<board>/hardware-map.yml --suite usart_baud
```
