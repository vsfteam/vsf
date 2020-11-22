
#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_USE_AWTK == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#include "kernel/vsf_kernel.h"
#include "utilities/ooc_class.h"

#include "tkc/mem.h"
#include "tkc/utils.h"
#include "tkc/thread.h"
#include "tkc/mutex.h"

#if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
extern vsf_err_t __vsf_eda_set_priority(vsf_eda_t *pthis, vsf_prio_t priority);
extern vsf_prio_t __vsf_eda_get_cur_priority(vsf_eda_t *pthis);
#endif

struct _tk_thread_t {
  implement(vsf_thread_t)

  void* args;
  tk_thread_entry_t entry;

#if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
  vsf_thread_cb_t cb;
  bool_t running;
#else
  uint32_t stack_size;
#endif
  const char* name;
  uint32_t thread_priority;
  tk_mutex_t* mutex;
};

ret_t tk_thread_set_name(tk_thread_t* thread, const char* name) {
  return_value_if_fail(thread != NULL && name != NULL, RET_BAD_PARAMS);

  thread->name = name;

  return RET_OK;
}

ret_t tk_thread_set_stack_size(tk_thread_t* thread, uint32_t stack_size) {
  return_value_if_fail(thread != NULL && stack_size >= 512, RET_BAD_PARAMS);

#if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
  thread->cb.stack_size = stack_size;
#else
  thread->stack_size = stack_size;
#endif

  return RET_OK;
}

ret_t tk_thread_set_priority(tk_thread_t* thread, uint32_t priority) {
  return_value_if_fail(thread != NULL, RET_BAD_PARAMS);

  thread->thread_priority = priority;
#if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
  if (thread->running) {
    __vsf_eda_set_priority(&thread->use_as__vsf_eda_t, (vsf_prio_t)priority);
  }
#endif

  return RET_OK;
}

void* tk_thread_get_args(tk_thread_t* thread) {
  return_value_if_fail(thread != NULL, NULL);

  return thread->args;
}

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
static void tk_thread_entry(vsf_thread_cb_t *cb) {
  tk_thread_t* thread = (tk_thread_t*)vsf_eda_get_cur();

  tk_mutex_lock(thread->mutex);
#if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
  thread->running = TRUE;
#endif
  thread->entry(thread->args);
#if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
  thread->running = FALSE;
#endif
  tk_mutex_unlock(thread->mutex);
}
#else
static void tk_thread_entry(vsf_thread_t *vsf_thread) {
  tk_thread_t* thread = (tk_thread_t*)vsf_thread;

  tk_mutex_lock(thread->mutex);
  thread->running = TRUE;
  thread->entry(thread->args);
  thread->running = FALSE;
  tk_mutex_unlock(thread->mutex);
}
#endif

tk_thread_t* tk_thread_create(tk_thread_entry_t entry, void* args) {
  tk_thread_t* thread = NULL;
  return_value_if_fail(entry != NULL, NULL);

  thread = (tk_thread_t*)TKMEM_ZALLOC(tk_thread_t);
  return_value_if_fail(thread != NULL, NULL);

  thread->args = args;
  thread->entry = entry;
  thread->mutex = tk_mutex_create();

  return thread;
}

ret_t tk_thread_start(tk_thread_t* thread) {
  return_value_if_fail(thread != NULL && !thread->running, RET_BAD_PARAMS);

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
  uint_fast32_t stack_size = thread->cb.stack_size;
#else
  uint_fast32_t stack_size = thread->stack_size;
#endif

  stack_size += (1 << VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT) - 1;
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
  thread->cb.entry = tk_thread_entry;
  thread->cb.stack = (uint64_t *)((uintptr_t)TKMEM_ALLOC(stack_size) & ~((1 << VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT) - 1));
  return_value_if_fail(thread->cb.stack != NULL, RET_OOM);
  vsf_thread_start(&thread->use_as__vsf_thread_t, &thread->cb, (vsf_prio_t)thread->thread_priority);
#else
  thread->entry = tk_thread_entry;
  thread->stack = (uint64_t *)((uintptr_t)TKMEM_ALLOC(stack_size) & ~((1 << VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT) - 1));
  return_value_if_fail(thread->stack != NULL, RET_OOM);
  vsf_thread_start(&thread->use_as__vsf_thread_t, (vsf_prio_t)thread->thread_priority);
#endif

  return RET_OK;
}

ret_t tk_thread_join(tk_thread_t* thread) {
  return_value_if_fail(thread != NULL, RET_BAD_PARAMS);

  return_value_if_fail(tk_mutex_lock(thread->mutex) == RET_OK, RET_FAIL);
  tk_mutex_unlock(thread->mutex);

  return RET_OK;
}

ret_t tk_thread_destroy(tk_thread_t* thread) {
  return_value_if_fail(thread != NULL, RET_BAD_PARAMS);

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
  if (thread->cb.stack != NULL) {
    TKMEM_FREE(thread->cb.stack);
  }
#else
  if (thread->stack != NULL) {
    TKMEM_FREE(thread->stack);
  }
#endif

  if (thread->mutex != NULL) {
    tk_mutex_destroy(thread->mutex);
  }

  vsf_eda_fini(&thread->use_as__vsf_eda_t);
  memset(thread, 0x00, sizeof(tk_thread_t));
  TKMEM_FREE(thread);

  return RET_OK;
}

#endif      // VSF_USE_AWTK
