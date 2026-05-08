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
 * Clean-room re-implementation of ESP-IDF public API "esp_vfs_eventfd.h".
 *
 * Bridges to VSF Linux subsystem's eventfd implementation.
 *
 * Baseline: ESP-IDF v5.1 public API.
 */

#ifndef __VSF_ESPIDF_ESP_VFS_EVENTFD_H__
#define __VSF_ESPIDF_ESP_VFS_EVENTFD_H__

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#include "esp_err.h"

typedef uint64_t eventfd_t;

#define EFD_SUPPORT_ISR   (1 << 4)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    size_t max_fds;
} esp_vfs_eventfd_config_t;

#define ESP_VFS_EVENTD_CONFIG_DEFAULT()  (esp_vfs_eventfd_config_t){ .max_fds = 5 }

esp_err_t esp_vfs_eventfd_register(const esp_vfs_eventfd_config_t *config);
esp_err_t esp_vfs_eventfd_unregister(void);

int eventfd(unsigned int initval, int flags);

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_ESP_VFS_EVENTFD_H__
