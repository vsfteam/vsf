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
 * Port implementation for "esp_app_trace.h" on VSF — virtual transport.
 *
 * Provides a virtual transport that routes up-channel (target -> host) data
 * to stdout and reads down-channel (host -> target) data from stdin.
 * Configuration via vsf_espidf_cfg_t::app_trace allows redirecting the
 * streams to arbitrary FILE* handles (e.g. pipes, files).
 *
 * Also includes the utility layer (timeout, ring buffer helpers) required
 * by the app_trace subsystem.
 */

/*============================ INCLUDES ======================================*/

#include "../vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED && VSF_ESPIDF_CFG_USE_APP_TRACE == ENABLED

#include "esp_app_trace_port.h"
#include "esp_app_trace_util.h"
#include "esp_app_trace_config.h"
#include "esp_timer.h"

#include "../vsf_espidf.h"
#include "kernel/vsf_kernel.h"
#if defined(VSF_USE_HEAP) && VSF_USE_HEAP == ENABLED
#   include "service/heap/vsf_heap.h"
#else
#   error "VSF_ESPIDF_CFG_USE_APP_TRACE requires VSF_USE_HEAP"
#endif

#include <string.h>
#include <stdio.h>

/*============================ TYPES =========================================*/

typedef struct {
    bool                inited;

    /* up channel (target -> host) */
    uint8_t            *up_msg_buff;
    uint32_t            up_msg_buff_size;
    uint32_t            up_pending_size;
    FILE               *up_stream;

    /* down channel (host -> target) */
    uint8_t            *down_msg_buff;
    uint32_t            down_msg_buff_size;
    FILE               *down_stream;

    esp_apptrace_dest_t dest;
} esp_apptrace_virt_data_t;

/*============================ GLOBAL VARIABLES ==============================*/

static esp_apptrace_virt_data_t __vsf_espidf_apptrace_virt;

/*============================ STATIC VARIABLES ==============================*/

static vsf_espidf_app_trace_cfg_t __vsf_espidf_apptrace_cfg;

/*============================ IMPLEMENTATION ================================*/

/* ---- util: timeout ----------------------------------------------------- */

void esp_apptrace_tmo_init(esp_apptrace_tmo_t *tmo, uint32_t user_tmo)
{
    tmo->start   = esp_timer_get_time();
    tmo->tmo     = (user_tmo == ESP_APPTRACE_TMO_INFINITE)
                        ? (int64_t)-1 : (int64_t)user_tmo;
    tmo->elapsed = 0;
}

esp_err_t esp_apptrace_tmo_check(esp_apptrace_tmo_t *tmo)
{
    if (tmo->tmo == (int64_t)-1) {
        return ESP_OK;
    }
    tmo->elapsed = esp_timer_get_time() - tmo->start;
    if (tmo->elapsed >= tmo->tmo) {
        return ESP_ERR_TIMEOUT;
    }
    return ESP_OK;
}

uint32_t esp_apptrace_tmo_remaining_us(esp_apptrace_tmo_t *tmo)
{
    if (tmo->tmo == (int64_t)-1) {
        return ESP_APPTRACE_TMO_INFINITE;
    }
    int64_t remaining = tmo->tmo - (esp_timer_get_time() - tmo->start);
    return (remaining > 0) ? (uint32_t)remaining : 0;
}

/* ---- util: ring buffer ------------------------------------------------- */

void esp_apptrace_rb_init(esp_apptrace_rb_t *rb, uint8_t *data, uint32_t size)
{
    rb->data     = data;
    rb->size     = size;
    rb->cur_size = size;
    rb->rd       = 0;
    rb->wr       = 0;
}

uint8_t *esp_apptrace_rb_produce(esp_apptrace_rb_t *rb, uint32_t size)
{
    uint32_t free_sz = rb->cur_size - rb->wr;
    if (free_sz < size) {
        if (rb->rd > size) {
            rb->cur_size = rb->wr;
            rb->wr = 0;
        } else {
            return NULL;
        }
    }
    uint8_t *ptr = rb->data + rb->wr;
    rb->wr += size;
    return ptr;
}

