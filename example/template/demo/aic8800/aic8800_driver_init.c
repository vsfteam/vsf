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

#include "reg_iomux.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

bool vsf_driver_aic8800_init(void)
{
    iomux_gpio_config_sel_setf(10, 0x01); // PA10 as uart1 rx
    iomux_gpio_config_sel_setf(11, 0x01); // PA11 as uart1 tx

    iomux_gpio_config_sel_setf(6, 0x02); // PA10 as uart2 rx
    iomux_gpio_config_sel_setf(7, 0x02); // PA11 as uart2 tx

    return true;
}

#endif      // __AIC8800__

/* EOF */
