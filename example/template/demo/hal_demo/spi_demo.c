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

#include "vsf.h"

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_SPI_DEMO == ENABLED && VSF_HAL_USE_SPI == ENABLED

/*============================ MACROS ========================================*/

#ifdef APP_SPI_DEMO_CFG_SPI_PREFIX
#   undef VSF_SPI_CFG_PREFIX
#   define VSF_SPI_CFG_PREFIX                           APP_SPI_DEMO_CFG_SPI_PREFIX
#endif

#ifndef APP_SPI_DEMO_CFG_SPI
#   define APP_SPI_DEMO_CFG_SPI                         (vsf_spi_t *)&vsf_hw_spi0
#endif

#ifndef APP_SPI_DEMO_CFG_DATASIZE
#   define APP_SPI_DEMO_CFG_DATASIZE                    VSF_SPI_DATASIZE_8
#endif

#ifndef APP_SPI_DEMO_CFG_AUTO_CS_EN
#   define APP_SPI_DEMO_CFG_AUTO_CS_EN                  DISABLED
#endif

#define __APP_SPI_DEMO_MODE                             (VSF_SPI_MASTER | VSF_SPI_CLOCK_MODE_3 | VSF_SPI_MSB_FIRST | APP_SPI_DEMO_CFG_DATASIZE)
#if APP_SPI_DEMO_CFG_AUTO_CS_EN == ENABLED
#   define APP_SPI_DEMO_CFG_MODE                        (__APP_SPI_DEMO_MODE | VSF_SPI_AUTO_CS_ENABLE)
#else
#   define APP_SPI_DEMO_CFG_MODE                        (__APP_SPI_DEMO_MODE | VSF_SPI_AUTO_CS_DISABLE)
#endif

#ifndef APP_SPI_DEMO_CFG_SPEED
#   define APP_SPI_DEMO_CFG_SPEED                       (1ul * 1000ul * 1000ul)
#endif

#ifndef APP_SPI_DEMO_IRQ_PRIO
#   define APP_SPI_DEMO_IRQ_PRIO                        vsf_arch_prio_2
#endif

#ifndef APP_SPI_DEMO_CFG_BUFFER_SIZE
#   define APP_SPI_DEMO_CFG_BUFFER_SIZE                 (1024)
#endif

#ifndef APP_SPI_DEMO_CFG_FIFO_SEND_TEST
#   define APP_SPI_DEMO_CFG_FIFO_SEND_TEST              DISABLED
#endif

#ifndef APP_SPI_DEMO_CFG_FIFO_RECV_TEST
#   define APP_SPI_DEMO_CFG_FIFO_RECV_TEST              DISABLED
#endif

#ifndef APP_SPI_DEMO_CFG_FIFO_SEND_RECV_TEST
#   define APP_SPI_DEMO_CFG_FIFO_SEND_RECV_TEST         DISABLED
#endif

#ifndef APP_SPI_DEMO_CFG_REQUEST_SEND_TEST
#   define APP_SPI_DEMO_CFG_REQUEST_SEND_TEST           DISABLED
#endif

#ifndef APP_SPI_DEMO_CFG_REQUEST_RECV_TEST
#   define APP_SPI_DEMO_CFG_REQUEST_RECV_TEST           DISABLED
#endif

#ifndef APP_SPI_DEMO_CFG_REQUEST_SEND_RECV_TEST
#   define APP_SPI_DEMO_CFG_REQUEST_SEND_RECV_TEST      ENABLED
#endif

#if    (APP_SPI_DEMO_CFG_FIFO_SEND_TEST          == ENABLED) \
    || (APP_SPI_DEMO_CFG_FIFO_RECV_TEST          == ENABLED) \
    || (APP_SPI_DEMO_CFG_FIFO_SEND_RECV_TEST     == ENABLED)
#   define APP_SPI_DEMO_CFG_FIFO                        ENABLED
#endif

