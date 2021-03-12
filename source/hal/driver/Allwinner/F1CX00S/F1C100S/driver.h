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

#ifdef __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

#   include "../__device.h"

#else

#   ifndef __HAL_DRIVER_ALLWINNER_F1C100S_H__
#       define __HAL_DRIVER_ALLWINNER_F1C100S_H__

/*============================ INCLUDES ======================================*/
#       include "hal/vsf_hal_cfg.h"
#       include "./device.h"
#       include "../common/dram/dram.h"
#       include "../common/debug_uart/debug_uart.h"
#       include "../common/uart/uart.h"
#       include "../common/fb/fb.h"
#       include "../common/usb/usb.h"
#       include "../common/intc/intc.h"
#       include "../common/rtp/rtp.h"
#       include "./interrupt_swi.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_USE_HEAP == ENABLED
#   if defined(VSF_HAL_CFG_COHERENT_START) && defined(VSF_HAL_CFG_COHERENT_SIZE)
extern void * vsf_dma_alloc_coherent(uint_fast32_t size);
extern void vsf_dma_free_coherent(void *buffer);
#   endif

extern void * vsf_dma_alloc_noncoherent(uint_fast32_t size);
extern void vsf_dma_free_noncoherent(void *buffer);
#endif

#   endif

#endif
/* EOF */
