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
#   define VSF_I2C_CFG_PREFIX                   VSF_MULTIPLEXER_I2C_CFG_CALL_PREFIX
#endif

#ifndef VSF_MULTIPLEX_I2C_PROTECT_LEVEL
#   define VSF_MULTIPLEX_I2C_PROTECT_LEVEL      interrupt
#endif

#define VSF_I2C_CFG_IMP_EXTERN_OP               ENABLED

#define vsf_multiplex_i2c_protect               vsf_protect(VSF_MULTIPLEX_I2C_PROTECT_LEVEL)
#define vsf_multiplex_i2c_unprotect             vsf_unprotect(VSF_MULTIPLEX_I2C_PROTECT_LEVEL)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static void __i2c_isr_handler(void *target_ptr, vsf_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask);

/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __m_i2c_request(vsf_multiplex_i2c_t *m_i2c_ptr, bool is_init, bool is_enabled, bool is_req)
{
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t * const multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);

    vsf_err_t result = VSF_ERR_NONE;

    if (is_enabled) {
        while (fsm_rt_cpl != vsf_i2c_disable(multiplexer->i2c_ptr));
    }

    if (is_init) {
        vsf_i2c_cfg_t cfg = m_i2c_ptr->cfg;
        cfg.isr.handler_fn = __i2c_isr_handler;
        cfg.isr.target_ptr = m_i2c_ptr;
        result = vsf_i2c_init(multiplexer->i2c_ptr, &cfg);
        if (result != VSF_ERR_NONE) {
            VSF_HAL_ASSERT(0);
            return result;
        }

        vsf_protect_t state = vsf_multiplex_i2c_protect();
            multiplexer->inited_m_i2c = m_i2c_ptr;
        vsf_multiplex_i2c_unprotect(state);
    }

    if (is_enabled) {
        while (fsm_rt_cpl != vsf_i2c_enable(multiplexer->i2c_ptr));
    }

    if (is_req) {
        result = vsf_i2c_master_request(multiplexer->i2c_ptr,
                                        m_i2c_ptr->request.address,
                                        m_i2c_ptr->request.cmd,
                                        m_i2c_ptr->request.count,
                                        m_i2c_ptr->request.buffer_ptr);
    }

    return result;
}

static void __i2c_isr_handler(void *target_ptr, vsf_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    vsf_multiplex_i2c_t *m_i2c_ptr = (vsf_multiplex_i2c_t *)target_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t * const multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);

    // TODO: make sure some interrupts(stop/nak/cpl) are enabled

    // UM10204 p10:
    //   When SDA remains HIGH during this ninth clock pulse, this is defined as the Not
    //   Acknowledge signal. The master can then generate either a STOP condition to abort the
    //   transfer, or a repeated START condition to start a new transfer.
    //
    // The current request may end without generating a STOP when the NAK is received.
    // So the next request must have START.
    vsf_i2c_irq_mask_t cpl_irq_mask = VSF_I2C_IRQ_MASK_MASTER_STOPPED
                                    | VSF_I2C_IRQ_MASK_MASTER_NACK_DETECT
                                    | VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK
                                    | VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST
                                    | VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE;

    if (irq_mask & cpl_irq_mask) {
        bool is_stop = m_i2c_ptr->request.cmd & VSF_I2C_CMD_STOP;
        if (is_stop) {   // switch new multiplex i2c instance
            bool is_need = false;
            vsf_multiplex_i2c_t *new_m_i2c_ptr;
            vsf_protect_t state = vsf_multiplex_i2c_protect();
                if (!vsf_slist_queue_is_empty(&multiplexer->waiting_queue)) {
                    is_need = true;
                    // pop new instance
                    vsf_slist_queue_dequeue(vsf_multiplex_i2c_t, slist_node, &multiplexer->waiting_queue, multiplexer->req_m_i2c);
                    new_m_i2c_ptr = multiplexer->req_m_i2c;
                    VSF_HAL_ASSERT(multiplexer->en_mask & (1 << new_m_i2c_ptr->id));
                } else {
                    multiplexer->req_m_i2c = NULL; // switch to idle
                }
            vsf_multiplex_i2c_unprotect(state);

            if (is_need) {
                vsf_err_t result = __m_i2c_request(new_m_i2c_ptr, true, true, true);
                (void) result;
                VSF_ASSERT(result == VSF_ERR_NONE);
            }
        }
    }

    vsf_i2c_isr_t *isr_ptr = &m_i2c_ptr->cfg.isr;
    vsf_i2c_irq_mask_t real_irq_mask = irq_mask & m_i2c_ptr->irq_mask;
    if ((real_irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_i2c_t *)m_i2c_ptr, real_irq_mask);
    }
}

