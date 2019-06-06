/****************************************************************************
*   Copyright (C) 2009 - 2019 by Simon Qian <SimonQian@SimonQian.com>       *
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

#ifndef __VSF_TIMQ_DLIST_H__
#define __VSF_TIMQ_DLIST_H__

/*============================ INCLUDES ======================================*/
#include "kernel/vsf_kernel_cfg.h"

/*============================ MACROS ========================================*/

#define vsf_timq_init(__queue)              vsf_dlist_init(__queue)

#define vsf_timq_insert(__queue, __teda)                                        \
        vsf_dlist_insert(                                                       \
                vsf_teda_t, timer_node,                                         \
                (__queue),                                                      \
                (__teda),                                                       \
                _->due >= (__teda)->due)

#define vsf_timq_remove(__queue, __teda)                                        \
        vsf_dlist_remove(                                                       \
                vsf_teda_t, timer_node,                                         \
                (__queue),                                                      \
                (__teda))

#define vsf_timq_peek(__queue, __teda)                                          \
        vsf_dlist_queue_peek(                                                   \
                vsf_teda_t, timer_node,                                         \
                (__queue),                                                      \
                (__teda))

#define vsf_timq_dequeue(__queue, __teda)                                       \
        vsf_dlist_queue_dequeue(                                                \
                vsf_teda_t, timer_node,                                         \
                (__queue),                                                      \
                (__teda))



#define vsf_callback_timq_init(__queue)     vsf_dlist_init(__queue)

#define vsf_callback_timq_enqueue(__queue, __timer)                             \
        vsf_dlist_queue_enqueue(                                                \
                vsf_callback_timer_t, timer_node,                               \
                (__queue),                                                      \
                (__timer))

#define vsf_callback_timq_insert(__queue, __timer)                              \
        vsf_dlist_insert(                                                       \
                vsf_callback_timer_t, timer_node,                               \
                (__queue),                                                      \
                (__timer),                                                      \
                _->due >= (__timer)->due)

#define vsf_callback_timq_remove(__queue, __timer)                              \
        vsf_dlist_remove(                                                       \
                vsf_callback_timer_t, timer_node,                               \
                (__queue),                                                      \
                (__timer))

#define vsf_callback_timq_peek(__queue, __timer)                                \
        vsf_dlist_queue_peek(                                                   \
                vsf_callback_timer_t, timer_node,                               \
                (__queue),                                                      \
                (__timer))

#define vsf_callback_timq_dequeue(__queue, __timer)                             \
        vsf_dlist_queue_dequeue(                                                \
                vsf_callback_timer_t, timer_node,                               \
                (__queue),                                                      \
                (__timer))

/*============================ TYPES =========================================*/

typedef vsf_dlist_t vsf_timer_queue_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif
