# VSF HAL Driver Reference

## Common patterns

### Per-instance parameterization in device.h

All per-instance differences (register base, IRQn, IRQ handler name, reset bit, clock ID) are **parameterized as macros in `device.h`** (or `__device.h`), never hardcoded in the driver `.c` file. This makes the driver generic: the same `uart.c` works for any instance count because it only references `VSF_HW_USART##N##_REG` and `VSF_HW_USART##N##_IRQN`.

**Naming convention** (must match what `IMP_LV0` consumes):

```c
// device.h
#define VSF_HW_<PERIPH>_COUNT       N

// Per-instance macros (indexed 0..N-1)
#define VSF_HW_<PERIPH>0_IRQN       UART0_IRQ_IRQn
#define VSF_HW_<PERIPH>0_IRQHandler UART0_IRQHandler
#define VSF_HW_<PERIPH>0_REG        UART0_BASE
#define VSF_HW_<PERIPH>0_RST_BIT    (1u << RESET_UART0)
#define VSF_HW_<PERIPH>1_IRQN       UART1_IRQ_IRQn
#define VSF_HW_<PERIPH>1_IRQHandler UART1_IRQHandler
#define VSF_HW_<PERIPH>1_REG        UART1_BASE
#define VSF_HW_<PERIPH>1_RST_BIT    (1u << RESET_UART1)
```

The chip's reset/clock identifiers (e.g. `RESET_UART0` from `hardware/regs/resets.h`) appear **only inside `device.h`** — never inside the driver `.c`. The driver stores the resolved value in a struct field (`uint32_t rst_bit;`) initialized by `IMP_LV0` and used in `init()`.

**How `driver.h` consumes them:**

```c
// driver.h
#if VSF_HAL_USE_USART == ENABLED
#   include "hal/driver/common/template/vsf_template_usart.h"
#   define VSF_USART_CFG_DEC_PREFIX         vsf_hw
#   define VSF_USART_CFG_DEC_UPCASE_PREFIX  VSF_HW
#   include "hal/driver/common/usart/usart_template.h"
#endif
```

`usart_template.h` -> `vsf_template_instance_declaration.h` uses `VSF_HW_USART_COUNT`/`VSF_HW_USART_MASK` to emit extern declarations for `vsf_hw_usart0`...`vsf_hw_usartN`.

**Mask macros (`VSF_HW_<PERIPH>_MASK`) are mandatory for `acquire_from_all`:**

The `*_template.inc` default implementation of `channel_acquire_from_all` (e.g. `vsf_hw_dma_channel_acquire_from_all`) uses `VSF_HW_<PERIPH>_MASK` to iterate device instances. If the mask is missing, the macro expands to 0 and `acquire_from_all` returns `NULL` immediately. Always define the mask in `device.h` even when `COUNT == 1`:

```c
#define VSF_HW_DMA_COUNT    1
#define VSF_HW_DMA_MASK     0x1     // REQUIRED for acquire_from_all
```

**How `IMP_LV0` consumes them:**

```c
#define VSF_USART_CFG_IMP_LV0(ID, OP)                                \
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t)                 \
        VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart, ID) = {       \
        .reg     = (void *)VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX,\
                                        _USART, ID, _REG),           \
        .irqn    = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX,     \
                                _USART, ID, _IRQN),                  \
        .rst_bit = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX,     \
                                _USART, ID, _RST_BIT),               \
        OP                                                           \
    };                                                               \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX,  \
                                   _USART, ID, _IRQHandler)(void)    \
    { ... }
```

**Rule:** if a new peripheral is added, first add its `VSF_HW_<PERIPH>_COUNT` and per-instance macros to `device.h`, then write the driver -- the driver must never contain literal addresses like `0x40034000`, IRQ names like `UART0_IRQ_IRQn`, or reset/clock bits like `RESET_UART0` / `RESETS_RESET_<PERIPH>_LSB`. The checker rule `hardcoded-reset` catches all common SDK reset-name shapes (RP2040 short-form `RESET_<PERIPH>` and long-form `RESETS_RESET_<PERIPH>_LSB|BITS|MASK`). `instance-index-branch` also covers pointer-equality against per-instance SDK handles like `reg == spi0_hw`.

### Include convention

A peripheral driver `.c` file only includes:

```c
#include "hal/vsf_hal.h"
#include "hal/driver/vendor_driver.h"   // canonical — NOT "RP2040.h" / "stm32h7xx.h" / etc.
// IPCore (VSF-owned) headers stay here too:
#include "hal/driver/IPCore/ARM/PL011/vsf_pl011_uart_reg.h"   // if the driver embeds PL011
```

Vendor peripheral struct/reg headers (`hardware/structs/<periph>.h`, `hardware/regs/<periph>.h`) are **not** included in the driver. They live in **the chip's `device.h`** main block (the `__VSF_HAL_SHOW_VENDOR_INFO__`-NOT-defined branch), and reach the driver transitively because `hal/vsf_hal.h` → `driver.h` → chip-`device.h`. The template at `template/__series_name_a__/__device_name_a__/device.h` documents this slot — look for the `__common.h` comment near the top of the main block. A new chip port either:

