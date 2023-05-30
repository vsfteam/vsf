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

#ifdef __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

#else

#ifndef __HAL_DEVICE_X86_WIN_H__
#define __HAL_DEVICE_X86_WIN_H__

/*============================ MACROS ========================================*/

#define VSF_HW_RTC_COUNT                        1
#define VSF_HW_RNG_COUNT                        1
#   define VSF_HW_RNG_BITLEN                    8

#ifndef VSF_WIN_USART_COUNT
#   define VSF_WIN_USART_COUNT                  32
#endif

#define VSF_HW_FLASH_COUNT                      1
#define VSF_HW_SPI_COUNT                        3


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_X86_WIN_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