#if    (APP_SPI_DEMO_CFG_REQUEST_SEND_TEST       == ENABLED) \
    || (APP_SPI_DEMO_CFG_REQUEST_RECV_TEST       == ENABLED) \
    || (APP_SPI_DEMO_CFG_REQUEST_SEND_RECV_TEST  == ENABLED)
#   define APP_SPI_DEMO_CFG_REQUEST                     ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct app_spi_demo_t {
    uint8_t * send_buff;
    uint8_t * recv_buff;
    size_t size;
} app_spi_demo_t;

/*============================ GLOBAL VARIABLES ==============================*/

#ifdef APP_SPI_DEMO_CFG_SPI_EXTERN
    APP_SPI_DEMO_CFG_SPI_EXTERN
#endif

/*============================ LOCAL VARIABLES ===============================*/

#define APP_SPI_DEMO_CFG_USE_FIFO2REQ   ENABLED

#if APP_SPI_DEMO_CFG_USE_FIFO2REQ == ENABLED
#ifndef APP_SPI_DEMO_CFG_USE_FIFO2REQ_NAME
#   define APP_SPI_DEMO_CFG_USE_FIFO2REQ_NAME       vsf_fifo2req_spi0
#endif
/*static*/ describe_fifo2req_spi(vsf_fifo2req_spi0, APP_SPI_DEMO_CFG_SPI);
#undef APP_SPI_DEMO_CFG_SPI
#define APP_SPI_DEMO_CFG_SPI                        (vsf_spi_t *)&APP_SPI_DEMO_CFG_USE_FIFO2REQ_NAME
#endif

#if APP_SPI_DEMO_CFG_USE_MULTIPLEX_CS == ENABLED
#ifndef APP_SPI_DEMO_CFG_USE_MULTIPLEX_CS_NAME
#   define APP_SPI_DEMO_CFG_USE_MULTIPLEX_CS_NAME   vsf_multiplex_cs_spi0
#endif
/*static*/ describe_multiplex_cs_spi(vsf_multiplex_cs_spi0, APP_SPI_DEMO_CFG_SPI);
#undef APP_SPI_DEMO_CFG_SPI
#define APP_SPI_DEMO_CFG_SPI                        (vsf_spi_t *)&APP_SPI_DEMO_CFG_USE_MULTIPLEX_CS_NAME
#endif

/*static*/ app_spi_demo_t __app_spi_demo;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __spi_demo_init(vsf_spi_t * spi,
                                 vsf_spi_isr_handler_t * handler_fn,
                                 void *target_ptr,
                                 vsf_arch_prio_t prio,
                                 vsf_spi_irq_mask_t mask)
{
    vsf_err_t init_result;

    vsf_spi_cfg_t spi_cfg = {
        .mode           = APP_SPI_DEMO_CFG_MODE,
        .clock_hz       = APP_SPI_DEMO_CFG_SPEED,
        .isr            = {
            .handler_fn = handler_fn,
            .target_ptr = target_ptr,
            .prio       = prio,
        },
    };

    init_result = vsf_spi_init(spi, &spi_cfg);
    if (init_result != VSF_ERR_NONE) {
        return init_result;
    }

    while (fsm_rt_cpl != vsf_spi_enable(spi));

    if (mask & VSF_SPI_IRQ_ALL_BITS_MASK) {
        vsf_spi_irq_enable(spi, mask);
    }

    return VSF_ERR_NONE;
}

static void __spi_demo_deinit(vsf_spi_t * spi, vsf_spi_irq_mask_t mask)
{
    if (mask & VSF_SPI_IRQ_ALL_BITS_MASK) {
        vsf_spi_irq_disable(spi, mask);
    }

    while (fsm_rt_cpl != vsf_spi_disable(spi));
}

