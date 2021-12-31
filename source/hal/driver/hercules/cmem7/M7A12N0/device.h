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

#ifndef __HAL_DEVICE_CMEM7_H__
#define __HAL_DEVICE_CMEM7_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"

/*============================ MACROS ========================================*/
#define USB_OTG_COUNT               1
#define USB_OTG0_IRQHandler         USB_IRQHandler
// required by dwcotg, define the max ep number of dwcotg
#define USB_DWCOTG_MAX_EP_NUM       4

#define USB_OTG0_CONFIG                                                         \
    .ep_num = 8,                                                                \
    .irq = USB_INT_IRQn,                                                        \
    .reg = USB_BASE,                                                            \
    .use_as__vsf_dwcotg_hw_info_t.buffer_word_size = 0x800 >> 2,                \
    .use_as__vsf_dwcotg_hw_info_t.speed = USB_SPEED_HIGH,                       \
	.use_as__vsf_dwcotg_hw_info_t.dma_en = false,                               \
	.use_as__vsf_dwcotg_hw_info_t.ulpi_en = false,                              \
	.use_as__vsf_dwcotg_hw_info_t.utmi_en = true,                               \
	.use_as__vsf_dwcotg_hw_info_t.vbus_en = false,


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
