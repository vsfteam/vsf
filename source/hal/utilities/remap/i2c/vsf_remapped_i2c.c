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

#include "hal/vsf_hal.h"

#if VSF_HAL_USE_I2C == ENABLED

#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_REMAPPED_I2C_CLASS_IMPLEMENT
#include "./vsf_remapped_i2c.h"

#if VSF_I2C_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_REMAPPED_I2C_CFG_MULTI_CLASS == ENABLED
const vsf_i2c_op_t vsf_remapped_i2c_op = {
#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API   VSF_HAL_TEMPLATE_API_OP

    VSF_I2C_APIS(vsf_remapped_i2c)
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_remapped_i2c_init(vsf_remapped_i2c_t *i2c, vsf_i2c_cfg_t *cfg)
{
    VSF_HAL_ASSERT((i2c != NULL) && (i2c->target != NULL));
    return vsf_i2c_init(i2c->target, cfg);
}

void vsf_remapped_i2c_fini(vsf_remapped_i2c_t *i2c)
{
    VSF_HAL_ASSERT((i2c != NULL) && (i2c->target != NULL));
    vsf_i2c_fini(i2c->target);
}

vsf_err_t vsf_remapped_i2c_get_configuration(vsf_remapped_i2c_t *i2c, vsf_i2c_cfg_t *cfg)
{
    VSF_HAL_ASSERT((i2c != NULL) && (i2c->target != NULL));
    return vsf_i2c_get_configuration(i2c->target, cfg);
}

fsm_rt_t vsf_remapped_i2c_enable(vsf_remapped_i2c_t *i2c)
{
    VSF_HAL_ASSERT((i2c != NULL) && (i2c->target != NULL));
    return vsf_i2c_enable(i2c->target);
}

fsm_rt_t vsf_remapped_i2c_disable(vsf_remapped_i2c_t *i2c)
{
    VSF_HAL_ASSERT((i2c != NULL) && (i2c->target != NULL));
    return vsf_i2c_disable(i2c->target);
}

void vsf_remapped_i2c_irq_enable(vsf_remapped_i2c_t *i2c, vsf_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT((i2c != NULL) && (i2c->target != NULL));
    vsf_i2c_irq_enable(i2c->target, irq_mask);
}

void vsf_remapped_i2c_irq_disable(vsf_remapped_i2c_t *i2c, vsf_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT((i2c != NULL) && (i2c->target != NULL));
    vsf_i2c_irq_disable(i2c->target, irq_mask);
}

vsf_i2c_status_t vsf_remapped_i2c_status(vsf_remapped_i2c_t *i2c)
{
    VSF_HAL_ASSERT((i2c != NULL) && (i2c->target != NULL));
    return vsf_i2c_status(i2c->target);
}

vsf_i2c_capability_t vsf_remapped_i2c_capability(vsf_remapped_i2c_t *i2c)
{
    VSF_HAL_ASSERT((i2c != NULL) && (i2c->target != NULL));
    return vsf_i2c_capability(i2c->target);
}

void vsf_remapped_i2c_master_fifo_transfer(vsf_remapped_i2c_t *i2c,
        uint16_t address, vsf_i2c_cmd_t cmd, uint_fast16_t count, uint8_t *buffer,
        vsf_i2c_cmd_t *cur_cmd_ptr, uint_fast16_t *offset_ptr)
{
    VSF_HAL_ASSERT((i2c != NULL) && (i2c->target != NULL));
    vsf_i2c_master_fifo_transfer(i2c->target, address, cmd, count, buffer, cur_cmd_ptr, offset_ptr);
}

uint_fast16_t vsf_remapped_i2c_slave_fifo_transfer(vsf_remapped_i2c_t *i2c,
        bool transmit_or_receive, uint_fast16_t count, uint8_t *buffer)
{
    VSF_HAL_ASSERT((i2c != NULL) && (i2c->target != NULL));
    return vsf_i2c_slave_fifo_transfer(i2c->target, transmit_or_receive, count, buffer);
}

vsf_err_t vsf_remapped_i2c_master_request(vsf_remapped_i2c_t *i2c,
        uint16_t address, vsf_i2c_cmd_t cmd, uint_fast16_t count, uint8_t *buffer)
{
    VSF_HAL_ASSERT((i2c != NULL) && (i2c->target != NULL));
    return vsf_i2c_master_request(i2c->target, address, cmd, count, buffer);
}

vsf_err_t vsf_remapped_i2c_slave_request(vsf_remapped_i2c_t *i2c,
        bool transmit_or_receive, uint_fast16_t count, uint8_t *buffer)
{
    VSF_HAL_ASSERT((i2c != NULL) && (i2c->target != NULL));
    return vsf_i2c_slave_request(i2c->target, transmit_or_receive, count, buffer);
}

uint_fast32_t vsf_remapped_i2c_master_get_transferred_count(vsf_remapped_i2c_t *i2c)
{
    VSF_HAL_ASSERT((i2c != NULL) && (i2c->target != NULL));
    return vsf_i2c_master_get_transferred_count(i2c->target);
}

uint_fast32_t vsf_remapped_i2c_slave_get_transferred_count(vsf_remapped_i2c_t *i2c)
{
    VSF_HAL_ASSERT((i2c != NULL) && (i2c->target != NULL));
    return vsf_i2c_slave_get_transferred_count(i2c->target);
}

vsf_err_t vsf_remapped_i2c_ctrl(vsf_remapped_i2c_t *i2c, vsf_i2c_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT((i2c != NULL) && (i2c->target != NULL));
    return vsf_i2c_ctrl(i2c->target, ctrl, param);
}

#endif
#endif
