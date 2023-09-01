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

#ifndef __HAL_DRIVER_DW_APB_I2C_H__
#define __HAL_DRIVER_DW_APB_I2C_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_I2C == ENABLED

#include "./vsf_dw_apb_i2c_reg.h"

#if     defined(__VSF_HAL_DW_APB_I2C_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_DW_APB_I2C_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_DW_APB_I2C_CFG_MULTI_CLASS
#   define VSF_DW_APB_I2C_CFG_MULTI_CLASS   VSF_I2C_CFG_MULTI_CLASS
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// if VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD is already defined, means chip driver implement
//  it's own vsf_i2c_cmd_t, skip the vsf_i2c_cmd_t here.
#if !defined(VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD)
#   define VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD ENABLED
typedef enum vsf_i2c_cmd_t {
    VSF_I2C_CMD_WRITE           = (0x00ul << 3),
    VSF_I2C_CMD_READ            = (0x01ul << 3),

    VSF_I2C_CMD_START           = (0x01ul << 0),
    VSF_I2C_CMD_RESTART         = (0x01ul << 21),

    VSF_I2C_CMD_7_BITS          = (0x00ul << 23),
    VSF_I2C_CMD_10_BITS         = (0x01ul << 23),

    __I2C_CMD_HW_MASK           = VSF_I2C_CMD_WRITE |
                                  VSF_I2C_CMD_READ |
                                  VSF_I2C_CMD_START |
                                  VSF_I2C_CMD_RESTART,

    VSF_I2C_CMD_NO_START        = (0x01ul << 24),
    VSF_I2C_CMD_STOP            = (0x01ul << 25),
    VSF_I2C_CMD_NO_STOP_RESTART = (0x01ul << 26),
} vsf_i2c_cmd_t;
#endif

/*============================ INCLUDES ======================================*/

// IP cores which will export class(es), need to include template before definition
//  of the class. Because the class member(s) maybe depend on the template.
#include "hal/driver/common/template/vsf_template_i2c.h"

/*============================ TYPES =========================================*/

vsf_class(vsf_dw_apb_i2c_t) {
#if VSF_DW_APB_I2C_CFG_MULTI_CLASS == ENABLED
    public_member(
        vsf_i2c_t               vsf_i2c;
    )
#endif
    public_member(
        vsf_dw_apb_i2c_reg_t    *reg;
    )
    private_member(
        vsf_i2c_isr_t           isr;
    )
};

/*============================ INCLUDES ======================================*/

extern vsf_err_t vsf_dw_apb_i2c_init(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr, vsf_i2c_cfg_t *cfg_ptr);
extern void vsf_dw_apb_i2c_fini(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr);
extern fsm_rt_t vsf_dw_apb_i2c_enable(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr);
extern fsm_rt_t vsf_dw_apb_i2c_disable(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr);
extern vsf_i2c_irq_mask_t vsf_dw_apb_i2c_irq_enable(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr,
        vsf_i2c_irq_mask_t irq_mask);
extern vsf_i2c_irq_mask_t vsf_dw_apb_i2c_irq_disable(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr,
        vsf_i2c_irq_mask_t irq_mask);
extern vsf_i2c_status_t vsf_dw_apb_i2c_status(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr);
extern void vsf_dw_apb_i2c_isrhandler(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr);
extern vsf_err_t vsf_dw_apb_i2c_master_request(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr,
        uint16_t address, vsf_i2c_cmd_t cmd, uint16_t count, uint8_t *buffer);
extern uint_fast32_t vsf_dw_apb_i2c_get_transferred_count(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr);
extern vsf_i2c_capability_t vsf_dw_apb_i2c_capability(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr);
extern void vsf_dw_apb_i2c_irqhandler(vsf_dw_apb_i2c_t *dw_apb_i2c_ptr);

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_I2C == ENABLED
#endif
/* EOF */
