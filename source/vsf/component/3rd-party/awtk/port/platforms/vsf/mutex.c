#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_USE_AWTK == ENABLED

#include "kernel/vsf_kernel.h"
#include "utilities/ooc_class.h"

#include "tkc/mem.h"
#include "tkc/mutex.h"

struct _tk_mutex_t {
  implement(vsf_mutex_t)
};

tk_mutex_t* tk_mutex_create() {
  tk_mutex_t* mutex = TKMEM_ZALLOC(tk_mutex_t);
  return_value_if_fail(mutex != NULL, NULL);

  vsf_eda_mutex_init(&mutex->use_as__vsf_mutex_t);

  return mutex;
}

ret_t tk_mutex_lock(tk_mutex_t* mutex) {
  return_value_if_fail(mutex != NULL, RET_BAD_PARAMS);

  vsf_thread_mutex_enter(&mutex->use_as__vsf_mutex_t, -1);

  return RET_OK;
}

ret_t tk_mutex_unlock(tk_mutex_t* mutex) {
  return_value_if_fail(mutex != NULL, RET_BAD_PARAMS);

  vsf_thread_mutex_leave(&mutex->use_as__vsf_mutex_t);

  return RET_OK;
}

ret_t tk_mutex_destroy(tk_mutex_t* mutex) {
  return_value_if_fail(mutex != NULL, RET_BAD_PARAMS);

  memset(mutex, 0x00, sizeof(tk_mutex_t));
  TKMEM_FREE(mutex);

  return RET_OK;
}

#endif
