#include "../vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED && VSF_USE_LINUX == ENABLED

#include <string.h>
#define __VSF_LINUX_CLASS_INHERIT__
#include "shell/sys/linux/vsf_linux.h"
#include "kernel/vsf_kernel.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifndef __VSF_ESPIDF_FREERTOS_TASK_H__
#   error espidf/include MUST be put before freertos/include
#endif

/* ---------------------------- the op -------------------------------------- */

static void __vsf_espidf_task_on_run(vsf_thread_cb_t *cb)
{
    (void)cb;
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    StaticTask_t *ft = (StaticTask_t *)thread;
    if (ft->freertos_entry != NULL) {
        ft->freertos_entry(ft->arg);
    }
    vsf_linux_thread_on_terminate(thread);
}

static const vsf_linux_thread_op_t __vsf_espidf_task_op = {
    .priv_size = sizeof(StaticTask_t) - sizeof(vsf_linux_thread_t),
    .on_run = __vsf_espidf_task_on_run,
    .on_terminate = vsf_linux_thread_on_terminate,
};

/* ---------------------------- helpers ------------------------------------- */

static uint32_t __espidf_round_stack(uint32_t stack_bytes)
{
    uint32_t page = (uint32_t)VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE;
    uint32_t guard = (uint32_t)VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE;
    uint32_t min_bytes = page + guard;
    if (stack_bytes < min_bytes) {
        stack_bytes = min_bytes;
    }
    stack_bytes = (stack_bytes + (page - 1u)) & ~(page - 1u);
    stack_bytes = (stack_bytes + 7u) & ~7u;
    return stack_bytes;
}

/* ---------------------------- xTaskCreate --------------------------------- */

BaseType_t xTaskCreate(TaskFunction_t pxTaskCode,
                       const char * const pcName,
                       const uint32_t usStackDepth,
                       void * const pvParameters,
                       UBaseType_t uxPriority,
                       TaskHandle_t * const pxCreatedTask)
{
    (void)pcName;
    (void)uxPriority;

    if (pxTaskCode == NULL) {
        if (pxCreatedTask != NULL) { *pxCreatedTask = NULL; }
        return pdFAIL;
    }

    uint32_t stack_bytes = __espidf_round_stack(usStackDepth);

    vsf_linux_thread_t *thread = vsf_linux_create_thread(
        NULL, &__vsf_espidf_task_op, stack_bytes, NULL);
    if (thread == NULL) {
        if (pxCreatedTask != NULL) { *pxCreatedTask = NULL; }
        return pdFAIL;
    }

    StaticTask_t *ft = (StaticTask_t *)thread;
    ft->freertos_entry = (void (*)(void *))pxTaskCode;
    ft->arg   = pvParameters;

#if VSF_FREERTOS_CFG_USE_NOTIFY == ENABLED
    vsf_eda_sem_init(&ft->notify_sem, 0, 1);
    ft->notify_value   = 0;
    ft->notify_pending = false;
#endif

    vsf_linux_start_thread(thread, VSF_FREERTOS_CFG_DEFAULT_VSF_PRIO);

    if (pxCreatedTask != NULL) { *pxCreatedTask = (TaskHandle_t)ft; }
    return pdPASS;
}

/* ---------------------------- xTaskCreateStatic --------------------------- */

TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode,
                               const char * const pcName,
                               const uint32_t ulStackDepth,
                               void * const pvParameters,
                               UBaseType_t uxPriority,
                               StackType_t * const puxStackBuffer,
                               StaticTask_t * const pxTaskBuffer)
{
    (void)pcName;
    (void)uxPriority;

    if ((pxTaskCode == NULL) || (pxTaskBuffer == NULL) || (puxStackBuffer == NULL)) {
        return NULL;
    }

    uint32_t page = (uint32_t)VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE;
    uint32_t guard = (uint32_t)VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE;
    if (ulStackDepth < (page + guard)) {
        return NULL;
    }
    if ((ulStackDepth & (page - 1u)) != 0u) {
        return NULL;
    }
    if ((ulStackDepth & 7u) != 0u) {
        return NULL;
    }
    if (((uintptr_t)puxStackBuffer & 7u) != 0u) {
        return NULL;
    }

    vsf_linux_thread_t *thread = vsf_linux_create_thread(
        NULL, &__vsf_espidf_task_op, ulStackDepth, puxStackBuffer);
    if (thread == NULL) {
        return NULL;
    }

    StaticTask_t *ft = (StaticTask_t *)thread;
    ft->freertos_entry = (void (*)(void *))pxTaskCode;
    ft->arg   = pvParameters;

#if VSF_FREERTOS_CFG_USE_NOTIFY == ENABLED
    vsf_eda_sem_init(&ft->notify_sem, 0, 1);
    ft->notify_value   = 0;
    ft->notify_pending = false;
#endif

    vsf_linux_start_thread(thread, VSF_FREERTOS_CFG_DEFAULT_VSF_PRIO);

    return (TaskHandle_t)ft;
}

/* ---------------------------- vTaskDelete --------------------------------- */

void vTaskDelete(TaskHandle_t xTaskToDelete)
{
    if (xTaskToDelete == NULL) {
        xTaskToDelete = xTaskGetCurrentTaskHandle();
    }
    if (xTaskToDelete != NULL) {
        StaticTask_t *ft = (StaticTask_t *)xTaskToDelete;
        vsf_linux_thread_on_terminate(&ft->use_as__vsf_linux_thread_t);
    }
}

/* ---------------------------- xTaskGetCurrentTaskHandle ------------------- */

TaskHandle_t xTaskGetCurrentTaskHandle(void)
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    return (TaskHandle_t)thread;
}

#endif
