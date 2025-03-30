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

#if VSF_HAL_USE_SPI == ENABLED && VSF_HAL_USE_MULTIPLEX_CS_SPI == ENABLED

#include "hal/driver/driver.h"

/*============================ MACROS ========================================*/

#ifdef VSF_MULTIPLEXER_SPI_CFG_CALL_PREFIX
#   define VSF_SPI_CFG_PREFIX                       VSF_MULTIPLEXER_SPI_CFG_CALL_PREFIX
#endif

#define VSF_MULTIPLEXER_SPI_MAX_CS_COUNT            (sizeof(SPI_MULTIPLEXER_MASK_TYPE) * 8)

#ifndef VSF_MULTIPLEXER_SPI_CFG_GPIO_MODE
#   define VSF_MULTIPLEXER_SPI_CFG_GPIO_MODE        (VSF_GPIO_PULL_UP)
#endif

#ifndef VSF_MULTIPLEXER_SPI_PROTECT_LEVEL
#   define VSF_MULTIPLEXER_SPI_PROTECT_LEVEL        interrupt
#endif

#define VSF_SPI_IMP_DEC_EXTERN_OP                   ENABLED

#define vsf_multiplex_spi_protect                   vsf_protect(VSF_MULTIPLEXER_SPI_PROTECT_LEVEL)
#define vsf_multiplex_spi_unprotect                 vsf_unprotect(VSF_MULTIPLEXER_SPI_PROTECT_LEVEL)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static void __vsf_multiplex_spi_irq_handler(void *target_ptr, vsf_spi_t *spi_ptr, vsf_spi_irq_mask_t irq_mask);

/*============================ IMPLEMENTATION ================================*/

static bool __spi_mode_is_auto_cs(vsf_spi_mode_t mode)
{
    return (mode & VSF_SPI_CS_MODE_MASK) == VSF_SPI_CS_HARDWARE_MODE;
}

static bool __spi_cs_pin_is_hardware(vsf_multiplex_spi_t *m_spi_ptr)
{
    vsf_spi_capability_t spi_capability = vsf_spi_capability(m_spi_ptr->spi_info_ptr->spi);
    return (m_spi_ptr->cs_index < spi_capability.cs_count);
}

static vsf_err_t __spi_cs_active(vsf_multiplex_spi_t *m_spi_ptr)
{
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    if (__spi_cs_pin_is_hardware(m_spi_ptr)) {
        return vsf_spi_cs_active(m_spi_ptr->spi_info_ptr->spi, m_spi_ptr->cs_index);
    } else {
#if VSF_HAL_USE_GPIO == ENABLED
        VSF_HAL_ASSERT(m_spi_ptr->gpio != NULL);
        VSF_HAL_ASSERT(m_spi_ptr->pin_mask != 0);
        vsf_gpio_clear(m_spi_ptr->gpio, m_spi_ptr->pin_mask);
#else
        VSF_HAL_ASSERT(0);
#endif
        return VSF_ERR_NONE;
    }
}

static vsf_err_t __spi_cs_inactive(vsf_multiplex_spi_t *m_spi_ptr)
{
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    if (__spi_cs_pin_is_hardware(m_spi_ptr)) {
        return vsf_spi_cs_inactive(m_spi_ptr->spi_info_ptr->spi, m_spi_ptr->cs_index);
    } else {
#if VSF_HAL_USE_GPIO == ENABLED
        VSF_HAL_ASSERT(m_spi_ptr->gpio != NULL);
        VSF_HAL_ASSERT(m_spi_ptr->pin_mask != 0);
        vsf_gpio_set(m_spi_ptr->gpio, m_spi_ptr->pin_mask);
#else
        VSF_HAL_ASSERT(0);
#endif
        return VSF_ERR_NONE;
    }
}