- inlines all needed peripheral headers (`#include "hardware/structs/<periph>.h"` etc.) directly into `device.h`, or
- aggregates them into a sibling `__common.h` and includes that one file.

Either way, the driver `.c` doesn't name vendor peripheral headers — that is a porting bug. The RP2040 port follows the inline-in-`device.h` form; see `RaspberryPi/RP2040/device.h` for the concrete example (one block for `hardware/structs/*.h`, one for `hardware/regs/*.h`).

**Two indirections to keep straight:**

| Concern | Indirection | Where the chip-specific name appears |
|---|---|---|
| The chip's top-level header (`RP2040.h`, `stm32h7xx.h`) | `hal/driver/vendor_driver.h` → re-enters `driver.h` with `__VSF_HAL_SHOW_VENDOR_INFO__` → `device.h` SHOW_VENDOR_INFO branch | The SHOW_VENDOR_INFO branch of `device.h` |
| Vendor peripheral struct/reg headers (`hardware/structs/uart.h`, `hardware/regs/dma.h`) | Transitively through `hal/vsf_hal.h` → `device.h` main block | `device.h` main block (or its `__common.h` aggregate) |

**Exempt:** the chip-level `driver.c` (clock-tree / PLL / power-on bring-up) IS the vendor integration layer — it can include `hardware/structs/<periph>.h` for peripherals not covered by the centralized block (e.g. `xosc`, `pll`). Peripheral driver `.c` files are NOT exempt.

The migration table under "Style migration" still lists `#include "RP2040.h"` → `#include "hal/driver/vendor_driver.h"` for legacy drivers, but greenfield ports should use `vendor_driver.h` from the start AND keep peripheral headers out of the driver `.c`.

### Macro prefix convention

In driver `.c` files, prefix internal/local macros with `__` to avoid colliding with headers:

| Category | Example | Prefix | Location |
|---|---|---|---|
| Template config macros (consumed by `<periph>_template.inc`) | `VSF_USART_CFG_IMP_LV0`, `VSF_USART_CFG_IMP_PREFIX` | none -- template system requires exact names | `.c` file, before `#include "..._template.inc"` |
| OOC class control macros | `__VSF_HAL_PL011_UART_CLASS_INHERIT__`, `__VSF_HAL_DW_APB_I2C_CLASS_IMPLEMENT` | `__` | `.c` file, before `#include "hal/vsf_hal.h"` |
| Driver-local helpers | `__vsf_hw_usart_irqhandler`, `__uart_tx_fifo_depth` | `__` | `.c` file |
| Chip-wide hardware constants | `VSF_HW_ADC_CHANNEL_COUNT`, `VSF_HW_ADC_MAX_DATA_BITS` | `VSF_HW_<PERIPH>_` | `device.h` |
| Driver-internal constants | `__VSF_HW_ADC_SUPPORTED_IRQ_MASK` | `__VSF_HW_<PERIPH>_` | `.c` file |

**Why:** vendor SDK headers and VSF template headers define many unprefixed macros. A local helper like `REG` or `IRQN` can silently shadow or be shadowed by a header macro. Always `__` prefix macros that are not part of the VSF template API surface.

**`VSF_HW_<PERIPH>_` vs `__VSF_HW_<PERIPH>_`:**
- **`VSF_HW_` public constants** — hardware invariants that may be needed by the driver, the board file, or the application (channel count, data-bit width, FIFO depth). These live in `device.h` so every consumer sees the same value.
- **`__VSF_HW_` private constants** — driver-internal knowledge (supported IRQ mask, local threshold, retry count) that no other file needs. These stay inside the `.c` file with a `__` prefix so they never leak into the global namespace.

### Architecture: IPCore vs Direct

- **IPCore**: chip reuses existing IP block (e.g. ARM PL011 for USART). Driver calls `implement(vsf_<ip>_<periph>_t)`, IPCore handles register/IQ/baudrate. Chip provides reset, NVIC, clock. Set `__VSF_HAL_${IP}_<PERIPH>_CLASS_INHERIT__` before including `vsf_hal.h`.
- **Direct**: raw register access. Struct `{.reg, .isr}`, implement all APIs via register ops. **Pure direct drivers do NOT define any `__VSF_HAL_*_CLASS_*` macro** -- they just `#include "hal/vsf_hal.h"` directly. The CLASS macros are only needed when an IPCore struct is embedded via `implement()`.

### IMP_LV0

Every driver defines a `VSF_<PERIPH>_CFG_IMP_LV0(ID, OP)` macro before including `<periph>_template.inc`. Instantiates the `vsf_hw_<periph>##ID` struct and (optionally) its IRQ handler. Single-port chips may hardcode instance name instead of using `ID`.

### MULTI_CLASS

When `VSF_HW_<PERIPH>_CFG_MULTI_CLASS == ENABLED`, the driver struct embeds `vsf_<periph>_t` as first member for polymorphic dispatch through a vtable (`ptr->op->fn`). `<periph>_common.c` provides default implementations; driver overrides by setting `REIMPLEMENT_API_<FN> = ENABLED`.

