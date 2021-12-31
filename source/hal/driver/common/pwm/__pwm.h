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

#ifndef __HAL_DRIVER_COMMON_PWM_H__
#define __HAL_DRIVER_COMMON_PWM_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


#if PWM_MAX_PORT >= 0 && VSF_HAL_USE_PWM0 == ENABLED && (PWM_PORT_MASK & (1 << 0))
#   ifndef VSF_HAL_PWM0_CS_CNT
#       define VSF_HAL_PWM0_CS_CNT 2
#   endif
VSF_MREPEAT(VSF_HAL_PWM0_CS_CNT, __MULTI_PWM_DEF, VSF_PWM0_CS)
#endif

#if PWM_MAX_PORT >= 1 && VSF_HAL_USE_PWM1 == ENABLED && (PWM_PORT_MASK & (1 << 1))
#   ifndef VSF_HAL_PWM1_CS_CNT
#       define VSF_HAL_PWM1_CS_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_PWM1_CS_CNT, __MULTI_PWM_DEF, VSF_PWM1_CS)
#endif

#if PWM_MAX_PORT >= 2 && VSF_HAL_USE_PWM2 == ENABLED && (PWM_PORT_MASK & (1 << 2))
#   ifndef VSF_HAL_PWM2_CS_CNT
#       define VSF_HAL_PWM2_CS_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_PWM2_CS_CNT, __MULTI_PWM_DEF, VSF_PWM2_CS)
#endif

#if PWM_MAX_PORT >= 3 && VSF_HAL_USE_PWM3 == ENABLED && (PWM_PORT_MASK & (1 << 3))
#   ifndef VSF_HAL_PWM3_CS_CNT
#       define VSF_HAL_PWM3_CS_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_PWM3_CS_CNT, __MULTI_PWM_DEF, VSF_PWM3_CS)
#endif

#if PWM_MAX_PORT >= 4 && VSF_HAL_USE_PWM4 == ENABLED && (PWM_PORT_MASK & (1 << 4))
#   ifndef VSF_HAL_PWM4_CS_CNT
#       define VSF_HAL_PWM4_CS_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_PWM4_CS_CNT, __MULTI_PWM_DEF, VSF_PWM4_CS)
#endif

#if PWM_MAX_PORT >= 5 && VSF_HAL_USE_PWM5 == ENABLED && (PWM_PORT_MASK & (1 << 5))
#   ifndef VSF_HAL_PWM5_CS_CNT
#       define VSF_HAL_PWM5_CS_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_PWM5_CS_CNT, V__MULTI_PWM_DEF, VSF_PWM5_CS)
#endif

#if PWM_MAX_PORT >= 6 && VSF_HAL_USE_PWM6 == ENABLED && (PWM_PORT_MASK & (1 << 6))
#   ifndef VSF_HAL_PWM6_CS_CNT
#       define VSF_HAL_PWM6_CS_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_PWM6_CS_CNT, __MULTI_PWM_DEF, VSF_PWM6_CS)
#endif

#if PWM_MAX_PORT >= 7 && VSF_HAL_USE_PWM7 == ENABLED && (PWM_PORT_MASK & (1 << 7))
#   ifndef VSF_HAL_PWM7_CS_CNT
#       define VSF_HAL_PWM7_CS_CNT 1
#   endif
VSF_MREPEAT(VSF_HAL_PWM7_CS_CNT, __MULTI_PWM_DEF, VSF_PWM7_CS)
#endif

#endif