static void __spi_cs_gpio_init(vsf_multiplex_spi_t *m_spi_ptr)
{
    bool is_hardware = __spi_cs_pin_is_hardware(m_spi_ptr);
    if (!is_hardware) {
#if VSF_HAL_USE_GPIO == ENABLED
        VSF_HAL_ASSERT(m_spi_ptr->gpio != NULL);
        VSF_HAL_ASSERT(m_spi_ptr->pin_mask != 0);

        vsf_gpio_cfg_t cfg = {
            .mode = VSF_MULTIPLEXER_SPI_CFG_GPIO_MODE,
        };
        vsf_gpio_port_config_pins(m_spi_ptr->gpio, m_spi_ptr->pin_mask, &cfg);
        vsf_gpio_output_and_set(m_spi_ptr->gpio, m_spi_ptr->pin_mask);
#else
        VSF_HAL_ASSERT(0);
#endif
    }
}

static vsf_err_t __spi_init_en_req(vsf_multiplex_spi_t *m_spi_ptr, bool need_reconf, bool is_trans)
{
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);

    vsf_spi_t *spi = spi_info_ptr->spi;
    VSF_HAL_ASSERT(spi != 0);

    vsf_err_t result = VSF_ERR_NONE;

    if (is_trans && (spi_info_ptr->cfg_spi_ptr != m_spi_ptr)) {
        need_reconf = true;
    }

    if (need_reconf) {
        vsf_spi_cfg_t local_cfg = m_spi_ptr->spi_cfg;
        // force disable auto cs mode
        local_cfg.mode =  (local_cfg.mode & ~VSF_SPI_CS_MODE_MASK) | VSF_SPI_CS_SOFTWARE_MODE;
        local_cfg.isr.handler_fn = __vsf_multiplex_spi_irq_handler;
        local_cfg.isr.target_ptr = m_spi_ptr;

        result = vsf_spi_init(spi, &local_cfg);
        if (result != VSF_ERR_NONE) {
            VSF_HAL_ASSERT(result == VSF_ERR_NONE);
            return result;
        } else {
            spi_info_ptr->cfg_spi_ptr = m_spi_ptr;
        }

        if (__spi_mode_is_auto_cs(m_spi_ptr->spi_cfg.mode)) {
            spi_info_ptr->cs_mask |= (1 << m_spi_ptr->cs_index);
        }
    }

    if (is_trans) {
        if (spi_info_ptr->cs_mask & (1 << m_spi_ptr->cs_index)) {
            spi_info_ptr->cs_mask &= ~(1 << m_spi_ptr->cs_index);
            __spi_cs_active(m_spi_ptr);
        }

        multiplex_spi_request_t * request_ptr = &m_spi_ptr->request;
        result = vsf_spi_request_transfer(spi, request_ptr->out_buffer_ptr, request_ptr->in_buffer_ptr, request_ptr->count);
        VSF_HAL_ASSERT(result == VSF_ERR_NONE);
    }

    return result;
}

static void __spi_start_next(vsf_multiplex_spi_info_t *spi_info_ptr)
{
    VSF_HAL_ASSERT(spi_info_ptr != NULL);
    vsf_multiplex_spi_t *cur_m_spi_ptr;

    vsf_slist_queue_dequeue(vsf_multiplex_spi_t, node, &spi_info_ptr->list, cur_m_spi_ptr);

    if (!vsf_slist_queue_is_empty(&spi_info_ptr->list)) {
        vsf_slist_queue_peek(vsf_multiplex_spi_t, node, &spi_info_ptr->list, cur_m_spi_ptr);
        vsf_err_t result = __spi_init_en_req(cur_m_spi_ptr, false, true);
        VSF_HAL_ASSERT(result == VSF_ERR_NONE);
        (void)result;
    }
}

