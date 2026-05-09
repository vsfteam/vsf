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
 * Core implementation for "esp_app_trace.h" on VSF.
 *
 * Implements the ESP-IDF esp_apptrace_* public API layer. Routes all
 * operations through the virtual transport port (esp_apptrace_virt_hw_get).
 */

/*============================ INCLUDES ======================================*/

#include "../vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED && VSF_ESPIDF_CFG_USE_APP_TRACE == ENABLED

#include "esp_app_trace.h"
#include "esp_app_trace_port.h"

#include "../vsf_espidf.h"
#include "kernel/vsf_kernel.h"

#include <string.h>
#include <stdio.h>
#include <inttypes.h>

/*============================ MACROS ========================================*/

#define ESP_APPTRACE_MAX_VPRINTF_ARGS    256

/*============================ TYPES =========================================*/

typedef struct {
    esp_apptrace_hw_t      *hw;
    void                   *hw_data;
    esp_apptrace_dest_t     dest;
} esp_apptrace_channel_t;

/*============================ GLOBAL VARIABLES ==============================*/

static esp_apptrace_channel_t s_trace_ch;

/*============================ IMPLEMENTATION ================================*/

esp_err_t esp_apptrace_init(const esp_apptrace_config_t *config)
{
    if (config == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    void *hw_data = NULL;
    s_trace_ch.hw = esp_apptrace_virt_hw_get(&hw_data);
    s_trace_ch.hw_data = hw_data;
    s_trace_ch.dest = config->dest;

    if (s_trace_ch.hw == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    if (s_trace_ch.hw->init != NULL) {
        int res = s_trace_ch.hw->init(s_trace_ch.hw_data, config);
        if (res != ESP_OK) {
            return res;
        }
    }

    return ESP_OK;
}

esp_err_t esp_apptrace_down_buffer_config(uint8_t *buf, uint32_t size)
{
    if (buf == NULL || size == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_trace_ch.hw == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (s_trace_ch.hw->down_buffer_config != NULL) {
        s_trace_ch.hw->down_buffer_config(s_trace_ch.hw_data, buf, size);
    }
    return ESP_OK;
}

uint8_t *esp_apptrace_down_buffer_get(uint32_t *size, uint32_t user_tmo)
{
    if (size == NULL || *size == 0) {
        return NULL;
    }
    if (s_trace_ch.hw == NULL) {
        return NULL;
    }
    if (s_trace_ch.hw->get_down_buffer == NULL) {
        return NULL;
    }

    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, user_tmo);
    return s_trace_ch.hw->get_down_buffer(s_trace_ch.hw_data, size, &tmo);
}

esp_err_t esp_apptrace_down_buffer_put(uint8_t *ptr, uint32_t user_tmo)
{
    if (ptr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_trace_ch.hw == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (s_trace_ch.hw->put_down_buffer == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, user_tmo);
    return s_trace_ch.hw->put_down_buffer(s_trace_ch.hw_data, ptr, &tmo);
}

esp_err_t esp_apptrace_read(void *buf, uint32_t *size, uint32_t user_tmo)
{
    if (buf == NULL || size == NULL || *size == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_trace_ch.hw == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (s_trace_ch.hw->get_down_buffer == NULL
        || s_trace_ch.hw->put_down_buffer == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, user_tmo);

    uint32_t act_sz = *size;
    *size = 0;
    uint8_t *ptr = s_trace_ch.hw->get_down_buffer(s_trace_ch.hw_data,
                                                   &act_sz, &tmo);
    if (ptr != NULL && act_sz > 0) {
        memcpy(buf, ptr, act_sz);
        *size = act_sz;
        return s_trace_ch.hw->put_down_buffer(s_trace_ch.hw_data, ptr, &tmo);
    }

    return ESP_ERR_TIMEOUT;
}

uint8_t *esp_apptrace_buffer_get(uint32_t size, uint32_t user_tmo)
{
    if (size == 0) {
        return NULL;
    }
    if (s_trace_ch.hw == NULL) {
        return NULL;
    }
    if (s_trace_ch.hw->get_up_buffer == NULL) {
        return NULL;
    }

    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, user_tmo);
    return s_trace_ch.hw->get_up_buffer(s_trace_ch.hw_data, size, &tmo);
}

esp_err_t esp_apptrace_buffer_put(uint8_t *ptr, uint32_t user_tmo)
{
    if (ptr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_trace_ch.hw == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (s_trace_ch.hw->put_up_buffer == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, user_tmo);
    return s_trace_ch.hw->put_up_buffer(s_trace_ch.hw_data, ptr, &tmo);
}

esp_err_t esp_apptrace_write(const void *data, uint32_t size, uint32_t user_tmo)
{
    if (data == NULL || size == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_trace_ch.hw == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (s_trace_ch.hw->get_up_buffer == NULL
        || s_trace_ch.hw->put_up_buffer == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, user_tmo);

    uint8_t *ptr = s_trace_ch.hw->get_up_buffer(s_trace_ch.hw_data,
                                                  size, &tmo);
    if (ptr == NULL) {
        return ESP_ERR_NO_MEM;
    }

    memcpy(ptr, data, size);
    return s_trace_ch.hw->put_up_buffer(s_trace_ch.hw_data, ptr, &tmo);
}

int esp_apptrace_vprintf_to(uint32_t user_tmo, const char *fmt, va_list ap)
{
    uint16_t nargs = 0;
    uint8_t *p = (uint8_t *)fmt;

    if (fmt == NULL) {
        return -1;
    }
    if (s_trace_ch.hw == NULL) {
        return -1;
    }
    if (s_trace_ch.hw->get_up_buffer == NULL
        || s_trace_ch.hw->put_up_buffer == NULL) {
        return -1;
    }

    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, user_tmo);

    while ((p = (uint8_t *)strchr((char *)p, '%')) != NULL
           && nargs < ESP_APPTRACE_MAX_VPRINTF_ARGS) {
        p++;
        if (*p != '%' && *p != 0) {
            nargs++;
        }
    }

    uint8_t *pout = s_trace_ch.hw->get_up_buffer(s_trace_ch.hw_data,
        1 + sizeof(char *) + nargs * sizeof(uint32_t), &tmo);
    if (pout == NULL) {
        return -1;
    }

    p = pout;
    *pout = nargs;
    pout++;
    *(const char **)pout = fmt;
    pout += sizeof(char *);

    while (nargs-- > 0) {
        uint32_t arg = va_arg(ap, uint32_t);
        *(uint32_t *)pout = arg;
        pout += sizeof(uint32_t);
    }

    int ret = s_trace_ch.hw->put_up_buffer(s_trace_ch.hw_data, p, &tmo);
    if (ret != ESP_OK) {
        return -1;
    }

    return (int)(pout - p);
}

int esp_apptrace_vprintf(const char *fmt, va_list ap)
{
    return esp_apptrace_vprintf_to(0, fmt, ap);
}

esp_err_t esp_apptrace_flush_nolock(uint32_t min_sz, uint32_t usr_tmo)
{
    if (s_trace_ch.hw == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (s_trace_ch.hw->flush_up_buffer_nolock == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, usr_tmo);
    return s_trace_ch.hw->flush_up_buffer_nolock(s_trace_ch.hw_data,
                                                  min_sz, &tmo);
}

esp_err_t esp_apptrace_flush(uint32_t usr_tmo)
{
    if (s_trace_ch.hw == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (s_trace_ch.hw->flush_up_buffer == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, usr_tmo);
    return s_trace_ch.hw->flush_up_buffer(s_trace_ch.hw_data, &tmo);
}

bool esp_apptrace_host_is_connected(void)
{
    if (s_trace_ch.hw == NULL) {
        return false;
    }
    if (s_trace_ch.hw->host_is_connected == NULL) {
        return false;
    }
    return s_trace_ch.hw->host_is_connected(s_trace_ch.hw_data);
}

esp_apptrace_dest_t esp_apptrace_get_destination(void)
{
    return s_trace_ch.dest;
}

esp_err_t esp_apptrace_set_header_size(esp_apptrace_header_size_t header_size)
{
    if (s_trace_ch.hw == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (s_trace_ch.hw->set_header_size != NULL) {
        s_trace_ch.hw->set_header_size(s_trace_ch.hw_data, header_size);
    }
    return ESP_OK;
}

#endif  /* VSF_USE_ESPIDF && VSF_ESPIDF_CFG_USE_APP_TRACE */
