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

#define __VSF_I2C_MERGER_REQUEST_CLASS_IMPLEMENT

#include "hal/vsf_hal_cfg.h"

#if (VSF_HAL_USE_I2C == ENABLED) && (VSF_HAL_USE_MERGER_REQUEST_I2C == ENABLED)

#include "hal/driver/driver.h"

/*============================ MACROS ========================================*/

#ifdef VSF_MREQUEST_I2C_CFG_CALL_PREFIX
#   undef VSF_I2C_CFG_IMP_PREFIX
#   define VSF_I2C_CFG_IMP_PREFIX               VSF_MREQUEST_I2C_CFG_CALL_PREFIX
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __i2c_cpl_isr_handler(void *target_ptr, vsf_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    vsf_mrequest_i2c_t *m_i2c_ptr = (vsf_mrequest_i2c_t *)target_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);

    vsf_i2c_isr_t *isr_ptr = &m_i2c_ptr->cfg.isr;
    vsf_i2c_irq_mask_t real_irq_mask = irq_mask & m_i2c_ptr->irq_mask;
    if ((real_irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_i2c_t *)m_i2c_ptr, real_irq_mask);
    }
}

static void __i2c_next_handler(void *target_ptr, vsf_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    vsf_mrequest_i2c_t *m_i2c_ptr = (vsf_mrequest_i2c_t *)target_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);

    if (m_i2c_ptr->request.count != 0) {        // buffer is empty
        // Because the completion interrupt of the first request has been returned
        // if the first request returns other interrupts, only assertions can be made here.
        VSF_HAL_ASSERT((irq_mask & ~(I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE | I2C_IRQ_MASK_MASTER_TX_EMPTY)) == 0);

        if (irq_mask & I2C_IRQ_MASK_MASTER_TX_EMPTY) {
            // Request type is write then write, first write complete
            vsf_i2c_irq_disable(m_i2c_ptr->i2c_ptr, I2C_IRQ_MASK_MASTER_TX_EMPTY);
        }

        vsf_err_t result = vsf_i2c_master_request(m_i2c_ptr->i2c_ptr,
                                                  m_i2c_ptr->request.address,
                                                  m_i2c_ptr->request.cmd,
                                                  m_i2c_ptr->request.count,
                                                  m_i2c_ptr->request.buffer_ptr);
        memset(&m_i2c_ptr->request, 0, sizeof(m_i2c_ptr->request));
        VSF_HAL_ASSERT(result == VSF_ERR_NONE);
        (void) result;
    } else {
        __i2c_cpl_isr_handler(target_ptr, i2c_ptr, irq_mask);
    }
}

