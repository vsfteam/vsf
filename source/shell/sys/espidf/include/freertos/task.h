#ifndef __VSF_ESPIDF_FREERTOS_TASK_H__
#define __VSF_ESPIDF_FREERTOS_TASK_H__

#include "FreeRTOS.h"
#include "shell/sys/linux/vsf_linux.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct StaticTask {
    implement(vsf_linux_thread_t)
    void              (*freertos_entry)(void *);
    void               *arg;
#if VSF_FREERTOS_CFG_USE_NOTIFY == ENABLED
    vsf_sem_t           notify_sem;
    uint32_t            notify_value;
    bool                notify_pending;
#endif
    // is_static and is_stack_static are needed for compatibility only, actually not used
    bool                is_static;
    bool                is_stack_static;
} StaticTask_t;

typedef StaticTask_t *           TaskHandle_t;
typedef void (*TaskFunction_t)(void *);
typedef uint8_t StackType_t;

extern void vTaskDelay(const TickType_t xTicksToDelay);
extern void vTaskDelayUntil(TickType_t * const pxPreviousWakeTime,
                            const TickType_t xTimeIncrement);
extern TickType_t xTaskGetTickCount(void);
extern TickType_t xTaskGetTickCountFromISR(void);
extern void vTaskYield(void);

#define taskYIELD()             vTaskYield()

extern BaseType_t xTaskCreate(TaskFunction_t pxTaskCode,
                              const char * const pcName,
                              const uint32_t usStackDepth,
                              void * const pvParameters,
                              UBaseType_t uxPriority,
                              TaskHandle_t * const pxCreatedTask);

extern TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode,
                                      const char * const pcName,
                                      const uint32_t ulStackDepth,
                                      void * const pvParameters,
                                      UBaseType_t uxPriority,
                                      StackType_t * const puxStackBuffer,
                                      StaticTask_t * const pxTaskBuffer);

extern void vTaskDelete(TaskHandle_t xTaskToDelete);
extern TaskHandle_t xTaskGetCurrentTaskHandle(void);

extern void       vTaskSuspendAll(void);
extern BaseType_t xTaskResumeAll(void);
extern void       vTaskEnterCritical(void);
extern void       vTaskExitCritical(void);
extern UBaseType_t vTaskEnterCriticalFromISR(void);
extern void        vTaskExitCriticalFromISR(UBaseType_t uxSavedInterruptState);

typedef enum {
    eNoAction = 0,
    eSetBits,
    eIncrement,
    eSetValueWithOverwrite,
    eSetValueWithoutOverwrite,
} eNotifyAction;

extern BaseType_t xTaskNotify(TaskHandle_t xTaskToNotify,
                              uint32_t ulValue,
                              eNotifyAction eAction);
extern BaseType_t xTaskNotifyFromISR(TaskHandle_t xTaskToNotify,
                                     uint32_t ulValue,
                                     eNotifyAction eAction,
                                     BaseType_t *pxHigherPriorityTaskWoken);
extern BaseType_t xTaskNotifyGive(TaskHandle_t xTaskToNotify);
extern void       vTaskNotifyGiveFromISR(TaskHandle_t xTaskToNotify,
                                         BaseType_t *pxHigherPriorityTaskWoken);
extern uint32_t   ulTaskNotifyTake(BaseType_t xClearCountOnExit,
                                   TickType_t xTicksToWait);
extern BaseType_t xTaskNotifyWait(uint32_t ulBitsToClearOnEntry,
                                  uint32_t ulBitsToClearOnExit,
                                  uint32_t *pulNotificationValue,
                                  TickType_t xTicksToWait);

#ifdef __cplusplus
}
#endif

#endif
