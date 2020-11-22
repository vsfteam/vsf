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



/*============================ INCLUDES ======================================*/
#ifndef __HAL_DRIVER_COMMON_UART_POLL_UTILS_H__
#define __HAL_DRIVER_COMMON_UART_POLL_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if USART_MAX_PORT >= 0
#ifndef VSF_HAL_DRV_CFG_USART0_POLL_MODE_EN
#   define VSF_HAL_DRV_CFG_USART0_POLL_MODE_EN     DISABLED
#endif
#endif

#if USART_MAX_PORT >= 1
#ifndef VSF_HAL_DRV_CFG_USART1_POLL_MODE_EN
#   define VSF_HAL_DRV_CFG_USART1_POLL_MODE_EN     DISABLED
#endif
#endif

#if USART_MAX_PORT >= 2
#ifndef VSF_HAL_DRV_CFG_USART2_POLL_MODE_EN
#   define VSF_HAL_DRV_CFG_USART2_POLL_MODE_EN     DISABLED
#endif
#endif

#if USART_MAX_PORT >= 3
#ifndef VSF_HAL_DRV_CFG_USART3_POLL_MODE_EN
#   define VSF_HAL_DRV_CFG_USART3_POLL_MODE_EN     DISABLED
#endif
#endif

#if USART_MAX_PORT >= 4
#ifndef VSF_HAL_DRV_CFG_USART4_POLL_MODE_EN
#   define VSF_HAL_DRV_CFG_USART4_POLL_MODE_EN     DISABLED
#endif
#endif

#if USART_MAX_PORT >= 5
#ifndef VSF_HAL_DRV_CFG_USART5_POLL_MODE_EN
#   define VSF_HAL_DRV_CFG_USART5_POLL_MODE_EN     DISABLED
#endif
#endif


#if USART_MAX_PORT >= 6
#ifndef VSF_HAL_DRV_CFG_USART6_POLL_MODE_EN
#   define VSF_HAL_DRV_CFG_USART6_POLL_MODE_EN     DISABLED
#endif
#endif

#if USART_MAX_PORT >= 7
#ifndef VSF_HAL_DRV_CFG_USART7_POLL_MODE_EN
#   define VSF_HAL_DRV_CFG_USART7_POLL_MODE_EN     DISABLED
#endif
#endif

#if USART_MAX_PORT >= 8
#ifndef VSF_HAL_DRV_CFG_USART8_POLL_MODE_EN
#   define VSF_HAL_DRV_CFG_USART8_POLL_MODE_EN     DISABLED
#endif
#endif

#if USART_MAX_PORT >= 0
//  get macro for USART_MAX_PORT + 1
#   define MFUNC_IN_U8_DEC_VALUE       (USART_MAX_PORT + 1)
#   include "utilities/preprocessor/mf_u8_dec2str.h"
#   define __USART_MAX_NORNIMAL_PORT   MFUNC_OUT_DEC_STR
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if USART_MAX_PORT >= 0
#   define __USART_POLL_AGENT(__N, ...)                                         \
    if (VSF_HAL_DRV_CFG_USART##__N##_POLL_MODE_EN) {                            \
        ret &= vsf_usart##__N##_poll();                                         \
    }

#   define __USART_POLL_ENTRY(__N, ...)                                         \
    extern bool vsf_usart##__N##_poll(void);

#   define USART_POLL_AGENTS   REPEAT_MACRO(__USART_MAX_NORNIMAL_PORT, __USART_POLL_AGENT, NULL)
#else
#   define USART_POLL_AGENTS
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#if USART_MAX_PORT >= 0
REPEAT_MACRO(__USART_MAX_NORNIMAL_PORT, __USART_POLL_ENTRY, NULL)
#endif


#ifdef __cplusplus
}
#endif

#endif
/* EOF */
