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

#include "./vsf_crc.h"

#if VSF_HASH_USE_CRC == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

const vsf_crc_t vsf_crc8_ccitt = {
    .bitlen = 8,
    .poly   = 0x07,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

uint_fast32_t vsf_crc(const vsf_crc_t *crc, uint_fast32_t initial, uint8_t *buff, uint_fast32_t bytesize)
{
    uint_fast8_t shift = crc->bitlen - 8;
    uint_fast32_t msb = 1UL << (crc->bitlen - 1);

	while (bytesize--) {
        initial ^= *buff++ << shift;
		for (uint_fast8_t i = 0; i < 8; i++) {
            if (initial & msb) {
                initial = (initial << 1) ^ crc->poly;
            } else {
                initial <<= 1;
            }
		}
	}
    return initial & ((1UL << crc->bitlen) - 1);
}

#endif

/* EOF */
