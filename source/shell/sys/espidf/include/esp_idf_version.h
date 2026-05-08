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
 * Clean-room re-implementation of ESP-IDF public API "esp_idf_version.h".
 *
 * Baseline: matches the version declared in vsf_espidf_cfg.h.
 */

#ifndef __VSF_ESPIDF_ESP_IDF_VERSION_H__
#define __VSF_ESPIDF_ESP_IDF_VERSION_H__

#include "../vsf_espidf_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_IDF_VERSION_MAJOR   VSF_ESPIDF_CFG_VERSION_MAJOR
#define ESP_IDF_VERSION_MINOR   VSF_ESPIDF_CFG_VERSION_MINOR
#define ESP_IDF_VERSION_PATCH   VSF_ESPIDF_CFG_VERSION_PATCH

#define ESP_IDF_VERSION_VAL(major, minor, patch) \
    (((major) << 16) | ((minor) << 8) | (patch))

#define ESP_IDF_VERSION  ESP_IDF_VERSION_VAL(ESP_IDF_VERSION_MAJOR, \
                                             ESP_IDF_VERSION_MINOR, \
                                             ESP_IDF_VERSION_PATCH)

const char * esp_get_idf_version(void);

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_ESP_IDF_VERSION_H__
