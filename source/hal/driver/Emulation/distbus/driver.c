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

#include "hal/vsf_hal_cfg.h"
#include "./device.h"

#include "hal/driver/vendor_driver.h"
#include "hal/utilities/stream/usart/vsf_usart_stream.h"

#include "service/vsf_service.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HAL_DISTBUS_MSGPOLL_SIZE
#   define VSF_HAL_DISTBUS_MSGPOLL_SIZE         64
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

describe_mem_stream(vsf_distbus_transport_stream_rx, 1024)
describe_mem_stream(vsf_distbus_transport_stream_tx, 1024)

typedef struct __vsf_hal_distbus_msg_t {
    implement(vsf_distbus_msg_t)
    uint8_t buffer[VSF_HAL_DISTBUS_CFG_MTU + vsf_offset_of(vsf_distbus_msg_t, header)];
} __vsf_hal_distbus_msg_t;

dcl_vsf_pool(__vsf_hal_distbus_msg_pool)
def_vsf_pool(__vsf_hal_distbus_msg_pool, __vsf_hal_distbus_msg_t)

typedef struct vsf_hal_distbus_ctx_t {
    vsf_distbus_t distbus;
    vsf_distbus_transport_stream_t transport;
    vsf_usart_stream_t usart_stream;
    vsf_pool(__vsf_hal_distbus_msg_pool) msg_pool;

    vsf_hal_distbus_t hal;

    struct {
#define VSF_HAL_DISTBUS_DEFINE(__TYPE)                                          \
        struct {                                                                \
            uint8_t dev_num;                                                    \
            VSF_MCONNECT(vsf_, __TYPE, _t) *dev[32];                            \
        } __TYPE;

#define __VSF_HAL_DISTBUS_ENUM  VSF_HAL_DISTBUS_DEFINE
#include "./vsf_hal_distbus_enum_with_peripheral_count.inc"
    } chip;

    vsf_arch_irq_request_t irq_request;
} vsf_hal_distbus_ctx_t;

#define VSF_HAL_DISTBUS_PERIPHERAL_TYPE_DEF(__TYPE)                             \
        typedef struct VSF_MCONNECT(vsf_hw_, __TYPE, _t) {                      \
            VSF_MCONNECT(vsf_remapped_, __TYPE, _t) _;                          \
        } VSF_MCONNECT(vsf_hw_, __TYPE, _t);
#define __VSF_HAL_DISTBUS_ENUM  VSF_HAL_DISTBUS_PERIPHERAL_TYPE_DEF
#include "./vsf_hal_distbus_enum_with_peripheral_count.inc"

/*============================ PROTOTYPES ====================================*/

static void * __vsf_hal_distbus_alloc_msg(uint_fast32_t size);
static void __vsf_hal_distbus_free_msg(void *msg);

static void __vsf_hal_distbus_on_error(vsf_distbus_t *distbus);
static void __vsf_hal_distbus_on_remote_connected(vsf_hal_distbus_t *hal_distbus);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static vsf_hal_distbus_ctx_t __vsf_hal_distbus_ctx = {
    .distbus                    = {
        .op                     = {
            .mem                = {
                .alloc_msg      = __vsf_hal_distbus_alloc_msg,
                .free_msg       = __vsf_hal_distbus_free_msg,
            },
            .bus                = {
                .transport      = &__vsf_hal_distbus_ctx.transport,
                .init           = vsf_distbus_transport_stream_init,
                .send           = vsf_distbus_transport_stream_send,
                .recv           = vsf_distbus_transport_stream_recv,
            },
            .on_error           = __vsf_hal_distbus_on_error,
        },
    },
    .hal                        = {
        .on_remote_connected    = __vsf_hal_distbus_on_remote_connected,
    },
    .transport                  = {
        .stream_rx              = &vsf_distbus_transport_stream_rx.use_as__vsf_stream_t,
        .stream_tx              = &vsf_distbus_transport_stream_tx.use_as__vsf_stream_t,
    },
    .usart_stream               = {
        .stream_rx              = &vsf_distbus_transport_stream_rx.use_as__vsf_stream_t,
        .stream_tx              = &vsf_distbus_transport_stream_tx.use_as__vsf_stream_t,
    },
};

