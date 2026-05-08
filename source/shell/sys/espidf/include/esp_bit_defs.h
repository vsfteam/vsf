/*****************************************************************************
 *   Copyright(C)2009-2026 by VSF Team                                       *
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

/*
 * Clean-room re-implementation of ESP-IDF public API "esp_bit_defs.h".
 *
 * Baseline: ESP-IDF v5.1 public API.
 */

#ifndef __VSF_ESPIDF_ESP_BIT_DEFS_H__
#define __VSF_ESPIDF_ESP_BIT_DEFS_H__

/* 32-bit bitmask constants */
#define BIT31   0x80000000u
#define BIT30   0x40000000u
#define BIT29   0x20000000u
#define BIT28   0x10000000u
#define BIT27   0x08000000u
#define BIT26   0x04000000u
#define BIT25   0x02000000u
#define BIT24   0x01000000u
#define BIT23   0x00800000u
#define BIT22   0x00400000u
#define BIT21   0x00200000u
#define BIT20   0x00100000u
#define BIT19   0x00080000u
#define BIT18   0x00040000u
#define BIT17   0x00020000u
#define BIT16   0x00010000u
#define BIT15   0x00008000u
#define BIT14   0x00004000u
#define BIT13   0x00002000u
#define BIT12   0x00001000u
#define BIT11   0x00000800u
#define BIT10   0x00000400u
#define BIT9    0x00000200u
#define BIT8    0x00000100u
#define BIT7    0x00000080u
#define BIT6    0x00000040u
#define BIT5    0x00000020u
#define BIT4    0x00000010u
#define BIT3    0x00000008u
#define BIT2    0x00000004u
#define BIT1    0x00000002u
#define BIT0    0x00000001u

/* 64-bit bitmask constants */
#define BIT63   (0x80000000ull << 32)
#define BIT62   (0x40000000ull << 32)
#define BIT61   (0x20000000ull << 32)
#define BIT60   (0x10000000ull << 32)
#define BIT59   (0x08000000ull << 32)
#define BIT58   (0x04000000ull << 32)
#define BIT57   (0x02000000ull << 32)
#define BIT56   (0x01000000ull << 32)
#define BIT55   (0x00800000ull << 32)
#define BIT54   (0x00400000ull << 32)
#define BIT53   (0x00200000ull << 32)
#define BIT52   (0x00100000ull << 32)
#define BIT51   (0x00080000ull << 32)
#define BIT50   (0x00040000ull << 32)
#define BIT49   (0x00020000ull << 32)
#define BIT48   (0x00010000ull << 32)
#define BIT47   (0x00008000ull << 32)
#define BIT46   (0x00004000ull << 32)
#define BIT45   (0x00002000ull << 32)
#define BIT44   (0x00001000ull << 32)
#define BIT43   (0x00000800ull << 32)
#define BIT42   (0x00000400ull << 32)
#define BIT41   (0x00000200ull << 32)
#define BIT40   (0x00000100ull << 32)
#define BIT39   (0x00000080ull << 32)
#define BIT38   (0x00000040ull << 32)
#define BIT37   (0x00000020ull << 32)
#define BIT36   (0x00000010ull << 32)
#define BIT35   (0x00000008ull << 32)
#define BIT34   (0x00000004ull << 32)
#define BIT33   (0x00000002ull << 32)
#define BIT32   (0x00000001ull << 32)

#ifndef BIT
#define BIT(nr)                 (1ul << (nr))
#endif
#ifndef BIT64
#define BIT64(nr)               (1ull << (nr))
#endif

#endif      // __VSF_ESPIDF_ESP_BIT_DEFS_H__
