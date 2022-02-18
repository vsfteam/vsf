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

#define __VSF_MULTIPLEX_SPI_CLASS_IMPLEMENT
#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_SPI == ENABLED

#include "hal/driver/driver.h"

/*============================ MACROS ========================================*/

#define VSF_MULTIPLEXER_SPI_MAX_CS_COUNT            (sizeof(SPI_MULTIPLEXER_MASK_TYPE) * 8)

#ifndef VSF_MULTIPLEXER_SPI_CFG_GPIO_FEATURE
#   define VSF_MULTIPLEXER_SPI_CFG_GPIO_FEATURE     (IO_PULL_UP)
#endif

#ifndef VSF_MULTIPLEXER_SPI_PROTECT_LEVEL
#   define VSF_MULTIPLEXER_SPI_PROTECT_LEVEL        interrupt
#endif

#define vsf_multiplex_spi_protect                   vsf_protect(VSF_MULTIPLEXER_SPI_PROTECT_LEVEL)
#define vsf_multiplex_spi_unprotect                 vsf_unprotect(VSF_MULTIPLEXER_SPI_PROTECT_LEVEL)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static bool __spi_mode_is_auto_cs(em_spi_mode_t mode)
{
    return (mode & SPI_AUTO_CS_MASK == SPI_AUTO_CS_ENABLE);
}

static bool __spi_cs_pin_is_hardware(vsf_multiplex_spi_t *m_spi_ptr)
{
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);

    vsf_spi_t *spi = spi_info_ptr->spi;
    VSF_HAL_ASSERT(spi != 0);

    spi_capability_t spi_capability = vsf_spi_capability(spi);

    return (m_spi_ptr->cs_index < spi_capability.cs_count);
}

static void __spi_pop_and_next(vsf_multiplex_spi_t *m_spi_ptr)
{
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);

    vsf_slist_queue_dequeue(vsf_multiplex_spi_t, node, &spi_info_ptr->list, m_spi_ptr);

    if (!vsf_slist_queue_is_empty(&spi_info_ptr->list)) {
        vsf_multiplex_spi_t *cur_m_spi_ptr;
        vsf_slist_queue_peek(vsf_multiplex_spi_t, node, &spi_info_ptr->list, cur_m_spi_ptr);
        vsf_spi_request_transfer(spi_info_ptr->spi,
                                 cur_m_spi_ptr->request.out_buffer_ptr,
                                 cur_m_spi_ptr->request.in_buffer_ptr,
                                 cur_m_spi_ptr->request.count);
    }
}

static void __vsf_multiplex_spi_irq_handler(void *target_ptr, vsf_spi_t *spi_ptr, em_spi_irq_mask_t irq_mask)
{
    vsf_multiplex_spi_t *m_spi_ptr = (vsf_multiplex_spi_t *)target_ptr;
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_spi_isr_t *isr_ptr = &m_spi_ptr->spi_cfg.isr;

    bool is_cmp_and_auto_mode =   (irq_mask & SPI_IRQ_MASK_CPL)
                                & __spi_mode_is_auto_cs(m_spi_ptr->spi_cfg.mode);

    if (is_cmp_and_auto_mode) {
        // auto cs and not hardware support
        if (!__spi_cs_pin_is_hardware(m_spi_ptr)) {
            VSF_HAL_ASSERT(m_spi_ptr->gpio != NULL);
            VSF_HAL_ASSERT(m_spi_ptr->pin_mask != 0);
        }
    }

    em_spi_irq_mask_t real_irq_mask = irq_mask & m_spi_ptr->irq_mask;
    if ((real_irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_spi_t *)m_spi_ptr, real_irq_mask);
    }

    // auto cs mode, call next if not empty
    if (is_cmp_and_auto_mode) {
        __spi_pop_and_next(m_spi_ptr);
    }
}