#define VSF_HAL_HW_IMPLEMENT(__N, __VALUE)                                      \
        VSF_MCONNECT(vsf_hw_, VSF_HAL_CFG_IMP_TYPE, _t) VSF_MCONNECT(vsf_hw_, VSF_HAL_CFG_IMP_TYPE, __N) = {\
            ._                  = {                                             \
                .op             = &VSF_MCONNECT(vsf_remapped_, VSF_HAL_CFG_IMP_TYPE, _op),\
            },                                                                  \
        };                                                                      \
        void VSF_MCONNECT(VSF_HAL_CFG_IMP_UPCASE_TYPE, __N, _IRQHandler)(void)  \
        {                                                                       \
        }
#define VSF_HAL_HW_IMPLEMENT_ARRAY(__N, __VALUE)                                \
        &VSF_MCONNECT(vsf_hw_, VSF_HAL_CFG_IMP_TYPE, __N),
#define VSF_HAL_HW_IMPLEMENT_MULTI()                                            \
        VSF_MREPEAT(VSF_MCONNECT(VSF_HW_, VSF_HAL_CFG_IMP_UPCASE_TYPE, _COUNT), VSF_HAL_HW_IMPLEMENT, NULL)\
        VSF_MCONNECT(vsf_hw_, VSF_HAL_CFG_IMP_TYPE, _t) *const VSF_MCONNECT(vsf_hw_, VSF_HAL_CFG_IMP_TYPE, s)[VSF_MCONNECT(VSF_HW_, VSF_HAL_CFG_IMP_UPCASE_TYPE, _COUNT)] = {\
            VSF_MREPEAT(VSF_MCONNECT(VSF_HW_, VSF_HAL_CFG_IMP_UPCASE_TYPE, _COUNT), VSF_HAL_HW_IMPLEMENT_ARRAY, NULL)\
        };

#if VSF_HAL_USE_GPIO == ENABLED && VSF_HAL_DISTBUS_USE_GPIO == ENABLED && VSF_HW_GPIO_COUNT > 0
#   undef VSF_HAL_CFG_IMP_TYPE
#   undef VSF_HAL_CFG_IMP_UPCASE_TYPE
#   define VSF_HAL_CFG_IMP_TYPE                 gpio
#   define VSF_HAL_CFG_IMP_UPCASE_TYPE          GPIO
VSF_HAL_HW_IMPLEMENT_MULTI()
#endif

#if VSF_HAL_USE_I2C == ENABLED && VSF_HAL_DISTBUS_USE_I2C == ENABLED && VSF_HW_I2C_COUNT > 0
#   undef VSF_HAL_CFG_IMP_TYPE
#   undef VSF_HAL_CFG_IMP_UPCASE_TYPE
#   define VSF_HAL_CFG_IMP_TYPE                 i2c
#   define VSF_HAL_CFG_IMP_UPCASE_TYPE          I2C
VSF_HAL_HW_IMPLEMENT_MULTI()
#endif

#if VSF_HAL_USE_SPI == ENABLED && VSF_HAL_DISTBUS_USE_SPI == ENABLED && VSF_HW_SPI_COUNT > 0
#   undef VSF_HAL_CFG_IMP_TYPE
#   undef VSF_HAL_CFG_IMP_UPCASE_TYPE
#   define VSF_HAL_CFG_IMP_TYPE                 spi
#   define VSF_HAL_CFG_IMP_UPCASE_TYPE          SPI
VSF_HAL_HW_IMPLEMENT_MULTI()
#endif

#if VSF_HAL_USE_SDIO == ENABLED && VSF_HAL_DISTBUS_USE_SDIO == ENABLED && VSF_HW_SDIO_COUNT > 0
#   undef VSF_HAL_CFG_IMP_TYPE
#   undef VSF_HAL_CFG_IMP_UPCASE_TYPE
#   define VSF_HAL_CFG_IMP_TYPE                 sdio
#   define VSF_HAL_CFG_IMP_UPCASE_TYPE          SDIO
VSF_HAL_HW_IMPLEMENT_MULTI()
#endif

#if VSF_HAL_USE_ADC == ENABLED && VSF_HAL_DISTBUS_USE_ADC == ENABLED && VSF_HW_ADC_COUNT > 0
#   undef VSF_HAL_CFG_IMP_TYPE
#   undef VSF_HAL_CFG_IMP_UPCASE_TYPE
#   define VSF_HAL_CFG_IMP_TYPE                 adc
#   define VSF_HAL_CFG_IMP_UPCASE_TYPE          ADC
VSF_HAL_HW_IMPLEMENT_MULTI()
#endif

