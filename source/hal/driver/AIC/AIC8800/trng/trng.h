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

#ifndef __HAL_DRIVER_AIC8800_TRNG_H__
#define __HAL_DRIVER_AIC8800_TRNG_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_RNG == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_rng_t vsf_hw_rng_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern vsf_hw_rng_t vsf_rng0;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

extern vsf_err_t vsf_hw_rng_init(vsf_hw_rng_t *rng);
extern void vsf_hw_rng_fini(vsf_hw_rng_t *rng);
extern vsf_err_t vsf_hw_rng_generate_request(vsf_hw_rng_t *rng, uint32_t *buffer, uint32_t num,
            void *param, void (*on_ready)(void *param, uint32_t *buffer, uint32_t num));

#endif /* VSF_HAL_USE_RNG */
#endif /* __HAL_DRIVER_AIC8800_TRNG_H__ */
