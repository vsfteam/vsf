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
 * Port implementation for "esp_log_buffer.h" on VSF.
 */

#include "../vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED && VSF_ESPIDF_CFG_USE_LOG == ENABLED

#include "esp_log_buffer.h"
#include "esp_log.h"
#include <stdio.h>

void esp_log_buffer_hex_internal(const char *tag, const void *buffer,
                                  uint16_t buff_len, esp_log_level_t level)
{
    const uint8_t *p = (const uint8_t *)buffer;
    char line[64];
    for (uint16_t i = 0; i < buff_len; i += 16) {
        int pos = 0;
        for (uint16_t j = 0; j < 16 && (i + j) < buff_len; j++) {
            pos += snprintf(line + pos, sizeof(line) - pos, "%02x ", p[i + j]);
        }
        esp_log_write(level, tag, "%s", line);
    }
}

void esp_log_buffer_char_internal(const char *tag, const void *buffer,
                                   uint16_t buff_len, esp_log_level_t level)
{
    const uint8_t *p = (const uint8_t *)buffer;
    for (uint16_t i = 0; i < buff_len; i += 16) {
        uint16_t chunk = (buff_len - i) > 16 ? 16 : (buff_len - i);
        esp_log_write(level, tag, "%.*s", chunk, p + i);
    }
}

void esp_log_buffer_hexdump_internal(const char *tag, const void *buffer,
                                      uint16_t buff_len, esp_log_level_t level)
{
    const uint8_t *p = (const uint8_t *)buffer;
    char line[128];
    for (uint16_t i = 0; i < buff_len; i += 16) {
        int pos = snprintf(line, sizeof(line), "0x%08x  ", (unsigned)(uintptr_t)(p + i));
        for (uint16_t j = 0; j < 16; j++) {
            if ((i + j) < buff_len) {
                pos += snprintf(line + pos, sizeof(line) - pos, "%02x ", p[i + j]);
            } else {
                pos += snprintf(line + pos, sizeof(line) - pos, "   ");
            }
            if (j == 7) {
                pos += snprintf(line + pos, sizeof(line) - pos, " ");
            }
        }
        pos += snprintf(line + pos, sizeof(line) - pos, " |");
        for (uint16_t j = 0; j < 16 && (i + j) < buff_len; j++) {
            uint8_t c = p[i + j];
            pos += snprintf(line + pos, sizeof(line) - pos, "%c",
                            (c >= 32 && c < 127) ? (char)c : '.');
        }
        snprintf(line + pos, sizeof(line) - pos, "|");
        esp_log_write(level, tag, "%s", line);
    }
}

#endif      // VSF_USE_ESPIDF && VSF_ESPIDF_CFG_USE_LOG
