/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software       *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "./vsf_wifi_mt76.h"

#if VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED

/*============================ GLOBAL VARIABLES ==============================*/

/* Weak firmware stubs.  The application links strong overrides generated from
 * the real mt7662_rom_patch.bin / mt7662.bin files. */

__attribute__((weak))
const uint8_t __mt76_rom_patch_firmware_data[1] = { 0 };

__attribute__((weak))
const uint32_t __mt76_rom_patch_firmware_size = 0;

__attribute__((weak))
const uint8_t __mt76_main_firmware_data[1] = { 0 };

__attribute__((weak))
const uint32_t __mt76_main_firmware_size = 0;

#endif      /* VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_MT76 == ENABLED */
