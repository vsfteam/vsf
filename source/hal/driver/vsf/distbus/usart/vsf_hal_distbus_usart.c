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

#if VSF_HAL_USE_USART == ENABLED && VSF_HAL_DISTBUS_USE_USART == ENABLED

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_HAL_DISTBUS_USART_CLASS_IMPLEMENT
#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/

#if VSF_USE_SIMPLE_STREAM != ENABLED
#   error vsf_hal_distbus_usart need simple_stream
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef VSF_HAL_DISTBUS_USART_CFG_PROTECT_LEVEL
#   define VSF_HAL_DISTBUS_USART_CFG_PROTECT_LEVEL  interrupt
#endif

#define __vsf_usart_protect                         vsf_protect(VSF_HAL_DISTBUS_USART_CFG_PROTECT_LEVEL)
#define __vsf_usart_unprotect                       vsf_unprotect(VSF_HAL_DISTBUS_USART_CFG_PROTECT_LEVEL)

/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static bool __vsf_hal_distbus_usart_msghandler(vsf_distbus_t *distbus, vsf_distbus_service_t *service, vsf_distbus_msg_t *msg);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const vsf_distbus_service_info_t __vsf_hal_distbus_usart_info = {
    .mtu                = VSF_HAL_DISTBUS_CFG_MTU,
    .type               = 0,
    .addr_range         = VSF_HAL_DISTBUS_USART_CMD_ADDR_RANGE,
    .handler            = __vsf_hal_distbus_usart_msghandler,
};

#define VSF_USART_CFG_IMP_PREFIX                    vsf_hal_distbus
#define VSF_USART_CFG_IMP_UPCASE_PREFIX             VSF_HAL_DISTBUS
#define VSF_USART_CFG_REIMPLEMENT_API_REQUEST       DISABLED
#define VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY    ENABLED
#include "hal/driver/common/usart/usart_template.inc"


/*============================ IMPLEMENTATION ================================*/

static void __vsf_hal_distbus_usart_tx(vsf_hal_distbus_usart_t *usart, uint8_t *buffer, uint16_t size)
{
    uint8_t *param;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usart->distbus, size, &param);
    VSF_HAL_ASSERT(msg != NULL);

    msg->header.addr = VSF_HAL_DISTBUS_USART_CMD_TX;
    memcpy(param, buffer, size);
    vsf_distbus_send_msg(usart->distbus, &usart->service, msg);
}

static void __vsf_hal_distbus_usart_txfifo(vsf_hal_distbus_usart_t *usart, vsf_protect_t orig)
{
    bool is_pending = usart->fifo.tx.is_pending;
    usart->fifo.tx.is_pending = true;
    __vsf_usart_unprotect(orig);

    if (!is_pending) {
        uint8_t *buffer;
        uint_fast32_t size = vsf_stream_get_rbuf(&usart->fifo.tx.stream.use_as__vsf_stream_t, &buffer);

        if (size > 0) {
            size = vsf_min(size, VSF_HAL_DISTBUS_CFG_MTU);
            __vsf_hal_distbus_usart_tx(usart, buffer, size);
        }
    }
}

