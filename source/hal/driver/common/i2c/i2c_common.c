/*****************************************************************************
 *   Cop->right(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a cop-> of the License at                                  *
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

#define VSF_I2C_CFG_FUNCTION_RENAME DISABLED

#include "hal/driver/driver.h"

#if VSF_HAL_USE_I2C == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

#if VSF_I2C_CFG_MULTI_CLASS == ENABLED

vsf_err_t vsf_i2c_init(vsf_i2c_t *i2c_ptr, i2c_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(i2c_ptr != NULL);
    VSF_HAL_ASSERT(i2c_ptr->op != NULL);
    VSF_HAL_ASSERT(i2c_ptr->op->init != NULL);

    return i2c_ptr->op->init(i2c_ptr, cfg_ptr);
}

fsm_rt_t vsf_i2c_enable(vsf_i2c_t *i2c_ptr)
{
    VSF_HAL_ASSERT(i2c_ptr != NULL);
    VSF_HAL_ASSERT(i2c_ptr->op != NULL);
    VSF_HAL_ASSERT(i2c_ptr->op->enable != NULL);

    return i2c_ptr->op->enable(i2c_ptr);
}

fsm_rt_t vsf_i2c_disable(vsf_i2c_t *i2c_ptr)
{
    VSF_HAL_ASSERT(i2c_ptr != NULL);
    VSF_HAL_ASSERT(i2c_ptr->op != NULL);
    VSF_HAL_ASSERT(i2c_ptr->op->disable != NULL);

    return i2c_ptr->op->disable(i2c_ptr);
}

void vsf_i2c_irq_enable(vsf_i2c_t *i2c_ptr, em_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(i2c_ptr != NULL);
    VSF_HAL_ASSERT(i2c_ptr->op != NULL);
    VSF_HAL_ASSERT(i2c_ptr->op->irq_enable != NULL);

    i2c_ptr->op->irq_enable(i2c_ptr, irq_mask);
}

void vsf_i2c_irq_disable(vsf_i2c_t *i2c_ptr, em_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(i2c_ptr != NULL);
    VSF_HAL_ASSERT(i2c_ptr->op != NULL);
    VSF_HAL_ASSERT(i2c_ptr->op->irq_disable != NULL);

    i2c_ptr->op->irq_disable(i2c_ptr, irq_mask);
}

i2c_status_t vsf_i2c_status(vsf_i2c_t *i2c_ptr)
{
    VSF_HAL_ASSERT(i2c_ptr != NULL);
    VSF_HAL_ASSERT(i2c_ptr->op != NULL);
    VSF_HAL_ASSERT(i2c_ptr->op->status != NULL);

    return i2c_ptr->op->status(i2c_ptr);
}

i2c_capability_t vsf_i2c_capability(vsf_i2c_t *i2c_ptr)
{
    VSF_HAL_ASSERT(i2c_ptr != NULL);
    VSF_HAL_ASSERT(i2c_ptr->op != NULL);
    VSF_HAL_ASSERT(i2c_ptr->op->capability != NULL);

    return i2c_ptr->op->capability(i2c_ptr);
}

vsf_err_t vsf_i2c_master_request(vsf_i2c_t *i2c_ptr,
                                 uint16_t address,
                                 em_i2c_cmd_t cmd,
                                 uint16_t count,
                                 uint8_t *buffer_ptr)
{
    VSF_HAL_ASSERT(i2c_ptr != NULL);
    VSF_HAL_ASSERT(i2c_ptr->op != NULL);
    VSF_HAL_ASSERT(i2c_ptr->op->master_request != NULL);

    return i2c_ptr->op->master_request(i2c_ptr, address, cmd, count, buffer_ptr);
}

#endif /* VSF_I2C_CFG_MULTI_CLASS == ENABLED */
#endif /* VSF_HAL_USE_I2C == ENABLED */
