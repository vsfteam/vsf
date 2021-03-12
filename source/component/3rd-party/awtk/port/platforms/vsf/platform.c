#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_USE_AWTK == ENABLED

#include "kernel/vsf_kernel.h"
#include "tkc/types_def.h"
#include "base/main_loop.h"

ret_t platform_prepare(void) {
  return RET_OK;
}

ret_t platform_disaptch_input(main_loop_t *l) {
  return RET_OK;
}

uint64_t get_time_ms64(void) {
  return (uint64_t)vsf_systimer_get_ms();
}

void sleep_ms(uint32_t ms) {
  vsf_thread_delay(vsf_systimer_ms_to_tick(ms));
}

#endif
