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

#ifndef __HAL_DRIVER_USB_INTERFACE_H__
#define __HAL_DRIVER_USB_INTERFACE_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "hal/arch/vsf_arch.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __USB_HC_FUNC_DEF(__N, __VALUE)                                         \
static vsf_err_t    usb_hc##__N##_init(usb_hc_cfg_t *cfg);                      \
static void *       usb_hc##__N##_get_regbase(void);

#define __USB_HC_INTERFACE_DEF(__N, __VALUE)                                    \
            {                                                                   \
                &usb_hc##__N##_init,                                            \
                &usb_hc##__N##_get_regbase,                                     \
            },

#define __USB_HC_BODY(__N, __VALUE)                                             \
static vsf_err_t usb_hc##__N##_init(usb_hc_cfg_t *cfg)                          \
{                                                                               \
    return vsf_usb_hc_init((vsf_usb_hc_t *)&USB_HC##__N, cfg);                  \
}                                                                               \
static void * usb_hc##__N##_get_regbase(void)                                   \
{                                                                               \
    return vsf_usb_hc_get_regbase((vsf_usb_hc_t *)&USB_HC##__N);                \
}

/*============================ TYPES =========================================*/

typedef struct vsf_usb_hc_t vsf_usb_hc_t;
typedef void (*vsf_usb_hc_irq_handler_t)(void *param);

//! \name usb_hc configuration structure
//! @{
typedef struct {
    vsf_arch_priority_t         priority;       //!< interrupt priority
    vsf_usb_hc_irq_handler_t    irq_handler;    //!< irq_handler function
    void *                      param;          //!< hcd related parameters
} usb_hc_cfg_t;
//! @}

//! \name usb_hc control interface
//! @{
def_interface(i_usb_hc_t)

    //! initialize
    vsf_err_t       (*Init)             (usb_hc_cfg_t *cfg);

    //£¡ get register base
    void *          (*GetRegBase)       (void);

end_def_interface(i_usb_hc_t)
//! @}

//! \name usb user interface
//! @{
#if USB_HC_COUNT > 0 || USB_DC_COUNT > 0
def_interface(i_usb_t)

#if USB_HC_COUNT > 0
    i_usb_hc_t      Hc[USB_HC_COUNT];
#endif

end_def_interface(i_usb_t)
//! @}
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_usb_hc_init(vsf_usb_hc_t *hc, usb_hc_cfg_t *cfg);
extern void *vsf_usb_hc_get_regbase(vsf_usb_hc_t *hc);

#endif
/* EOF */