static void __vsf_multiplex_spi_irq_handler(void *target_ptr, vsf_spi_t *spi_ptr, vsf_spi_irq_mask_t irq_mask)
{
    vsf_multiplex_spi_t *m_spi_ptr = (vsf_multiplex_spi_t *)target_ptr;
    VSF_HAL_ASSERT(m_spi_ptr != NULL);
    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);

    if (irq_mask & VSF_SPI_IRQ_MASK_CPL) {
        m_spi_ptr->transferred_count = m_spi_ptr->request.count;
        m_spi_ptr->request.count = 0;
        m_spi_ptr->request.in_buffer_ptr = NULL;
        m_spi_ptr->request.out_buffer_ptr = NULL;

        // auto cs is base on manual cs
        if (__spi_mode_is_auto_cs(m_spi_ptr->spi_cfg.mode)) {
            __spi_cs_inactive(m_spi_ptr);
            __spi_start_next(spi_info_ptr);
        }
    }

    vsf_spi_isr_t *isr_ptr = &m_spi_ptr->spi_cfg.isr;
    vsf_spi_irq_mask_t real_irq_mask = irq_mask & m_spi_ptr->irq_mask;
    if ((real_irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_spi_t *)m_spi_ptr, real_irq_mask);
    }
}

vsf_err_t vsf_multiplex_spi_init(vsf_multiplex_spi_t *m_spi_ptr, vsf_spi_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);

    vsf_spi_t *spi = spi_info_ptr->spi;
    VSF_HAL_ASSERT(spi != 0);

    VSF_HAL_ASSERT(cfg_ptr != 0);
    m_spi_ptr->spi_cfg = *cfg_ptr;

    __spi_cs_gpio_init(m_spi_ptr);

    vsf_err_t result = VSF_ERR_NONE;
    vsf_protect_t state = vsf_multiplex_spi_protect();
        if (   (spi_info_ptr->cfg_spi_ptr == NULL)              // first init
            || (spi_info_ptr->cfg_spi_ptr == m_spi_ptr)) {      // re-init
            vsf_slist_queue_init(&spi_info_ptr->list);
            result = __spi_init_en_req(m_spi_ptr, true, false);
        }
        if (result == VSF_ERR_NONE) {
            spi_info_ptr->init_mask |= (1 << m_spi_ptr->cs_index);
        }
    vsf_multiplex_spi_unprotect(state);

    return result;
}

fsm_rt_t vsf_multiplex_spi_enable(vsf_multiplex_spi_t *m_spi_ptr)
{
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);
    VSF_HAL_ASSERT(spi_info_ptr->spi != NULL);

    // before call spi_enable, be sure to call spi_init
    VSF_HAL_ASSERT(spi_info_ptr->init_mask & (1 << m_spi_ptr->cs_index));

    vsf_protect_t state = vsf_multiplex_spi_protect();
        if (spi_info_ptr->en_mask == 0) {
            while (fsm_rt_cpl != vsf_spi_enable(spi_info_ptr->spi));
        }
        spi_info_ptr->en_mask |= (1 << m_spi_ptr->cs_index);
    vsf_multiplex_spi_unprotect(state);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_multiplex_spi_disable(vsf_multiplex_spi_t *m_spi_ptr)
{
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);
    VSF_HAL_ASSERT(spi_info_ptr->spi != NULL);

    // before call spi_enable, be sure to call spi_init
    VSF_HAL_ASSERT(spi_info_ptr->init_mask & (1 << m_spi_ptr->cs_index));

    vsf_protect_t state = vsf_multiplex_spi_protect();
        spi_info_ptr->en_mask &= ~(1 << m_spi_ptr->cs_index);
        if (spi_info_ptr->en_mask == 0) {       // when all spi are disabled,
            while (fsm_rt_cpl != vsf_spi_disable(spi_info_ptr->spi));
            spi_info_ptr->cfg_spi_ptr = NULL;
        }
    vsf_multiplex_spi_unprotect(state);

    return fsm_rt_cpl;
}

void vsf_multiplex_spi_irq_enable(vsf_multiplex_spi_t *m_spi_ptr, vsf_spi_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);
    VSF_HAL_ASSERT(spi_info_ptr->spi != NULL);

    // before call spi_enable, be sure to call spi_init
    VSF_HAL_ASSERT(spi_info_ptr->init_mask & (1 << m_spi_ptr->cs_index));

    vsf_protect_t state = vsf_multiplex_spi_protect();
        m_spi_ptr->irq_mask |= irq_mask;
        vsf_spi_irq_mask_t new_irq_mask = ~spi_info_ptr->irq_mask & irq_mask;
        if (new_irq_mask) {
            spi_info_ptr->irq_mask |= new_irq_mask;
            vsf_spi_irq_enable(spi_info_ptr->spi, new_irq_mask);
        }
    vsf_multiplex_spi_unprotect(state);
}

