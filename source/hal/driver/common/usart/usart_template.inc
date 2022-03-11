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

#if VSF_HAL_USE_USART == ENABLED

#define VSF_USART_CFG_FIFO2REQ_PREFIX           vsf_fifo2req

#if VSF_USART_CFG_IMPLEMENT_OP == ENABLED
#   define __VSF_USART_OP_VAR                     VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_op)
#   define VSF_USART_OP                           .vsf_usart.op = & __VSF_USART_OP_VAR,
#else
#   define VSF_USART_OP
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

#if VSF_USART_CFG_IMPLEMENT_OP == ENABLED

#   undef VSF_USART_API
#   define VSF_USART_API(__prefix_name, __return, __name, ...)                   \
        VSF_TEMPLATE_HAL_API_OP(__prefix_name, _usart_, __return, __name, __VA_ARGS__)

static const vsf_usart_op_t __VSF_USART_OP_VAR = {
    VSF_USART_APIS(VSF_USART_CFG_INSTANCE_PREFIX)
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_MCONNECT(VSF_USART_CFG_IMP_UPPERCASE_PREFIX, _USART_COUNT)
#   define __VSF_USART_IMP_COUNT VSF_MCONNECT(VSF_USART_CFG_IMP_UPPERCASE_PREFIX, _USART_COUNT)

#   if VSF_MCONNECT(VSF_USART_CFG_IMP_UPPERCASE_PREFIX, _USART_MASK)
#       define __VSF_USART_IMP_MASK    VSF_MCONNECT(VSF_USART_CFG_IMP_UPPERCASE_PREFIX, _USART_MASK)
#   else
#       define __VSF_USART_IMP_MASK    ((1ul << __VSF_USART_IMP_COUNT) - 1)
#   endif

#   if __VSF_USART_IMP_MASK & (1 << 0)
        VSF_USART_CFG_IMP_LV0(0, NULL)
#   endif
#   if __VSF_USART_IMP_MASK & (1 << 1)
        VSF_USART_CFG_IMP_LV0(1, NULL)
#   endif
#   if __VSF_USART_IMP_MASK & (1 << 2)
        VSF_USART_CFG_IMP_LV0(2, NULL)
#   endif
#   if __VSF_USART_IMP_MASK & (1 << 3)
        VSF_USART_CFG_IMP_LV0(3, NULL)
#   endif
#   if __VSF_USART_IMP_MASK & (1 << 4)
        VSF_USART_CFG_IMP_LV0(4, NULL)
#   endif
#   if __VSF_USART_IMP_MASK & (1 << 5)
        VSF_USART_CFG_IMP_LV0(5, NULL)
#   endif
#   if __VSF_USART_IMP_MASK & (1 << 6)
        VSF_USART_CFG_IMP_LV0(6, NULL)
#   endif
#   if __VSF_USART_IMP_MASK & (1 << 7)
        VSF_USART_CFG_IMP_LV0(7, NULL)
#   endif

#   undef __VSF_USART_IMP_COUNT
#   undef __VSF_USART_IMP_MASK
#   undef VSF_USART_CFG_IMP_LV0
#endif


#endif // VSF_HAL_USE_USART == ENABLED
