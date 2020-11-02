#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_USE_AWTK == ENABLED

#include "kernel/vsf_kernel.h"
#include "utilities/ooc_class.h"

#include "tkc/mem.h"
#include "tkc/time_now.h"
#include "tkc/platform.h"
#include "tkc/semaphore.h"

struct _tk_semaphore_t {
  implement(vsf_sem_t)
};

tk_semaphore_t* tk_semaphore_create(uint32_t value, const char* name) {
  tk_semaphore_t* semaphore = TKMEM_ZALLOC(tk_semaphore_t);
  return_value_if_fail(semaphore != NULL, NULL);

  vsf_eda_sem_init(&semaphore->use_as__vsf_sem_t, value);
  return semaphore;
}

ret_t tk_semaphore_wait(tk_semaphore_t* semaphore, uint32_t timeout_ms) {
  vsf_systimer_cnt_t tick = vsf_systimer_ms_to_tick(timeout_ms);
  vsf_sync_reason_t reason;
  return_value_if_fail(semaphore != NULL, RET_BAD_PARAMS);

  reason = vsf_thread_sem_pend(&semaphore->use_as__vsf_sem_t, tick);
  if (reason != VSF_SYNC_GET) {
    return RET_TIMEOUT;
  }

  return RET_OK;
}

ret_t tk_semaphore_post(tk_semaphore_t* semaphore) {
  return_value_if_fail(semaphore != NULL, RET_BAD_PARAMS);

  vsf_thread_sem_post(&semaphore->use_as__vsf_sem_t);
  return RET_OK;
}

ret_t tk_semaphore_destroy(tk_semaphore_t* semaphore) {
  return_value_if_fail(semaphore != NULL, RET_BAD_PARAMS);

  memset(semaphore, 0x00, sizeof(tk_semaphore_t));
  TKMEM_FREE(semaphore);

  return RET_OK;
}

#endif