vsf_err_t vsf_multiplex_spi_init(vsf_spi_t *spi_ptr, spi_cfg_t *cfg_ptr)
{
    vsf_multiplex_spi_t *m_spi_ptr = (vsf_multiplex_spi_t *)spi_ptr;
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);

    vsf_spi_t *spi = spi_info_ptr->spi;
    VSF_HAL_ASSERT(spi != 0);

    VSF_HAL_ASSERT(cfg_ptr != 0);
    m_spi_ptr->spi_cfg = *cfg_ptr;

    int8_t cs_index = m_spi_ptr->cs_index;
    VSF_HAL_ASSERT(cfg_ptr != 0);

    bool is_hardware = __spi_cs_pin_is_hardware(m_spi_ptr);

    if (!is_hardware) {
        VSF_HAL_ASSERT(m_spi_ptr->gpio != NULL);
        VSF_HAL_ASSERT(m_spi_ptr->pin_mask != 0);

        vsf_gpio_config_pin(m_spi_ptr->gpio, m_spi_ptr->pin_mask, VSF_MULTIPLEXER_SPI_CFG_GPIO_FEATURE);
        vsf_gpio_output_and_set(m_spi_ptr->gpio, m_spi_ptr->pin_mask);
    }

    vsf_err_t result = VSF_ERR_NONE;
        vsf_protect_t state = vsf_multiplex_spi_protect();
        if (spi_info_ptr->api_mask.init == 0) {
            vsf_slist_queue_init(&spi_info_ptr->list);

            spi_cfg_t local_cfg;
            local_cfg = *cfg_ptr;
            if (!is_hardware) {
                cfg_ptr->mode =  (cfg_ptr->mode & ~SPI_AUTO_CS_MASK) | SPI_AUTO_CS_DISABLE;
            }
            local_cfg.isr.handler_fn = __vsf_multiplex_spi_irq_handler;
            local_cfg.isr.target_ptr = m_spi_ptr;
            result = vsf_spi_init(spi, &local_cfg);
        }
        spi_info_ptr->api_mask.init |= (1 << cs_index);
    vsf_multiplex_spi_unprotect(state);

    return result;
}

fsm_rt_t vsf_multiplex_spi_enable(vsf_spi_t *spi_ptr)
{
    vsf_multiplex_spi_t *m_spi_ptr = (vsf_multiplex_spi_t *)spi_ptr;
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);
    VSF_HAL_ASSERT(spi_info_ptr->spi != NULL);

    // before call spi_enable, be sure to call spi_init
    VSF_HAL_ASSERT(spi_info_ptr->api_mask.init & (1 << m_spi_ptr->cs_index));

    vsf_protect_t state = vsf_multiplex_spi_protect();
        if (spi_info_ptr->api_mask.en == 0) {
            spi_info_ptr->api_mask.en = (1 << m_spi_ptr->cs_index);
            while (fsm_rt_cpl != vsf_spi_enable(spi_info_ptr->spi));
        }
    vsf_multiplex_spi_unprotect(state);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_multiplex_spi_disable(vsf_spi_t *spi_ptr)
{
    vsf_multiplex_spi_t *m_spi_ptr = (vsf_multiplex_spi_t *)spi_ptr;
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);
    VSF_HAL_ASSERT(spi_info_ptr->spi != NULL);

    // before call spi_enable, be sure to call spi_init
    VSF_HAL_ASSERT(spi_info_ptr->api_mask.init & (1 << m_spi_ptr->cs_index));

    vsf_protect_t state = vsf_multiplex_spi_protect();
        spi_info_ptr->api_mask.en &= ~(1 << m_spi_ptr->cs_index);
        if (spi_info_ptr->api_mask.en == 0) {       // when all spi are disabled,
            while (fsm_rt_cpl != vsf_spi_disable(spi_info_ptr->spi));
        }
    vsf_multiplex_spi_unprotect(state);

    return fsm_rt_cpl;
}

void vsf_multiplex_spi_irq_enable(vsf_spi_t *spi_ptr, em_spi_irq_mask_t irq_mask)
{
    vsf_multiplex_spi_t *m_spi_ptr = (vsf_multiplex_spi_t *)spi_ptr;
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);
    VSF_HAL_ASSERT(spi_info_ptr->spi != NULL);

    // before call spi_enable, be sure to call spi_init
    VSF_HAL_ASSERT(spi_info_ptr->api_mask.init & (1 << m_spi_ptr->cs_index));

    vsf_protect_t state = vsf_multiplex_spi_protect();
        m_spi_ptr->irq_mask |= irq_mask;
        em_spi_irq_mask_t new_irq_mask = ~spi_info_ptr->irq_mask & irq_mask;
        if (new_irq_mask) {
            spi_info_ptr->irq_mask |= new_irq_mask;
            vsf_spi_irq_enable(spi_info_ptr->spi, new_irq_mask);
        }
    vsf_multiplex_spi_unprotect(state);
}

