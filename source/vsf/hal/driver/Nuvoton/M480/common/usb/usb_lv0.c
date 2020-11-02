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
#include "./ohci/ohci.h"
#include "./dc/usbd_hs.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_USE_USB_HOST == ENABLED
#define ____USB_HC_IMPLEMENT(__N, __TYPE)                                       \
static const m480_##__TYPE##_const_t __USB_HC##__N##_const = {                  \
    USB_HC##__N##_CONFIG                                                        \
};                                                                              \
m480_##__TYPE##_t USB_HC##__N##_IP = {                                          \
    .param = &__USB_HC##__N##_const,                                            \
};                                                                              \
ROOT ISR(USB_HC##__N##_IRQHandler)                                              \
{                                                                               \
    m480_##__TYPE##_irq(&USB_HC##__N##_IP);                                     \
}
#else
#define ____USB_HC_IMPLEMENT(__N, __TYPE)                                       \
static const m480_##__TYPE##_const_t __USB_HC##__N##_const = {                  \
    USB_HC##__N##_CONFIG                                                        \
};                                                                              \
m480_##__TYPE##_t USB_HC##__N##_IP = {                                          \
    .param = &__USB_HC##__N##_const,                                            \
};                                                                              \
ISR(USB_HC##__N##_IRQHandler)                                                   \
{                                                                               \
    m480_##__TYPE##_irq(&USB_HC##__N##_IP);                                     \
}
#endif

#define __USB_HC_IMPLEMENT(__N, __TYPE)     ____USB_HC_IMPLEMENT(__N, __TYPE)
#define USB_HC_IMPLEMENT(__N, __VALUE)      __USB_HC_IMPLEMENT(__N, USB_HC##__N##_TYPE)



 
#ifdef M480_USBD_HS_WROKAROUND_ISO
#   if VSF_USE_USB_DEVICE == ENABLED
#   define __USB_DC_IMPLEMENT(__N, __TYPE)                                      \
static uint16_t __USB_DC##__N##_tx_size[USB_DC##__N##_EP_NUM - 2];              \
static uint8_t __USB_DC##__N##_tx_retry_cnt[USB_DC##__N##_EP_NUM - 2];          \
static const m480_##__TYPE##_const_t __USB_DC##__N##_const = {                  \
    .tx_size = __USB_DC##__N##_tx_size,                                         \
    .tx_retry_cnt = __USB_DC##__N##_tx_retry_cnt,                               \
    .ep_num = USB_DC##__N##_EP_NUM,                                             \
    USB_DC##__N##_CONFIG                                                        \
};                                                                              \
m480_##__TYPE##_t USB_DC##__N = {                                               \
    .param = &__USB_DC##__N##_const                                             \
};                                                                              \
ROOT ISR(USB_DC##__N##_IRQHandler)                                              \
{                                                                               \
    m480_##__TYPE##_irq(&USB_DC##__N);                                          \
}
#   else
#   define __USB_DC_IMPLEMENT(__N, __TYPE)                                      \
static uint16_t __USB_DC##__N##_tx_size[USB_DC##__N##_EP_NUM - 2];              \
static uint8_t __USB_DC##__N##_tx_retry_cnt[USB_DC##__N##_EP_NUM - 2];          \
static const m480_##__TYPE##_const_t __USB_DC##__N##_const = {                  \
    .tx_size = __USB_DC##__N##_tx_size,                                         \
    .tx_retry_cnt = __USB_DC##__N##_tx_retry_cnt,                               \
    .ep_num = USB_DC##__N##_EP_NUM,                                             \
    USB_DC##__N##_CONFIG                                                        \
};                                                                              \
m480_##__TYPE##_t USB_DC##__N = {                                               \
    .param = &__USB_DC##__N##_const                                             \
};                                                                              \
ISR(USB_DC##__N##_IRQHandler)                                                   \
{                                                                               \
    m480_##__TYPE##_irq(&USB_DC##__N);                                          \
}
#   endif   // VSF_USE_USB_DEVICE
#else       // M480_USBD_HS_WROKAROUND_ISO
#   if VSF_USE_USB_DEVICE == ENABLED
#   define __USB_DC_IMPLEMENT(__N, __TYPE)                                      \
static const m480_##__TYPE##_const_t __USB_DC##__N##_const = {                  \
    USB_DC##__N##_CONFIG                                                        \
};                                                                              \
m480_##__TYPE##_t USB_DC##__N = {                                               \
    .param = &__USB_DC##__N##_const                                             \
};                                                                              \
ROOT ISR(USB_DC##__N##_IRQHandler)                                              \
{                                                                               \
    m480_##__TYPE##_irq(&USB_DC##__N);                                          \
}
#   else
#   define __USB_DC_IMPLEMENT(__N, __TYPE)                                      \
static const m480_##__TYPE##_const_t __USB_DC##__N##_const = {                  \
    USB_DC##__N##_CONFIG                                                        \
};                                                                              \
m480_##__TYPE##_t USB_DC##__N = {                                               \
    .param = &__USB_DC##__N##_const                                             \
};                                                                              \
ISR(USB_DC##__N##_IRQHandler)                                                   \
{                                                                               \
    m480_##__TYPE##_irq(&USB_DC##__N);                                          \
}
#   endif   // VSF_USE_USB_DEVICE
#endif      // M480_USBD_HS_WROKAROUND_ISO

#define _USB_DC_IMPLEMENT(__N, __TYPE)      __USB_DC_IMPLEMENT(__N, __TYPE)
#define USB_DC_IMPLEMENT(__N, __VALUE)      _USB_DC_IMPLEMENT(__N, USB_DC##__N##_TYPE)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

REPEAT_MACRO(USB_HC_COUNT, USB_HC_IMPLEMENT, NULL)
REPEAT_MACRO(USB_DC_COUNT, USB_DC_IMPLEMENT, NULL)