uint8_t *esp_apptrace_rb_consume(esp_apptrace_rb_t *rb, uint32_t size)
{
    if (rb->wr - rb->rd < size) {
        if (rb->cur_size - rb->rd < size) {
            return NULL;
        }
    }
    uint8_t *ptr = rb->data + rb->rd;
    rb->rd += size;
    if (rb->rd >= rb->cur_size) {
        rb->rd  = 0;
        rb->wr -= rb->cur_size;
        rb->cur_size = rb->size;
    }
    return ptr;
}

uint32_t esp_apptrace_rb_read_size_get(esp_apptrace_rb_t *rb)
{
    return rb->wr - rb->rd;
}

uint32_t esp_apptrace_rb_write_size_get(esp_apptrace_rb_t *rb)
{
    return rb->cur_size - rb->wr;
}

/* ---- virtual port ------------------------------------------------------ */

static esp_err_t __virt_lock(esp_apptrace_virt_data_t *vd, esp_apptrace_tmo_t *tmo)
{
    (void)vd;
    (void)tmo;
    return ESP_OK;
}

static esp_err_t __virt_unlock(esp_apptrace_virt_data_t *vd)
{
    (void)vd;
    return ESP_OK;
}

static esp_err_t __virt_init(void *hw_data, const esp_apptrace_config_t *config)
{
    esp_apptrace_virt_data_t *vd = (esp_apptrace_virt_data_t *)hw_data;

    if (vd->inited) {
        return ESP_OK;
    }

    vd->dest = config->dest;

    if (vd->up_msg_buff == NULL) {
        vd->up_msg_buff_size = (config->dest == ESP_APPTRACE_DEST_UART)
            ? config->dest_cfg.uart.tx_msg_size
            : CONFIG_APPTRACE_UART_TX_MSG_SIZE;
        if (vd->up_msg_buff_size == 0) {
            vd->up_msg_buff_size = CONFIG_APPTRACE_UART_TX_MSG_SIZE;
        }
        vd->up_msg_buff = (uint8_t *)vsf_heap_malloc(vd->up_msg_buff_size);
        if (vd->up_msg_buff == NULL) {
            return ESP_ERR_NO_MEM;
        }
        memset(vd->up_msg_buff, 0, vd->up_msg_buff_size);
    }

    if (vd->up_stream == NULL) {
        vd->up_stream = stdout;
    }
    if (vd->down_stream == NULL) {
        vd->down_stream = stdin;
    }

    vd->inited = true;
    return ESP_OK;
}

static uint8_t *__virt_up_buffer_get(void *hw_data, uint32_t size,
                                      esp_apptrace_tmo_t *tmo)
{
    esp_apptrace_virt_data_t *vd = (esp_apptrace_virt_data_t *)hw_data;

    if (size == 0 || size > vd->up_msg_buff_size) {
        return NULL;
    }

    if (__virt_lock(vd, tmo) != ESP_OK) {
        return NULL;
    }

    if (vd->up_pending_size != 0) {
        __virt_unlock(vd);
        return NULL;
    }

    vd->up_pending_size = size;
    __virt_unlock(vd);
    return vd->up_msg_buff;
}

static esp_err_t __virt_up_buffer_put(void *hw_data, uint8_t *ptr,
                                       esp_apptrace_tmo_t *tmo)
{
    esp_apptrace_virt_data_t *vd = (esp_apptrace_virt_data_t *)hw_data;

    if (__virt_lock(vd, tmo) != ESP_OK) {
        return ESP_FAIL;
    }

    if (vd->up_stream != NULL && vd->up_pending_size > 0) {
        fwrite(ptr, 1, vd->up_pending_size, vd->up_stream);
        fflush(vd->up_stream);
    }
    vd->up_pending_size = 0;

    __virt_unlock(vd);
    return ESP_OK;
}

static esp_err_t __virt_flush_nolock(void *hw_data, uint32_t min_sz,
                                      esp_apptrace_tmo_t *tmo)
{
    esp_apptrace_virt_data_t *vd = (esp_apptrace_virt_data_t *)hw_data;
    (void)min_sz;
    (void)tmo;

    if (vd->up_stream != NULL) {
        fflush(vd->up_stream);
    }
    return ESP_OK;
}

static esp_err_t __virt_flush(void *hw_data, esp_apptrace_tmo_t *tmo)
{
    esp_apptrace_virt_data_t *vd = (esp_apptrace_virt_data_t *)hw_data;

    if (__virt_lock(vd, tmo) != ESP_OK) {
        return ESP_FAIL;
    }

    esp_err_t ret = __virt_flush_nolock(hw_data, 0, tmo);
    __virt_unlock(vd);
    return ret;
}