### REIMPLEMENT macros

Two categories in `<periph>.h`:
- `REIMPLEMENT_TYPE_*=ENABLED`: redefine mode/irq/status/cfg enums and structs from template defaults
- `REIMPLEMENT_API_<FN>=ENABLED`: replace template's default function body with driver-specific implementation

### Unimplemented API convention

For **chip-specific driver `.c` files** (e.g. `RaspberryPi/RP2040/<periph>/<periph>.c`), every function body that exists only because the API contract requires it — for a peripheral capability the hardware doesn't support, or because the chip didn't implement it yet — must **both assert AND return an error**:

```c
vsf_err_t VSF_MCONNECT(VSF_<PERIPH>_CFG_IMP_PREFIX, _<periph>_<api>)(...)
{
    VSF_HAL_ASSERT(0);
    return VSF_ERR_NOT_SUPPORT;
}
```

For functions returning a value (count, mask, status), return a safe sentinel after the assert:

```c
vsf_<periph>_pin_mask_t VSF_MCONNECT(..., _<periph>_read)(...)
{
    VSF_HAL_ASSERT(0);
    return 0;
}
```

For `void` functions, the `VSF_HAL_ASSERT(0);` alone is sufficient — the assert halts execution in debug builds and in release builds the function returns harmlessly after doing nothing.

**Why both**: in debug builds `VSF_HAL_ASSERT(0)` halts immediately at the offending call site, so the bug is caught where it's introduced. In release builds asserts compile out, but the error return propagates so a caller that handles the error path still has a defined failure mode rather than fabricated success.

**Error-code choice**:
- `VSF_ERR_NOT_SUPPORT` — the hardware genuinely cannot do the operation (e.g., RP2040 has no RTC, so `vsf_hw_rtc_set_time` returns `VSF_ERR_NOT_SUPPORT`).
- `VSF_ERR_FAIL` — the function exists in the API but the template default should never have been reached (typical for `request_tx` / `request_rx` template defaults).

**Anti-pattern** (silent stub) — a function that returns `VSF_ERR_NONE` or `0` after only a pointer assert:

```c
vsf_err_t VSF_MCONNECT(..., _<periph>_config)(...)
{
    VSF_HAL_ASSERT(NULL != ptr);   // parameter assert only
    return VSF_ERR_NONE;            // caller thinks it worked — but nothing was configured
}
```

If a stub like this gets called during a port (because the developer forgot to implement that API), the caller proceeds as if hardware was configured, then fails later in a way that's far from the root cause. Add `VSF_HAL_ASSERT(0);` immediately before the return so the failure surfaces at the offending call.

**Does NOT apply to**:

- `template/__series_name_a__/common/**/*.c` — these are starter skeletons that the porter copies and edits. They intentionally return `VSF_ERR_NONE` / `0` because they're scaffolding: the porter replaces each stub body with real logic during a port, and may want to leave some unfilled while testing others. Once the file lands as a chip-specific driver (e.g. `RaspberryPi/<Chip>/<periph>/<periph>.c`), the rule above kicks in.
- Switch-case `default:` branches in real `_ctrl` implementations — the caller may legitimately probe support by trying a ctrl value, so silently returning `VSF_ERR_NOT_SUPPORT` is the contract there.
- Conditional `if (...) return VSF_ERR_NOT_SUPPORT;` inside otherwise-real functions (e.g. clock-divider out-of-range checks) — these are error paths in a real function, not stubs.

### Parameter validation — conservative assert policy

Every chip-specific driver API must validate its arguments with `VSF_HAL_ASSERT`. The policy is **fail-fast**: if a caller passes an invalid value, halt immediately in debug builds rather than silently ignoring the error and letting the bug propagate.

**Mandatory asserts:**

1. **Pointer asserts** — all functions taking an instance pointer must assert non-NULL:
   ```c
   VSF_HAL_ASSERT(NULL != <periph>_ptr);
   ```

2. **Bitmask asserts** — functions taking `irq_mask`, `mode`, or `ctrl` bitmasks must assert that no unsupported bits are set. The supported mask must match what `capability()` advertises. This applies to `irq_enable`, `irq_disable`, `irq_clear`, and any other API that accepts an `irq_mask`:
   ```c
   #define __VSF_HW_<PERIPH>_SUPPORTED_IRQ_MASK    VSF_<PERIPH>_IRQ_MASK_<X>

   void vsf_hw_<periph>_irq_enable(vsf_hw_<periph>_t *<periph>_ptr, vsf_<periph>_irq_mask_t irq_mask)
   {
       VSF_HAL_ASSERT(NULL != <periph>_ptr);
       VSF_HAL_ASSERT(0 == (irq_mask & ~__VSF_HW_<PERIPH>_SUPPORTED_IRQ_MASK));
       // ... only supported bits remain after the assert
   }
   ```

