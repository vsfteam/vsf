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
 * Clean-room re-implementation of ESP-IDF public API "esp_check.h".
 *
 * Error-checking convenience macros. Each macro evaluates an esp_err_t
 * expression or boolean condition and, on failure, either returns the
 * error code, returns void, or jumps to a goto label — always logging
 * the failure site with ESP_LOGE / ESP_EARLY_LOGE.
 *
 * Dependency chain: esp_err.h → esp_log.h → esp_compiler.h → this file.
 *
 * Baseline: ESP-IDF v5.1 public API.
 */

#ifndef __VSF_ESPIDF_ESP_CHECK_H__
#define __VSF_ESPIDF_ESP_CHECK_H__

#include "esp_err.h"
#include "esp_log.h"
#include "esp_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/* Helper: ISR-safe log alias. VSF has no early-log distinction;       */
/* ESP_EARLY_LOGE maps to the same trace service as ESP_LOGE.          */
/* ------------------------------------------------------------------ */
#ifndef ESP_EARLY_LOGE
#   define ESP_EARLY_LOGE(tag, format, ...) \
        ESP_LOGE(tag, format, ##__VA_ARGS__)
#endif

/* ------------------------------------------------------------------ */
/* ESP_RETURN_ON_ERROR(x, tag, fmt, ...)                               */
/*   If x != ESP_OK → log + return err_rc_.                            */
/* ------------------------------------------------------------------ */
#define ESP_RETURN_ON_ERROR(x, log_tag, format, ...)  do {                       \
        esp_err_t err_rc_ = (x);                                                 \
        if (unlikely(err_rc_ != ESP_OK)) {                                       \
            ESP_LOGE(log_tag, "%s(%d): " format,                                 \
                     __FUNCTION__, __LINE__, ##__VA_ARGS__);                     \
            return err_rc_;                                                      \
        }                                                                        \
    } while(0)

#define ESP_RETURN_ON_ERROR_ISR(x, log_tag, format, ...)  do {                   \
        esp_err_t err_rc_ = (x);                                                 \
        if (unlikely(err_rc_ != ESP_OK)) {                                       \
            ESP_EARLY_LOGE(log_tag, "%s(%d): " format,                           \
                           __FUNCTION__, __LINE__, ##__VA_ARGS__);               \
            return err_rc_;                                                      \
        }                                                                        \
    } while(0)

/* ------------------------------------------------------------------ */
/* ESP_RETURN_VOID_ON_ERROR(x, tag, fmt, ...)                          */
/*   If x != ESP_OK → log + return (void).                             */
/* ------------------------------------------------------------------ */
#define ESP_RETURN_VOID_ON_ERROR(x, log_tag, format, ...)  do {                   \
        esp_err_t err_rc_ = (x);                                                 \
        if (unlikely(err_rc_ != ESP_OK)) {                                       \
            ESP_LOGE(log_tag, "%s(%d): " format,                                 \
                     __FUNCTION__, __LINE__, ##__VA_ARGS__);                     \
            return;                                                              \
        }                                                                        \
    } while(0)

#define ESP_RETURN_VOID_ON_ERROR_ISR(x, log_tag, format, ...)  do {              \
        esp_err_t err_rc_ = (x);                                                 \
        if (unlikely(err_rc_ != ESP_OK)) {                                       \
            ESP_EARLY_LOGE(log_tag, "%s(%d): " format,                           \
                           __FUNCTION__, __LINE__, ##__VA_ARGS__);               \
            return;                                                              \
        }                                                                        \
    } while(0)

/* ------------------------------------------------------------------ */
/* ESP_GOTO_ON_ERROR(x, goto_tag, tag, fmt, ...)                       */
/*   If x != ESP_OK → log, ret = err_rc_, goto goto_tag.               */
/* ------------------------------------------------------------------ */
#define ESP_GOTO_ON_ERROR(x, goto_tag, log_tag, format, ...)  do {                \
        esp_err_t err_rc_ = (x);                                                 \
        if (unlikely(err_rc_ != ESP_OK)) {                                       \
            ESP_LOGE(log_tag, "%s(%d): " format,                                 \
                     __FUNCTION__, __LINE__, ##__VA_ARGS__);                     \
            ret = err_rc_;                                                       \
            goto goto_tag;                                                       \
        }                                                                        \
    } while(0)

#define ESP_GOTO_ON_ERROR_ISR(x, goto_tag, log_tag, format, ...)  do {           \
        esp_err_t err_rc_ = (x);                                                 \
        if (unlikely(err_rc_ != ESP_OK)) {                                       \
            ESP_EARLY_LOGE(log_tag, "%s(%d): " format,                           \
                           __FUNCTION__, __LINE__, ##__VA_ARGS__);               \
            ret = err_rc_;                                                       \
            goto goto_tag;                                                       \
        }                                                                        \
    } while(0)

/* ------------------------------------------------------------------ */
/* ESP_RETURN_ON_FALSE(cond, err_code, tag, fmt, ...)                  */
/*   If !cond → log + return err_code.                                 */
/* ------------------------------------------------------------------ */
#define ESP_RETURN_ON_FALSE(a, err_code, log_tag, format, ...)  do {              \
        if (unlikely(!(a))) {                                                    \
            ESP_LOGE(log_tag, "%s(%d): " format,                                 \
                     __FUNCTION__, __LINE__, ##__VA_ARGS__);                     \
            return err_code;                                                     \
        }                                                                        \
    } while(0)

#define ESP_RETURN_ON_FALSE_ISR(a, err_code, log_tag, format, ...)  do {         \
        if (unlikely(!(a))) {                                                    \
            ESP_EARLY_LOGE(log_tag, "%s(%d): " format,                           \
                           __FUNCTION__, __LINE__, ##__VA_ARGS__);               \
            return err_code;                                                     \
        }                                                                        \
    } while(0)

/* ------------------------------------------------------------------ */
/* ESP_RETURN_VOID_ON_FALSE(cond, tag, fmt, ...)                       */
/*   If !cond → log + return (void).                                   */
/* ------------------------------------------------------------------ */
#define ESP_RETURN_VOID_ON_FALSE(a, log_tag, format, ...)  do {                   \
        if (unlikely(!(a))) {                                                    \
            ESP_LOGE(log_tag, "%s(%d): " format,                                 \
                     __FUNCTION__, __LINE__, ##__VA_ARGS__);                     \
            return;                                                              \
        }                                                                        \
    } while(0)

#define ESP_RETURN_VOID_ON_FALSE_ISR(a, log_tag, format, ...)  do {              \
        if (unlikely(!(a))) {                                                    \
            ESP_EARLY_LOGE(log_tag, "%s(%d): " format,                           \
                           __FUNCTION__, __LINE__, ##__VA_ARGS__);               \
            return;                                                              \
        }                                                                        \
    } while(0)

/* ------------------------------------------------------------------ */
/* ESP_GOTO_ON_FALSE(cond, err_code, goto_tag, tag, fmt, ...)          */
/*   If !cond → log, ret = err_code, goto goto_tag.                    */
/* ------------------------------------------------------------------ */
#define ESP_GOTO_ON_FALSE(a, err_code, goto_tag, log_tag, format, ...)  do {      \
        if (unlikely(!(a))) {                                                    \
            ESP_LOGE(log_tag, "%s(%d): " format,                                 \
                     __FUNCTION__, __LINE__, ##__VA_ARGS__);                     \
            ret = err_code;                                                      \
            goto goto_tag;                                                       \
        }                                                                        \
    } while(0)

#define ESP_GOTO_ON_FALSE_ISR(a, err_code, goto_tag, log_tag, format, ...)  do { \
        if (unlikely(!(a))) {                                                    \
            ESP_EARLY_LOGE(log_tag, "%s(%d): " format,                           \
                           __FUNCTION__, __LINE__, ##__VA_ARGS__);               \
            ret = err_code;                                                      \
            goto goto_tag;                                                       \
        }                                                                        \
    } while(0)

/* ------------------------------------------------------------------ */
/* ESP_RETURN_ON_ERROR_CLEANUP(x, cleanup...)                          */
/*   If x != ESP_OK → run cleanup code, then return err_rc_.           */
/* ------------------------------------------------------------------ */
#define ESP_RETURN_ON_ERROR_CLEANUP(x, ...)  do {                                \
        esp_err_t err_rc_ = (x);                                                 \
        if (unlikely(err_rc_ != ESP_OK)) {                                       \
            __VA_ARGS__;                                                         \
            return err_rc_;                                                      \
        }                                                                        \
    } while(0)

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_ESP_CHECK_H__
