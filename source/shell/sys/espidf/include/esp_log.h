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
 * Clean-room re-implementation of ESP-IDF public API "esp_log.h".
 *
 * Authored from the ESP-IDF v5.x public API reference only. No code or
 * data is copied from the ESP-IDF source tree. Names, enum values and
 * macro semantics are preserved so that unmodified ESP-IDF example code
 * can compile against this header.
 *
 * The VSF port forwards all output through vsf_trace, so log lines share
 * the same stream, coloring and redirection handling as the rest of VSF.
 */

#ifndef __VSF_ESPIDF_ESP_LOG_H__
#define __VSF_ESPIDF_ESP_LOG_H__

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============================ TYPES =========================================*/

typedef enum {
    ESP_LOG_NONE    = 0,    /*!< No log output                               */
    ESP_LOG_ERROR   = 1,    /*!< Critical errors, software malfunction        */
    ESP_LOG_WARN    = 2,    /*!< Recoverable warnings                         */
    ESP_LOG_INFO    = 3,    /*!< Informational messages                       */
    ESP_LOG_DEBUG   = 4,    /*!< Extra information, not needed for normal use */
    ESP_LOG_VERBOSE = 5,    /*!< Bigger chunks of debug information           */
} esp_log_level_t;

/* Drop-in replacement for the vprintf-shaped redirection hook.
 * Returns number of characters written. */
typedef int (*vprintf_like_t)(const char *, va_list);

/*============================ PROTOTYPES ====================================*/

/* Install tag-specific log level. Tag "*" sets the default level applied to
 * any tag that does not have its own override. Pass level=ESP_LOG_NONE to
 * silence a tag. */
void esp_log_level_set(const char *tag, esp_log_level_t level);

/* Query effective level for a given tag. Unknown tags fall back to the
 * default level set via tag "*". */
esp_log_level_t esp_log_level_get(const char *tag);

/* Install a custom output function; the previous one is returned so callers
 * can chain. Pass NULL to restore the built-in vsf_trace backend. */
vprintf_like_t esp_log_set_vprintf(vprintf_like_t func);

/* Emit a log line at the given level. Does nothing if filtered. The VSF
 * port prefixes each line with "<LEVEL> <TAG>: " and appends the standard
 * VSF_TRACE_CFG_LINEEND. */
void esp_log_write(esp_log_level_t level, const char *tag, const char *format, ...)
#if defined(__GNUC__) || defined(__clang__)
    __attribute__((format(printf, 3, 4)))
#endif
;

void esp_log_writev(esp_log_level_t level, const char *tag, const char *format, va_list args);

/* Timestamp in milliseconds since boot. Monotonic, wraps around at 2^32 ms. */
uint32_t esp_log_timestamp(void);

/* Formatted wall-clock-like timestamp as static string; VSF provides only a
 * "%08lu" tick representation since there is no RTC baseline. */
char * esp_log_system_timestamp(void);

/* Early-log helpers exist in ESP-IDF to bypass locking during boot. VSF has
 * no such distinction, so they are aliased. */
#define esp_earlylog_write          esp_log_write

/*============================ MACROS ========================================*/

/* Compile-time master level gate. Callers may #define LOG_LOCAL_LEVEL
 * before including this header to pre-filter a translation unit. */
#ifndef LOG_LOCAL_LEVEL
#   define LOG_LOCAL_LEVEL          ESP_LOG_VERBOSE
#endif

/* Check both compile-time and run-time gates. */
#define ESP_LOG_ENABLED(level)           ((level) <= LOG_LOCAL_LEVEL)

#define ESP_LOG_LEVEL_LOCAL(level, tag, format, ...)                        \
    do {                                                                    \
        if ((level) <= LOG_LOCAL_LEVEL) {                                   \
            esp_log_write((level), (tag), format, ##__VA_ARGS__);           \
        }                                                                   \
    } while (0)

#define ESP_LOG_LEVEL(level, tag, format, ...)                              \
    ESP_LOG_LEVEL_LOCAL(level, tag, format, ##__VA_ARGS__)

#define ESP_LOGE(tag, format, ...)  ESP_LOG_LEVEL_LOCAL(ESP_LOG_ERROR,   tag, format, ##__VA_ARGS__)
#define ESP_LOGW(tag, format, ...)  ESP_LOG_LEVEL_LOCAL(ESP_LOG_WARN,    tag, format, ##__VA_ARGS__)
#define ESP_LOGI(tag, format, ...)  ESP_LOG_LEVEL_LOCAL(ESP_LOG_INFO,    tag, format, ##__VA_ARGS__)
#define ESP_LOGD(tag, format, ...)  ESP_LOG_LEVEL_LOCAL(ESP_LOG_DEBUG,   tag, format, ##__VA_ARGS__)
#define ESP_LOGV(tag, format, ...)  ESP_LOG_LEVEL_LOCAL(ESP_LOG_VERBOSE, tag, format, ##__VA_ARGS__)

/* The EARLY variants exist only for API compatibility; they share the same
 * path in this port because there is no pre-scheduler window distinct from
 * post-scheduler in VSF's cooperative model. */
#define ESP_EARLY_LOGE(tag, format, ...) ESP_LOGE(tag, format, ##__VA_ARGS__)
#define ESP_EARLY_LOGW(tag, format, ...) ESP_LOGW(tag, format, ##__VA_ARGS__)
#define ESP_EARLY_LOGI(tag, format, ...) ESP_LOGI(tag, format, ##__VA_ARGS__)
#define ESP_EARLY_LOGD(tag, format, ...) ESP_LOGD(tag, format, ##__VA_ARGS__)
#define ESP_EARLY_LOGV(tag, format, ...) ESP_LOGV(tag, format, ##__VA_ARGS__)

#define ESP_DRAM_LOGE(tag, format, ...)  ESP_LOGE(tag, format, ##__VA_ARGS__)
#define ESP_DRAM_LOGW(tag, format, ...)  ESP_LOGW(tag, format, ##__VA_ARGS__)
#define ESP_DRAM_LOGI(tag, format, ...)  ESP_LOGI(tag, format, ##__VA_ARGS__)
#define ESP_DRAM_LOGD(tag, format, ...)  ESP_LOGD(tag, format, ##__VA_ARGS__)
#define ESP_DRAM_LOGV(tag, format, ...)  ESP_LOGV(tag, format, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_ESP_LOG_H__