3. **Register pointer asserts** — after dereferencing the instance struct to obtain the hardware register base pointer (`reg`), assert it is non-NULL. This catches `IMP_LV0` misconfiguration (e.g. a macro resolving to `0` or an omitted `_REG` definition):
   ```c
   <vendor_reg_t> *reg = <periph>_ptr->reg;
   VSF_HAL_ASSERT(NULL != reg);
   ```

4. **Range asserts** — channel numbers, buffer sizes, clock dividers, and other numeric parameters must be checked against hardware limits:
   ```c
   VSF_HAL_ASSERT(channel < VSF_HW_<PERIPH>_CHANNEL_COUNT);
   ```

**Why conservative**: Silent ignore of invalid parameters creates latent bugs. A caller that passes an `irq_mask` with bits outside `capability().irq_mask` is making a programming error — it likely forgot to check `capability()` or has stale bit definitions. The assert surfaces this at the call site rather than letting the driver behave unpredictably.

**Release builds**: `VSF_HAL_ASSERT` compiles out, so there is no runtime cost. The assert is a development-time safety net, not a runtime error-handling mechanism.

**Anti-pattern** — silently masking off unsupported bits:

```c
// bad — caller thinks all bits were handled, but only bit 0 took effect
irq_mask &= VSF_<PERIPH>_IRQ_MASK_<X>;   // silently drops invalid bits
```

### Mode and IRQ definitions

**Mode bits**: each peripheral's mode field is a bitmask. Some bits are mandatory (must exist even if HW doesn't support them). Template defines the bit layout and mandatory placeholder values -- match your HW register bits to the template fields. If the HW has no direct equivalent, put the placeholder value in a high bit range that won't conflict.

**IRQ mask**: each peripheral's IRQ mask is a bitmask. Template provides default bits. Extra bits need explicit `#define VSF_<PERIPH>_IRQ_MASK_<X>` or VSF treats them as unsupported.

**IRQ handler pattern**:
```c
void VSF_HW_<PERIPH><N>_IRQHandler(void) {
    uintptr_t ctx = vsf_hal_irq_enter();
    // read raw status, read enabled mask, active = raw & mask
    // clear active irqs, if active && handler_fn: call it
    vsf_hal_irq_leave(ctx);
}
```

### Register access: read side effects and caching

**Default assumption**: every read of a peripheral register may have side effects (clear-on-read, FIFO pop, auto-increment, write-once-lock). Assume this until the datasheet proves otherwise.

**Cache rule**: when a register value is needed for multiple checks or operations, read it **once** into a local variable and operate on the local copy. Only re-read the hardware register when the value is expected to have changed.

**Read-modify-write rule**: configuration RMW follows the same shape — one read into a local, modify the local, one write back.

**Canonical IRQ-handler example** (already used by `vsf_pl011_uart.c`):

```c
// good — UARTMIS read once, used for both clearing and dispatch
vsf_usart_irq_mask_t mask = reg->UARTMIS.VALUE;
reg->UARTICR.VALUE = mask;
if (mask && (isr->handler_fn != NULL)) {
    isr->handler_fn(isr->target_ptr, ..., mask);
}
```

```c
// bad — re-reading UARTMIS the second time can clear bits or report stale state
if (reg->UARTMIS.VALUE & RXIM) {
    reg->UARTICR.VALUE = reg->UARTMIS.VALUE;   // second read
    isr->handler_fn(..., reg->UARTMIS.VALUE);  // third read — different value
}
```

**Exceptions** — when **not** to cache:

- **Clear-on-read status registers**: the read itself clears the bits. Cache *after* the read so subsequent users see the captured value.
- **FIFO data registers**: each read pops the next datum. Caching is nonsensical; each read returns a different byte.
- **Polling loops**: the whole point is to see a new value. Re-read every iteration.

**Symptoms that you missed this rule**:

- A status flag that "isn't set" in the second check but logs say it fired (first check consumed it).
- A polling loop that succeeds, then the immediate follow-up read returns a different value (loop exit value was the truth — keep it).
- An RMW that produces an unexpected register state (two reads sandwiched a hardware-side update).

**Wrap every register access that has a read side effect**:

Any read of a hardware register whose purpose is side-effect-only (clear-on-read acknowledge, FIFO pop, auto-increment counter, write-once-lock) must go through a **static inline helper** with a name that describes the intent. Never scatter bare register reads with `(void)` casts across the driver.

**Principle** — the helper owns the hardware contract; call sites own the intent:

```c
// good — helper encapsulates the access pattern and its side effect
static inline void __vsf_hw_<periph>_ack_irq(<vendor_reg_t> *reg)
{
    // Reading STATUS clears the pending flag (hardware clear-on-read).
    VSF_UNUSED_PARAM(reg->STATUS);
}
```

```c
// bad — bare read: side effect is hidden, comment must be duplicated
VSF_UNUSED_PARAM(reg->STATUS);   // clear-on-read
```

**Why wrap**:
1. **Single point of truth** — if the silicon changes from clear-on-read to write-to-clear, you update one helper body, not every call site.
2. **Self-documenting at the call site** — a named helper states intent; a bare register read does not.
3. **Zero overhead** — `static inline` compiles away; there is no runtime cost.

