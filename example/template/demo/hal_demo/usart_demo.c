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

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_USART_DEMO == ENABLED && VSF_HAL_USE_USART == ENABLED

/*============================ MACROS ========================================*/

#ifdef APP_USART_DEMO_CFG_USART_PREFIX
#   undef VSF_USART_CFG_PREFIX
#   define VSF_USART_CFG_PREFIX                         APP_USART_DEMO_CFG_USART_PREFIX
#endif

#ifndef APP_USART_DEMO_CFG_DEBUG
#   define APP_USART_DEMO_CFG_DEBUG                     ENABLED
#endif

// If the buffer is small (64 bytes) and the baudrate is big (921600),
// then it may only need to be sent once
#ifndef APP_USART_DEMO_CFG_READ_WRITE_ECHO_SIZE
#   define APP_USART_DEMO_CFG_READ_WRITE_ECHO_SIZE      256
#endif

#ifndef APP_USART_DEMO_CFG_USART
#   define APP_USART_DEMO_CFG_USART                     (vsf_usart_t *)&vsf_hw_usart0
#endif

#ifndef APP_USART_DEMO_CFG_FIFO_POLL_WRITE_TEST
#   define APP_USART_DEMO_CFG_FIFO_POLL_WRITE_TEST      DISABLED
#endif

#ifndef APP_USART_DEMO_CFG_FIFO_ISR_WRITE_TEST
#   define APP_USART_DEMO_CFG_FIFO_ISR_WRITE_TEST       DISABLED
#endif

#ifndef APP_USART_DEMO_CFG_FIFO_ISR_READ_TEST
#   define APP_USART_DEMO_CFG_FIFO_ISR_READ_TEST        DISABLED
#endif

#ifndef APP_USART_DEMO_CFG_FIFO_ECHO_TEST
#   define APP_USART_DEMO_CFG_FIFO_ECHO_TEST            DISABLED
#endif

#ifndef APP_USART_DEMO_CFG_FIFO_WRITE_THEN_READ
#   define APP_USART_DEMO_CFG_FIFO_WRITE_THEN_READ      DISABLED
#endif

#ifndef APP_USART_DEMO_CFG_REQUEST_ECHO_TEST
#   define APP_USART_DEMO_CFG_REQUEST_ECHO_TEST         ENABLED
#endif

#ifndef APP_USART_DEMO_CFG_REQUEST_WRITE_THEN_READ
#   define APP_USART_DEMO_CFG_REQUEST_WRITE_THEN_READ   DISABLED
#endif

#ifndef APP_USART_DEMO_CFG_BAUDRATE
#   define APP_USART_DEMO_CFG_BAUDRATE                  115200
#endif

#ifndef APP_USART_DEMO_CFG_MODE
#   define APP_USART_DEMO_CFG_MODE                      (USART_8_BIT_LENGTH | USART_1_STOPBIT | USART_NO_PARITY | USART_TX_ENABLE | USART_RX_ENABLE)
#endif

#ifndef APP_USART_DEMO_IRQ_PRIO
#   define APP_USART_DEMO_IRQ_PRIO                      vsf_arch_prio_2
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#ifdef APP_USART_DEMO_CFG_USART_EXTERN
    APP_USART_DEMO_CFG_USART_EXTERN
#endif

/*============================ TYPES =========================================*/

