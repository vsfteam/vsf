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

#ifndef __VSF_CRC_H__
#define __VSF_CRC_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_crypto_cfg.h"

#if VSF_HASH_USE_CRC == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_crc_t {
    enum {
        VSF_CRC_BITLEN8 = 8,
        VSF_CRC_BITLEN16 = 16,
        VSF_CRC_BITLEN32 = 32,
    } bitlen;
    uint32_t poly;
} vsf_crc_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_crc_t vsf_crc8_ccitt;

/*============================ PROTOTYPES ====================================*/

extern uint_fast32_t vsf_crc(const vsf_crc_t *crc, uint_fast32_t initial, uint8_t *buff, uint_fast32_t bytesize);

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