### Multi-subunit instantiation with VSF_MREPEAT

Peripherals with internal sub-units (e.g., DMA channels, SDIO functions, USB endpoints) can use `VSF_MREPEAT` inside `IMP_LV0` to generate per-sub-unit IRQ handlers and initializers without manual copy-paste:

```c
#define VSF_DMA_IMP_IRQHANDLER(__CHANNEL_IDX, __DMA_IDX)                    \
    void VSF_MCONNECT(VSF_DMA_CFG_IMP_UPCASE_PREFIX, _DMA, __DMA_IDX,       \
                      _CH, __CHANNEL_IDX, _IRQHandler)(void) {              \
        ...                                                                 \
    }

#define VSF_DMA_IMP_CHANNEL(__CHANNEL_IDX, __DMA_IDX)                       \
    [__CHANNEL_IDX] = { .irqn = VSF_MCONNECT(..., _CH, __CHANNEL_IDX, _IRQN) },

#define VSF_DMA_CFG_IMP_LV0(__IDX, __HAL_OP)                                \
    VSF_MREPEAT(VSF_HW_DMA_CHANNEL_NUM, VSF_DMA_IMP_IRQHANDLER, __IDX)      \
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t)                            \
        VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma, __IDX) = {               \
        .channels = {                                                         \
            VSF_MREPEAT(VSF_HW_DMA_CHANNEL_NUM, VSF_DMA_IMP_CHANNEL, __IDX) \
        },                                                                    \
        __HAL_OP                                                              \
    };
```

Reference: `driver/Artery/AT32F402_405/common/dma/dma.c`.

### Driver init() responsibilities

The peripheral driver's `init()` (e.g., `vsf_hw_usart_init`) owns **per-instance peripheral setup**. Anything that belongs to the peripheral's own state goes here; anything shared with other peripherals or owned by the board does NOT.

**Belongs in driver init() (cross-module work for the peripheral itself):**
- Reset deassert for this instance — e.g., `resets_hw->reset & ~(1u << RESET_UART0)`
- Peripheral clock enable / gate for this instance — e.g., RCC bit, `vsf_hw_peripheral_enable(usart_ptr->en)`
- IPCore init delegation, passing this instance's clock frequency (`clock_get_hz(clk_peri)`)
- NVIC priority + enable for this instance's IRQn, gated on `cfg->isr.handler_fn != NULL`
- Mode / baudrate / addressing register programming

**Does NOT belong in driver init() (board- or chip-level concerns):**
- **GPIO pinmux** — pin assignment is board-specific. The same chip may route UART0 to different GPIOs on different boards. Pinmux belongs **outside** the peripheral driver — `vsf_board.c` is the conventional home, but it can also live in application init, a project-specific board file, or any board-level setup. `vsf_board.c` is not mandatory; what matters is that the peripheral driver itself never touches pin routing.
- Chip-wide clock tree (PLL, AHB/APB dividers) — `vsf_driver_init()` owns this
- DMA channel allocation across peripherals — board/chip level

**Rule of thumb:** if the resource is shared with other peripherals (pins, clock tree, DMA pool), it does not belong in the per-peripheral driver. If it is the peripheral's own state (its reset bit, its clock gate, its IRQ line), `init()` owns it.

Reference: `driver/RaspberryPi/RP2040/uart/uart.c` `vsf_hw_usart_init` deasserts the per-instance UART reset, calls `vsf_pl011_usart_init` with `clock_get_hz(clk_peri)`, and configures NVIC — but does no pinmux. GP0/GP1 (UART0) and GP8/GP9 (UART1) routing is done in `board/pico/vsf_board.c` (the conventional location for this board; not a fixed requirement).

### get_configuration Convention

`get_configuration` (and its channel-scoped sibling `channel_get_configuration`) must return the **actual** configuration the hardware is operating with, not the values the caller last passed to `init` or `config`. There are three valid implementation patterns, in order of preference:

**1. Cached copy — preferred when the driver already caches the full configuration.**

If `init()` or `config()` stores a complete copy in the instance struct (e.g. `dma_ptr->cfg`), return that cached struct. This is cheap and accurate because every field was written by the driver itself.

```c
vsf_err_t vsf_hw_dma_get_configuration(vsf_hw_dma_t *dma_ptr, vsf_dma_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((NULL != dma_ptr) && (NULL != cfg_ptr));
    *cfg_ptr = dma_ptr->cfg;
    return VSF_ERR_NONE;
}
```

**2. Read hardware — required when the driver does not cache the full state.**

Read the relevant registers and reconstruct the configuration struct. This is necessary when the hardware supports partial reconfiguration (e.g. baudrate changed via `_ctrl` after `init`) or when the driver delegates to an IPCore that owns its own state.

```c
vsf_err_t vsf_hw_usart_get_configuration(...)
{
    // read divisor from HW, reconstruct baudrate
    // read data-bits, parity, stop-bits from control register
}
```

**3. Hardcoded with comment — acceptable only for invariant hardware defaults.**