typedef struct app_usart_demo_t {
    union {
        uint8_t buff[APP_USART_DEMO_CFG_READ_WRITE_ECHO_SIZE];
        uint8_t txbuff[APP_USART_DEMO_CFG_READ_WRITE_ECHO_SIZE];
    };

#if    (APP_USART_DEMO_CFG_FIFO_WRITE_THEN_READ == ENABLED) \
    || (APP_USART_DEMO_CFG_REQUEST_WRITE_THEN_READ == ENABLED)
    uint8_t rxbuff[APP_USART_DEMO_CFG_READ_WRITE_ECHO_SIZE];
#endif

    volatile bool is_to_exit;
    uint32_t cnt;

#ifdef __WIN__
    bool is_inited;
    uint8_t port_available;
#endif
} app_usart_demo_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static app_usart_demo_t __app_usart_demo = {
    .is_to_exit     = false,
#ifdef __WIN__
    .is_inited      = false,
#endif
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#ifdef __WIN__
static void __update_vsf_usart_win(void)
{
    if (__app_usart_demo.is_inited) {
        return;
    }
    __app_usart_demo.is_inited = true;

    vsf_usart_get_can_used_port(&__app_usart_demo.port_available);

    vsf_usart_win_expression_t com_vsf_usart[VSF_USART_CFG_PORT_NUM] = {0};
    while (!vsf_usart_get_com_num(com_vsf_usart, dimof(com_vsf_usart))) {
        vsf_delay_ms(10);
    }

    for (int i = 0; i < __app_usart_demo.port_available; i++) {
        if (com_vsf_usart[i].win_serial_port_num != 0) {
            vsf_trace(VSF_TRACE_INFO, "com%d---vsf_usart%d\n", com_vsf_usart[i].win_serial_port_num, i);
        }
    }
}
#endif

static vsf_err_t __usart_demo_init(vsf_usart_t * usart,
                                   vsf_usart_isr_handler_t * handler_fn,
                                   void *target_ptr,
                                   vsf_arch_prio_t prio,
                                   em_usart_irq_mask_t mask)
{
    vsf_err_t init_result;

    usart_cfg_t usart_cfg = {
        .mode           = APP_USART_DEMO_CFG_MODE,
        .baudrate       = APP_USART_DEMO_CFG_BAUDRATE,
        .rx_timeout     = 0,
        .isr            = {
            .handler_fn = handler_fn,
            .target_ptr = target_ptr,
            .prio       = prio,
        },
    };

    init_result = vsf_usart_init(usart, &usart_cfg);
    if (init_result != VSF_ERR_NONE) {
        return init_result;
    }

    while (fsm_rt_cpl != vsf_usart_enable(usart));

    if (mask & USART_IRQ_MASK) {
        vsf_usart_irq_enable(usart, mask);
    }

    __app_usart_demo.cnt = 0;
    for (int i = 0; i < sizeof(__app_usart_demo.buff); i++) {
		// 012345678901223....
        __app_usart_demo.buff[i] = (i % 10)+ '0';
    }

    return VSF_ERR_NONE;
}

static void __usart_demo_deinit(vsf_usart_t * usart, em_usart_irq_mask_t mask)
{
    if (mask & USART_IRQ_MASK) {
        vsf_usart_irq_disable(usart, mask);
    }

    while (fsm_rt_cpl != vsf_usart_disable(usart));
}

#if APP_USART_DEMO_CFG_FIFO_POLL_WRITE_TEST == ENABLED
static void __usart_demo_fifo_poll_write(vsf_usart_t * usart)
{
    VSF_ASSERT(usart != NULL);
    app_usart_demo_t * demo = (app_usart_demo_t *)&__app_usart_demo;

    vsf_err_t err = __usart_demo_init(usart, NULL, NULL, APP_USART_DEMO_IRQ_PRIO, 0);
    VSF_ASSERT(err == VSF_ERR_NONE);

    for (uint_fast16_t i = 0; i < sizeof(demo->buff);) {
        i += vsf_usart_fifo_write(usart, &demo->buff[i], sizeof(demo->buff) - i);
    }

    while (!__app_usart_demo.is_to_exit);
    __usart_demo_deinit(usart, 0);
}
#endif

#if APP_USART_DEMO_CFG_FIFO_ISR_WRITE_TEST == ENABLED
static void __usart_write_isr_handler(void *target, vsf_usart_t *usart,
                                      em_usart_irq_mask_t irq_mask)
{
    app_usart_demo_t * demo = (app_usart_demo_t *)target;
    VSF_ASSERT(demo != NULL);

    if (irq_mask & USART_IRQ_MASK_TX) {
        if (demo->cnt < dimof(demo->buff)) {
            demo->cnt += vsf_usart_fifo_write(usart, (void *)&demo->buff[demo->cnt],
                                              dimof(demo->buff) - demo->cnt);
        } else {
            vsf_usart_irq_disable(usart, USART_IRQ_MASK_TX);
            demo->is_to_exit = true;
        }
    }
}

static void __usart_demo_fifo_isr_write(vsf_usart_t * usart)
{
    VSF_ASSERT(usart != NULL);

    app_usart_demo_t * demo = (app_usart_demo_t *)&__app_usart_demo;
    vsf_err_t err = __usart_demo_init(usart, __usart_write_isr_handler,
                                      (void *)demo, APP_USART_DEMO_IRQ_PRIO, USART_IRQ_MASK_TX);
    VSF_ASSERT(err == VSF_ERR_NONE);

    __usart_write_isr_handler((void *)demo, usart, USART_IRQ_MASK_TX);

    while (!__app_usart_demo.is_to_exit);
    __usart_demo_deinit(usart, 0);
}
#endif

#if APP_USART_DEMO_CFG_FIFO_ISR_READ_TEST == ENABLED
static void __usart_read_isr_handler(void *target, vsf_usart_t *usart, em_usart_irq_mask_t irq_mask)
{
    app_usart_demo_t * demo = (app_usart_demo_t *)target;
    VSF_ASSERT(demo != NULL);

    demo->cnt += vsf_usart_fifo_read(usart, (void *)&demo->buff[demo->cnt],
                                    dimof(demo->buff) - demo->cnt);

    if (demo->cnt >= dimof(demo->buff)) {
        vsf_usart_irq_disable(usart, USART_IRQ_MASK_RX_CPL);
        demo->is_to_exit = true;
    }
}

static void __usart_demo_fifo_isr_read(vsf_usart_t * usart)
{
    VSF_ASSERT(usart != NULL);
    app_usart_demo_t * demo = (app_usart_demo_t *)&__app_usart_demo;

    demo->cnt = 0;
    vsf_err_t err = __usart_demo_init(usart, __usart_read_isr_handler,
                                      (void *)demo, APP_USART_DEMO_IRQ_PRIO, USART_IRQ_MASK_RX_CPL);
    VSF_ASSERT(err == VSF_ERR_NONE);

    while (!demo->is_to_exit);

    __usart_demo_deinit(usart, 0);
}
#endif

#if APP_USART_DEMO_CFG_FIFO_ECHO_TEST == ENABLED
static void __usart_fifo_echo(vsf_usart_t * usart)
{
    uint_fast16_t cur_size = 0;
    VSF_ASSERT(usart != NULL);
    app_usart_demo_t * demo = (app_usart_demo_t *)&__app_usart_demo;

    vsf_err_t err = __usart_demo_init(usart, NULL, NULL, APP_USART_DEMO_IRQ_PRIO, 0);
    VSF_ASSERT(err == VSF_ERR_NONE);

    while (!demo->is_to_exit) {
        cur_size = vsf_usart_fifo_read(usart, demo->buff, sizeof(demo->buff));
        for (uint_fast16_t i = 0; i < cur_size;) {
            i += vsf_usart_fifo_write(usart, &demo->buff[i], cur_size - i);
        }
    }

    __usart_demo_deinit(usart, 0);
}
#endif

#if APP_USART_DEMO_CFG_FIFO_WRITE_THEN_READ == ENABLED
static void __usart_fifo_write_then_read(vsf_usart_t * usart)
{
    uint_fast16_t cur_size = 0;
    VSF_ASSERT(usart != NULL);
    app_usart_demo_t * demo = (app_usart_demo_t *)&__app_usart_demo;

    vsf_err_t err = __usart_demo_init(usart, NULL, NULL, APP_USART_DEMO_IRQ_PRIO, 0);
    VSF_ASSERT(err == VSF_ERR_NONE);

    while (!demo->is_to_exit) {
        cur_size = vsf_usart_fifo_write(usart, demo->txbuff, sizeof(demo->txbuff));
        for (uint_fast16_t i = 0; i < cur_size;) {
            i += vsf_usart_fifo_read(usart, &demo->rxbuff[i], cur_size - i);
        }
        for (uint_fast16_t i = 0; i < cur_size; i++) {
            if (demo->rxbuff[i] != demo->txbuff[i]) {
                VSF_ASSERT(false);
            }
        }
    }

    __usart_demo_deinit(usart, 0);
}
#endif

#if APP_USART_DEMO_CFG_REQUEST_ECHO_TEST == ENABLED
static void __usart_isr_handler(void *target,
                                vsf_usart_t *usart,
                                em_usart_irq_mask_t irq_mask)
{
    vsf_err_t err;
    app_usart_demo_t * demo = (app_usart_demo_t *)target;
    int_fast32_t count;

    VSF_ASSERT(demo != NULL);
    VSF_ASSERT(usart != NULL);

    if (irq_mask & USART_IRQ_MASK_RX_CPL) {
        count = vsf_usart_get_rx_count(usart);
        if (count != 0) {
            err = vsf_usart_request_tx(usart, demo->buff, sizeof(demo->buff));
            VSF_ASSERT(VSF_ERR_NONE == err);
        }
    }

    if (irq_mask & USART_IRQ_MASK_TX_CPL) {
        if (demo->is_to_exit) {
            __usart_demo_deinit(usart, USART_IRQ_MASK_RX_CPL | USART_IRQ_MASK_TX_CPL);
        } else {
            err = vsf_usart_request_rx(usart, demo->buff, sizeof(demo->buff));
            VSF_ASSERT(VSF_ERR_NONE == err);
        }
    }
}

static void __usart_request_echo(vsf_usart_t * usart)
{
    vsf_err_t err;
    app_usart_demo_t * demo = (app_usart_demo_t *)&__app_usart_demo;

    VSF_ASSERT(demo != NULL);
    VSF_ASSERT(usart != NULL);

    err = __usart_demo_init(usart, __usart_isr_handler, demo, APP_USART_DEMO_IRQ_PRIO,
                            USART_IRQ_MASK_RX_CPL | USART_IRQ_MASK_TX_CPL);
    VSF_ASSERT(VSF_ERR_NONE == err);

    __usart_isr_handler(demo, usart, USART_IRQ_MASK_TX_CPL);
}
#endif

#if APP_USART_DEMO_CFG_REQUEST_WRITE_THEN_READ_TEST == ENABLED
static void __usart_isr_handler(void *target,
                                vsf_usart_t *usart,
                                em_usart_irq_mask_t irq_mask)
{
    vsf_err_t err;
    app_usart_demo_t * demo = (app_usart_demo_t *)target;

    VSF_ASSERT(demo != NULL);
    VSF_ASSERT(usart != NULL);

    if (irq_mask & USART_IRQ_MASK_RX_CPL) {
        for (uint_fast16_t i = 0; i < sizeof(demo->rxbuff); i++) {
            if (demo->rxbuff[i] != demo->txbuff[i]) {
                VSF_ASSERT(false);
            }
        }

        err = vsf_usart_request_rx(usart, demo->rxbuff, sizeof(demo->rxbuff));
        VSF_ASSERT(VSF_ERR_NONE == err);
        err = vsf_usart_request_tx(usart, demo->txbuff, sizeof(demo->txbuff));
        VSF_ASSERT(VSF_ERR_NONE == err);
    }
    if (irq_mask & USART_IRQ_MASK_TX_CPL) {
        if (demo->is_to_exit) {
            __usart_demo_deinit(usart, USART_IRQ_MASK_RX_CPL | USART_IRQ_MASK_TX_CPL);
        }
    }
}

static void __usart_request_write_then_read(vsf_usart_t * usart)
{
    vsf_err_t err;
    app_usart_demo_t * demo = (app_usart_demo_t *)&__app_usart_demo;

    VSF_ASSERT(demo != NULL);
    VSF_ASSERT(usart != NULL);

    err = __usart_demo_init(usart, __usart_isr_handler, demo, APP_USART_DEMO_IRQ_PRIO,
                            USART_IRQ_MASK_RX_CPL | USART_IRQ_MASK_TX_CPL);
    VSF_ASSERT(VSF_ERR_NONE == err);

    // generate fake rx data to bypass check
    for (uint_fast16_t i = 0; i < sizeof(demo->rxbuff); i++) {
        demo->rxbuff[i] = demo->txbuff[i];
    }
    __usart_isr_handler(demo, usart, USART_IRQ_MASK_TX_CPL);
}
#endif

#if APP_USE_LINUX_DEMO == ENABLED
int usart_main(int argc, char *argv[])
{
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#   endif
#   if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#   endif
#endif

#if __WIN__
    __update_vsf_usart_win();
#endif

#if APP_USART_DEMO_CFG_FIFO_POLL_WRITE_TEST == ENABLED
    __usart_demo_fifo_poll_write(APP_USART_DEMO_CFG_USART);
#endif
#if APP_USART_DEMO_CFG_FIFO_ISR_WRITE_TEST == ENABLED
    __usart_demo_fifo_isr_write(APP_USART_DEMO_CFG_USART);
#endif
#if APP_USART_DEMO_CFG_FIFO_ISR_READ_TEST == ENABLED
    __usart_demo_fifo_isr_read(APP_USART_DEMO_CFG_USART);
#endif
#if APP_USART_DEMO_CFG_FIFO_ECHO_TEST == ENABLED
    __usart_fifo_echo(APP_USART_DEMO_CFG_USART);
#endif
#if APP_USART_DEMO_CFG_FIFO_WRITE_THEN_READ_TEST == ENABLED
    __usart_fifo_write_then_read(APP_USART_DEMO_CFG_USART);
#endif
#if APP_USART_DEMO_CFG_REQUEST_ECHO_TEST == ENABLED
    __usart_request_echo(APP_USART_DEMO_CFG_USART);
#endif
#if APP_USART_DEMO_CFG_REQUEST_WRITE_THEN_READ_TEST == ENABLED
    __usart_request_write_then_read(APP_USART_DEMO_CFG_USART);
#endif

    return 0;
}

#endif