vsf_err_t vsf_multiplex_i2c_init(vsf_multiplex_i2c_t *m_i2c_ptr, vsf_i2c_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t * const multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    m_i2c_ptr->cfg = *cfg_ptr;

    vsf_err_t result = VSF_ERR_NONE;
    bool need = false;
    vsf_protect_t state;
    state = vsf_multiplex_i2c_protect();
        if (multiplexer->init_mask == 0) {                          // first init
            vsf_slist_queue_init(&multiplexer->waiting_queue);
            need = true;
        } else if (multiplexer->inited_m_i2c == m_i2c_ptr) {        // re-init
            need = true;
        }

        if ((multiplexer->init_mask & (1 << m_i2c_ptr->id)) == 0) { // current instance is not inited
            int_fast8_t id = vsf_ffz32(multiplexer->init_mask);
            if (id < 0) {
                vsf_multiplex_i2c_unprotect(state);
                return VSF_ERR_NOT_ENOUGH_RESOURCES;
            }

            multiplexer->init_mask |= 1 << id;
            m_i2c_ptr->id = id;
        }
    vsf_multiplex_i2c_unprotect(state);

    if (need) {
        result = __m_i2c_request(m_i2c_ptr, true, false, false);
    }

    return result;
}

void vsf_multiplex_i2c_fini(vsf_multiplex_i2c_t *m_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t * const multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);
    VSF_HAL_ASSERT(multiplexer->init_mask & (1 << m_i2c_ptr->id));

    bool need = false;
    vsf_protect_t state = vsf_multiplex_i2c_protect();
        multiplexer->init_mask &= ~(1 << m_i2c_ptr->id);
        need = (multiplexer->init_mask == 0);
    vsf_multiplex_i2c_unprotect(state);

    if (need) {
        vsf_i2c_fini(m_i2c_ptr->multiplexer->i2c_ptr);
    }
}

fsm_rt_t vsf_multiplex_i2c_enable(vsf_multiplex_i2c_t *m_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t * const multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);

    VSF_HAL_ASSERT(multiplexer->init_mask & (1 << m_i2c_ptr->id));

    bool need = false;
    vsf_protect_t state = vsf_multiplex_i2c_protect();
        need = (multiplexer->en_mask == 0);
        multiplexer->en_mask |= (1 << m_i2c_ptr->id);
    vsf_multiplex_i2c_unprotect(state);

    if (need) {
        while (fsm_rt_cpl != vsf_i2c_enable(multiplexer->i2c_ptr));
    }

    return fsm_rt_cpl;
}

fsm_rt_t vsf_multiplex_i2c_disable(vsf_multiplex_i2c_t *m_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t * const multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);

    VSF_HAL_ASSERT(multiplexer->init_mask & (1 << m_i2c_ptr->id));
    VSF_HAL_ASSERT(multiplexer->en_mask & (1 << m_i2c_ptr->id));

    bool need = false;
    vsf_protect_t state = vsf_multiplex_i2c_protect();
        multiplexer->en_mask &= ~(1 << m_i2c_ptr->id);
        need = (multiplexer->en_mask == 0);       // when all i2c are disabled,
    vsf_multiplex_i2c_unprotect(state);

    if (need) {
        while (fsm_rt_cpl != vsf_i2c_disable(multiplexer->i2c_ptr));
    }

    return fsm_rt_cpl;
}

void vsf_multiplex_i2c_irq_enable(vsf_multiplex_i2c_t *m_i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t * const multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);

    VSF_HAL_ASSERT(multiplexer->init_mask & (1 << m_i2c_ptr->id));

    bool need = false;
    vsf_protect_t state = vsf_multiplex_i2c_protect();
        m_i2c_ptr->irq_mask |= irq_mask;
        vsf_i2c_irq_mask_t new_irq_mask = ~multiplexer->irq_mask & irq_mask;
        if (new_irq_mask) {
            multiplexer->irq_mask |= new_irq_mask;
            need = true;
        }
    vsf_multiplex_i2c_unprotect(state);

    if (need) {
        vsf_i2c_irq_enable(multiplexer->i2c_ptr, new_irq_mask);
    }
}