If a field is truly invariant for this chip (e.g. RP2040 DMA always has 12 channels), returning a hardcoded value is allowed **only if accompanied by a comment explaining the invariant**.

```c
// RP2040 DMA has exactly 12 channels; this is architectural, not configurable.
capability.channel_count = 12;
```

**Rule: no un-commented hardcoded values.** A bare literal like `.channel_count = 12` with no explanation is a porting bug — the next porter has no way to know whether 12 is a hardware limit, a safe default, or a leftover from copy-paste. Every hardcoded value must be justified in a comment or in a named macro (`#define RP2040_DMA_CHANNEL_COUNT 12`).

### Thin wrapper philosophy

The HAL is a **thin wrapper** — each driver exposes exactly what the hardware natively supports, without software emulation. If the hardware cannot do an operation, the driver returns `VSF_ERR_NOT_SUPPORT` (with `VSF_HAL_ASSERT(0)`). The driver never emulates missing features in software (busy-wait loops, software timers, state machines).

**Rationale:** Emulating hardware features inside the driver adds complexity, hides the true hardware capability from the caller, creates blocking code paths, and blurs the line between HAL and application logic. If a caller needs a higher-level abstraction, it builds it on top of the HAL using application-level primitives (timers, threads, IRQs).

**Example — break signalling:** PL011 only supports `SET_BREAK` / `CLEAR_BREAK` (a manual BRK bit). It has no auto-timed break hardware. The RP2040 UART driver implements `SET_BREAK` and `CLEAR_BREAK` as single register writes; `SEND_BREAK` returns `VSF_ERR_NOT_SUPPORT`. A caller that needs a timed break uses `SET_BREAK` + its own timer + `CLEAR_BREAK`. See [peripherals/usart.md](../peripherals/usart.md).

### Non-blocking API requirement

All HAL API functions must be **non-blocking**. A function initiates a hardware operation and returns immediately; it must not busy-wait (spin-loop, volatile delay counter) for the operation to complete.

**Rationale:** A busy-wait steals the CPU from the caller — which may be an RTOS thread that should yield, an interrupt handler that must return quickly, or a test scenario polling multiple peripherals.

**Exception — negligible delay only:** A short busy-wait is acceptable ONLY when the driver developer confirms the delay is negligible in all configurations, AND adds a comment explaining *why*. "Negligible" means a few microseconds at most, independent of runtime configuration (baud rate, clock speed). Delay must not grow with caller-controlled parameters.

**Spin-wait on hardware state — mandatory comment:**

Any `while` loop that polls a hardware register until a bit flips (reset-done, ready flag, busy clear, etc.) is a **blocking spin-wait**. These loops can hang indefinitely if the hardware is broken or misconfigured. Every such loop MUST be preceded by a comment that:

1. States the expected maximum duration (e.g. "a few bus cycles", "< 1 us").
2. Explains why spin-wait is required (hardware has no interrupt for this state, or the operation must complete before subsequent register writes).
3. Marks it as a risk point so future readers know where to look if the system hangs.

```c
// Spin-wait: reset deassert → reset_done is a few clk_ref cycles (< 1 us).
// If this hangs, the peripheral clock or reset wiring is misconfigured.
resets_hw->reset &= ~rst_bit;
while (!(resets_hw->reset_done & rst_bit));
```

```c
// Spin-wait: conversion time is fixed by N clk_<periph> cycles (~2 us at 48 MHz).
// No IRQ for single-shot completion in polling mode; must wait before reading RESULT.
while (!(reg->status & <PERIPH>_STATUS_READY_BITS));
```

**Anti-pattern** — bare spin-wait with no comment:

```c
// bad — reader has no idea how long this might block or whether it's safe
while (!(reg->status & READY));
```

### Template file convention

```
source/hal/driver/
  common/template/vsf_template_<periph>.h    <- API declarations, default types
  common/<periph>/<periph>_template.inc      <- IMP_LV0 instantiation
  common/<periph>/<periph>_interface.c       <- multi-class dispatch
  common/<periph>/<periph>_common.c          <- default implementations
  template/__series_name_a__/common/<periph>/<periph>.h  <- header skeleton
  template/__series_name_a__/common/<periph>/<periph>.c  <- impl skeleton
```

### Board wiring

Board-level setup typically lives in `vsf_board.c` (a convention, not a requirement). At minimum it owns **pinmux** (GPIO function selection) — that part must NOT be in the peripheral driver. Calling `vsf_hw_<periph>_init(...)` itself is up to the user/developer: it can be done from `vsf_board.c`, from the application, or wherever fits the project. The fixed boundary is:

- Peripheral driver `init()`: per-instance reset/clock/NVIC and register programming.
- Outside the peripheral driver (board file or application): pinmux. Calling `init()` is a user choice — `vsf_board.c` is one place, but not the only one.

If `vsf_board.c` does call peripheral `init()`, it usually also exposes the instance pointer via `vsf_board.<name>` so the application can use it. See "Driver init() responsibilities" above for what `init()` itself does.

### Chip-level initialization (`vsf_driver_init`)

