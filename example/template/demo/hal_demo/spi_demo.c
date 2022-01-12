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

#ifndef APP_SPI_DEMO_CFG_DEBUG
#   define APP_SPI_DEMO_CFG_DEBUG                       ENABLED
#endif

#ifndef APP_SPI_DEMO_CFG_SPI
#   define APP_SPI_DEMO_CFG_SPI                         (vsf_spi_t *)&vsf_spi0
#endif

#ifndef APP_SPI_DEMO_CFG_MODE
#   define APP_SPI_DEMO_CFG_MODE                        (SPI_MASTER | SPI_MODE_0 | SPI_MSB_FIRST | SPI_AUTO_CS_DISABLE | SPI_DATASIZE_8)
#endif

#ifndef APP_SPI_DEMO_CFG_SPEED
#   define APP_SPI_DEMO_CFG_SPEED                       (1ul * 1000ul * 1000ul)
#endif

#ifndef APP_SPI_DEMO_IRQ_PRIO
#   define APP_SPI_DEMO_IRQ_PRIO                        vsf_arch_prio_2
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct app_spi_demo_t {
    volatile bool is_to_exit;
} app_spi_demo_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static app_spi_demo_t __app_spi_demo = {
    .is_to_exit     = false,
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __spi_demo_init(vsf_spi_t * spi,
                                 vsf_spi_isr_handler_t * handler_fn,
                                 void *target_ptr,
                                 vsf_arch_prio_t prio,
                                 em_spi_irq_mask_t mask)
{
    vsf_err_t init_result;

    spi_cfg_t spi_cfg = {
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

    if (mask & SPI_IRQ_MASK) {
        vsf_spi_irq_enable(spi, mask);
    }

    return VSF_ERR_NONE;
}

static void __spi_demo_disable(vsf_spi_t * spi, em_spi_irq_mask_t mask)
{
    if (mask & SPI_IRQ_MASK) {
        vsf_spi_irq_disable(spi, mask);
    }

    while (fsm_rt_cpl != vsf_spi_disable(spi));
}

static void __spi_demo_fifo_poll_write(vsf_spi_t * spi)
{
    VSF_ASSERT(spi != NULL);
    app_spi_demo_t * demo = (app_spi_demo_t *)&__app_spi_demo;

    vsf_err_t err = __spi_demo_init(spi, NULL, NULL, APP_SPI_DEMO_IRQ_PRIO, 0);
    VSF_ASSERT(err == VSF_ERR_NONE);

    __spi_demo_disable(spi, 0);
}

#if APP_USE_LINUX_DEMO == ENABLED
int spi_main(int argc, char *argv[])
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

    return 0;
}

#endif