#if APP_SPI_DEMO_CFG_FIFO == ENABLED
static void __spi_demo_fifo_poll(vsf_spi_t *spi,
                                       void             *send_buff,
                                       void             *recv_buff,
                                       uint_fast32_t     count)
{
    VSF_ASSERT(spi != NULL);

    vsf_err_t err = __spi_demo_init(spi, NULL, NULL, APP_SPI_DEMO_IRQ_PRIO, 0);
    VSF_ASSERT(err == VSF_ERR_NONE);

    uint32_t send_count = 0;
    uint32_t recv_count = 0;

    while (1) {
        vsf_spi_fifo_transfer(spi, send_buff, count, &send_count, recv_buff, count, &recv_count);
        if ((send_count == count) && (recv_count == count)) {
            break;
        }
    }

    __spi_demo_deinit(spi, 0);
}
#endif

#if APP_SPI_DEMO_CFG_FIFO_SEND_TEST == ENABLED
static void __spi_demo_fifo_send_only(vsf_spi_t *spi)
{
    VSF_ASSERT(spi != NULL);
    app_spi_demo_t * demo = (app_spi_demo_t *)&__app_spi_demo;

    __spi_demo_fifo_poll(spi, demo->send_buf, NULL, demo->size);
}
#endif

#if APP_SPI_DEMO_CFG_FIFO_RECV_TEST == ENABLED
static void __spi_demo_fifo_recv_only(vsf_spi_t *spi)
{
    VSF_ASSERT(spi != NULL);
    app_spi_demo_t * demo = (app_spi_demo_t *)&__app_spi_demo;

    __spi_demo_fifo_poll(spi, NULL, demo->recv_buf, demo->size);
}
#endif

#if APP_SPI_DEMO_CFG_FIFO_SEND_RECV_TEST == ENABLED
static void __spi_demo_fifo_send_recv(vsf_spi_t *spi)
{
    VSF_ASSERT(spi != NULL);
    app_spi_demo_t * demo = (app_spi_demo_t *)&__app_spi_demo;

    __spi_demo_fifo_poll(spi, demo->send_buf, demo->recv_buf, demo->size);
}
#endif

#if APP_SPI_DEMO_CFG_REQUEST == ENABLED
static void __spi_request_isr_handler(void              *target,
                                      vsf_spi_t         *spi_ptr,
                                      vsf_spi_irq_mask_t  irq_mask)
{
    app_spi_demo_t * demo = (app_spi_demo_t *)target;
    VSF_ASSERT(demo != NULL);

    if (irq_mask & VSF_SPI_IRQ_MASK_CPL) {
#if APP_SPI_DEMO_CFG_AUTO_CS_EN == DISABLED
        vsf_spi_cs_inactive(spi_ptr, 0);
#endif

        __spi_demo_deinit(spi_ptr, 0);

        vsf_trace_debug("spi request finished" VSF_TRACE_CFG_LINEEND, __DATE__, __TIME__);
        vsf_trace_debug("send buff:" VSF_TRACE_CFG_LINEEND, __DATE__, __TIME__);
        vsf_trace_buffer(VSF_TRACE_DEBUG, demo->send_buff, demo->size);
        vsf_trace_debug("recv buff:" VSF_TRACE_CFG_LINEEND, __DATE__, __TIME__);
        vsf_trace_buffer(VSF_TRACE_DEBUG, demo->recv_buff, demo->size);
    }
}

static void __spi_demo_request(app_spi_demo_t   *demo,
                               vsf_spi_t        *spi_ptr,
                               void             *send_buff,
                               void             *recv_buff,
                               uint_fast32_t     count)
{

    vsf_err_t err = __spi_demo_init(spi_ptr, __spi_request_isr_handler,
                          demo, APP_SPI_DEMO_IRQ_PRIO, VSF_SPI_IRQ_MASK_CPL);
    VSF_ASSERT(err == VSF_ERR_NONE);

#if APP_SPI_DEMO_CFG_AUTO_CS_EN == DISABLED
    vsf_spi_cs_active(spi_ptr, 0);
#endif

    err = vsf_spi_request_transfer(spi_ptr, send_buff, recv_buff, count);
    VSF_ASSERT(err == VSF_ERR_NONE);

    (void)err;
}
#endif