#if VSF_HAL_USE_DAC == ENABLED && VSF_HAL_DISTBUS_USE_DAC == ENABLED && VSF_HW_DAC_COUNT > 0
#   undef VSF_HAL_CFG_IMP_TYPE
#   undef VSF_HAL_CFG_IMP_UPCASE_TYPE
#   define VSF_HAL_CFG_IMP_TYPE                 dac
#   define VSF_HAL_CFG_IMP_UPCASE_TYPE          DAC
VSF_HAL_HW_IMPLEMENT_MULTI()
#endif

#if VSF_HAL_USE_PWM == ENABLED && VSF_HAL_DISTBUS_USE_PWM == ENABLED && VSF_HW_PWM_COUNT > 0
#   undef VSF_HAL_CFG_IMP_TYPE
#   undef VSF_HAL_CFG_IMP_UPCASE_TYPE
#   define VSF_HAL_CFG_IMP_TYPE                 pwm
#   define VSF_HAL_CFG_IMP_UPCASE_TYPE          PWM
VSF_HAL_HW_IMPLEMENT_MULTI()
#endif

#if VSF_HAL_USE_I2S == ENABLED && VSF_HAL_DISTBUS_USE_I2S == ENABLED && VSF_HW_I2S_COUNT > 0
#   undef VSF_HAL_CFG_IMP_TYPE
#   undef VSF_HAL_CFG_IMP_UPCASE_TYPE
#   define VSF_HAL_CFG_IMP_TYPE                 i2s
#   define VSF_HAL_CFG_IMP_UPCASE_TYPE          I2S
VSF_HAL_HW_IMPLEMENT_MULTI()
#endif

#if VSF_HAL_USE_USART == ENABLED && VSF_HAL_DISTBUS_USE_USART == ENABLED && VSF_HW_USART_COUNT > 0
#   undef VSF_HAL_CFG_IMP_TYPE
#   undef VSF_HAL_CFG_IMP_UPCASE_TYPE
#   define VSF_HAL_CFG_IMP_TYPE                 usart
#   define VSF_HAL_CFG_IMP_UPCASE_TYPE          USART
VSF_HAL_HW_IMPLEMENT_MULTI()
#endif

/*============================ IMPLEMENTATION ================================*/

imp_vsf_pool(__vsf_hal_distbus_msg_pool, __vsf_hal_distbus_msg_t)

static void __vsf_hal_distbus_on_error(vsf_distbus_t *distbus)
{
    VSF_ASSERT(false);
}

static void * __vsf_hal_distbus_alloc_msg(uint_fast32_t size)
{
    VSF_ASSERT(size <= sizeof(((__vsf_hal_distbus_msg_t *)NULL))->buffer);
    return VSF_POOL_ALLOC(__vsf_hal_distbus_msg_pool, &__vsf_hal_distbus_ctx.msg_pool);
}

static void __vsf_hal_distbus_free_msg(void *msg)
{
    VSF_POOL_FREE(__vsf_hal_distbus_msg_pool, &__vsf_hal_distbus_ctx.msg_pool, msg);
}

static void __vsf_hal_distbus_on_remote_connected(vsf_hal_distbus_t *hal_distbus)
{
    vsf_hal_distbus_start(hal_distbus);
}

void vsf_hal_distbus_on_new(vsf_hal_distbus_t *hal_distbus, vsf_hal_distbus_type_t type, uint8_t num, void *devs)
{
    union {
        void *ptr;
#define VSF_HAL_DISTBUS_DEFINE_DEVS(__TYPE)                                     \
        VSF_MCONNECT(vsf_hal_distbus_, __TYPE, _t) *__TYPE;

#define __VSF_HAL_DISTBUS_ENUM      VSF_HAL_DISTBUS_DEFINE_DEVS
#include "./vsf_hal_distbus_enum_with_peripheral_count.inc"
    } u_devs;
    u_devs.ptr = devs;

    switch (type) {
#define VSF_HAL_DISTBUS_ENUM(__TYPE)                                            \
    case VSF_MCONNECT(VSF_HAL_DISTBUS_, __TYPE):                                \
        if (!__vsf_hal_distbus_ctx.chip.__TYPE.dev_num) {                       \
            __vsf_hal_distbus_ctx.chip.__TYPE.dev_num = vsf_min(num, dimof(__vsf_hal_distbus_ctx.chip.__TYPE.dev));\
            for (uint8_t i = 0; i < __vsf_hal_distbus_ctx.chip.__TYPE.dev_num; i++) {\
                __vsf_hal_distbus_ctx.chip.__TYPE.dev[i] = (VSF_MCONNECT(vsf_, __TYPE, _t) *)&u_devs.__TYPE[i];\
                VSF_MCONNECT(vsf_hw_, __TYPE, s)[i]->_.target = (VSF_MCONNECT(vsf_, __TYPE, _t) *)__vsf_hal_distbus_ctx.chip.__TYPE.dev[i];\
                __vsf_arch_trace(0, "[hal_distbus] new " VSF_STR(__TYPE) "%d %p" VSF_TRACE_CFG_LINEEND, i, __vsf_hal_distbus_ctx.chip.__TYPE.dev[i]);\
            }                                                                   \
        }                                                                       \
        break;

#define __VSF_HAL_DISTBUS_ENUM      VSF_HAL_DISTBUS_ENUM
#include "./vsf_hal_distbus_enum_with_peripheral_count.inc"
    }

    __vsf_arch_trace(0, "distbus slave connected\n");
    __vsf_arch_irq_request_send(&__vsf_hal_distbus_ctx.irq_request);
}

