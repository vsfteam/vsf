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

#include "vsf.h"

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_USART_DEMO == ENABLED && VSF_HAL_USE_USART == ENABLED

/*============================ MACROS ========================================*/

#ifndef APP_USART_DEMO_CFG_DEBUG
#   define APP_USART_DEMO_CFG_DEBUG                     ENABLED
#endif

#ifndef APP_USART_DEMO_CFG_READ_WRITE_ECHO_SIZE
#   define APP_USART_DEMO_CFG_READ_WRITE_ECHO_SIZE      64
#endif

#ifndef APP_USART_DEMO_CFG_USART
#   define APP_USART_DEMO_CFG_USART                     vsf_usart0
#endif

#ifndef APP_USART_DEMO_CFG_FIFO_WRITE_WITH_ISR_TEST
#   define APP_USART_DEMO_CFG_FIFO_WRITE_WITH_ISR_TEST  DISABLED
#endif

#ifndef APP_USART_DEMO_CFG_FIFO_READ_WITH_ISR_TEST
#   define APP_USART_DEMO_CFG_FIFO_READ_WITH_ISR_TEST   DISABLED
#endif

#ifndef APP_USART_DEMO_CFG_FIFO_ECHO_TEST
#   define APP_USART_DEMO_CFG_FIFO_ECHO_TEST            DISABLED
#endif

#ifndef APP_USART_DEMO_CFG_REQUEST_TEST
#   define APP_USART_DEMO_CFG_REQUEST_TEST              ENABLED
#endif

#ifndef APP_USART_DEMO_CFG_TXRX
// if APP_USART_DEMO_CFG_TXRX is enabled, txbuff is initialized, and is used for tx
//  then rxbuff is used for rx
// if APP_USART_DEMO_CFG_TXRX is disabled, rxbuff is used for rx and then for tx
#   define APP_USART_DEMO_CFG_TXRX                      ENABLED
#endif

#ifndef APP_USART_DEMO_CFG_BAUDRATE
#   define APP_USART_DEMO_CFG_BAUDRATE                  115200
#endif

#ifndef APP_USART_DEMO_CFG_MODE
#   define APP_USART_DEMO_CFG_MODE                      (USART_8_BIT_LENGTH | USART_1_STOPBIT | USART_NO_PARITY | USART_TX_EN | USART_RX_EN)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct app_usart_demo_t {
#if APP_USART_DEMO_CFG_TXRX == ENABLED
    uint8_t txbuff[APP_USART_DEMO_CFG_READ_WRITE_ECHO_SIZE];
#endif
    uint8_t rxbuff[APP_USART_DEMO_CFG_READ_WRITE_ECHO_SIZE];
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
    } else {
        __app_usart_demo.is_inited = true;
    }

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
    fsm_rt_t enable_status;

#if APP_USART_DEMO_CFG_TXRX == ENABLED
    for (uint_fast16_t i = 0; i < sizeof(__app_usart_demo.txbuff); i++) {
        __app_usart_demo.txbuff[i] = i & 0xFF;
    }
#endif

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

    do {
        enable_status = vsf_usart_enable(usart);
    } while (enable_status != fsm_rt_cpl);

    if (mask) {
        vsf_usart_irq_enable(usart, mask);
    }

    return VSF_ERR_NONE;
}

static void __usart_demo_disable(vsf_usart_t * usart, em_usart_irq_mask_t mask)
{
    if (mask) {
        vsf_usart_irq_disable(usart, mask);
    }

    fsm_rt_t disalbe_status;
    do {
        disalbe_status = vsf_usart_disable(usart);
    } while (disalbe_status != fsm_rt_cpl);
}

#if APP_USART_DEMO_CFG_FIFO_READ_WITH_ISR_TEST == ENABLED
uint32_t __cnts[100];
uint32_t __cnt_index = 0;

