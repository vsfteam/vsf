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

#if defined(__VSF_HEADER_ONLY_SHOW_ARCH_INFO__) || defined(__VSF_HAL_SHOW_VENDOR_INFO__)

#   include "__device.h"

#else

#ifndef __HAL_DRIVER_LUAT_AIR05_H__
#define __HAL_DRIVER_LUAT_AIR05_H__

/*============================ INCLUDES ======================================*/

#       include "hal/vsf_hal_cfg.h"
#       include "./device.h"

#       include "./uart/uart.h"
#       include "./usb/usb.h"

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/

#if VSF_HAL_USE_USART == ENABLED
#   define VSF_USART_CFG_REIMPLEMENT_TYPE_MODE              ENABLED
#   define VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK          ENABLED
#   include "hal/driver/common/template/vsf_template_usart.h"

#   define VSF_USART_CFG_DEC_PREFIX                         __vsf_hw
#   define VSF_USART_CFG_DEC_UPCASE_PREFIX                  __VSF_HW
#   define VSF_USART_CFG_DEC_COUNT_MASK_PREFIX              VSF_HW
#   include "hal/driver/common/usart/usart_template.h"

#   define VSF_USART_CFG_DEC_PREFIX                         vsf_hw
#   define VSF_USART_CFG_DEC_UPCASE_PREFIX                  VSF_HW
#   define VSF_USART_CFG_DEC_REMAP_PREFIX                   vsf_fifo2req
#   include "hal/driver/common/usart/usart_template.h"
#endif

#   endif   // __HAL_DRIVER_LUAT_AIR05_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
