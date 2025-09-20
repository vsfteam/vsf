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

#ifndef __ST_HAL_INTERNAL_H__
#define __ST_HAL_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal.h"

#ifdef VSF_STHAL_CONF_HEADER
#   include VSF_STHAL_CONF_HEADER
#else
#   include "sthal_conf_template.h"
#endif

#include "sthal_def.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern bool __sthal_check_timeout(uint32_t start, uint32_t timeout);
extern HAL_StatusTypeDef __spi_is_idle(vsf_spi_t *spi, uint32_t Timeout, uint32_t Tickstart);

/*============================ INCLUDES ======================================*/


#ifdef __cplusplus
}
#endif

#endif