static void __usart_read_isr_handler(void *target, vsf_usart_t *usart, em_usart_irq_mask_t irq_mask)
{
    __app_usart_demo.cnt += vsf_usart_fifo_read(usart, 
                                                (void *)&__app_usart_demo.rxbuff[__app_usart_demo.cnt],
                                                dimof(__app_usart_demo.rxbuff) - __app_usart_demo.cnt);
    
    if (__app_usart_demo.cnt >= dimof(__app_usart_demo.rxbuff)) {
        vsf_usart_irq_disable(usart, USART_IRQ_MASK_RX);
        __app_usart_demo.is_to_exit = true;
    }
}

static void __usart_echo_demo_read_with_isr(vsf_usart_t * usart)
{
    ASSERT(usart != NULL);

    __app_usart_demo.cnt = 0;

    vsf_err_t err = __usart_demo_init(usart, __usart_read_isr_handler, NULL, vsf_arch_prio_0, USART_IRQ_MASK_RX);
    ASSERT (err == VSF_ERR_NONE);

    while (!__app_usart_demo.is_to_exit);

    __usart_demo_disable(usart, 0);
}
#endif

#if APP_USART_DEMO_CFG_FIFO_WRITE_WITH_ISR_TEST == ENABLED
static void __usart_write_isr_handler(void *target,
                                                  vsf_usart_t *usart,
                                                  em_usart_irq_mask_t irq_mask)
{
    if (__app_usart_demo.cnt < dimof(__app_usart_demo.rxbuff)) {
        __app_usart_demo.cnt += vsf_usart_fifo_write(usart,
                                                     (void *)&__app_usart_demo.rxbuff[__app_usart_demo.cnt], 
                                                     dimof(__app_usart_demo.rxbuff) - __app_usart_demo.cnt);
    } else {
        vsf_usart_irq_disable(usart, USART_IRQ_MASK_TX);
        __app_usart_demo.is_to_exit = true;
    }
}

static void __usart_echo_demo_write_with_isr(vsf_usart_t * usart)
{
    ASSERT(usart != NULL);

    __app_usart_demo.cnt = 0;
    for (int i = 0; i < sizeof(__app_usart_demo.rxbuff); i++) {
        __app_usart_demo.rxbuff[i] = i + '!'; // first printable and not empty characters
    }

    vsf_err_t err = __usart_demo_init(usart, __usart_write_isr_handler, NULL, vsf_arch_prio_0, USART_IRQ_MASK_TX);
    ASSERT (err == VSF_ERR_NONE);

    while (!__app_usart_demo.is_to_exit);

    __usart_demo_disable(usart, 0);
}
#endif

#if APP_USART_DEMO_CFG_FIFO_ECHO_TEST == ENABLED
static void __usart_echo_demo_by_fifo(vsf_usart_t * usart)
{
    uint_fast16_t cur_size;
    ASSERT(usart != NULL);

    vsf_err_t err = __usart_demo_init(usart, NULL, NULL, vsf_arch_prio_0, 0);
    ASSERT (err == VSF_ERR_NONE);

    while (!__app_usart_demo.is_to_exit) {
#   if APP_USART_DEMO_CFG_TXRX == ENABLED
        cur_size = vsf_usart_fifo_write(usart, __app_usart_demo.txbuff, sizeof(__app_usart_demo.txbuff));
        for (uint_fast16_t i = 0; i < cur_size;) {
            i += vsf_usart_fifo_read(usart, &__app_usart_demo.rxbuff[i], sizeof(__app_usart_demo.rxbuff) - i);
        }
        for (uint_fast16_t i = 0; i < cur_size; i++) {
            if (__app_usart_demo.rxbuff[i] != __app_usart_demo.txbuff[i]) {
                ASSERT(false);
            }
        }
#   else
        cur_size = vsf_usart_fifo_read(usart, __app_usart_demo.rxbuff, sizeof(__app_usart_demo.rxbuff));
        for (uint_fast16_t i = 0; i < cur_size;) {
            i += vsf_usart_fifo_write(usart, &__app_usart_demo.rxbuff[i], sizeof(__app_usart_demo.rxbuff) - i);
        }
#   endif
    }

    __usart_demo_disable(usart, 0);
}
#endif

