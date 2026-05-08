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
 * Clean-room re-implementation of ESP-IDF public API "esp_vfs_semihost.h".
 *
 * VSF explicitly disables ARM semihosting project-wide
 * (__use_no_semihosting / #pragma import(__use_no_semihosting)).
 * Console I/O uses vsf_stream_t instead. All API entries return
 * ESP_ERR_NOT_SUPPORTED.
 *
 * Baseline: ESP-IDF v5.1 public API.
 */

#ifndef __VSF_ESPIDF_ESP_VFS_SEMIHOST_H__
#define __VSF_ESPIDF_ESP_VFS_SEMIHOST_H__

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t esp_vfs_semihost_register(const char *base_path);
esp_err_t esp_vfs_semihost_unregister(const char *base_path);

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_ESP_VFS_SEMIHOST_H__
