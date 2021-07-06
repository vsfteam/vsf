#include "vsf.h"

#if VSF_USE_EVM == ENABLED && VSF_EVM_USE_LUA == ENABLED

#include "luat_base.h"
#include "luat_malloc.h"
#include "luat_timer.h"

typedef struct rtos_timer_t {
    implement(vsf_callback_timer_t)
    implement(vsf_dlist_node_t)
    int index;
    luat_timer_t *luat_timer;
} rtos_timer_t;

static int rtos_timer_index = 0;
static vsf_dlist_t rtos_timer_list;

static void luat_timer_due(vsf_callback_timer_t *timer)
{
    rtos_timer_t *os_timer = (rtos_timer_t *)timer;
    luat_timer_t *luat_timer = os_timer->luat_timer;

    rtos_msg_t msg = {
        .handler    = luat_timer->func,
        .ptr        = luat_timer,
    };
    luat_msgbus_put(&msg, 0);
}

int luat_timer_start(luat_timer_t *timer) {
    rtos_timer_t *os_timer = evm_malloc(sizeof(rtos_timer_t));
    if (NULL == os_timer) {
        return 1;
    }

    vsf_callback_timer_init(&os_timer->use_as__vsf_callback_timer_t);
    os_timer->index = rtos_timer_index++;
    vsf_dlist_init_node(rtos_timer_t, use_as__vsf_dlist_node_t, os_timer);
    vsf_dlist_add_to_tail(rtos_timer_t, use_as__vsf_dlist_node_t, &rtos_timer_list, os_timer);
    timer->os_timer = os_timer;
    os_timer->luat_timer = timer;
    os_timer->use_as__vsf_callback_timer_t.on_timer = luat_timer_due;
    vsf_callback_timer_add_ms(&os_timer->use_as__vsf_callback_timer_t, timer->timeout);
    return 0;
}

int luat_timer_stop(luat_timer_t *timer) {
    if (!timer) {
        return 1;
    }
    rtos_timer_t *os_timer = timer->os_timer;
    vsf_dlist_remove(rtos_timer_t, use_as__vsf_dlist_node_t, &rtos_timer_list, os_timer);
    vsf_callback_timer_remove(&os_timer->use_as__vsf_callback_timer_t);
    evm_free(os_timer);
    return 0;
};

luat_timer_t * luat_timer_get(size_t timer_id) {
    __vsf_dlist_foreach_unsafe(rtos_timer_t, use_as__vsf_dlist_node_t, &rtos_timer_list) {
        if (_->index == timer_id) {
            return _->luat_timer;
        }
    }
    return NULL;
}

int luat_timer_mdelay(size_t ms) {
    if (ms > 0) {
        vsf_thread_delay_ms(ms);
    }
    return 0;
}

#endif      // VSF_USE_EVM && VSF_EVM_USE_LUA