#if APP_SPI_DEMO_CFG_REQUEST_SEND_TEST == ENABLED
static void __spi_demo_request_send_only(vsf_spi_t *spi_ptr)
{
    VSF_ASSERT(spi_ptr != NULL);

    app_spi_demo_t * demo = (app_spi_demo_t *)&__app_spi_demo;
    for (int i = 0; i < demo->size; i++) {
        demo->send_buf[i] = i;
    }
    __spi_demo_request(demo, spi_ptr, demo->send_buf, NULL, demo->size);
}
#endif

#if APP_SPI_DEMO_CFG_REQUEST_RECV_TEST == ENABLED
static void __spi_demo_request_recv_only(vsf_spi_t *spi_ptr)
{
    VSF_ASSERT(spi_ptr != NULL);

    app_spi_demo_t * demo = (app_spi_demo_t *)&__app_spi_demo;
    __spi_demo_request(demo, spi_ptr, NULL, demo->recv_buf, demo->size);
}
#endif

#if APP_SPI_DEMO_CFG_REQUEST_SEND_RECV_TEST == ENABLED
static void __spi_demo_request_send_recv(vsf_spi_t *spi_ptr)
{
    VSF_ASSERT(spi_ptr != NULL);

    app_spi_demo_t * demo = (app_spi_demo_t *)&__app_spi_demo;
    for (int i = 0; i < demo->size; i++) {
        demo->send_buff[i] = i;
    }
    __spi_demo_request(demo, spi_ptr, demo->send_buff, demo->recv_buff, demo->size);
}
#endif

#if APP_USE_LINUX_DEMO == ENABLED
int spi_main(int argc, char *argv[])
{
    size_t size;
    if (argc >= 2) {
        size = atoi(argv[1]);
    } else {
        size = APP_SPI_DEMO_CFG_BUFFER_SIZE;
    }
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#   endif
#   if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#   endif

    size_t size = APP_SPI_DEMO_CFG_BUFFER_SIZE;
#endif

    size *= vsf_spi_get_data_bytes_from_mode(APP_SPI_DEMO_CFG_MODE);
    __app_spi_demo.recv_buff = (uint8_t *)malloc(size);
    if (__app_spi_demo.recv_buff == NULL) {
        return -1;
    }
    __app_spi_demo.send_buff = (uint8_t *)malloc(size);
    if (__app_spi_demo.send_buff == NULL) {
        return -1;
    }
    __app_spi_demo.size = size;

#if APP_SPI_DEMO_CFG_FIFO_SEND_TEST == ENABLED
    __spi_demo_fifo_send_only(APP_SPI_DEMO_CFG_SPI);
#endif

#if APP_SPI_DEMO_CFG_FIFO_RECV_TEST == ENABLED
    __spi_demo_fifo_recv_only(APP_SPI_DEMO_CFG_SPI);
#endif

#if APP_SPI_DEMO_CFG_FIFO_SEND_RECV_TEST == ENABLED
    __spi_demo_fifo_send_recv(APP_SPI_DEMO_CFG_SPI);
#endif

#if APP_SPI_DEMO_CFG_REQUEST_SEND_TEST == ENABLED
    __spi_demo_request_send_only(APP_SPI_DEMO_CFG_SPI);
#endif

#if APP_SPI_DEMO_CFG_REQUEST_RECV_TEST == ENABLED
    __spi_demo_request_recv_only(APP_SPI_DEMO_CFG_SPI);
#endif

#if APP_SPI_DEMO_CFG_REQUEST_SEND_RECV_TEST == ENABLED
    __spi_demo_request_send_recv(APP_SPI_DEMO_CFG_SPI);
#endif

    return 0;
}

#endif