static void __virt_down_buffer_config(void *hw_data, uint8_t *buf,
                                       uint32_t size)
{
    esp_apptrace_virt_data_t *vd = (esp_apptrace_virt_data_t *)hw_data;

    vd->down_msg_buff      = buf;
    vd->down_msg_buff_size = size;
}

static uint8_t *__virt_down_buffer_get(void *hw_data, uint32_t *size,
                                        esp_apptrace_tmo_t *tmo)
{
    esp_apptrace_virt_data_t *vd = (esp_apptrace_virt_data_t *)hw_data;

    if (size == NULL || *size == 0) {
        return NULL;
    }
    if (vd->down_msg_buff == NULL) {
        return NULL;
    }
    if (vd->down_stream == NULL) {
        return NULL;
    }

    if (__virt_lock(vd, tmo) != ESP_OK) {
        return NULL;
    }

    uint32_t req = (*size < vd->down_msg_buff_size)
                        ? *size : vd->down_msg_buff_size;
    uint32_t total = 0;

    while (total < req) {
        int c = fgetc(vd->down_stream);
        if (c == EOF) {
            clearerr(vd->down_stream);
            if (esp_apptrace_tmo_check(tmo) != ESP_OK) {
                break;
            }
            continue;
        }
        vd->down_msg_buff[total++] = (uint8_t)c;
    }

    *size = total;
    __virt_unlock(vd);
    return (total > 0) ? vd->down_msg_buff : NULL;
}

static esp_err_t __virt_down_buffer_put(void *hw_data, uint8_t *ptr,
                                         esp_apptrace_tmo_t *tmo)
{
    (void)hw_data;
    (void)ptr;
    (void)tmo;
    return ESP_OK;
}

static bool __virt_host_is_connected(void *hw_data)
{
    esp_apptrace_virt_data_t *vd = (esp_apptrace_virt_data_t *)hw_data;
    return vd->inited;
}

static void __virt_set_header_size(void *hw_data,
                                    esp_apptrace_header_size_t header_size)
{
    (void)hw_data;
    (void)header_size;
}

/* ---- public API -------------------------------------------------------- */

esp_apptrace_hw_t *esp_apptrace_virt_hw_get(void **data)
{
    static esp_apptrace_hw_t s_virt_hw = {
        .init                 = __virt_init,
        .get_up_buffer        = __virt_up_buffer_get,
        .put_up_buffer        = __virt_up_buffer_put,
        .flush_up_buffer_nolock = __virt_flush_nolock,
        .flush_up_buffer      = __virt_flush,
        .down_buffer_config   = __virt_down_buffer_config,
        .get_down_buffer      = __virt_down_buffer_get,
        .put_down_buffer      = __virt_down_buffer_put,
        .host_is_connected    = __virt_host_is_connected,
        .set_header_size      = __virt_set_header_size,
    };
    *data = &__vsf_espidf_apptrace_virt;
    return &s_virt_hw;
}

/* ---- cfg init ---------------------------------------------------------- */

void vsf_espidf_app_trace_init(const vsf_espidf_app_trace_cfg_t *cfg)
{
    if (cfg != NULL) {
        __vsf_espidf_apptrace_cfg = *cfg;
    } else {
        memset(&__vsf_espidf_apptrace_cfg, 0, sizeof(__vsf_espidf_apptrace_cfg));
    }

    esp_apptrace_virt_data_t *vd = &__vsf_espidf_apptrace_virt;
    memset(vd, 0, sizeof(*vd));

    if (__vsf_espidf_apptrace_cfg.up_stream != NULL) {
        vd->up_stream = __vsf_espidf_apptrace_cfg.up_stream;
    }
    if (__vsf_espidf_apptrace_cfg.down_stream != NULL) {
        vd->down_stream = __vsf_espidf_apptrace_cfg.down_stream;
    }
}

const vsf_espidf_app_trace_cfg_t *vsf_espidf_get_app_trace_cfg(void)
{
    return &__vsf_espidf_apptrace_cfg;
}

#endif  /* VSF_USE_ESPIDF && VSF_ESPIDF_CFG_USE_APP_TRACE */
