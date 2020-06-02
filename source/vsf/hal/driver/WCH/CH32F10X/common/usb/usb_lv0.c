/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#include "./usbd/usbd.h"

#if VSF_USE_USB_DEVICE == ENABLED

/*============================ MACROS ========================================*/

#ifdef USB_DC_HAS_CONST
#   define __USB_DC_IMPLEMENT(__N, __TYPE)                                      \
static const ch32f10x_##__TYPE##_const_t __USB_DC##__N##_const = {              \
    USB_DC##__N##_CONFIG                                                        \
};                                                                              \
ch32f10x_##__TYPE##_t USB_DC##__N = {                                           \
    .param = &__USB_DC##__N##_const                                             \
};                                                                              \
ISR(USB_DC##__N##_IRQHandler)                                                   \
{                                                                               \
    ch32f10x_##__TYPE##_irq(&USB_DC##__N);                                      \
}
#else
#   define __USB_DC_IMPLEMENT(__N, __TYPE)                                      \
ch32f10x_##__TYPE##_t USB_DC##__N;                                              \
ISR(USB_DC##__N##_IRQHandler)                                                   \
{                                                                               \
    ch32f10x_##__TYPE##_irq(&USB_DC##__N);                                      \
}
#endif

#define _USB_DC_IMPLEMENT(__N, __TYPE)   __USB_DC_IMPLEMENT(__N, __TYPE)
#define USB_DC_IMPLEMENT(__N, __VALUE)   _USB_DC_IMPLEMENT(__N, USB_DC##__N##_TYPE)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

REPEAT_MACRO(USB_DC_COUNT, USB_DC_IMPLEMENT, NULL)

#endif      // VSF_USE_USB_DEVICE