static void __user_hal_distbus_on_remote_connected(vsf_hal_distbus_t *hal_distbus)
{
    vsf_hal_distbus_start(hal_distbus);
}

// IRQ: TODO

void vsf_irq_enable(int irqn)
{
}

void vsf_irq_disable(int irqn)
{
}

bool vsf_irq_is_enabled(int irqn)
{
    return false;
}

void vsf_irq_pend(int irqn)
{
}

void vsf_irq_unpend(int irqn)
{
}

bool vsf_irq_is_pending(int irqn)
{
    return false;
}

void vsf_irq_set_priority(int irqn, uint32_t priority)
{
}

uint32_t vsf_irq_get_priority(int irqn)
{
    return 0;
}

/*! \note initialize device driver
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
VSF_CAL_WEAK(vsf_driver_init)
bool vsf_driver_init(void)
{
    if (!vsf_hostos_driver_init()) {
        return false;
    }

    vsf_hostos_usart_device_t usart_devices[8];
    while (true) {
        uint8_t usart_devnum = vsf_hostos_usart_scan_devices((vsf_hostos_usart_device_t*)&usart_devices, dimof(usart_devices));
        if (0 == usart_devnum) {
            __vsf_arch_trace(0, "[dustbus_hal]: no usart device found in current hostos, which is necessary for distbus hal\n");
            return false;
        }

        if (usart_devnum > 0) {
            __vsf_arch_trace(0, "Plesase select a usart as distbus transport:\n");
            for (uint8_t i = 0; i < usart_devnum; i++) {
                __vsf_arch_trace(0, "\r%d: %s\n", i, usart_devices[i].friendly_name);
            }
            if (usart_devnum > 1) {
                __vsf_arch_trace(0, "input [0 - %d], default 0:", usart_devnum - 1);
            }
            else {
                __vsf_arch_trace(0, "input enter to select the usart0:");
            }

            char buf[32] = { 0 };
            __vsf_arch_console_readline(buf, sizeof(buf));
            int index = atoi(buf);
            if ((index < 0) || (index >= usart_devnum)) {
                continue;
            }

            __vsf_hal_distbus_ctx.usart_stream.usart = usart_devices[index].instance;
            break;
        }
    }

    VSF_POOL_INIT(__vsf_hal_distbus_msg_pool, &__vsf_hal_distbus_ctx.msg_pool, VSF_HAL_DISTBUS_MSGPOLL_SIZE);
    VSF_STREAM_INIT(&vsf_distbus_transport_stream_rx);
    VSF_STREAM_INIT(&vsf_distbus_transport_stream_tx);

    vsf_usart_stream_init(&__vsf_hal_distbus_ctx.usart_stream, &(vsf_usart_cfg_t) {
        .mode               = VSF_USART_8_BIT_LENGTH | VSF_USART_NO_PARITY | VSF_USART_1_STOPBIT | VSF_USART_TX_ENABLE | VSF_USART_RX_ENABLE,
        .baudrate           = 921600,
    });
    vsf_distbus_init(&__vsf_hal_distbus_ctx.distbus);
    vsf_hal_distbus_register(&__vsf_hal_distbus_ctx.distbus, &__vsf_hal_distbus_ctx.hal);

    __vsf_arch_irq_request_init(&__vsf_hal_distbus_ctx.irq_request);
    vsf_distbus_start(&__vsf_hal_distbus_ctx.distbus);
    __vsf_arch_trace(0, "waiting for distbus slave...\n");
    __vsf_arch_irq_request_pend(&__vsf_hal_distbus_ctx.irq_request);
    return true;
}

/* EOF */
