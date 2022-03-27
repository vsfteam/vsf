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

#include "vsf.h"

#include "btstack_memory.h"
#include "hci.h"
#include "btstack_event.h"
#include "csr/btstack_chipset_csr.h"
#include "bcm/btstack_chipset_bcm.h"
#include "btstack_run_loop.h"

#include "component/3rd-party/btstack/port/btstack_run_loop_vsf.h"
#include "component/3rd-party/btstack/extension/btstack_oo/btstack_oo.h"

#include "usrapp_usbh_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef union btstack_host_dev_all_t {
    btstack_host_dev_t          dev;
#   if BTSTACK_OO_USE_HOST_HID == ENABLED
#       if BTSTACK_OO_USE_HOST_JOYCON == ENABLED
    btstack_host_joycon_t       joycon;
#       endif
#       if BTSTACK_OO_USE_HOST_DS4 == ENABLED
    btstack_host_ds4_t          ds4;
#       endif
#   endif
} btstack_host_dev_all_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#if BTSTACK_OS_USE_DEVICE == ENABLED
static btstack_device_nspro_t btstack_dev = {
    .op = &btstack_device_xxxx_drv.use_as__btstack_drv_op_t,
};
#endif

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

btstack_host_dev_t * btstack_host_malloc_dev(void)
{
    return vsf_heap_malloc(sizeof(btstack_host_dev_all_t));
}

void btstack_host_free_dev(btstack_host_dev_t *dev)
{
    vsf_heap_free(dev);
}

int btstack_evthandler(btstack_evt_t evt, void *param)
{
    btstack_dev_t *dev = param;

    switch (evt) {
    case BTSTACK_HOST_ON_INQUIRY_RESULT:
        // return true to connect
        return btstack_host_is_dev_supported((btstack_host_dev_t *)param);
    case BTSTACK_ON_DISCONNECTION_COMPLETE:
    case BTSTACK_ON_CHANNELS_OPEN_COMPLETE:
        if (!dev->is_device && !btstack_is_dev_connected(dev)) {
            if (evt == BTSTACK_ON_CHANNELS_OPEN_COMPLETE) {
                btstack_host_remove_dev((btstack_host_dev_t *)dev, NULL);
            }
        }
        // fall through
    case BTSTACK_HOST_ON_INQUIRY_COMPLETE: {
            btstack_linked_list_iterator_t it;
            bool is_connected = false;

            btstack_get_devs(&it);
            while (btstack_linked_list_iterator_has_next(&it)) {
                dev = (btstack_dev_t *)btstack_linked_list_iterator_next(&it);
                if (!dev->is_device && !btstack_is_dev_connected(dev)) {
                    btstack_host_connect_dev((btstack_host_dev_t *)dev);
                    is_connected = true;
                    break;
                }
            }
            if (is_connected) {
                break;
            }
        }
        btstack_host_scan(1);
        break;
    case BTSTACK_ON_INITIALIZED:
#if BTSTACK_OO_USE_HOST == ENABLED
        btstack_host_scan(1);
#endif
#if BTSTACK_OO_USE_DEVICE == ENABLED
        if (is_paired) {
            btstack_set_timer(200);
        } else {
            btstack_device_try_pair(&btstack_dev.use_as__btstack_device_dev_t);
        }
#endif
        break;
#if BTSTACK_OO_USE_DEVICE == ENABLED
    case BTSTACK_ON_TIMER:
        if (is_paired) {
            btstack_connect_dev(&btstack_dev.use_as__btstack_dev_t);
        }
        break;
#endif
    }
    return 0;
}

int btstack_main(int argc, const char * argv[])
{
    btstack_init((bd_addr_t){0});
#if BTSTACK_OO_USE_HOST == ENABLED
        btstack_host_init();
#   if BTSTACK_OO_USE_HOST_HID == ENABLED && BTSTACK_OO_USE_HOST_JOYCON == ENABLED
            static btstack_host_drv_t __joycon_left_drv = {
                .op = &btstack_host_joycon_left_drv,
            };
            static btstack_host_drv_t __joycon_right_drv = {
                .op = &btstack_host_joycon_right_drv,
            };
            btstack_host_register_drv(&__joycon_left_drv);
            btstack_host_register_drv(&__joycon_right_drv);
#   endif
#   if BTSTACK_OO_USE_HOST_HID == ENABLED && BTSTACK_OO_USE_HOST_DS4 == ENABLED
            static btstack_host_drv_t __ds4_drv = {
                .op = &btstack_host_ds4_drv,
            };
            btstack_host_register_drv(&__ds4_drv);
#   endif
#endif
#if BTSTACK_OO_USE_DEVICE == ENABLED
        btstack_device_init(&btstack_dev.use_as__btstack_device_dev_t);
#endif
    btstack_start();

    return 0;
}

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_BTHCI == ENABLED
vsf_err_t vsf_bluetooth_h2_on_new(void *dev, vk_usbh_dev_id_t *id)
{
    btstack_memory_init();
    btstack_run_loop_init(btstack_run_loop_vsf_get_instance());
    hci_init(hci_transport_usb_instance(), dev);

    if ((id->idVendor == 0x0A12) && (id->idProduct == 0x0001)) {
        hci_set_chipset(btstack_chipset_csr_instance());
    } else if ((id->idVendor == 0x0A5C) && (id->idProduct == 0x21E8)) {
        hci_set_chipset(btstack_chipset_bcm_instance());
    } else {
        return VSF_ERR_FAIL;
    }

    btstack_main(0, NULL);
    return VSF_ERR_NONE;
}
#endif

int main(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#   endif

    // usbh init
    usrapp_usbh_common_init();
    return 0;
}
