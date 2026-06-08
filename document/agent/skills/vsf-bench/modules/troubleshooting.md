# Troubleshooting

| Symptom | Likely cause | Action |
|---------|-------------|--------|
| Build fails | cmake or SDK not installed, wrong `build.source_dir` | verify build.source_dir path; check cmake/SDK installation |
| Program fails | board not connected, wrong runner selected | check USB cable; verify runner config in hardware-map.yml |
| Test timeout | firmware not outputting expected pattern, baud rate mismatch | verify serial.port and baudrate in hardware-map.yml match firmware |
| `Suite not found` | suite disabled in firmware config | check `VSF_USE_TEST_<SUITE>` is ENABLED; verify suite name spelling |
| No suites listed | test framework not enabled | verify `VSF_USE_TEST = ENABLED` in `vsf_usr_cfg.h` |
| LA (logic analyzer) errors | dsview-cli not in PATH, or channel name mismatch | ensure dsview-cli is in PATH; verify `logic_analyzer.channels` |
| Script fails with `suite not found` | script registered under wrong suite name | check script filename matches YAML `script:` field |

## Pipeline step failures

| Symptom | Likely cause | Action |
|---------|-------------|--------|
| Pipeline wait_for timeout | pattern not matched in serial output within timeout | check `run.log` for firmware output; increase timeout or adjust regex |
| Pipeline gpio_set fails | FT232H not connected or pyftdi missing | check USB; `pip install pyftdi`; verify pin number and port |
| Pipeline la_start fails | DSLogic not connected or dsview-cli not found | check USB; verify dsview-cli in PATH |
| Pipeline run exits non-zero | shell command returned error code | check script path; verify `$RUN_DIR` is set |
| Pipeline serial_send no effect | serial port not open or firmware not listening | verify `serial_send` appears before `wait_for` in steps |
| Pipeline --set has no effect | id not found or key path wrong | verify step `id` exactly matches; check `--set id.key=value` syntax |
| Pipeline --repeat ignored | no `loop` block in pipeline | add a `loop:` block to pipeline steps |

## Isolating regression failures

A suite that fails during `--all` but passes when run alone indicates **state leakage** from a previous suite, not a driver bug.

**Procedure:**

1. Note the first failing suite in the full run (e.g. `pwm_basic`).
2. Reflash to reset the device:
   ```bash
   vsf-bench --program --hardware-map board/hardware-map.yml --board <board>
   ```
3. Run only that suite:
   ```bash
   vsf-bench --test --hardware-map board/hardware-map.yml --board <board> --suite pwm_basic
   ```
4. **If it passes:** the bug is in the suite *before* it (state cleanup). Use `vsf-hal-driver` skill to fix the preceding suite's teardown.
5. **If it still fails:** it is a genuine driver bug. Use `vsf-hal-driver` skill with the single-suite log.
