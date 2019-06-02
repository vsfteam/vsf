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

#include "btstack_event.h"
#include "btstack_run_loop.h"
#include "btstack_memory.h"
#include "hci.h"
#include "btstack_chipset_csr.h"
#include "component/3rd-party/btstack/port/btstack_run_loop_vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

int btstack_main(int argc, const char * argv[]);
vsf_err_t vsf_bluetooth_h2_on_new(void *dev, vsf_usbh_dev_id_t *id)
{
	if ((id->idVendor == 0x0A12) && (id->idProduct == 0x0001)) {
		btstack_memory_init();
		btstack_run_loop_init(btstack_run_loop_vsf_get_instance());
		hci_init(hci_transport_usb_instance(), dev);
		hci_set_chipset(btstack_chipset_csr_instance());
		btstack_main(0, NULL);
        return VSF_ERR_NONE;
	}
    return VSF_ERR_FAIL;
}
