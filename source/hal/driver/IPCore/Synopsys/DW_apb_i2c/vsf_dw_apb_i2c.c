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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_I2C == ENABLED

#define __VSF_HAL_DW_APB_I2C_CLASS_IMPLEMENT
#include "hal/vsf_hal.h"

// if __HAL_DRIVER_DW_APB_I2C_H__ is defined, means chip driver include "vsf_dw_apb_i2c.h".
#ifdef __HAL_DRIVER_DW_APB_I2C_H__

#include "./vsf_dw_apb_i2c_reg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_dw_apb_i2c_init(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr, vsf_i2c_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    return VSF_ERR_NONE;
}

void vsf_dw_apb_i2c_fini(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);
}

fsm_rt_t vsf_dw_apb_i2c_enable(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_dw_apb_i2c_disable(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);

    return fsm_rt_cpl;
}

vsf_i2c_irq_mask_t vsf_dw_apb_i2c_irq_enable(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);
}

vsf_i2c_irq_mask_t vsf_dw_apb_i2c_irq_disable(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);
}

vsf_i2c_status_t vsf_dw_apb_i2c_status(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);

    return (vsf_i2c_status_t){0};
}

void vsf_dw_apb_i2c_isrhandler(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);
    if (dw_apb_i2c_ptr->isr.handler_fn != NULL) {
        dw_apb_i2c_ptr->isr.handler_fn(dw_apb_i2c_ptr->isr.target_ptr,
                (vsf_i2c_t *)dw_apb_i2c_ptr, 0);
    }
}

vsf_err_t vsf_dw_apb_i2c_master_request(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr,
                                    uint16_t address,
                                    vsf_i2c_cmd_t cmd,
                                    uint16_t count,
                                    uint8_t *buffer)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);
    return VSF_ERR_NONE;
}

uint_fast32_t vsf_dw_apb_i2c_get_transferred_count(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);
    return 0;
}

vsf_i2c_capability_t vsf_dw_apb_i2c_capability(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr)
{
    VSF_HAL_ASSERT(NULL != dw_apb_i2c_ptr);
    vsf_i2c_capability_t capability = {
        .irq_mask = 0,
        .support_no_start = 1,
        .support_no_stop_restart = 1,
        .support_restart = 1,
        .max_transfer_size = 0xFFFF,
        .min_transfer_size = 0,
    };
    return capability;
}

void vsf_dw_apb_i2c_irqhandler(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr)
{
}

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#endif      // __HAL_DRIVER_DW_APB_I2C_H__
#endif      // VSF_HAL_USE_I2C
