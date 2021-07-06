#include "vsf.h"

#if VSF_USE_EVM == ENABLED && VSF_EVM_USE_LUA == ENABLED

#include "luat_msgbus.h"

#if VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE != ENABLED
#   error VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE is required for luat_msgbus
#endif
#if VSF_EDA_QUEUE_CFG_SUPPORT_ISR != ENABLED
#   error VSF_EDA_QUEUE_CFG_SUPPORT_ISR is required
#endif

#define QUEUE_LENGTH 0xFF

typedef struct luat_msgbus_t {
    implement(vsf_eda_queue_t)

    uint16_t head;
    uint16_t tail;

    rtos_msg_t node_buffer[QUEUE_LENGTH];
} luat_msgbus_t;

static luat_msgbus_t luat_msgbus;

static bool luat_msgbus_enqueue(vsf_eda_queue_t *queue, void *node)
{
    luat_msgbus_t *msgbus = (luat_msgbus_t *)queue;
    msgbus->node_buffer[msgbus->head] = *(rtos_msg_t *)node;
    if (++msgbus->head >= QUEUE_LENGTH) {
        msgbus->head = 0;
    }
    return true;
}

static bool luat_msgbus_dequeue(vsf_eda_queue_t *queue, void **node)
{
    luat_msgbus_t *msgbus = (luat_msgbus_t *)queue;
    *(rtos_msg_t *)(*node) = msgbus->node_buffer[msgbus->tail];
    if (++msgbus->tail >= QUEUE_LENGTH) {
        msgbus->tail = 0;
    }
    return true;
}

void luat_msgbus_init(void) {
    luat_msgbus.head = luat_msgbus.tail = 0;
    luat_msgbus.op.dequeue = luat_msgbus_dequeue;
    luat_msgbus.op.enqueue = luat_msgbus_enqueue;
#if VSF_EDA_QUEUE_CFG_REGION == ENABLED
    luat_msgbus.region = (vsf_protect_region_t *)&vsf_protect_region_int;
#endif
    vsf_eda_queue_init(&luat_msgbus.use_as__vsf_eda_queue_t, QUEUE_LENGTH);
}

uint32_t luat_msgbus_put(rtos_msg_t *msg, size_t timeout) {
    vsf_err_t err = vsf_eda_queue_send(&luat_msgbus.use_as__vsf_eda_queue_t, msg, timeout);
    return VSF_ERR_NONE == err ? 0 : 1;
}

uint32_t luat_msgbus_get(rtos_msg_t *msg, size_t timeout) {
    vsf_err_t err = vsf_eda_queue_recv(&luat_msgbus.use_as__vsf_eda_queue_t, (void **)&msg, timeout);
    return VSF_ERR_NONE == err ? 0 : 1;
}

uint32_t luat_msgbus_freesize(void) {
    return 1;
}

#endif      // VSF_USE_EVM && VSF_EVM_USE_LUA
