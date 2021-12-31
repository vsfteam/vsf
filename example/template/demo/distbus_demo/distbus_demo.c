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
    bool                                    is_master;
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

bool __user_distbus_is_master(void)
{
    return __user_distbus.is_master;
}

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

static void __user_distbus_on_connected(vsf_distbus_t *distbus)
{
    if (__user_distbus_is_master()) {
    } else {
    }
}

int distbus_main(int argc, char *argv[])
{
    if ((argc < 2) || (argv[1][0] != '-')) {
    print_help:
        printf("format: %s -s/-m\r\n", argv[0]);
        return -1;
    }
    switch (argv[1][1]) {
    case 's':
        __user_distbus.is_master = false;
        break;
    case 'm':
        __user_distbus.is_master = true;
        break;
    default:
        goto print_help;
    }

    if (__user_distbus_is_master()) {
#if APP_USE_DISTBUS_HAL_DEMO == ENABLED
        usrapp_usbd_common.hal_distbus.distbus = &__user_distbus.distbus;
        vsf_hal_distbus_register_service(&usrapp_usbd_common.hal_distbus);
#endif
    } else {
#if APP_USE_DISTBUS_HAL_DEMO == ENABLED
        extern void __user_distbus_hal_service_init(vsf_distbus_t *distbus);
        __user_distbus_hal_service_init(&__user_distbus.distbus);
#endif
    }

    VSF_POOL_INIT(__user_distbus_msg_pool, &__user_distbus.msg_pool, APP_DISTBUS_DEMO_CFG_POOL_NUM);
    __user_distbus.distbus.on_connected = __user_distbus_on_connected;
    vsf_distbus_init(&__user_distbus.distbus);

    return 0;
}

#endif
