#include "vsf.h"
#include "tkc/types_def.h"
#include "base/main_loop.h"

ret_t platform_prepare(void) {
  return RET_OK;
}

ret_t platform_disaptch_input(main_loop_t *l) {
  return RET_OK;
}

uint64_t get_time_ms64(void) {
  return (uint64_t)vsf_systimer_tick_to_ms(vsf_timer_get_tick());
}

void sleep_ms(uint32_t ms) {
  vsf_thread_delay(vsf_systimer_ms_to_tick(ms));
}