# Examples

## Full build-flash-test loop after driver changes

```bash
vsf-bench --all vsf.demo/board/rp2040/hardware-map.yml
```

## Pipeline failure handling

- **Build fails:** fix source/config, then retry
- **Build succeeds but program fails:** report error and stop — do not proceed to test
- **Program succeeds but test fails:** capture verbose output, then invoke `Skill("vsf-hal-driver")` for driver debugging

## Debugging a failing peripheral with IO verification first

```bash
vsf-bench --all vsf.demo/board/<board>/hardware-map.yml --suite gpio_io_check
```

## Running a single test suite during driver development

```bash
vsf-bench --all vsf.demo/board/<board>/hardware-map.yml --suite usart_baud
```

## BT pairing stress test pipeline

```yaml
# hardware-map.yml
pipelines:
  bt_stress:
    description: "BT 配对压力测试"
    steps:
      - build: application-standalone
      - program: application-standalone
      - power_cycle
      - wait_for:
          expect:
            - pattern: "bt_stack_ready"
              verdict: pass
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

# 调试：跑 5 次，拉长配对等待
vsf-bench --pipeline bt_stress --board b1 --repeat 5 --set pair_wait.delay=3.0
```

## Build + flash only (no test)

```yaml
pipelines:
  full_flash:
    steps:
      - build: bootloader
      - program: bootloader
      - power_cycle
      - wait_for:
          expect:
            - pattern: "entering DFU"
              verdict: pass
          timeout: 5
      - build: application-standalone
      - program: application-standalone
```

```bash
vsf-bench --pipeline full_flash --board b1
```

## IO trigger test (no build/flash)

```yaml
pipelines:
  io_trigger_test:
    steps:
      - power_cycle
      - delay: 1.0
      - gpio_set: { level: low, duration: 0.1 }
      - wait_for:
          expect:
            - pattern: "IRQ triggered"
              verdict: pass
          timeout: 5
```
