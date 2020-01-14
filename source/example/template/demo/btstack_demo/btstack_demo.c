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

#include "vsf.h"

#if APP_CFG_USE_BTSTACK_DEMO == ENABLED

#include "btstack_event.h"
#include "btstack_run_loop.h"
#include "btstack_memory.h"
#include "hci.h"
#include "csr/btstack_chipset_csr.h"
#include "bcm/btstack_chipset_bcm.h"
#include "component/3rd-party/btstack/port/btstack_run_loop_vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern int btstack_main(int argc, const char * argv[]);

/*============================ IMPLEMENTATION ================================*/

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