void vsf_multiplex_spi_irq_disable(vsf_spi_t *spi_ptr, em_spi_irq_mask_t irq_mask)
{
    vsf_multiplex_spi_t *m_spi_ptr = (vsf_multiplex_spi_t *)spi_ptr;
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);
    VSF_HAL_ASSERT(spi_info_ptr->spi != NULL);

    // before call spi_enable, be sure to call spi_init
    VSF_HAL_ASSERT(spi_info_ptr->api_mask.init & (1 << m_spi_ptr->cs_index));

    vsf_protect_t state = vsf_multiplex_spi_protect();
        m_spi_ptr->irq_mask &= ~irq_mask;
        em_spi_irq_mask_t new_irq_mask = spi_info_ptr->irq_mask & irq_mask;
        if (new_irq_mask) {
            spi_info_ptr->irq_mask &= ~new_irq_mask;
            vsf_spi_irq_disable(spi_info_ptr->spi, new_irq_mask);
        }
    vsf_multiplex_spi_unprotect(state);
}

void vsf_multiplex_spi_cs_active(vsf_spi_t *spi_ptr, uint_fast8_t cs_index)
{
    vsf_multiplex_spi_t *m_spi_ptr = (vsf_multiplex_spi_t *)spi_ptr;
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);

    VSF_HAL_ASSERT(cs_index == 0);
    cs_index = m_spi_ptr->cs_index; // don't care cs_index arg

    // before call spi_enable, be sure to call init and enable

    VSF_HAL_ASSERT(spi_info_ptr->api_mask.init & (1 << cs_index));
    VSF_HAL_ASSERT(spi_info_ptr->api_mask.en & (1 << cs_index));

    vsf_spi_t *spi = spi_info_ptr->spi;
    VSF_HAL_ASSERT(spi != 0);
    bool is_hardware = __spi_cs_pin_is_hardware(m_spi_ptr);

    vsf_protect_t state = vsf_multiplex_spi_protect();
        if (is_hardware) {
            vsf_spi_cs_active(spi, cs_index);
        } else {
            VSF_HAL_ASSERT(m_spi_ptr->gpio != NULL);
            VSF_HAL_ASSERT(m_spi_ptr->pin_mask != 0);
            vsf_gpio_clear(m_spi_ptr->gpio, m_spi_ptr->pin_mask);
        }
    vsf_multiplex_spi_unprotect(state);
}

void vsf_multiplex_spi_cs_inactive(vsf_spi_t *spi_ptr, uint_fast8_t cs_index)
{
    vsf_multiplex_spi_t *m_spi_ptr = (vsf_multiplex_spi_t *)spi_ptr;
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);

    VSF_HAL_ASSERT(cs_index == 0);
    cs_index = m_spi_ptr->cs_index; // don't care cs_index arg

    // before call spi_enable, be sure to call init and enable
    VSF_HAL_ASSERT(spi_info_ptr->api_mask.init & (1 << cs_index));
    VSF_HAL_ASSERT(spi_info_ptr->api_mask.en & (1 << cs_index));

    vsf_spi_t *spi = spi_info_ptr->spi;
    VSF_HAL_ASSERT(spi != 0);
    bool is_hardware = __spi_cs_pin_is_hardware(m_spi_ptr);
    bool is_auto = __spi_mode_is_auto_cs(m_spi_ptr->spi_cfg.mode);

    vsf_protect_t state = vsf_multiplex_spi_protect();
        if (is_hardware) {
            vsf_spi_cs_inactive(spi, cs_index);
        } else {
            VSF_HAL_ASSERT(m_spi_ptr->gpio != NULL);
            VSF_HAL_ASSERT(m_spi_ptr->pin_mask != 0);
            vsf_gpio_set(m_spi_ptr->gpio, m_spi_ptr->pin_mask);
        }

        // manual cs
        if (!is_auto) {
            __spi_pop_and_next(m_spi_ptr);    // call next if not empty
        }
    vsf_multiplex_spi_unprotect(state);
}


