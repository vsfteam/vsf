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

#include "../common.h"
#include "./ohci/ohci.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __USB_HC_IMPLEMENT(__N, __TYPE)                                         \
const m480_##__TYPE##_t USB_HC##__N = { .index = __N, USB_HC##__N##_CONFIG };   \
ROOT void USB_HC##__N##_IRQHandler(void)                                        \
{                                                                               \
    if (__m480_usb_hc[__N].irq_handler != NULL) {                               \
        __m480_usb_hc[__N].irq_handler(__m480_usb_hc[__N].param);               \
    }                                                                           \
}
#define _USB_HC_IMPLEMENT(__N, __TYPE)      __USB_HC_IMPLEMENT(__N, __TYPE)
#define USB_HC_IMPLEMENT(__N, __VALUE)                                          \
    _USB_HC_IMPLEMENT(__N, USB_HC##__N##_TYPE)

#define __USB_HC_INIT(__N, __TYPE)                                              \
    case __N: return m480_##__TYPE##_init((m480_##__TYPE##_t *)hc, cfg);
#define _USB_HC_INIT(__N, __TYPE)           __USB_HC_INIT(__N, __TYPE)
#define USB_HC_INIT(__N, __VALUE)           _USB_HC_INIT(__N, USB_HC##__N##_TYPE)

#define __USB_HC_GET_REGBASE(__N, __TYPE)                                       \
    case __N: return m480_##__TYPE##_get_regbase((m480_##__TYPE##_t *)hc);
#define _USB_HC_GET_REGBASE(__N, __TYPE)    __USB_HC_GET_REGBASE(__N, __TYPE)
#define USB_HC_GET_REGBASE(__N, __VALUE)    _USB_HC_GET_REGBASE(__N, USB_HC##__N##_TYPE)

/*============================ TYPES =========================================*/

struct __m480_usb_hc_t {
    void *param;
    void (*irq_handler)(void *param);
};
typedef struct __m480_usb_hc_t __m480_usb_hc_t;

struct vsf_usb_hc_t {
    uint8_t index;
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#if USB_HC_COUNT > 0
static NO_INIT __m480_usb_hc_t __m480_usb_hc[USB_HC_COUNT];
#endif

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

MREPEAT(USB_HC_COUNT, USB_HC_IMPLEMENT, NULL)

vsf_err_t vsf_usb_hc_init(vsf_usb_hc_t *hc, usb_hc_cfg_t *cfg)
{
#if USB_HC_COUNT > 0
    __m480_usb_hc[hc->index].param = cfg->param;
    __m480_usb_hc[hc->index].irq_handler = cfg->irq_handler;
#endif

    switch (hc->index) {
        MREPEAT(USB_HC_COUNT, USB_HC_INIT, NULL)
    default:
        return VSF_ERR_NOT_SUPPORT;
    }
}

void *vsf_usb_hc_get_regbase(vsf_usb_hc_t *hc)
{
    switch (hc->index) {
        MREPEAT(USB_HC_COUNT, USB_HC_GET_REGBASE, NULL)
    default:
        return NULL;
    }
}