void vsf_multiplex_spi_irq_disable(vsf_multiplex_spi_t *m_spi_ptr, vsf_spi_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);
    VSF_HAL_ASSERT(spi_info_ptr->spi != NULL);

    // before call spi_enable, be sure to call spi_init
    VSF_HAL_ASSERT(spi_info_ptr->init_mask & (1 << m_spi_ptr->cs_index));

    vsf_protect_t state = vsf_multiplex_spi_protect();
        m_spi_ptr->irq_mask &= ~irq_mask;
        vsf_spi_irq_mask_t new_irq_mask = spi_info_ptr->irq_mask & irq_mask;
        if (new_irq_mask) {
            spi_info_ptr->irq_mask &= ~new_irq_mask;
            vsf_spi_irq_disable(spi_info_ptr->spi, new_irq_mask);
        }
    vsf_multiplex_spi_unprotect(state);
}

vsf_err_t vsf_multiplex_spi_cs_active(vsf_multiplex_spi_t *m_spi_ptr, uint_fast8_t cs_index)
{
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);

    VSF_HAL_ASSERT(cs_index == 0);
    cs_index = m_spi_ptr->cs_index; // don't care cs_index arg

    // before call spi_enable, be sure to call init and enable
    VSF_HAL_ASSERT(spi_info_ptr->init_mask & (1 << cs_index));
    VSF_HAL_ASSERT(spi_info_ptr->en_mask & (1 << cs_index));

    vsf_protect_t state = vsf_multiplex_spi_protect();
        spi_info_ptr->cs_mask |= 1 << cs_index;
    vsf_multiplex_spi_unprotect(state);

    return VSF_ERR_NONE;
}

vsf_err_t vsf_multiplex_spi_cs_inactive(vsf_multiplex_spi_t *m_spi_ptr, uint_fast8_t cs_index)
{
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);

    VSF_HAL_ASSERT(cs_index == 0);
    cs_index = m_spi_ptr->cs_index; // don't care cs_index arg

    // before call spi_enable, be sure to call init and enable
    VSF_HAL_ASSERT(spi_info_ptr->init_mask & (1 << cs_index));
    VSF_HAL_ASSERT(spi_info_ptr->en_mask & (1 << cs_index));

    VSF_HAL_ASSERT(!__spi_mode_is_auto_cs(m_spi_ptr->spi_cfg.mode));

    vsf_spi_t *spi = spi_info_ptr->spi;
    VSF_HAL_ASSERT(spi != 0);

    vsf_protect_t state = vsf_multiplex_spi_protect();
        __spi_cs_inactive(m_spi_ptr);
        spi_info_ptr->cs_mask &= ~(1 << cs_index);
        __spi_start_next(spi_info_ptr);    // call next if not empty
    vsf_multiplex_spi_unprotect(state);

    return VSF_ERR_NONE;
}


void vsf_multiplex_spi_fifo_transfer(vsf_multiplex_spi_t *m_spi_ptr,
                                     void *out_buffer_ptr,
                                     uint_fast32_t* out_offset_ptr,
                                     void *in_buffer_ptr,
                                     uint_fast32_t* int_offset_ptr,
                                     uint_fast32_t cnt)
{
    // TODO: support fifo transfer
    VSF_HAL_ASSERT(0);
}

