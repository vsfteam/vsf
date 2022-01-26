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
    iomux_gpio_config_sel_setf(10, 0x03); // PA10 as spi sck
    iomux_gpio_config_sel_setf(11, 0x03); // PA11 as spi csn
    iomux_gpio_config_sel_setf(12, 0x03); // PA10 as spi di
    iomux_gpio_config_sel_setf(13, 0x03); // PA11 as spi do

#if VSF_DISP_USE_MIPI_LCD == ENABLED
    iomux_gpio_config_sel_setf(5, 0x00); // PA5 as LCD RESET
    iomux_gpio_config_sel_setf(6, 0x00); // PA6 as LCD DCS
    iomux_gpio_config_sel_setf(7, 0x00); // PA7 as LCD TE
#endif

    return true;
}

#endif      // __AIC8800__

/* EOF */
