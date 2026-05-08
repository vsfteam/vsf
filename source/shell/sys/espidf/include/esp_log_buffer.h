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
 * Clean-room re-implementation of ESP-IDF public API "esp_log_buffer.h".
 *
 * Baseline: ESP-IDF v5.1 public API.
 */

#ifndef __VSF_ESPIDF_ESP_LOG_BUFFER_H__
#define __VSF_ESPIDF_ESP_LOG_BUFFER_H__

#include <stdint.h>
#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

void esp_log_buffer_hex_internal(const char *tag, const void *buffer,
                                  uint16_t buff_len, esp_log_level_t level);
void esp_log_buffer_char_internal(const char *tag, const void *buffer,
                                   uint16_t buff_len, esp_log_level_t level);
void esp_log_buffer_hexdump_internal(const char *tag, const void *buffer,
                                      uint16_t buff_len, esp_log_level_t log_level);

#define ESP_LOG_BUFFER_HEX_LEVEL(tag, buffer, buff_len, level) \
    do { if (ESP_LOG_ENABLED(level)) { \
        esp_log_buffer_hex_internal(tag, buffer, buff_len, level); } } while(0)

#define ESP_LOG_BUFFER_CHAR_LEVEL(tag, buffer, buff_len, level) \
    do { if (ESP_LOG_ENABLED(level)) { \
        esp_log_buffer_char_internal(tag, buffer, buff_len, level); } } while(0)

#define ESP_LOG_BUFFER_HEXDUMP(tag, buffer, buff_len, level) \
    do { if (ESP_LOG_ENABLED(level)) { \
        esp_log_buffer_hexdump_internal(tag, buffer, buff_len, level); } } while(0)

#define ESP_LOG_BUFFER_HEX(tag, buffer, buff_len) \
    do { if (ESP_LOG_ENABLED(ESP_LOG_INFO)) { \
        ESP_LOG_BUFFER_HEX_LEVEL(tag, buffer, buff_len, ESP_LOG_INFO); } } while(0)

#define ESP_LOG_BUFFER_CHAR(tag, buffer, buff_len) \
    do { if (ESP_LOG_ENABLED(ESP_LOG_INFO)) { \
        ESP_LOG_BUFFER_CHAR_LEVEL(tag, buffer, buff_len, ESP_LOG_INFO); } } while(0)

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_ESP_LOG_BUFFER_H__