static bool __vsf_hal_distbus_usart_msghandler(vsf_distbus_t *distbus, vsf_distbus_service_t *service, vsf_distbus_msg_t *msg)
{
    vsf_hal_distbus_usart_t *usart = vsf_container_of(service, vsf_hal_distbus_usart_t, service);
    uint8_t *data = (uint8_t *)&msg->header + sizeof(msg->header);
    uint32_t datalen = msg->header.datalen;
    bool retain_msg = false;

    union {
        void *ptr;
        vsf_hal_distbus_usart_isr_t *isr;
        vsf_hal_distbus_usart_txed_cnt_t *txed_cnt;
    } u_arg;

    u_arg.ptr = (uint8_t *)&msg->header + sizeof(msg->header);
    switch (msg->header.addr) {
    case VSF_HAL_DISTBUS_USART_CMD_TXED_COUNT:
        VSF_HAL_ASSERT(datalen == sizeof(*u_arg.txed_cnt));
        vsf_stream_read(&usart->fifo.tx.stream.use_as__vsf_stream_t, NULL, le32_to_cpu(u_arg.txed_cnt->count));

        {
            vsf_protect_t orig = __vsf_usart_protect();
            usart->fifo.tx.is_pending = false;
            __vsf_hal_distbus_usart_txfifo(usart, orig);
        }
        break;
    case VSF_HAL_DISTBUS_USART_CMD_RX:
        VSF_HAL_ASSERT(datalen > 0);
        if (datalen < vsf_stream_write(&usart->fifo.rx.stream.use_as__vsf_stream_t, data, datalen)) {
            usart->irq.triggered_mask = VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR & usart->irq.enabled_mask;
            if (usart->irq.triggered_mask && !vsf_hal_distbus_on_irq(usart, usart->irq.no) && (usart->irq.handler != NULL)) {
               usart->irq.handler(usart->irq.target, (vsf_usart_t *)usart, usart->irq.triggered_mask);
            }
        }
        break;
    case VSF_HAL_DISTBUS_USART_CMD_ISR:
        VSF_HAL_ASSERT(datalen == sizeof(*u_arg.isr));
        usart->irq.triggered_mask = le32_to_cpu(u_arg.isr->irq_mask) & usart->irq.enabled_mask;
        if (usart->irq.triggered_mask && !vsf_hal_distbus_on_irq(usart, usart->irq.no) && (usart->irq.handler != NULL)) {
            usart->irq.handler(usart->irq.target, (vsf_usart_t *)usart, usart->irq.triggered_mask);
        }
        break;
    default:
        VSF_HAL_ASSERT(false);
        break;
    }
    return retain_msg;
}

static void __vsf_hal_distbus_usart_txfifo_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vsf_hal_distbus_usart_t *usart = param;
    if (VSF_STREAM_ON_IN == evt) {
        __vsf_hal_distbus_usart_txfifo(usart, __vsf_usart_protect());
    }
}

uint32_t vsf_hal_distbus_usart_register_service(vsf_distbus_t *distbus, vsf_hal_distbus_usart_t *usart, void *info, uint32_t infolen)
{
    VSF_HAL_ASSERT(0 == infolen);

    usart->distbus = distbus;
    usart->service.info = &__vsf_hal_distbus_usart_info;
#if VSF_HAL_DISTBUS_USART_CFG_MULTI_CLASS == ENABLED
    usart->op = &vsf_fifo2req_usart_op;
#endif

    usart->fifo.rx.stream.align = 0;
    usart->fifo.rx.stream.op = &vsf_mem_stream_op;
    usart->fifo.rx.stream.buffer = usart->fifo.rx.buffer;
    usart->fifo.rx.stream.size = dimof(usart->fifo.rx.buffer);
    usart->fifo.tx.stream.align = 0;
    usart->fifo.tx.stream.op = &vsf_mem_stream_op;
    usart->fifo.tx.stream.buffer = usart->fifo.tx.buffer;
    usart->fifo.tx.stream.size = dimof(usart->fifo.tx.buffer);
    usart->fifo.tx.stream.rx.evthandler = __vsf_hal_distbus_usart_txfifo_evthandler;
    usart->fifo.tx.stream.rx.param = usart;
    vsf_stream_init(&usart->fifo.rx.stream.use_as__vsf_stream_t);
    vsf_stream_init(&usart->fifo.tx.stream.use_as__vsf_stream_t);
    vsf_stream_connect_rx(&usart->fifo.tx.stream.use_as__vsf_stream_t);

    vsf_distbus_register_service(distbus, &usart->service);
    return 0;
}

vsf_err_t vsf_hal_distbus_usart_init(vsf_hal_distbus_usart_t *usart, vsf_usart_cfg_t *cfg)
{
    VSF_HAL_ASSERT(NULL != usart);
    VSF_HAL_ASSERT(NULL != cfg);

    vsf_hal_distbus_usart_init_t *param;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usart->distbus, sizeof(*param), (uint8_t **)&param);
    VSF_HAL_ASSERT(msg != NULL);

    usart->irq.handler = cfg->isr.handler_fn;
    usart->irq.target = cfg->isr.target_ptr;

    msg->header.addr = VSF_HAL_DISTBUS_USART_CMD_INIT;
    param->mode = cpu_to_le32(cfg->mode);
    param->baudrate = cpu_to_le32(cfg->baudrate);
    param->rx_timeout = cpu_to_le32(cfg->rx_timeout);
    vsf_distbus_send_msg(usart->distbus, &usart->service, msg);
    return VSF_ERR_NONE;
}

void vsf_hal_distbus_usart_fini(vsf_hal_distbus_usart_t *usart)
{

}