VSF calls `vsf_driver_init()` during `vsf_hal_init()`. This is where the chip driver sets up the **chip-wide clock tree** (PLLs, AHB/APB dividers, clock source selection) and any silicon-specific bring-up shared across peripherals — **before** individual peripheral drivers are initialized. Per-instance peripheral reset/clock/NVIC is NOT done here; that belongs to each peripheral's `init()` (see "Driver init() responsibilities" above).

```c
// driver.c
VSF_CAL_WEAK(vsf_driver_init)
bool vsf_driver_init(void)
{
    // Example: RP2040 clocks and resets
    // Example: STM32 RCC clock tree configuration
    return true;
}
```

If an IPCore init API needs the peripheral clock frequency (e.g., `vsf_dw_apb_i2c_init(..., clock_get_hz(clk_sys))`), `clock_get_hz()` must be implemented in `driver.c` and populated by `vsf_driver_init()`.

Some GPIO drivers also require a chip-specific init call inside `vsf_driver_init()` (e.g., AIC8800, BL61X). Check the chip's GPIO header for `__vsf_xxx_gpio_init()` requirements.

---

## Peripheral guides

| Peripheral | File |
|---|---|
| USART | [peripherals/usart.md](../peripherals/usart.md) |
| GPIO | [peripherals/gpio.md](../peripherals/gpio.md) |
| I2C | [peripherals/i2c.md](../peripherals/i2c.md) |
| SPI | [peripherals/spi.md](../peripherals/spi.md) |
| ADC | [peripherals/adc.md](../peripherals/adc.md) |
| PWM | [peripherals/pwm.md](../peripherals/pwm.md) |

---

## Style migration (old -> template standard)

Older drivers (e.g. RP2040 uart.c) use hardcoded names: `vsf_hw_usart_init`, `vsf_hw_usart_t`. The current standard uses `VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_init)` with a configurable prefix. To migrate:

### Method: backup -> copy template -> fill logic

1. **Backup** the old `.c` file to e.g. `uart_old.c.bak`
2. **Copy** the template `.c` file from `template/__series_name_a__/common/<periph>/` over the old file
3. **Purge** irrelevant sections:
   - Remove `// IPCore` blocks (keep only `// HW` blocks for chip-level drivers)
   - Remove IPCore `IMP_PREFIX` definitions (`vsf_${IP}`), keep `vsf_hw`
4. **Fill in hardware logic** from the old driver into each template function body, preserving the template's function signature and structure
5. **Port config macros.** The template's `#include "<periph>_template.inc"` block is preceded by `REIMPLEMENT_API_*`, `CHECK_MODE`, and `IMP_LV0` macros. Migrate the old driver's macro values into the template's corresponding slots -- don't copy-paste the entire block from the old file.
6. **Move old IMP_LV0 fields** (e.g. `.irqn`, `.reg` base address) into the template's IMP_LV0 macro
7. **Keep template includes** (`vsf_hal_cfg.h` -> `vsf_hal.h` -> vendor SDK), add any chip-specific vendor headers needed
8. **Update the header** (`.h` file) to use `VSF_MCONNECT` for the struct type

### Key replacements

| Old pattern | Template equivalent |
|---|---|
| `vsf_hw_usart_init(...)` | `VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_init)(...)` |
| `vsf_hw_usart_t *hw_usart_ptr` | `VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr` |
| `typedef struct vsf_hw_usart_t {` | `typedef struct VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) {` |
| `vsf_hw_usart0` | `VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart, 0)` |
| `vsf_pl011_usart_init(&ptr->use_as__vsf_pl011_usart_t, ...)` | same -- IPCore delegation is fine |
| `#include "../driver.h"` | `#include "hal/vsf_hal_cfg.h"` |
| `#include "RP2040.h"` | `#include "hal/driver/vendor_driver.h"` |

### IPCore delegation

If migrating an IPCore-based driver (chip wraps an existing IP like PL011), the delegation patterns don't change -- only naming. `init()` still delegates to `vsf_<ip>_<periph>_init()`, `capability()` to `vsf_<ip>_<periph>_capability()`, etc. Move chip-specific add-ons (reset, NVIC, clock, extra IRQ mask bits) into the new template body.

### IPCore migration pitfalls

