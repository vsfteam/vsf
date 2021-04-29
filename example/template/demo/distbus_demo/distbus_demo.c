/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

/*============================ INCLUDES ======================================*/

#define __VSF_DISTBUS_CLASS_INHERIT__
#include "vsf.h"

#include "../common/usrapp_common.h"

#if VSF_USE_DISTBUS == ENABLED && APP_USE_DISTBUS_DEMO == ENABLED

/*============================ MACROS ========================================*/

#ifndef APP_DISTBUS_DEMO_CFG_MTU
#   define APP_DISTBUS_DEMO_CFG_MTU         1024
#endif
#ifndef APP_DISTBUS_DEMO_CFG_POOL_NUM
#   define APP_DISTBUS_DEMO_CFG_POOL_NUM    16
#endif

#define __APP_DISTBUS_DEMO_MTU              (APP_DISTBUS_DEMO_CFG_MTU + sizeof(vsf_distbus_msg_t))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct __user_distbus_msg_t {
    implement(vsf_distbus_msg_t)
    uint8_t buffer[__APP_DISTBUS_DEMO_MTU];
} __user_distbus_msg_t;

dcl_vsf_pool(__user_distbus_msg_pool)
def_vsf_pool(__user_distbus_msg_pool, __user_distbus_msg_t)

typedef struct __user_distbus_t {
    vsf_distbus_t                           distbus;
    vsf_pool(__user_distbus_msg_pool)       msg_pool;
} __user_distbus_t;

/*============================ PROTOTYPES ====================================*/

static void __user_distbus_on_error(vsf_distbus_t *distbus);
static void * __user_distbus_alloc_msg(uint_fast32_t size);
static void __user_distbus_free_msg(void *msg);

extern bool __user_distbus_init(void *p, void (*on_inited)(void *p));
extern bool __user_distbus_send(uint8_t *buffer, uint_fast32_t size, void *p, void (*on_sent)(void *p));
extern bool __user_distbus_recv(uint8_t *buffer, uint_fast32_t size, void *p, void (*on_recv)(void *p));

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static __user_distbus_t __user_distbus = {
    .distbus                    = {
        .op                     = {
            .mem                = {
                .alloc_msg      = __user_distbus_alloc_msg,
                .free_msg       = __user_distbus_free_msg,
            },
            .bus                = {
                .init           = __user_distbus_init,
                .send           = __user_distbus_send,
                .recv           = __user_distbus_recv,
            },
            .on_error           = __user_distbus_on_error,
        },
    },
};

/*============================ IMPLEMENTATION ================================*/

imp_vsf_pool(__user_distbus_msg_pool, __user_distbus_msg_t)

static void __user_distbus_on_error(vsf_distbus_t *distbus)
{
    VSF_ASSERT(false);
}

static void * __user_distbus_alloc_msg(uint_fast32_t size)
{
    VSF_ASSERT(size <= __APP_DISTBUS_DEMO_MTU);
    return VSF_POOL_ALLOC(__user_distbus_msg_pool, &__user_distbus.msg_pool);
}

static void __user_distbus_free_msg(void *msg)
{
    VSF_POOL_FREE(__user_distbus_msg_pool, &__user_distbus.msg_pool, msg);
}

#if APP_USE_LINUX_DEMO == ENABLED
int distbus_main(int argc, char *argv[])
{
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#       if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#       endif
#   endif
#endif

#if     APP_USE_DISTBUS_HAL_SLAVE_DEMO == ENABLED
    extern void __user_distbus_hal_service_init(vsf_distbus_t *distbus);
    __user_distbus_hal_service_init(&__user_distbus.distbus);
#elif   APP_USE_DISTBUS_HAL_MASTER_DEMO == ENABLED
    usrapp_usbd_common.hal_distbus.distbus = &__user_distbus.distbus;
    vsf_hal_distbus_register_service(&usrapp_usbd_common.hal_distbus);
#endif

    VSF_POOL_INIT(__user_distbus_msg_pool, &__user_distbus.msg_pool, APP_DISTBUS_DEMO_CFG_POOL_NUM);
    vsf_distbus_init(&__user_distbus.distbus);

#if APP_USE_LINUX_DEMO != ENABLED
    while (1) {
        vsf_arch_sleep(0);
    }
#endif

    return 0;
}

#endif
