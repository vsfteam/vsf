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

#define __VSF_I2C_MULTIPLEX_CLASS_IMPLEMENT

#include "hal/vsf_hal_cfg.h"

#if (VSF_HAL_USE_I2C == ENABLED) && (VSF_HAL_USE_MULTIPLEX_I2C == ENABLED)

#include "hal/driver/driver.h"

/*============================ MACROS ========================================*/

#ifdef VSF_MULTIPLEXER_I2C_CFG_CALL_PREFIX
#   undef VSF_I2C_CFG_PREFIX
#   define VSF_I2C_CFG_PREFIX               VSF_MULTIPLEXER_I2C_CFG_CALL_PREFIX
#endif

#ifndef VSF_MULTIPLEX_I2C_PROTECT_LEVEL
#   define VSF_MULTIPLEX_I2C_PROTECT_LEVEL  interrupt
#endif

#define vsf_multiplex_i2c_protect           vsf_protect(VSF_MULTIPLEX_I2C_PROTECT_LEVEL)
#define vsf_multiplex_i2c_unprotect         vsf_unprotect(VSF_MULTIPLEX_I2C_PROTECT_LEVEL)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __m_i2c_request(vsf_multiplex_i2c_t *m_i2c_ptr);
static void __i2c_isr_handler(void *target_ptr, vsf_i2c_t *i2c_ptr, em_i2c_irq_mask_t irq_mask);

/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __m_i2c_init(vsf_multiplex_i2c_t *m_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t * multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);

    i2c_cfg_t cfg = m_i2c_ptr->cfg;
    cfg.isr.handler_fn = __i2c_isr_handler;
    cfg.isr.target_ptr = m_i2c_ptr;
    vsf_err_t result = vsf_i2c_init(multiplexer->i2c_ptr, &cfg);
    if (result == VSF_ERR_NONE) {
        multiplexer->current_m_i2c = m_i2c_ptr;
    }

    return result;
}

static vsf_err_t __m_i2c_request(vsf_multiplex_i2c_t *m_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t *multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);

    vsf_err_t result = __m_i2c_init(m_i2c_ptr);
    VSF_HAL_ASSERT(result == VSF_ERR_NONE);

    return vsf_i2c_master_request(multiplexer->i2c_ptr,
                                  m_i2c_ptr->request.address,
                                  m_i2c_ptr->request.cmd,
                                  m_i2c_ptr->request.count,
                                  m_i2c_ptr->request.buffer_ptr);
}

static void __i2c_start_next(vsf_multiplexer_i2c_t *multiplexer)
{
    VSF_HAL_ASSERT(multiplexer != NULL);
    vsf_multiplex_i2c_t *cur_m_i2c_ptr;

    vsf_slist_queue_dequeue(vsf_multiplex_i2c_t, slist_node, &multiplexer->slist_queue, cur_m_i2c_ptr);

    if (!vsf_slist_queue_is_empty(&multiplexer->slist_queue)) {
        vsf_slist_queue_peek(vsf_multiplex_i2c_t, node, &multiplexer->slist_queue, cur_m_i2c_ptr);
        vsf_err_t result = __m_i2c_request(cur_m_i2c_ptr);
        VSF_HAL_ASSERT(result == VSF_ERR_NONE);
        (void)result;
    }
}

