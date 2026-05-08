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
 * esp_vfs_eventfd.h port — bridges to VSF Linux subsystem eventfd.
 * eventfd() itself is provided by VSF Linux (sys/eventfd.h).
 */

#include "../vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED && VSF_ESPIDF_CFG_USE_VFS == ENABLED

#include "esp_vfs_eventfd.h"

#if VSF_USE_LINUX == ENABLED

esp_err_t esp_vfs_eventfd_register(const esp_vfs_eventfd_config_t *config)
{
    (void)config;
    return ESP_OK;
}

esp_err_t esp_vfs_eventfd_unregister(void)
{
    return ESP_OK;
}

#else

esp_err_t esp_vfs_eventfd_register(const esp_vfs_eventfd_config_t *config)
{
    (void)config;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_vfs_eventfd_unregister(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}

#endif

#endif      // VSF_USE_ESPIDF && VSF_ESPIDF_CFG_USE_VFS
