/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifndef __AIC_AIC8800_RTOS_AL_H__
#define __AIC_AIC8800_RTOS_AL_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"

#include "dbg_assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define RTOS_TASK_FCT(__NAME)               void __NAME(void *param)

#if 1
// IMPORTANT: DO NOT think you can fix the priority,
//  the library provided by vendor use a hardcoded one like below
//  fix the priority in rtos_task_create
#   define RTOS_TASK_PRIORITY(__PRIO)       (__PRIO)
#else
// __PRIO start from 1, and rtos_task MUST support priority promotion,
//  which is required by mutex if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY is enabled
#if     VSF_OS_CFG_ADD_EVTQ_TO_IDLE != ENABLED                                  \
    ||  VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY != ENABLED
#   define RTOS_TASK_PRIORITY(__PRIO)       ((__PRIO) - 1)
#else
#   define RTOS_TASK_PRIORITY(__PRIO)       (__PRIO)
#endif
#endif

#define pdTRUE                              true
#define pdFALSE                             false

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef uint32_t                            TickType_t;
typedef uint32_t                            UBaseType_t;

typedef struct rtos_timer_t *               TimerHandle_t;
typedef void (*TimerCallbackFunction_t)(TimerHandle_t xTimer);

typedef void (*rtos_task_fct)(void *);
typedef struct vsf_rtos_thread_t *          rtos_task_handle;
typedef vsf_prio_t                          rtos_prio;

typedef vsf_sem_t *                         rtos_semaphore;
typedef vsf_mutex_t *                       rtos_mutex;
typedef struct vsf_rtos_queue_t *           rtos_queue;

enum rtos_task_id {
    IDLE_TASK           = 0,
    TMR_DAEMON_TASK     = 1,
    CONSOLE_TASK        = 2,
    TEST_TASK           = 3,
    BT_TASK             = 4,
    INTERSYS_TASK       = 5,
    ASIO_TASK           = 6,
    AUDIO_TASK          = 7,
    WIFI_TASK           = 8,
    CONTROL_TASK        = 9,
    SUPPLICANT_TASK     = 10,
    IP_TASK             = 11,
    APPLICATION_TASK    = 12,
    TG_SEND_TASK        = 13,
    PING_SEND_TASK      = 14,
    IPERF_TASK          = 15,
    SMARTCONF_TASK      = 16,
    IPC_CNTRL_TASK      = 17,
    TCPUDP_FIRST_TASK   = 18,
    TCPUDP_LAST_TASK    = 21,
    RTP_TASK            = 22,
    USBH_TASK           = 23,
    MDNS_TASK           = 24,
    RTCP_TASK           = 25,
    MAX_TASK,
    UNDEF_TASK          = 255,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern int rtos_init(void);

extern void * rtos_malloc(uint32_t size);
extern void rtos_free(void *ptr);

extern uint32_t rtos_now(bool isr);

extern int rtos_task_create(
                        rtos_task_fct func,
                        const char * const name,
                        enum rtos_task_id task_id,
                        const uint16_t stack_depth,
                        void * const params,
                        rtos_prio prio,
                        rtos_task_handle * const task_handle);
extern void rtos_task_suspend(int duration);

extern uint32_t rtos_protect(void);
extern void rtos_unprotect(uint32_t protect);

extern int rtos_semaphore_create(rtos_semaphore *semaphore, int max_count, int init_count);
extern void rtos_semaphore_delete(rtos_semaphore semaphore);
extern int rtos_semaphore_wait(rtos_semaphore semaphore, int timeout);
extern int rtos_semaphore_signal(rtos_semaphore semaphore, bool isr);
extern int rtos_semaphore_get_count(rtos_semaphore semaphore);

extern int rtos_mutex_create(rtos_mutex *mutex);
extern void rtos_mutex_delete(rtos_mutex mutex);
extern int rtos_mutex_lock(rtos_mutex mutex, int timeout);
extern int rtos_mutex_unlock(rtos_mutex mutex);

extern int rtos_queue_create(int elt_size, int nb_elt, rtos_queue *queue);
extern void rtos_queue_delete(rtos_queue queue);
extern int rtos_queue_write(rtos_queue queue, void *msg, int timeout, bool isr);
extern int rtos_queue_read(rtos_queue queue, void *msg, int timeout, bool isr);
extern bool rtos_queue_is_empty(rtos_queue queue);

extern TimerHandle_t rtos_timer_create(
                        const char * const pcTimerName,
                        const TickType_t xTimerPeriodInTicks,
                        const UBaseType_t uxAutoReload,
                        void * const pvTimerID,
                        TimerCallbackFunction_t pxCallbackFunction);
extern int rtos_timer_delete(TimerHandle_t xTimer, TickType_t xTicksToWait);
extern int rtos_timer_start(TimerHandle_t xTimer, TickType_t xTicksToWait, bool isr);
extern int rtos_timer_restart(TimerHandle_t xTimer, TickType_t xTicksToWait, bool isr);
extern int rtos_timer_stop(TimerHandle_t xTimer, TickType_t xTicksToWait);
extern void * rtos_timer_get_pvTimerID(TimerHandle_t xTimer);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