void vsf_multiplex_i2c_irq_disable(vsf_multiplex_i2c_t *m_i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t * const multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);

    VSF_HAL_ASSERT(multiplexer->init_mask & (1 << m_i2c_ptr->id));

    bool need = false;
    vsf_protect_t state = vsf_multiplex_i2c_protect();
        m_i2c_ptr->irq_mask &= ~irq_mask;
        vsf_i2c_irq_mask_t new_irq_mask = multiplexer->irq_mask & irq_mask;
        if (new_irq_mask) {
            multiplexer->irq_mask &= ~new_irq_mask;
            need = true;
        }
    vsf_multiplex_i2c_unprotect(state);

    if (need) {
        vsf_i2c_irq_disable(multiplexer->i2c_ptr, new_irq_mask);
    }
}

vsf_i2c_status_t vsf_multiplex_i2c_status(vsf_multiplex_i2c_t *m_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t * const multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);

    VSF_HAL_ASSERT(multiplexer->init_mask & (1 << m_i2c_ptr->id));

    bool need = false;
    vsf_protect_t state = vsf_multiplex_i2c_protect();
        if (multiplexer->req_m_i2c == m_i2c_ptr) {
            need = true;
        }
    vsf_multiplex_i2c_unprotect(state);

    // TODO: add more status info
    vsf_i2c_status_t status = {0};
    if (need) {
        status = vsf_i2c_status(multiplexer->i2c_ptr);
    }

    return status;
}

vsf_err_t vsf_multiplex_i2c_master_request(vsf_multiplex_i2c_t *m_i2c_ptr,
                                           uint16_t address,
                                           vsf_i2c_cmd_t cmd,
                                           uint16_t count,
                                           uint8_t *buffer_ptr)
{
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t * const multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);

    m_i2c_ptr->request.cmd = cmd;
    m_i2c_ptr->request.address = address;
    m_i2c_ptr->request.count = count;
    m_i2c_ptr->request.buffer_ptr = buffer_ptr;

    vsf_err_t result = VSF_ERR_NONE;
    bool need_init;
    bool need_enable;
    bool need_req = false;
    vsf_protect_t state = vsf_multiplex_i2c_protect();
        if (vsf_slist_queue_is_in(vsf_multiplex_i2c_t, slist_node, &multiplexer->waiting_queue, m_i2c_ptr)) {
            // the previous transmission is not yet complete
            VSF_HAL_ASSERT(0);
            result = VSF_ERR_FAIL;
        } else {
            if (multiplexer->req_m_i2c == NULL) {               // current is idle
                if ((cmd & VSF_I2C_CMD_START) == 0) {           // first request must include VSF_I2C_CMD_START
                    result = VSF_ERR_FAIL;
                } else {
                    need_req = true;
                    multiplexer->req_m_i2c = m_i2c_ptr;
                }
            } else if (multiplexer->req_m_i2c == m_i2c_ptr) {   // previous m_i2c_ptr request not yet completed
                need_req = true;
            } else {
                vsf_slist_queue_enqueue(vsf_multiplex_i2c_t, slist_node, &multiplexer->waiting_queue, m_i2c_ptr);
            }
        }

        need_init = (multiplexer->req_m_i2c != multiplexer->inited_m_i2c);
        need_enable = (multiplexer->en_mask != 0);
    vsf_multiplex_i2c_unprotect(state);

    if (need_req) {
        __m_i2c_request(m_i2c_ptr, need_init, need_enable, need_req);
    }

    return result;
}

uint_fast32_t vsf_multiplex_i2c_get_transferred_count(vsf_multiplex_i2c_t *m_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t * const multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);


    bool need = false;
    vsf_protect_t state = vsf_multiplex_i2c_protect();
        if (multiplexer->req_m_i2c == m_i2c_ptr) {
            need = true;
        }
    vsf_multiplex_i2c_unprotect(state);

    uint_fast32_t count = 0;
    if (need) {
        count = vsf_i2c_get_transferred_count(multiplexer->i2c_ptr);
    }

    return count;
}

vsf_i2c_capability_t vsf_multiplex_i2c_capability(vsf_multiplex_i2c_t *m_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_multiplexer_i2c_t * const multiplexer = m_i2c_ptr->multiplexer;
    VSF_HAL_ASSERT(NULL != multiplexer);

    return vsf_i2c_capability(multiplexer->i2c_ptr);
}

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_I2C_CFG_REIMPLEMENT_API_CAPABILITY  ENABLED
#define VSF_I2C_CFG_IMP_PREFIX                  vsf_multiplex
#define VSF_I2C_CFG_IMP_UPCASE_PREFIX           VSF_MULTIPLEX
#define VSF_I2C_CFG_IMP_EXTERN_OP               ENABLED
#include "hal/driver/common/i2c/i2c_template.inc"

#endif //!VSF_HAL_I2C_IMP_MULTIPLEX_I2C
