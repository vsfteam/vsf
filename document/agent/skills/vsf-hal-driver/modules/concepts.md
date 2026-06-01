# Concepts

- **LV0:** register-level driver — reads/writes hardware registers directly. LV1 (framework wrappers) and LV2 (application APIs) are out of scope. **Boundary note:** a crash inside the driver `.c` file is an LV0 bug (in scope); a crash in LV1 glue code that calls the driver incorrectly, even if the symptom appears as "driver returns error", is an LV1 bug (out of scope — use the `diagnose` skill or `vsf-bench` to isolate the layer).
- **IMP_LV0:** macro that expands into per-instance `struct` definitions and IRQ handler stubs, driven by macros in `device.h`.
- **VSF_MCONNECT:** token-paste macro `VSF_MCONNECT(prefix, suffix, __IDX)` → `prefix##__IDX##suffix` for building per-instance names.
- **Complete driver checklist:** `device.h` instance macros + `.h` API header + `.c` implementation + `IMP_LV0` block + `vsf_board.c` pinmux + `vsf_usr_cfg.h` enable flag.
- **Exit code semantics (all scripts):** exit 0 = pass; exit 2 = all findings are known-acceptable warnings (review and proceed); any other exit = errors that must be fixed. Applies to `check/skeleton.py`, `check/structure.py`, `check/quality.py`, and `check/audit.py`.
- **Reimplement-type macros (`VSF_<PERIPH>_CFG_REIMPLEMENT_TYPE_*`):** every VSF template header declares default enums and structs (mode bits, IRQ masks, config, status, capability, ctrl). When a chip's hardware layout differs from the generic template, or when the structure checker requires values to be visible in the chip-specific header, enable the corresponding macro and redefine the type in the chip's `.h` file. The template then skips its own definition and uses the chip-specific one. Common scenarios:
  - **`CFG_REIMPLEMENT_TYPE_MODE` / `CFG_REIMPLEMENT_TYPE_CHANNEL_MODE`:** mode bits directly encode hardware register fields (e.g., a serial peripheral's clock-polarity/phase bits placed at the same bit positions as the vendor CR0 register). This eliminates `if/else` translation in `.c` and is the preferred way to implement convention 8.
  - **`CFG_REIMPLEMENT_TYPE_IRQ_MASK`:** when the peripheral has a non-standard interrupt set, or when `check/structure.py` requires specific mask values (e.g., `VSF_<PERIPH>_IRQ_MASK_OVERFLOW`) to be present in the chip header file. The checker does not preprocess `#include`, so values inside an included template are invisible to it.
  - **`CFG_REIMPLEMENT_TYPE_CFG` / `CFG_REIMPLEMENT_TYPE_STATUS` / `CFG_REIMPLEMENT_TYPE_CAPABILITY`:** when the chip needs extra fields in the config/status/capability structs (e.g., a chip-specific clock source field).
  - **`CFG_REIMPLEMENT_TYPE_CTRL` / `CFG_REIMPLEMENT_TYPE_CHANNEL_CTRL`:** when the chip supports vendor-specific control commands beyond the generic set.

  Pattern: set the macro to `ENABLED`, define the type, then include the template. The same three-step pattern applies to **all** reimplementable types — mode, IRQ mask, cfg, status, capability, ctrl, etc. Only the macro name and the type being defined differ. Example (IRQ mask; mode and other types follow identically):
  ```c
  #define VSF_<PERIPH>_CFG_REIMPLEMENT_TYPE_IRQ_MASK     ENABLED
  typedef enum vsf_<periph>_irq_mask_t {
      VSF_<PERIPH>_IRQ_MASK_OVERFLOW = (0x01 << 0),
  } vsf_<periph>_irq_mask_t;
  #include "hal/driver/common/template/vsf_template_<periph>.h"
  ```
