/*****************************************************************************
 *   Copyright(C)2009-2020 by VSF Team                                       *
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

#ifndef __OSA_HAL_DRIVER_MULTIPLEX_IIC_H__
#define __OSA_HAL_DRIVER_MULTIPLEX_IIC_H__

/*============================ INCLUDES ======================================*/
#include "osa_hal/vsf_osa_hal_cfg.h"

#if VSF_USE_MULTIPLEX_HAL == ENABLED && VSF_USE_MULTIPLEX_IIC == ENABLED

#include "osa_hal/sw_peripheral/io_peripheral/iic/vsf_io_iic.h"
#include "../vsf_multiplex_hal_common.h"

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */

#if     defined(__VSF_MULTIPLEX_IIC_CLASS_IMPLEMENT)
#   undef __VSF_MULTIPLEX_IIC_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__

#elif   defined(__VSF_MULTIPLEX_IIC_CLASS_INHERIT)
#   undef __VSF_MULTIPLEX_IIC_CLASS_INHERIT
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_MULTIPLEX_IIC_RW_SIZE(__REG_ADDR_SIZE, __DATA_SIZE)                 \
     (3 + (__REG_ADDR_SIZE) + (__DATA_SIZE))
#define VSF_MULTIPLEX_IIC_WRITE_REG8(__REG, __DATA_SIZE, ...)                   \
     0x01, (__REG), ((__DATA_SIZE) >> 0) & 0xFF, ((__DATA_SIZE) >> 8) & 0xFF, __VA_ARGS__
#define VSF_MULTIPLEX_IIC_READ_REG8(__REG, __DATA_SIZE, ...)                    \
     0x11, (__REG), ((__DATA_SIZE) >> 0) & 0xFF, ((__DATA_SIZE) >> 8) & 0xFF, __VA_ARGS__

/*============================ TYPES =========================================*/

declare_simple_class(vsf_multiplex_iic_t)

def_simple_class(vsf_multiplex_iic_t) {
    public_member(
        implement(vsf_io_iic_t)
    )
    private_member(
        implement(vsf_multiplex_hal_t)
        uint8_t state;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_multiplex_iic_init(vsf_multiplex_iic_t *iic);
extern vsf_err_t vsf_multiplex_iic_read(vsf_multiplex_iic_t *iic, uint_fast8_t chip_addr, uint8_t *buffer, uint_fast32_t size, uint_fast8_t flags);
extern vsf_err_t vsf_multiplex_iic_write(vsf_multiplex_iic_t *iic, uint_fast8_t chip_addr, uint8_t *buffer, uint_fast32_t size, uint_fast8_t flags);
extern vsf_err_t vsf_multiplex_iic_read_reg(vsf_multiplex_iic_t *iic, uint_fast8_t chip_addr, uint_fast32_t reg_addr, uint_fast8_t reg_addr_size, uint8_t *buffer, uint_fast32_t size);
extern vsf_err_t vsf_multiplex_iic_write_reg(vsf_multiplex_iic_t *iic, uint_fast8_t chip_addr, uint_fast32_t reg_addr, uint_fast8_t reg_addr_size, uint8_t *buffer, uint_fast32_t size);
extern vsf_err_t vsf_multiplex_iic_execute(vsf_multiplex_iic_t *iic, uint_fast8_t chip_addr, uint8_t *cmd, uint_fast32_t size);

#endif
#endif

/* EOF */
