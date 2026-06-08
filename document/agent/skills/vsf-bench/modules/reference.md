# vsf-bench Reference

## CLI Interface

Tool name: **`vsf-bench`**

```bash
# Full pipeline: build + program + test all suites
vsf-bench --all --hardware-map board/hardware-map.yml

# Run specific suite (all cases)
vsf-bench --all --hardware-map board/hardware-map.yml --suite usart_baud

# Run specific case by parameter value
vsf-bench --all --hardware-map board/hardware-map.yml --suite usart_baud --case 921600

# Run specific case by index (fallback)
vsf-bench --all --hardware-map board/hardware-map.yml --suite usart_baud --case-index 7

# Run multiple suites
vsf-bench --all --hardware-map board/hardware-map.yml --suite usart_baud --suite usart_mode

# Individual steps
vsf-bench --build  --hardware-map board/hardware-map.yml
vsf-bench --program  --hardware-map board/hardware-map.yml --board b1
vsf-bench --test     --hardware-map board/hardware-map.yml --board b1

# Pipeline (the one and only orchestration concept)
vsf-bench --pipeline uart_flash_regression --board b1 --hardware-map board/hardware-map.yml

# Pipeline with overrides
vsf-bench --pipeline bt_stress --board b1 --repeat 1000 --set pair_wait.delay=3.0
```

## Pipeline: the unified orchestration model

Pipeline is the **only** concept. Every step type is equal — no "deployment" vs "test" distinction.

### Step Primitives

| Primitive | YAML | Parameters |
|-----------|------|-----------|
| `build` | `- build: <project>` | project name (from hardware-map.yml) |
| `program` | `- program: <project>` | project name |
| `power_cycle` | `- power_cycle` | — |
| `power_off` | `- power_off` | — |
| `power_on` | `- power_on` | — |
| `delay` | `- delay: 1.0` | float (seconds) |
| `serial_send` | `- serial_send: "cmd\r\n"` | string (sent over serial to firmware) |
| `wait_for` | `- wait_for: { expect: [...], timeout: 10 }` | `expect: [{pattern: regex, verdict: pass\|fail}], timeout: float` |
| `gpio_set` | `- gpio_set: { level: low, duration: 0.5 }` | `level: high\|low`, `duration?: float` |
| `la_start` | `- la_start: {}` | `channel?: str` (inherits board.la) |
| `la_stop` | `- la_stop: {}` | — |
| `la_decode` | `- la_decode: { baudrate: 2000000 }` | `baudrate: int`, `channel?: str` |
| `run` | `- run: "python analyze.py $RUN_DIR/..."` | string (shell cmd; `$RUN_DIR` env var set) |
| `loop` | `- loop: { repeat: 100, steps: [...] }` | `repeat: int`, `steps: [step]` |

### Step `id` and `--set` overrides

Any step can have an optional `id`:

```yaml
steps:
  - id: boot_wait
    delay: 1.0
  - id: trigger_pair
    gpio_set:
      level: low
      duration: 0.5
```

```bash
# Global override: all matching keys
vsf-bench --pipeline bt_stress --board b1 --set delay=2.0

# ID-targeted override
vsf-bench --pipeline bt_stress --board b1 --set pair_wait.delay=3.0
```

### `--repeat` override

```bash
# Override all loop.repeat values
vsf-bench --pipeline bt_stress --board b1 --repeat 5
```

### wait_for semantics

- `expect` list is ordered — **first matching pattern wins**
- `verdict: pass` → step succeeds, continue
- `verdict: fail` → step fails, pipeline stops
- `timeout` with no match → fail

### LA parameter inheritance

- `la_start` / `la_decode`: device, samplerate, channels inherit from `board.logic_analyzer`
- Step params only override differences
- `la_decode.baudrate` must be explicitly declared

### GPIO

- References `hardware-map.yml` `gpio_adapters` section
- `adapter` must match a key in `gpio_adapters`

## Output Path Convention

```
logs/<YYYYMMDD>-<HHMMSS>-<board>-<pipeline>/
  ├── run.log              ← TeeLogger combined log (host + firmware)
  ├── test-events.jsonl    ← Test suite audit + verdict
  ├── test-report.junit.xml ← JUnit XML
  ├── la-CH8.dsl           ← LA capture (non-loop)
  ├── la-serial.log        ← LA serial output
  ├── la-decode-CH8-<baud>.csv ← UART decode
  └── runs/                ← Loop iteration subdirectories
      ├── 01/
      │   ├── la-CH8.dsl
      │   ├── la-serial.log
      │   └── la-decode-CH8-<baud>.csv
      ├── 02/
      │   └── ...
      └── ...
```

- `logs/` is hardcoded — no `--log-dir`, no `log_dir` in config
- Directory name is self-describing: timestamp + board + pipeline
- Files prefixed by category: `test-`, `la-`, `run.`

## Orchestrator Behavior

```
build → program → for each suite: send trigger → run script
```

Trigger commands sent to firmware:
- No `--suite` → `vsf-test run all` (filtered to firmware-known suites)
- `--suite usart_baud` → `vsf-test run usart_baud`
- `--suite usart_baud --case 921600` → `vsf-test run usart_baud.7` (resolved from YAML)

## Script Behavior

```python
def run(serial: SerialInstrument, test_params_yml: str) -> None:
    params = load_test_params(test_params_yml)
    serial.expect_test_summary("gpio_toggle")
```

For scripts that need LA decode:

```python
def run(serial: SerialInstrument, la: LogicAnalyzer | None = None) -> None:
    serial.expect_test_summary("usart_baud", timeout=120.0)

def decode(la: LogicAnalyzer,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None) -> None:
    # decode markers, assert results...
```

## SerialInstrument API

`send(data)` — writes to serial, logs to audit log.

`expect(pattern, timeout=5.0)` — reads until regex matches accumulated buffer, returns matched line. Raises `TimeoutError` on timeout.

`expect_any(patterns: list[tuple[str, str]], timeout: float)` — reads until any pattern matches. Returns `(index, matched_line)`. `patterns[i] = (regex, verdict)` where verdict is `"pass"` or `"fail"`. Raises `TimeoutError` on timeout.

`read_all(timeout=2.0)` — reads until no new data arrives for `timeout` seconds.

`expect_test_summary(name, timeout=30.0)` — waits for test completion, parses Pass/Fail/Skip summary.

## Logic Analyzer

LA is auto-detected: if a script defines `decode()`, the orchestrator treats it as LA-needing.

Capture modes (`--la-mode`):
- `shared` (default): one continuous capture for the entire run
- `per-suite`: one capture per suite

Hardware config: `board.logic_analyzer` in hardware-map.yml.

### Installing dsview-cli

Download from https://github.com/liangyongxiang/DSView/releases, extract to `%LOCALAPPDATA%\DSView\`, add directory to `PATH`.
