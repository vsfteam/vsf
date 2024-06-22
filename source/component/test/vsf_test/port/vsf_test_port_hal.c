/*****************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
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

#include "../vsf_test.h"

/*============================ MACROS ========================================*/

#if VSF_USE_TEST == ENABLED && VSF_HAL_USE_WDT == ENABLED && VSF_TEST_CFG_HAL_WDT == ENABLED
#    ifndef VSF_TEST_CFG_HAL_WDT_DEVICE
#        define VSF_TEST_CFG_HAL_WDT_DEVICE vsf_hw_wdt
#    endif

#    ifndef VSF_TEST_CFG_HAL_WDT_MODE
#        define VSF_TEST_CFG_HAL_WDT_MODE                                      \
            (VSF_WDT_MODE_RESET_SOC | VSF_WDT_MODE_NO_EARLY_WAKEUP)
#    endif

/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_test_hal_wdt_init(vsf_test_wdt_t *wdt, uint32_t timeout_ms)
{
    vsf_wdt_cfg_t cfg = {.mode   = VSF_TEST_CFG_HAL_WDT_MODE,
                         .max_ms = timeout_ms};

    vsf_wdt_init(&VSF_TEST_CFG_HAL_WDT, &cfg)
}

void vsf_test_hal_wdt_feed(vsf_test_wdt_t *wdt)
{
    vsf_wdt_feed(&VSF_TEST_CFG_HAL_WDT);
}
#endif
