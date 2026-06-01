/*============================ INCLUDES ======================================*/

#include "vsf_test_arch_preempt.h"
#include "vsf_test_suites.h"

/*============================ EMBEDDED SUITE ==============================*/

/* This suite exercises VSF EDA priority preemption and requires the kernel.
 * Build with -DVSF_TEST_KERNEL_PROFILE_EDA when targeting bare-metal. */
#if VSF_TEST_ARCH_PREEMPT_ENABLE == ENABLED
#   if VSF_USE_KERNEL != ENABLED
#       error "arch_preempt requires VSF_USE_KERNEL == ENABLED. " \
              "Build with -DVSF_TEST_KERNEL_PROFILE_EDA"
#   endif

const struct vsf_test_arch_preempt_s {
    vsf_test_suite_t hdr;
    vsf_test_arch_preempt_params_t          params[VSF_TEST_ARCH_PREEMPT_CASE_COUNT];
} vsf_test_arch_preempt = {
    .hdr = {
        .name            = "arch_preempt",
        .jmp_fn          = vsf_test_arch_preempt_run,
        .case_count      = VSF_TEST_ARCH_PREEMPT_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_arch_preempt_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_ARCH,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_ARCH_PREEMPT_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_TEST_ARCH_PREEMPT_ENABLE == ENABLED

/* Busy-wait: each step adds __WORKER_BUSY_STEP_MS. Highest-priority worker
 * spins the shortest time, lowest the longest, so completion order is
 * unambiguously determined by priority, not coincidence. */
#ifndef VSF_TEST_ARCH_PREEMPT_BUSY_BASE_MS
#   define VSF_TEST_ARCH_PREEMPT_BUSY_BASE_MS     10
#endif
#ifndef VSF_TEST_ARCH_PREEMPT_BUSY_STEP_MS
#   define VSF_TEST_ARCH_PREEMPT_BUSY_STEP_MS     40
#endif

static uint32_t __worker_busy_ms(uint8_t id)
{
    uint8_t idx = VSF_TEST_ARCH_PREEMPT_WORKER_COUNT - 1 - id;
    return VSF_TEST_ARCH_PREEMPT_BUSY_BASE_MS + idx * VSF_TEST_ARCH_PREEMPT_BUSY_STEP_MS;
}

/* Work-item priorities, indexed by worker id (0 = highest priority).
 * Uses relative offsets from vsf_prio_highest so we adapt to whatever
 * VSF_OS_CFG_PRIORITY_NUM the board configures. */
static const vsf_prio_t __worker_prio[VSF_TEST_ARCH_PREEMPT_WORKER_COUNT] = {
    vsf_prio_highest - 0,
    vsf_prio_highest - 1,
};

/* Compile-time guard: need at least WORKER_COUNT priority levels */
_Static_assert(vsf_prio_highest >= VSF_TEST_ARCH_PREEMPT_WORKER_COUNT - 1,
    "arch_preempt: increase VSF_OS_CFG_PRIORITY_NUM to at least WORKER_COUNT");

/* Worker EDA struct — one instance per priority level */
typedef struct arch_preempt_worker_t {
    implement(vsf_eda_t)
    uint8_t  id;
} arch_preempt_worker_t;

static arch_preempt_worker_t __workers[VSF_TEST_ARCH_PREEMPT_WORKER_COUNT];

/* Worker event handler: busy-waits then logs completion.
 *
 * When all workers are pending simultaneously the scheduler must dispatch
 * the highest-priority one first, regardless of posting order.  Completion
 * markers must appear in id order (0 → 1 → 2 → 3, highest priority first). */
static void __worker_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    arch_preempt_worker_t *self = (arch_preempt_worker_t *)eda;
    switch (evt) {
    case VSF_EVT_INIT:
        break;
    case VSF_EVT_USER:
        vsf_test_busy_wait_ms(__worker_busy_ms(self->id));
        VSF_TEST_TRACE_INFO("arch_preempt:done id=%u" VSF_TRACE_CFG_LINEEND,
                            (unsigned)self->id);
        vsf_test_suite_data.arch.arch_preempt.done[self->id] = true;
        break;
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_arch_preempt_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    VSF_UNUSED_PARAM(suite);
    VSF_UNUSED_PARAM(tc);
    VSF_UNUSED_PARAM(inst);

    VSF_TEST_TRACE_INFO("arch_preempt:start" VSF_TRACE_CFG_LINEEND);

    /* Initialise worker state */
    for (int i = 0; i < VSF_TEST_ARCH_PREEMPT_WORKER_COUNT; i++) {
        __workers[i].id = (uint8_t)i;
        vsf_test_suite_data.arch.arch_preempt.done[i] = false;
    }

    /* Start workers and post VSF_EVT_USER atomically.
     * Events are posted in reverse priority order (lowest first) so that a
     * simple FIFO dispatch would produce the wrong completion sequence. */
    vsf_interrupt_safe() {
        for (int i = 0; i < VSF_TEST_ARCH_PREEMPT_WORKER_COUNT; i++) {
            vsf_eda_start(&__workers[i].use_as__vsf_eda_t, &(vsf_eda_cfg_t){
                .fn.evthandler  = __worker_evthandler,
                .priority       = __worker_prio[i],
            });
        }
        for (int i = VSF_TEST_ARCH_PREEMPT_WORKER_COUNT - 1; i >= 0; i--) {
            vsf_eda_post_evt(&__workers[i].use_as__vsf_eda_t, VSF_EVT_USER);
        }
    }
    /* Protect region exits → scheduler dispatches highest-priority task first. */

    /* Wait for all workers to complete */
    for (int i = 0; i < VSF_TEST_ARCH_PREEMPT_WORKER_COUNT; i++) {
        VSF_TEST_WAIT_FOR(vsf_test_suite_data.arch.arch_preempt.done[i],
                          VSF_TEST_ARCH_PREEMPT_TIMEOUT_MS);
        if (!vsf_test_suite_data.arch.arch_preempt.done[i]) {
            VSF_TEST_TRACE_ERROR("arch_preempt:worker %u timed out"
                                 VSF_TRACE_CFG_LINEEND, (unsigned)i);
        }
        VSF_TEST_ASSERT(vsf_test_suite_data.arch.arch_preempt.done[i]);
    }

    VSF_TEST_TRACE_INFO("arch_preempt:pass" VSF_TRACE_CFG_LINEEND);
}

#endif /* VSF_TEST_ARCH_PREEMPT_ENABLE == ENABLED */

/* EOF */
