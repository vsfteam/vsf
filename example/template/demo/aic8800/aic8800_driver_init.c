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

#ifdef __AIC8800__

#include "reg_iomux.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

bool vsf_app_driver_init(void)
{
    vsf_io_cfg_t cfgs[] = {
        {VSF_PA10, 0x03, 0}, // PA10 as spi sck
        {VSF_PA11, 0x03, 0}, // PA10 as spi sck
        {VSF_PA12, 0x03, 0}, // PA10 as spi sck
        {VSF_PA13, 0x03, 0}, // PA10 as spi sck

#if VSF_DISP_USE_MIPI_LCD == ENABLED
        {VSF_PA5, 0x00, 0},    // PA5 as LCD RESET
        {VSF_PA6, 0x00, 0},    // PA6 as LCD DCS
        {VSF_PA7, 0x00, 0},    // PA7 as LCD TE
#endif

#ifdef APP_USE_HAL_GPIO_DEMO
        {VSF_PA10,  0x00, IO_PULL_UP},
        {VSF_PB3,   0x00, IO_PULL_UP},
#endif
    };
    vsf_io_config(&vsf_hw_io0, cfgs, dimof(cfgs));

    return true;
}

#endif      // __AIC8800__

/* EOF */
