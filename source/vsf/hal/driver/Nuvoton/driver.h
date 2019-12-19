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
#include "hal/vsf_hal_cfg.h"

#undef VSF_NUVOTON_DRIVER_HEADER

#if     defined(__M484__)
#   define  VSF_NUVOTON_DRIVER_HEADER       "./M480/M484/driver.h"
#elif   defined(__NUC505__)
#   define  VSF_NUVOTON_DRIVER_HEADER       "./NUC500/NUC505/driver.h"
#else
#   error No supported device found.
#endif

/* include specified device driver header file */
#include VSF_NUVOTON_DRIVER_HEADER



#ifndef __HAL_DRIVER_NUVOTON_H__
#define __HAL_DRIVER_NUVOTON_H__


#if VSF_USE_SERVICE_VSFSTREAM == ENABLED || VSF_USE_SERVICE_STREAM == ENABLED
#include "service/vsf_service.h"
#endif



/*============================ MACROS ========================================*/

#if     VSF_USE_SERVICE_VSFSTREAM == ENABLED
#   define VSF_DEBUG_STREAM_NEED_POOL
#elif   VSF_USE_SERVICE_STREAM == ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if     VSF_USE_SERVICE_VSFSTREAM == ENABLED
extern vsf_stream_t VSF_DEBUG_STREAM_TX;
extern vsf_mem_stream_t VSF_DEBUG_STREAM_RX;
#elif   VSF_USE_SERVICE_STREAM == ENABLED
extern const vsf_stream_tx_t VSF_DEBUG_STREAM_TX;
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#if     VSF_USE_SERVICE_VSFSTREAM == ENABLED
extern void VSF_DEBUG_STREAM_POLL(void);
#elif   VSF_USE_SERVICE_STREAM == ENABLED
#endif

#endif
/* EOF */