vsf_err_t vsf_multiplex_spi_request_transfer(vsf_multiplex_spi_t *m_spi_ptr,
                                             void *out_buffer_ptr,
                                             void *in_buffer_ptr,
                                             uint_fast32_t count)
{
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);
    VSF_HAL_ASSERT(spi_info_ptr->spi != NULL);

    // Before request, cs must be active
    // before call spi_enable, be sure to call init/enable/cs_active
    VSF_HAL_ASSERT(spi_info_ptr->init_mask & (1 << m_spi_ptr->cs_index));
    VSF_HAL_ASSERT(spi_info_ptr->en_mask & (1 << m_spi_ptr->cs_index));

    VSF_HAL_ASSERT(count > 0);
    if (m_spi_ptr->request.count != 0) {
        VSF_HAL_ASSERT(0);  // The last transfer was not completed
        return VSF_ERR_NONE;
    }

    m_spi_ptr->request.out_buffer_ptr = out_buffer_ptr;
    m_spi_ptr->request.in_buffer_ptr  = in_buffer_ptr;
    m_spi_ptr->request.count          = count;

    vsf_err_t result = VSF_ERR_NONE;
    vsf_protect_t state = vsf_multiplex_spi_protect();
        if (!vsf_slist_queue_is_in(vsf_multiplex_spi_t, node, &spi_info_ptr->list, m_spi_ptr)) {
            vsf_slist_queue_enqueue(vsf_multiplex_spi_t, node, &spi_info_ptr->list, m_spi_ptr);
        }

        vsf_multiplex_spi_t *cur_m_spi_ptr;
        vsf_slist_queue_peek(vsf_multiplex_spi_t, node, &spi_info_ptr->list, cur_m_spi_ptr);

        bool is_trans = (cur_m_spi_ptr == m_spi_ptr); // is head ?
        result = __spi_init_en_req(m_spi_ptr, false, is_trans);
    vsf_multiplex_spi_unprotect(state);

    return result;
}

vsf_err_t vsf_multiplex_spi_cancel_transfer(vsf_multiplex_spi_t *m_spi_ptr)
{
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);

    vsf_err_t result = VSF_ERR_NONE;

    if (spi_info_ptr->cfg_spi_ptr == m_spi_ptr) {
        vsf_protect_t state = vsf_multiplex_spi_protect();
            result = vsf_spi_cancel_transfer(spi_info_ptr->spi);
        vsf_multiplex_spi_unprotect(state);
    } else {
        // TODO: remove from list;
    }

    return result;
}

vsf_spi_status_t vsf_multiplex_spi_status(vsf_multiplex_spi_t *m_spi_ptr)
{
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);

    // TODO: add more status info
    vsf_spi_status_t result = {0};

    if (spi_info_ptr->cfg_spi_ptr == m_spi_ptr) {
        vsf_protect_t state = vsf_multiplex_spi_protect();
            result = vsf_spi_status(spi_info_ptr->spi);
        vsf_multiplex_spi_unprotect(state);
    }

    return result;
}

vsf_err_t vsf_multiplex_spi_ctrl(vsf_multiplex_spi_t *m_spi_ptr, vsf_spi_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);

    vsf_err_t result = VSF_ERR_NOT_SUPPORT;

    if (spi_info_ptr->cfg_spi_ptr == m_spi_ptr) {
        vsf_protect_t state = vsf_multiplex_spi_protect();
            result = vsf_spi_ctrl(spi_info_ptr->spi, ctrl, param);
        vsf_multiplex_spi_unprotect(state);
    }

    return result;
}

void vsf_multiplex_spi_get_transferred_count(vsf_multiplex_spi_t *m_spi_ptr,
                                            uint_fast32_t * tx_count, uint_fast32_t *rx_count)
{
    VSF_HAL_ASSERT(m_spi_ptr != NULL);

    vsf_multiplex_spi_info_t *spi_info_ptr = m_spi_ptr->spi_info_ptr;
    VSF_HAL_ASSERT(spi_info_ptr != NULL);

    if (spi_info_ptr->cfg_spi_ptr == m_spi_ptr) {
        vsf_protect_t state = vsf_multiplex_spi_protect();
        vsf_spi_get_transferred_count(spi_info_ptr->spi, tx_count, rx_count);
        vsf_multiplex_spi_unprotect(state);
    } else {
        if (tx_count != NULL) {
            *tx_count = m_spi_ptr->transferred_count;
        }
        if (rx_count != NULL) {
            *rx_count = m_spi_ptr->transferred_count;
        }
    }
}

#endif  /* VSF_HAL_USE_SPI && VSF_HAL_USE_MULTIPLEX_CS_SPI */