fsm_rt_t vsf_hal_distbus_usart_enable(vsf_hal_distbus_usart_t *usart)
{
    VSF_HAL_ASSERT(NULL != usart);

    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usart->distbus, 0, NULL);
    VSF_HAL_ASSERT(msg != NULL);

    msg->header.addr = VSF_HAL_DISTBUS_USART_CMD_ENABLE;
    vsf_distbus_send_msg(usart->distbus, &usart->service, msg);
    return fsm_rt_cpl;
}

fsm_rt_t vsf_hal_distbus_usart_disable(vsf_hal_distbus_usart_t *usart)
{
    VSF_HAL_ASSERT(NULL != usart);

    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usart->distbus, 0, NULL);
    VSF_HAL_ASSERT(msg != NULL);

    msg->header.addr = VSF_HAL_DISTBUS_USART_CMD_DISABLE;
    vsf_distbus_send_msg(usart->distbus, &usart->service, msg);
    return fsm_rt_cpl;
}

void vsf_hal_distbus_usart_irq_enable(vsf_hal_distbus_usart_t *usart, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != usart);
    VSF_HAL_ASSERT(0 != irq_mask);
    usart->irq.enabled_mask |= irq_mask;

    vsf_hal_distbus_usart_isr_t *param;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usart->distbus, sizeof(*param), (uint8_t **)&param);
    VSF_HAL_ASSERT(msg != NULL);

    msg->header.addr = VSF_HAL_DISTBUS_USART_CMD_IRQ_ENABLE;
    param->irq_mask = cpu_to_le32(irq_mask);
    vsf_distbus_send_msg(usart->distbus, &usart->service, msg);
}

void vsf_hal_distbus_usart_irq_disable(vsf_hal_distbus_usart_t *usart, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != usart);
    VSF_HAL_ASSERT(0 != irq_mask);
    usart->irq.enabled_mask &= ~irq_mask;

    vsf_hal_distbus_usart_isr_t *param;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(usart->distbus, sizeof(*param), (uint8_t **)&param);
    VSF_HAL_ASSERT(msg != NULL);

    msg->header.addr = VSF_HAL_DISTBUS_USART_CMD_IRQ_DISABLE;
    param->irq_mask = cpu_to_le32(irq_mask);
    vsf_distbus_send_msg(usart->distbus, &usart->service, msg);
}

vsf_usart_capability_t vsf_hal_distbus_usart_capability(vsf_hal_distbus_usart_t *usart)
{
    return (vsf_usart_capability_t){ 0 };
}

vsf_usart_status_t vsf_hal_distbus_usart_status(vsf_hal_distbus_usart_t *usart)
{
    VSF_HAL_ASSERT(NULL != usart);
    vsf_usart_status_t status = {
        .is_busy = vsf_stream_get_data_size(&usart->fifo.tx.stream.use_as__vsf_stream_t) > 0,
    };
    return status;
}

uint_fast16_t vsf_hal_distbus_usart_rxfifo_get_data_count(vsf_hal_distbus_usart_t *usart)
{
    VSF_HAL_ASSERT(NULL != usart);
    return vsf_stream_get_data_size(&usart->fifo.rx.stream.use_as__vsf_stream_t);
}

uint_fast16_t vsf_hal_distbus_usart_rxfifo_read(vsf_hal_distbus_usart_t *usart, void *buffer, uint_fast16_t count)
{
    VSF_HAL_ASSERT(NULL != usart);
    VSF_HAL_ASSERT(NULL != buffer && count > 0);
    return vsf_stream_read(&usart->fifo.rx.stream.use_as__vsf_stream_t, buffer, count);
}

uint_fast16_t vsf_hal_distbus_usart_txfifo_get_free_count(vsf_hal_distbus_usart_t *usart)
{
    VSF_HAL_ASSERT(NULL != usart);
    return vsf_stream_get_free_size(&usart->fifo.tx.stream.use_as__vsf_stream_t);
}

uint_fast16_t vsf_hal_distbus_usart_txfifo_write(vsf_hal_distbus_usart_t *usart, void *buffer, uint_fast16_t count)
{
    VSF_HAL_ASSERT(NULL != usart);
    VSF_HAL_ASSERT(NULL != buffer && count > 0);
    return vsf_stream_write(&usart->fifo.tx.stream.use_as__vsf_stream_t, buffer, count);
}

#endif