#if APP_USART_DEMO_CFG_REQUEST_TEST == ENABLED
static void __usart_isr_handler(void *target,
                                vsf_usart_t *usart,
                                em_usart_irq_mask_t irq_mask)
{
    vsf_err_t err;

    ASSERT(usart != NULL);

    if (irq_mask & USART_IRQ_MASK_RX_CPL) {
#if APP_USART_DEMO_CFG_TXRX == ENABLED
        for (uint_fast16_t i = 0; i < sizeof(__app_usart_demo.rxbuff); i++) {
            if (__app_usart_demo.rxbuff[i] != __app_usart_demo.txbuff[i]) {
                ASSERT(false);
            }
        }

        err = vsf_usart_request_rx(usart, __app_usart_demo.rxbuff, sizeof(__app_usart_demo.rxbuff));
        ASSERT(VSF_ERR_NONE == err);
        err = vsf_usart_request_tx(usart, __app_usart_demo.txbuff, sizeof(__app_usart_demo.txbuff));
        ASSERT(VSF_ERR_NONE == err);
#else
        err = vsf_usart_request_tx(usart, __app_usart_demo.rxbuff, sizeof(__app_usart_demo.rxbuff));
        ASSERT(VSF_ERR_NONE == err);
#endif
    }
    if (irq_mask & USART_IRQ_MASK_TX_CPL) {
        if (__app_usart_demo.is_to_exit) {
            __usart_demo_disable(usart, USART_IRQ_MASK_RX_CPL | USART_IRQ_MASK_TX_CPL);
        } else {
#if APP_USART_DEMO_CFG_TXRX != ENABLED
            err = vsf_usart_request_rx(usart, __app_usart_demo.rxbuff, sizeof(__app_usart_demo.rxbuff));
            ASSERT(VSF_ERR_NONE == err);
#endif
        }
    }
}

static void __usart_echo_demo_by_request(vsf_usart_t * usart)
{
    vsf_err_t err;

    ASSERT(usart != NULL);

    err = __usart_demo_init(usart, __usart_isr_handler, NULL, vsf_arch_prio_0,
                            USART_IRQ_MASK_RX_CPL | USART_IRQ_MASK_TX_CPL);
    ASSERT (VSF_ERR_NONE == err);

#if APP_USART_DEMO_CFG_TXRX == ENABLED
    // generate fake rx data to bypass check
    for (uint_fast16_t i = 0; i < sizeof(__app_usart_demo.rxbuff); i++) {
        __app_usart_demo.rxbuff[i] = __app_usart_demo.txbuff[i];
    }
    __usart_isr_handler(NULL, usart, USART_IRQ_MASK_RX_CPL);
#else
    __usart_isr_handler(NULL, usart, USART_IRQ_MASK_TX_CPL);
#endif
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

#if APP_USART_DEMO_CFG_FIFO_READ_WITH_ISR_TEST == ENABLED
    __usart_echo_demo_read_with_isr(&APP_USART_DEMO_CFG_USART);
#elif APP_USART_DEMO_CFG_FIFO_WRITE_WITH_ISR_TEST == ENABLED
    __usart_echo_demo_write_with_isr(&APP_USART_DEMO_CFG_USART);
#elif APP_USART_DEMO_CFG_FIFO_ECHO_TEST == ENABLED
    __usart_echo_demo_by_fifo(&APP_USART_DEMO_CFG_USART);
#elif APP_USART_DEMO_CFG_REQUEST_TEST == ENABLED
    __usart_echo_demo_by_request(&APP_USART_DEMO_CFG_USART);
#endif

    return 0;
}

#endif
