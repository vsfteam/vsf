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

#ifndef __VSF_USBD_DRV_IFS_H__
#define __VSF_USBD_DRV_IFS_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#if defined(VSF_USBD_CFG_DRV_LV0_OO)
// oo-style lv0 API used as usbd driver
#   undef VSF_USBD_DRV_PREPARE
#   define VSF_USBD_DRV_PREPARE(__dev)

#   define vk_usbd_drv_func_name(__header, __func) __CONNECT3(__header, _, __func)

// lv0 API used as usbd driver
#   define vk_usbd_drv_init(__cfg)                                              \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, init)                       \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ, (__cfg))

#   define vk_usbd_drv_fini()                                                   \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, fini)                       \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ)

#   define vk_usbd_drv_reset(__cfg)                                             \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, reset)                      \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ, (__cfg))

#   define vk_usbd_drv_connect()                                                \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, connect)                    \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ)

#   define vk_usbd_drv_disconnect()                                             \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, disconnect)                 \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ)

#   define vk_usbd_drv_wakeup()                                                 \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, wakeup)                     \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ)

#   define vk_usbd_drv_set_address(__addr)                                      \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, set_address)                \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ, (__addr))

#   define vk_usbd_drv_get_setup(__request)                                     \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, get_setup)                  \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ, (uint8_t *)(__request))

#   define vk_usbd_drv_status_stage(__is_in)                                    \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, status_stage)               \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ, (__is_in))

#   define vk_usbd_drv_ep_add(__addr, __attr, __size)                           \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, ep_add)                     \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ, (__addr), (usb_ep_type_t)(__attr), (__size))

#   define vk_usbd_drv_ep_get_feature(__ep, __feature)                          \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, ep_get_feature)             \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ, (__ep), (__feature))

#   define vk_usbd_drv_ep_get_size(__ep)                                        \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, ep_get_size)                \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ, (__ep))

#   define vk_usbd_drv_ep_is_stalled(__ep)                                      \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, ep_is_stalled)              \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ, (__ep))

#   define vk_usbd_drv_ep_set_stall(__ep)                                       \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, ep_set_stall)               \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ, (__ep))

#   define vk_usbd_drv_ep_clear_stall(__ep)                                     \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, ep_clear_stall)             \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ, (__ep))

#   define vk_usbd_drv_ep_get_data_size(__ep)                                   \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, ep_get_data_size)           \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ, (__ep))

#   define vk_usbd_drv_ep_transaction_read_buffer(__ep, __buf, __size)          \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, ep_transaction_read_buffer) \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ, (__ep), (__buf), (__size))

#   define vk_usbd_drv_ep_transaction_write_buffer(__ep, __buf, __size)         \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, ep_transaction_write_buffer)\
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ, (__ep), (__buf), (__size))

#   define vk_usbd_drv_ep_transaction_set_data_size(__ep, __size)               \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, ep_transaction_set_data_size)\
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ, (__ep), (__size))

#   define vk_usbd_drv_ep_transaction_enable_out(__ep)                          \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, ep_transaction_enable_out)  \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ, (__ep))

#   define vk_usbd_drv_ep_transfer_send(__ep, __buf, __size, __zlp)             \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, ep_transfer_send)           \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ, (__ep), (__buf), (__size), (__zlp))

#   define vk_usbd_drv_ep_transfer_recv(__ep, __buf, __size)                    \
        vk_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0_OO_PREFIX, ep_transfer_recv)           \
            (&VSF_USBD_CFG_DRV_LV0_OO_OBJ, (__ep), (__buf), (__size))

#else
// interface-style dynamic API used as usbd driver
#   undef VSF_USBD_DRV_PREPARE
#   define VSF_USBD_DRV_PREPARE(__dev)                                          \
         const i_usb_dc_t *__drv = (__dev)->drv;

#   define vk_usbd_drv_init(__cfg)                     __drv->Init((__cfg))
#   define vk_usbd_drv_fini()                          __drv->Fini()
#   define vk_usbd_drv_reset(__cfg)                    __drv->Reset((__cfg))
#   define vk_usbd_drv_connect()                       __drv->Connect()
#   define vk_usbd_drv_disconnect()                    __drv->Disconnect()
#   define vk_usbd_drv_wakeup()                        __drv->Wakeup()
#   define vk_usbd_drv_set_address(__addr)             __drv->SetAddress((__addr))
#   define vk_usbd_drv_get_setup(__request)            __drv->GetSetup((uint8_t *)(__request))
#   define vk_usbd_drv_status_stage(__is_in)           __drv->StatusStage((__is_in))
#   define vk_usbd_drv_ep_add(__addr, __attr, __size)  __drv->Ep.Add((__addr), (usb_ep_type_t)(__attr), (__size))

#   define vk_usbd_drv_ep_get_feature(__ep, __feature) __drv->Ep.GetFeature((__ep), (__feature))
#   define vk_usbd_drv_ep_get_size(__ep)               __drv->Ep.GetSize((__ep))
#   define vk_usbd_drv_ep_is_stalled(__ep)             __drv->Ep.IsStalled((__ep))
#   define vk_usbd_drv_ep_set_stall(__ep)              __drv->Ep.SetStall((__ep))
#   define vk_usbd_drv_ep_clear_stall(__ep)            __drv->Ep.ClearStall((__ep))
#   define vk_usbd_drv_ep_get_data_size(__ep)          __drv->Ep.GetDataSize((__ep))
#   define vk_usbd_drv_ep_transaction_read_buffer(__ep, __buf, __size)          \
                __drv->Ep.Transaction.ReadBuffer((__ep), (__buf), (__size))
#   define vk_usbd_drv_ep_transaction_write_buffer(__ep, __buf, __size)         \
                __drv->Ep.Transaction.WriteBuffer((__ep), (__buf), (__size))
#   define vk_usbd_drv_ep_transaction_set_data_size(__ep, __size)               \
                __drv->Ep.Transaction.SetDataSize((__ep), (__size))
#   define vk_usbd_drv_ep_transaction_enable_out(__ep)                          \
                __drv->Ep.Transaction.EnableOut((__ep))
#   define vk_usbd_drv_ep_transfer_send(__ep, __buf, __size, __zlp)             \
                __drv->Ep.Transfer.Send((__ep), (__buf), (__size), (__zlp))
#   define vk_usbd_drv_ep_transfer_recv(__ep, __buf, __size)                    \
                __drv->Ep.Transfer.Recv((__ep), (__buf), (__size))

#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __VSF_USBD_DRV_IFS_H__
