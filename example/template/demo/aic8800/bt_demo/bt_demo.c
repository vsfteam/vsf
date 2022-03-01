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

#include "vsf_cfg.h"

#if AIC8800_APP_USE_BT_DEMO == ENABLED && APP_USE_BTSTACK_DEMO == ENABLED

#include "vsf.h"

#include "btstack.h"
#include "component/3rd-party/btstack/port/btstack_run_loop_vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern int btstack_main(int argc, const char * argv[]);

/*============================ IMPLEMENTATION ================================*/

vsf_err_t aic8800_bt_start(void)
{
    btstack_memory_init();
    btstack_run_loop_init(btstack_run_loop_vsf_get_instance());

    extern const hci_transport_t * hci_transport_aic8800_instance(void);
    hci_init(hci_transport_aic8800_instance(), NULL);
    hci_set_link_key_db(btstack_link_key_db_memory_instance());

    btstack_main(0, NULL);
    return VSF_ERR_NONE;
}

#endif      // APP_USE_BTSTACK_DEMO

/* EOF */
