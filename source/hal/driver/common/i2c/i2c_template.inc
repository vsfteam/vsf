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

#include "hal/driver/common/template/vsf_template_hal_driver.h"

/*============================ MACROS ========================================*/
#if VSF_HAL_USE_I2C == ENABLED

#if VSF_I2C_CFG_IMPLEMENT_OP == ENABLED
#   define __VSF_I2C_OP_VAR             VSF_MCONNECT(__, VSF_I2C_CFG_IMP_PREFIX, _i2c_op)
#   define VSF_I2C_OP                   .vsf_i2c.op = & __VSF_I2C_OP_VAR,
#else
#   define VSF_I2C_OP
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ LOCAL VARIABLES ===============================*/

#if VSF_I2C_CFG_IMPLEMENT_OP == ENABLED

#   undef VSF_I2C_API
#   define VSF_I2C_API(__prefix_name, __return, __name, ...)                   \
        VSF_TEMPLATE_HAL_API_OP(__prefix_name, _i2c_, __return, __name, __VA_ARGS__)

static const vsf_i2c_op_t __VSF_I2C_OP_VAR = {
    VSF_I2C_APIS(VSF_I2C_CFG_IMP_PREFIX)
};

#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_I2C_CFG_REQUEST_TEMPLATE == ENABLED
vsf_err_t vsf_i2c_master_request(vsf_i2c_t *i2c_ptr, uint16_t address,
                                 em_i2c_cmd_t cmd, uint16_t count, uint8_t *buffer_ptr)
{
#define VSF_I2C_TYPE VSF_MCONNECT(VSF_I2C_CFG_IMP_PREFIX, _i2c_t)

    VSF_I2C_TYPE * i2c_type_ptr = (VSF_I2C_TYPE *)i2c_ptr;
    return vsf_i2c_request_master_request(i2c_ptr, &i2c_type_ptr->request, address, cmd, count, buffer_ptr);
}
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_MCONNECT(VSF_I2C_CFG_IMP_UPPERCASE_PREFIX, _I2C_COUNT)
#   define __VSF_I2C_IMP_COUNT VSF_MCONNECT(VSF_I2C_CFG_IMP_UPPERCASE_PREFIX, _I2C_COUNT)

#   if VSF_MCONNECT(VSF_I2C_CFG_IMP_UPPERCASE_PREFIX, _I2C_MASK)
#       define __VSF_I2C_IMP_MASK    VSF_MCONNECT(VSF_I2C_CFG_IMP_UPPERCASE_PREFIX, _I2C_MASK)
#   else
#       define __VSF_I2C_IMP_MASK    ((1ul << __VSF_I2C_IMP_COUNT) - 1)
#   endif

#   if __VSF_I2C_IMP_MASK & (1 << 0)
        VSF_I2C_CFG_IMP_LV0(0, NULL)
#   endif
#   if __VSF_I2C_IMP_MASK & (1 << 1)
        VSF_I2C_CFG_IMP_LV0(1, NULL)
#   endif
#   if __VSF_I2C_IMP_MASK & (1 << 2)
        VSF_I2C_CFG_IMP_LV0(2, NULL)
#   endif
#   if __VSF_I2C_IMP_MASK & (1 << 3)
        VSF_I2C_CFG_IMP_LV0(3, NULL)
#   endif
#   if __VSF_I2C_IMP_MASK & (1 << 4)
        VSF_I2C_CFG_IMP_LV0(4, NULL)
#   endif
#   if __VSF_I2C_IMP_MASK & (1 << 5)
        VSF_I2C_CFG_IMP_LV0(5, NULL)
#   endif
#   if __VSF_I2C_IMP_MASK & (1 << 6)
        VSF_I2C_CFG_IMP_LV0(6, NULL)
#   endif
#   if __VSF_I2C_IMP_MASK & (1 << 7)
        VSF_I2C_CFG_IMP_LV0(7, NULL)
#   endif

#   undef __VSF_I2C_IMP_COUNT
#   undef __VSF_I2C_IMP_MASK
#   undef VSF_I2C_CFG_IMP_LV0
#endif

#endif // VSF_HAL_USE_I2C == ENABLED
