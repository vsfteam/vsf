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

#ifdef __AIC8800__

#include "rtos_al.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void aic8800_demo_init(void)
{
    if (rtos_init()) {
        VSF_HAL_ASSERT(false);
    }

#if AIC8800_APP_USE_WIFI_DEMO == ENABLED
    extern void aic8800_wifi_start(void);
    aic8800_wifi_start();
#endif
#if AIC8800_APP_USE_BT_DEMO == ENABLED
    extern vsf_err_t aic8800_bt_start(void);
    aic8800_bt_start();
#endif
}

#endif      // __AIC8800__

/* EOF */
