# Error Handling and Troubleshooting

## Script failures

| Failure | Cause | Fix |
|---------|-------|-----|
| `scaffold/peripheral.py` fails | wrong `--chip` path or target dir already exists | verify path under `source/hal/driver/`; if dir exists, edit directly |
| `check/skeleton.py` non-zero | function signature changed, macro renamed, or struct declaration removed from template | restore the original template skeleton; implementation should go inside the functions, not alter signatures |
| `check/structure.py` non-zero | missing required API, wrong prototype, or missing IMP_LV0 | read check output; add missing function/struct; rerun |
| `check/quality.py` non-zero | style or convention violation | fix the violation; only suppress with `// quality: allow-<rule-id>` after confirming it's a false positive |
| `check/audit.py` non-zero | cross-file mismatch (e.g., IRQ handler declared but not defined) or `missing-mask` on GPIO | fix mismatch; for GPIO `missing-mask` see note below — GPIO uses `VSF_HW_GPIO_PORT_MASK` not `VSF_HW_GPIO_MASK` |
| `util/enable.py` fails | peripheral name typo or `vsf_usr_cfg.h` not at expected path | check peripheral name against `peripheral-registry.yml` |

## Runtime failures

| Symptom | Likely cause | Action |
|---------|-------------|--------|
| Boot hang / no shell prompt | `vsf_driver_init()` crash — clock setup or NULL deref | add printf after each init step; check PLL lock |
| Compiles, no I/O on any pin | system timer not running (watchdog tick missing) | verify `watchdog_hw->tick = N \| WATCHDOG_TICK_ENABLE_BITS` in `driver.c` |
| Single peripheral: compiles, no I/O | init() missing reset, clock, or IRQ step | check all three (see Example: Silent peripheral) |
| IRQ never fires | NVIC enable missing in init(), or IRQ handler name doesn't match IMP_LV0 expansion | verify `NVIC_EnableIRQ()` called; check handler name against generated macro |
| DMA transfer never completes | DMA clock not enabled, or channel not assigned to peripheral | check `RCC->AHBENR` DMA clock bit; verify channel mapping in reference manual |
| vsf-bench fails, all static checks passed | wiring issue or wrong baudrate | run `gpio_io_check` suite first to isolate wiring; check R4 system clock timing |
| Peripheral works intermittently | missing `volatile` on register pointers, or spin-wait missing `< X us` comment (compiler optimizes away delay) | add volatile; add duration comment |

## Audit `missing-mask` — expected behavior

`check/audit.py` checks that every declared peripheral has either `VSF_HW_<PERIPH>_MASK` or `VSF_HW_<PERIPH>_COUNT` in `device.h` (templates derive one from the other). **GPIO is special:** it uses `VSF_HW_GPIO_PORT_MASK` / `VSF_HW_GPIO_PORT_COUNT`, and `vsf_template_gpio.h` defines `VSF_HW_GPIO_MASK` from `VSF_HW_GPIO_PORT_MASK`. If you see `[missing-mask] gpio` it means `VSF_HW_GPIO_PORT_MASK` or `VSF_HW_GPIO_PORT_COUNT` is missing — not `VSF_HW_GPIO_MASK`.

## Iteration loop

```
edit .c/.h → structure check → quality check → audit → vsf-bench
                                                          ↑
                                          └── fix ────────┘
```

Stop when vsf-bench passes. If static checks keep failing after several iterations, pause and re-read `conventions.md` — repeated failures usually mean a structural rule is being violated, not a typo. If the same failure persists across 5+ iterations: stop and tell the user — the issue is likely a tooling bug, an undocumented hardware quirk, or a misunderstanding of the convention rules that needs human clarification.

## When tools or documents are unavailable

- `vsf-bench` not installed: `pip install -e vsf.demo/vsf/test/vsf_bench`
- `scripts/` not on PATH: invoke with full path under `vsf.demo/vsf/document/agent/skills/vsf-hal-driver/scripts/`
- `PORTING.md` / `REFERENCE.md` / `conventions.md` not accessible: all are in the same directory as this SKILL.md
- `peripheral-registry.yml` missing: read `scripts/check-specs/<periph>.yml` for per-peripheral API specs
- **No hardware available:** static checks (structure + quality + audit) can still verify correctness. If all exit 0 or 2, the code is structurally sound — flag to user that hardware testing is pending. This skill hands off to vsf-bench: use `Skill("vsf-bench")` to invoke it, or tell the user to run `vsf-bench --all hardware-map.yml --suite <periph>_<scenario>`.
- Undocumented vendor registers: flag to user; this skill cannot authoritatively infer NDA-only register behavior
