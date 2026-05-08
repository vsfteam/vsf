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
 * Clean-room re-implementation of ESP-IDF public API "esp_heap_caps_init.h".
 *
 * Baseline: ESP-IDF v5.1 public API.
 */

#ifndef __VSF_ESPIDF_ESP_HEAP_CAPS_INIT_H__
#define __VSF_ESPIDF_ESP_HEAP_CAPS_INIT_H__

#include "esp_err.h"
#include "esp_heap_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

void heap_caps_init(void);
void heap_caps_enable_nonos_stack_heaps(void);
esp_err_t heap_caps_add_region(intptr_t start, intptr_t end);
esp_err_t heap_caps_add_region_with_caps(const uint32_t caps[], intptr_t start, intptr_t end);

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_ESP_HEAP_CAPS_INIT_H__
