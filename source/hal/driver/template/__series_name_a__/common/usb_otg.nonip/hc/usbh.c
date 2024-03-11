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

#if VSF_HAL_USE_USBH == ENABLED

#include "./usbh.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vsf_hw_usbh_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vk_usbh_hcd_t *hcd);
static vsf_err_t __vsf_hw_usbh_fini(vk_usbh_hcd_t *hcd);
static vsf_err_t __vsf_hw_usbh_suspend(vk_usbh_hcd_t *hcd);
static vsf_err_t __vsf_hw_usbh_resume(vk_usbh_hcd_t *hcd);
static uint_fast16_t __vsf_hw_usbh_get_frame_number(vk_usbh_hcd_t *hcd);
static vsf_err_t __vsf_hw_usbh_alloc_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static void __vsf_hw_usbh_free_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static vk_usbh_hcd_urb_t * __vsf_hw_usbh_alloc_urb(vk_usbh_hcd_t *hcd);
static void __vsf_hw_usbh_free_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __vsf_hw_usbh_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __vsf_hw_usbh_relink_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);

static int __vsf_hw_usbh_rh_control(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __vsf_hw_usbh_reset_dev(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static bool __vsf_hw_usbh_is_dev_reset(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_hcd_drv_t vk_hw_usbh_drv = {
    .init_evthandler            = __vsf_hw_usbh_init_evthandler,
    .fini                       = __vsf_hw_usbh_fini,
    .suspend                    = __vsf_hw_usbh_suspend,
    .resume                     = __vsf_hw_usbh_resume,
    .get_frame_number           = __vsf_hw_usbh_get_frame_number,
    .alloc_device               = __vsf_hw_usbh_alloc_device,
    .free_device                = __vsf_hw_usbh_free_device,
    .alloc_urb                  = __vsf_hw_usbh_alloc_urb,
    .free_urb                   = __vsf_hw_usbh_free_urb,
    .submit_urb                 = __vsf_hw_usbh_submit_urb,
    .relink_urb                 = __vsf_hw_usbh_relink_urb,
    .rh_control                 = __vsf_hw_usbh_rh_control,
    .reset_dev                  = __vsf_hw_usbh_reset_dev,
    .is_dev_reset               = __vsf_hw_usbh_is_dev_reset,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __vsf_hw_usbh_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vsf_hw_usbh_fini(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vsf_hw_usbh_suspend(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vsf_hw_usbh_resume(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static uint_fast16_t __vsf_hw_usbh_get_frame_number(vk_usbh_hcd_t *hcd)
{
    return 0;
}

static vsf_err_t __vsf_hw_usbh_alloc_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    return VSF_ERR_NONE;
}

static void __vsf_hw_usbh_free_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{

}

static vk_usbh_hcd_urb_t * __vsf_hw_usbh_alloc_urb(vk_usbh_hcd_t *hcd)
{
    return NULL;
}

static void __vsf_hw_usbh_free_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{

}

static vsf_err_t __vsf_hw_usbh_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vsf_hw_usbh_relink_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    return VSF_ERR_NONE;
}

static int __vsf_hw_usbh_rh_control(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    return 0;
}

static vsf_err_t __vsf_hw_usbh_reset_dev(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    return VSF_ERR_NONE;
}

static bool __vsf_hw_usbh_is_dev_reset(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    return false;
}

#endif      // VSF_HAL_USE_USBH