void vsf_multiplex_spi_fifo_transfer(vsf_spi_t *spi_ptr,
                                     void *out_buffer_ptr,
                                     uint_fast32_t out_cnt,
                                     uint_fast32_t* out_offset_ptr,
                                     void *in_buffer_ptr,
                                     uint_fast32_t in_cnt,
                                     uint_fast32_t* int_offset_ptr)
{
    // TODO: support fifo transfer
    VSF_HAL_ASSERT(0);
}

vsf_err_t vsf_multiplex_spi_request_transfer(vsf_spi_t *spi_ptr,
                                             uint8_t cs_index,
                                             void *out_buffer_ptr,
                                             void *in_buffer_ptr,
                                             uint_fast32_t count)
{
    vsf_multiplex_spi_t *m_spi_ptr = (vsf_multiplex_spi_t *)spi_ptr;
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);
    VSF_HAL_ASSERT(spi_info_ptr->spi != NULL);

    VSF_HAL_ASSERT(cs_index == 0);
    cs_index = m_spi_ptr->cs_index; // don't care cs_index arg

    // Before request, cs must be active
    // before call spi_enable, be sure to call init/enable/cs_active
    VSF_HAL_ASSERT(spi_info_ptr->api_mask.init & (1 << cs_index));
    VSF_HAL_ASSERT(spi_info_ptr->api_mask.en   & (1 << cs_index));
    VSF_HAL_ASSERT(spi_info_ptr->api_mask.cs   & (1 << cs_index));

    m_spi_ptr->request.out_buffer_ptr = out_buffer_ptr;
    m_spi_ptr->request.in_buffer_ptr  = in_buffer_ptr;
    m_spi_ptr->request.count          = count;

    vsf_err_t result = VSF_ERR_NONE;
    vsf_protect_t state = vsf_multiplex_spi_protect();
        if (vsf_slist_queue_is_in(vsf_multiplex_spi_t, node, &spi_info_ptr->list, m_spi_ptr)) {
            VSF_HAL_ASSERT(0);
            result = VSF_ERR_FAIL;
        } else {
            vsf_slist_queue_enqueue(vsf_multiplex_spi_t, node, &spi_info_ptr->list, m_spi_ptr);
        }

        vsf_multiplex_spi_t *cur_m_spi_ptr;
        vsf_slist_queue_peek(vsf_multiplex_spi_t, node, &spi_info_ptr->list, cur_m_spi_ptr);
        if (cur_m_spi_ptr == m_spi_ptr) {
            result = vsf_spi_request_transfer(spi_info_ptr->spi,
                                              m_spi_ptr->request.out_buffer_ptr,
                                              m_spi_ptr->request.in_buffer_ptr,
                                              m_spi_ptr->request.count);
        }
    vsf_multiplex_spi_unprotect(state);

    return result;
}

vsf_err_t vsf_multiplex_spi_cancel_transfer(vsf_spi_t *spi_ptr)
{
    vsf_multiplex_spi_t *m_spi_ptr = (vsf_multiplex_spi_t *)spi_ptr;
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);

    vsf_err_t result;
    vsf_protect_t state = vsf_multiplex_spi_protect();
        result = vsf_spi_cancel_transfer(spi_info_ptr->spi);
    vsf_multiplex_spi_unprotect(state);

    return result;
}

spi_status_t vsf_multiplex_spi_status(vsf_spi_t *spi_ptr)
{
    vsf_multiplex_spi_t *m_spi_ptr = (vsf_multiplex_spi_t *)spi_ptr;
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);

    spi_status_t result;
    vsf_protect_t state = vsf_multiplex_spi_protect();
        result = vsf_spi_status(spi_info_ptr->spi);
    vsf_multiplex_spi_unprotect(state);

    return result;
}

int_fast32_t vsf_multiplex_spi_get_transfered_count(vsf_spi_t *spi_ptr)
{
    vsf_multiplex_spi_t *m_spi_ptr = (vsf_multiplex_spi_t *)spi_ptr;
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    return m_spi_ptr->transfered_count;
}

#endif