| Pitfall | Symptom | Fix |
|---------|---------|-----|
| Duplicate base-class member | `duplicate member 'vsf_usart'` | `implement(vsf_pl011_usart_t)` already embeds `vsf_usart` when MULTI_CLASS is on -- don't declare it again in the wrapping struct |
| `IRQ_MASK_CHECK_UNIQUE` with aliased IRQs | `static assertion failed: Enum values must have disjoint bits: VSF_USART_IRQ_MASK_RX_TIMEOUT and VSF_USART_IRQ_MASK_RX_IDLE` | Use `VSF_HAL_CHECK_MODE_LOOSE` (not STRICT) and `#undef` the alias macro (e.g. `VSF_USART_IRQ_MASK_RX_IDLE`) right before `#include "...usart_template.inc"`, then `#define` it back. This keeps the check active for all non-alias bits. |
| `MODE_CHECK_UNIQUE` with zero-valued mode bits | static assertion on mode values sharing bit 0 | PL011 places several unsupported modes at high bits but `HALF_DUPLEX_DISABLE=0` overlaps `NO_PARITY=0`. Use `VSF_USART_CFG_MODE_CHECK_UNIQUE = VSF_HAL_CHECK_MODE_LOOSE` |
| `irq_clear` needs IP register access | undefined `reg->UARTICR` | Include the IP's `_reg.h` (e.g. `hal/driver/IPCore/ARM/PL011/vsf_pl011_uart_reg.h`) and cast `usart_ptr->reg` to the register struct type |
| Type mismatch on FIFO functions | implicit conversion warnings | Template uses `uint_fast32_t` for `rxfifo_read`/`txfifo_write`/`_get_data_count`/`_get_free_count`. IP may use `uint_fast16_t` -- cast in the wrapper |

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| No output | Deassert reset, check IMP_LV0 IRQ wiring |
| Garbage (USART) | Wrong baudrate formula |
| Spurious IRQ | Check mask before status read |
| Pull-up not working | Check PADS base doesn't set conflicting pull bits |
| `output_and_set` no effect | Verify PADS.OD not set |
| Template overwrites existing | `scaffold/chip.py` new chips only; edit existing directly |
| `duplicate member` in IPCore struct | Remove explicit `vsf_<periph>_t` -- `implement(vsf_<ip>_<periph>_t)` already includes it |
| `CHECK_UNIQUE` failure on mode/IRQ bits | See IPCore migration pitfalls table above |

## Examples

New chip USART (IPCore):
1. `scaffold/chip.py --driver-dir source/hal/driver/MyVendor/MyChip`
2. Copy uart.{h,c}, `implement(vsf_pl011_usart_t)`, add reset/NVIC/clock
3. `VSF_USART_CFG_IMP_LV0`, board.c pinmux+init
4. Verify: `check-usart-*.py` then vsf-board-run

Existing chip, new GPIO:
1. `scaffold/peripheral.py --driver-dir source/hal/driver --chip MyVendor/MyChip --periph gpio`
2. Implement APIs via register access, `VSF_GPIO_CFG_IMP_LV0`
3. Verify with `check/structure.py --periph gpio --side header/source`, then vsf-board-run

## Script Reference

### When to use each script

| Script | Use when... | Input | Exit 0 means |
|--------|------------|-------|--------------|
| `scaffold/chip.py` | Starting a brand-new chip port | YAML chip config | Skeleton directory created |
| `scaffold/peripheral.py` | Adding a peripheral to an existing chip | `--periph <name> --chip Vendor/Chip` | Template files copied and renamed |
| `scaffold/macros.py` | Adding/editing peripheral instances in device.h | YAML instance map | Macros written to device.h zone |
| `check/structure.py` | Verifying a driver file is structurally complete | `--periph <name> --side header\|source <file>` | All mandatory checks pass |
| `check/quality.py` | Checking for anti-patterns | One or more `.c`/`.h` files | Zero quality findings |
| `check/audit.py` | Cross-file consistency check | `--chip Vendor/Chip` | No wiring gaps found |
| `util/enable.py` | Toggling `VSF_HAL_USE_*` in vsf_usr_cfg.h | `--enable usart,spi --disable i2c` | All requested toggles applied |
| `vsf-bench` | Build + flash + run test scenes | hardware-map.yml | All scenes pass |

### Typical workflow order

```
1. scaffold/chip.py           ← once per chip (R1)
2. scaffold/macros.py  ← edit device.h (R1)
3. scaffold/peripheral.py     ← one per peripheral (R3a/R3b/R5)
4. [edit driver .c / .h]      ← implement register logic
5. util/enable.py           ← enable the peripheral (R2)
6. check/structure.py  ← structural completeness (header + source)
7. check/quality.py    ← no anti-patterns
8. check/audit.py              ← cross-file consistency
9. vsf-bench                  ← build+flash+test on hardware
```

**Structure vs quality checkers** — `check/structure.py` verifies the driver *has the right shape* (guard macros present, all mandatory APIs exist, template includes correct, IMP_LV0 defined). It reads peripheral-specific rules from `scripts/check-specs/<periph>.yml`. `check/quality.py` verifies the driver *doesn't have the wrong content* (no hardcoded instances, no pinmux-in-driver, no bare IRQ names). Run structure first (cheap, catches mechanical omissions), then quality.

### Peripherals with YAML specs

| Peripheral | YAML spec |
|---|---|
| USART | `check-specs/usart.yml` |
| GPIO | `check-specs/gpio.yml` |
| I2C | `check-specs/i2c.yml` |
| SPI | `check-specs/spi.yml` |
| ADC | `check-specs/adc.yml` |
| PWM | `check-specs/pwm.yml` |

Adding a checker for a new peripheral = adding a YAML file under `scripts/check-specs/`. For complex peripheral-specific logic that YAML cannot express, extend `check/structure.py` with a conditional block (e.g. the GPIO pinmux boundary check).