vsf_err_t vsf_mrequest_i2c_init(vsf_i2c_t *i2c_ptr, vsf_i2c_cfg_t *cfg_ptr)
{
    vsf_mrequest_i2c_t *m_i2c_ptr = (vsf_mrequest_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_i2c_t * real_i2c_ptr = m_i2c_ptr->i2c_ptr;
    VSF_HAL_ASSERT(NULL != real_i2c_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    m_i2c_ptr->cfg = *cfg_ptr;

    vsf_i2c_cfg_t cfg = *cfg_ptr;
    cfg.isr.handler_fn = __i2c_next_handler;
    cfg.isr.target_ptr = m_i2c_ptr;

    return vsf_i2c_init(real_i2c_ptr, &cfg);
}

fsm_rt_t vsf_mrequest_i2c_enable(vsf_i2c_t *i2c_ptr)
{
    vsf_mrequest_i2c_t *m_i2c_ptr = (vsf_mrequest_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_i2c_t * real_i2c_ptr = m_i2c_ptr->i2c_ptr;
    VSF_HAL_ASSERT(NULL != real_i2c_ptr);

    return fsm_rt_cpl != vsf_i2c_enable(real_i2c_ptr);
}

fsm_rt_t vsf_mrequest_i2c_disable(vsf_i2c_t *i2c_ptr)
{
    vsf_mrequest_i2c_t *m_i2c_ptr = (vsf_mrequest_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_i2c_t * real_i2c_ptr = m_i2c_ptr->i2c_ptr;
    VSF_HAL_ASSERT(NULL != real_i2c_ptr);

    return fsm_rt_cpl != vsf_i2c_disable(real_i2c_ptr);
}

void vsf_mrequest_i2c_irq_enable(vsf_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    vsf_mrequest_i2c_t *m_i2c_ptr = (vsf_mrequest_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_i2c_t * real_i2c_ptr = m_i2c_ptr->i2c_ptr;
    VSF_HAL_ASSERT(NULL != real_i2c_ptr);

    vsf_i2c_irq_enable(real_i2c_ptr, irq_mask);
    m_i2c_ptr->irq_mask |= irq_mask;
}

void vsf_mrequest_i2c_irq_disable(vsf_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    vsf_mrequest_i2c_t *m_i2c_ptr = (vsf_mrequest_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_i2c_t * real_i2c_ptr = m_i2c_ptr->i2c_ptr;
    VSF_HAL_ASSERT(NULL != real_i2c_ptr);

    vsf_i2c_irq_disable(real_i2c_ptr, irq_mask);
    m_i2c_ptr->irq_mask &= ~irq_mask;
}

vsf_i2c_status_t vsf_mrequest_i2c_status(vsf_i2c_t *i2c_ptr)
{
    vsf_mrequest_i2c_t *m_i2c_ptr = (vsf_mrequest_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_i2c_t * real_i2c_ptr = m_i2c_ptr->i2c_ptr;
    VSF_HAL_ASSERT(NULL != real_i2c_ptr);

    return vsf_i2c_status(real_i2c_ptr);
}

vsf_err_t vsf_mrequest_i2c_master_request(vsf_i2c_t *i2c_ptr,
                                          uint16_t address,
                                          vsf_i2c_cmd_t cmd,
                                          uint16_t count,
                                          uint8_t *buffer_ptr)
{
    vsf_mrequest_i2c_t *m_i2c_ptr = (vsf_mrequest_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_i2c_t * real_i2c_ptr = m_i2c_ptr->i2c_ptr;
    VSF_HAL_ASSERT(NULL != real_i2c_ptr);
    VSF_HAL_ASSERT(m_i2c_ptr->irq_mask & I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE);

    bool is_start = cmd & I2C_CMD_START;
    bool is_stop = cmd & I2C_CMD_STOP;
    bool is_read = (cmd & I2C_CMD_RW_MASK) == I2C_CMD_READ;

    if (m_i2c_ptr->request.count == 0) {                                // buffer is empty
        if (is_start && is_stop) {                                      // auto start/stop
            return vsf_i2c_master_request(real_i2c_ptr, address, cmd, count, buffer_ptr);
        } else if (is_start && count <= dimof(m_i2c_ptr->buffer)) {     // only start in request: wait next request
            memcpy(m_i2c_ptr->buffer, buffer_ptr, count);
            m_i2c_ptr->request.cmd = cmd;
            m_i2c_ptr->request.count = count;
            m_i2c_ptr->request.address = address;
            m_i2c_ptr->request.buffer_ptr = m_i2c_ptr->buffer;

            __i2c_cpl_isr_handler((void *)m_i2c_ptr, real_i2c_ptr, I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE);
            return VSF_ERR_NONE;
        }
    } else {
        // write: maybe with restart, must have stop
        // read : must have restart, must have stop
        if ((!is_read && is_stop) || ((is_read && is_start && is_stop) )) {
            vsf_i2c_cmd_t first_cmd = m_i2c_ptr->request.cmd;
            uint8_t first_count = m_i2c_ptr->request.count;
            uint8_t* first_buffer_ptr = m_i2c_ptr->request.buffer_ptr;
            VSF_HAL_ASSERT(address == m_i2c_ptr->request.address);

            m_i2c_ptr->request.cmd = cmd;
            m_i2c_ptr->request.count = count;
            m_i2c_ptr->request.address = address;
            m_i2c_ptr->request.buffer_ptr = buffer_ptr;

            vsf_err_t result = vsf_i2c_master_request(real_i2c_ptr, address, first_cmd, first_count, first_buffer_ptr);

            if (!is_read) {
                vsf_i2c_irq_enable(real_i2c_ptr, I2C_IRQ_MASK_MASTER_TX_EMPTY);
            }

            return result;
        }
    }

    return VSF_ERR_NOT_SUPPORT;
}

vsf_i2c_capability_t vsf_mrequest_i2c_capability(vsf_i2c_t *i2c_ptr)
{
    vsf_mrequest_i2c_t *m_i2c_ptr = (vsf_mrequest_i2c_t *)i2c_ptr;
    VSF_HAL_ASSERT(NULL != m_i2c_ptr);
    vsf_i2c_t * real_i2c_ptr = m_i2c_ptr->i2c_ptr;
    VSF_HAL_ASSERT(NULL != real_i2c_ptr);
    vsf_err_t result = VSF_ERR_NONE;

    vsf_i2c_capability_t i2c_capability = vsf_mrequest_i2c_capability(real_i2c_ptr);
    i2c_capability.is_support_manual_start_stop = 1;
    return i2c_capability;
}

/*============================ GLOBAL VARIABLES ==============================*/

const vsf_i2c_op_t vsf_mreqeust_i2c_op = {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_OP
    VSF_I2C_APIS(vsf_mrequest)
};

#if (VSF_MREQUEST_I2C_CFG_INSTANCE == ENABLED) && (VSF_MREQUEST_I2C_CFG_INSTANCE_COUNT != 0)
VSF_MREPEAT(VSF_MREQUEST_I2C_CFG_INSTANCE_COUNT, VSF_MREQUEST_I2C_IMP, VSF_MREQUEST_I2C_CFG_INSTANCE_PREFIX)
#endif

#endif //!VSF_HAL_I2C_IMP_MULTIPLEX_I2C