static void __i2c_isr_handler(void *target_ptr, vsf_i2c_t *i2c_ptr, em_i2c_irq_mask_t irq_mask)
{
    vsf_multiplex_i2c_t *m_i2c_ptr = (vsf_multiplex_i2c_t *)target_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t * multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);

    bool is_restart = m_i2c_ptr->request.cmd & I2C_CMD_RESTAR;
    em_i2c_irq_mask_t cpl_irq_mask =  I2C_IRQ_MASK_MASTER_STOP_DETECT
                                    | I2C_IRQ_MASK_MASTER_NACK_DETECT
                                    | I2C_IRQ_MASK_MASTER_ADDRESS_NACK
                                    | I2C_IRQ_MASK_MASTER_ARBITRATION_LOST
                                    | I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE;

    if (!is_restart && (irq_mask & cpl_irq_mask)) {
        __i2c_start_next(multiplexer);
    }

    vsf_i2c_isr_t *isr_ptr = &m_i2c_ptr->cfg.isr;
    em_i2c_irq_mask_t real_irq_mask = irq_mask & m_i2c_ptr->irq_mask;
    if ((real_irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_i2c_t *)m_i2c_ptr, real_irq_mask);
    }
}

vsf_err_t vsf_multiplex_i2c_init(vsf_i2c_t *i2c_ptr, i2c_cfg_t *cfg_ptr)
{
    vsf_multiplex_i2c_t *m_i2c_ptr = (vsf_multiplex_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t * multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    m_i2c_ptr->cfg = *cfg_ptr;

    // TODO: Solve the problem of repeatedly calling vsf_multiplex_i2c_init
    vsf_err_t result = VSF_ERR_NONE;
    vsf_protect_t state = vsf_multiplex_i2c_protect();
        if (   (multiplexer->current_m_i2c == NULL)              // first init
            || (multiplexer->current_m_i2c == m_i2c_ptr)) {      // re-init
            vsf_slist_queue_init(&multiplexer->slist_queue);
            result = __m_i2c_init(m_i2c_ptr);
        }
        if (result == VSF_ERR_NONE) {
            multiplexer->init_mask |= (1 << m_i2c_ptr->id);
        }
    vsf_multiplex_i2c_unprotect(state);

    return result;
}

void vsf_multiplex_i2c_fini(vsf_i2c_t *i2c_ptr)
{
    vsf_multiplex_i2c_t *m_i2c_ptr = (vsf_multiplex_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t * multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);

    vsf_protect_t state = vsf_multiplex_i2c_protect();
        multiplexer->init_mask &= ~(1 << m_i2c_ptr->id);
        if (multiplexer->init_mask == 0) {
            vsf_i2c_fini(m_i2c_ptr->multiplexer->i2c_ptr);
        }
    vsf_multiplex_i2c_unprotect(state);

}

fsm_rt_t vsf_multiplex_i2c_enable(vsf_i2c_t *i2c_ptr)
{
    vsf_multiplex_i2c_t *m_i2c_ptr = (vsf_multiplex_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t *multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);

    VSF_HAL_ASSERT(multiplexer->init_mask & (1 << m_i2c_ptr->id));

    vsf_protect_t state = vsf_multiplex_i2c_protect();
        if (multiplexer->en_mask == 0) {
            while (fsm_rt_cpl != vsf_i2c_enable(multiplexer->i2c_ptr));
        }
        multiplexer->en_mask |= (1 << m_i2c_ptr->id);
    vsf_multiplex_i2c_unprotect(state);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_multiplex_i2c_disable(vsf_i2c_t *i2c_ptr)
{
    vsf_multiplex_i2c_t *m_i2c_ptr = (vsf_multiplex_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t *multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);

    VSF_HAL_ASSERT(multiplexer->init_mask & (1 << m_i2c_ptr->id));
    VSF_HAL_ASSERT(multiplexer->en_mask & (1 << m_i2c_ptr->id));


    vsf_protect_t state = vsf_multiplex_i2c_protect();
        multiplexer->en_mask &= ~(1 << m_i2c_ptr->id);
        if (multiplexer->en_mask == 0) {       // when all i2c are disabled,
            while (fsm_rt_cpl != vsf_i2c_disable(multiplexer->i2c_ptr));
        }
    vsf_multiplex_i2c_unprotect(state);

    return fsm_rt_cpl;
}

void vsf_multiplex_i2c_irq_enable(vsf_i2c_t *i2c_ptr, em_i2c_irq_mask_t irq_mask)
{
    vsf_multiplex_i2c_t *m_i2c_ptr = (vsf_multiplex_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t *multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);

    VSF_HAL_ASSERT(multiplexer->init_mask & (1 << m_i2c_ptr->id));

    vsf_protect_t state = vsf_multiplex_i2c_protect();
        m_i2c_ptr->irq_mask |= irq_mask;
        em_i2c_irq_mask_t new_irq_mask = ~multiplexer->irq_mask & irq_mask;
        if (new_irq_mask) {
            multiplexer->irq_mask |= new_irq_mask;
            vsf_i2c_irq_enable(multiplexer->i2c_ptr, new_irq_mask);
        }
    vsf_multiplex_i2c_unprotect(state);
}

void vsf_multiplex_i2c_irq_disable(vsf_i2c_t *i2c_ptr, em_i2c_irq_mask_t irq_mask)
{
    vsf_multiplex_i2c_t *m_i2c_ptr = (vsf_multiplex_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t *multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);

    VSF_HAL_ASSERT(multiplexer->init_mask & (1 << m_i2c_ptr->id));

    vsf_protect_t state = vsf_multiplex_i2c_protect();
        m_i2c_ptr->irq_mask &= ~irq_mask;
        em_i2c_irq_mask_t new_irq_mask = multiplexer->irq_mask & irq_mask;
        if (new_irq_mask) {
            multiplexer->irq_mask &= ~new_irq_mask;
            vsf_i2c_irq_disable(multiplexer->i2c_ptr, new_irq_mask);
        }
    vsf_multiplex_i2c_unprotect(state);
}

i2c_status_t vsf_multiplex_i2c_status(vsf_i2c_t *i2c_ptr)
{
    vsf_multiplex_i2c_t *m_i2c_ptr = (vsf_multiplex_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t *multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);

    VSF_HAL_ASSERT(multiplexer->init_mask & (1 << m_i2c_ptr->id));

    // TODO: add more status info
    i2c_status_t status = {0};

    if (multiplexer->current_m_i2c == m_i2c_ptr) {
        vsf_protect_t state = vsf_multiplex_i2c_protect();
            status = vsf_i2c_status(multiplexer->i2c_ptr);
        vsf_multiplex_i2c_unprotect(state);
    }

    return status;
}

vsf_err_t vsf_multiplex_i2c_master_request(vsf_i2c_t *i2c_ptr,
                                           uint16_t address,
                                           em_i2c_cmd_t cmd,
                                           uint16_t count,
                                           uint8_t *buffer_ptr)
{
    vsf_multiplex_i2c_t *m_i2c_ptr = (vsf_multiplex_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t *multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);

    m_i2c_ptr->request.cmd = cmd;
    m_i2c_ptr->request.address = address;
    m_i2c_ptr->request.count = count;
    m_i2c_ptr->request.buffer_ptr = buffer_ptr;

    vsf_err_t result = VSF_ERR_NONE;
    vsf_protect_t state = vsf_multiplex_i2c_protect();
        if (!vsf_slist_queue_is_in(vsf_multiplex_i2c_t, slist_node, &multiplexer->slist_queue, m_i2c_ptr)) {
            vsf_slist_queue_enqueue(vsf_multiplex_i2c_t, slist_node, &multiplexer->slist_queue, m_i2c_ptr);
        }

        vsf_multiplex_i2c_t *cur_m_i2c_ptr;
        vsf_slist_queue_peek(vsf_multiplex_i2c_t, slist_node, &multiplexer->slist_queue, cur_m_i2c_ptr);

        bool is_request = (cur_m_i2c_ptr == m_i2c_ptr); // is head ?
        if (is_request) {
            result = __m_i2c_request(m_i2c_ptr);
        }
    vsf_multiplex_i2c_unprotect(state);

    return result;
}

#endif //!VSF_HAL_I2C_IMP_MULTIPLEX_I2C
