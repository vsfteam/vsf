/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#ifndef __VSF_JSON_H__
#define __VSF_JSON_H__

/*============================ INCLUDES ======================================*/
#include "service/vsf_service_cfg.h"

#if VSF_USE_JSON == ENABLED

#include "utilities/vsf_utilities.h"

#if     defined(__VSF_JSON_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef __VSF_JSON_CLASS_IMPLEMENT
#elif   defined(__VSF_JSON_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#   undef __VSF_JSON_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_json_set_object(__c, __key, ...)                                    \
        do {                                                                    \
            int len = vsf_json_set_key((__c), (__key));                         \
            if (len < 0) break;                                                 \
            (__c)->first = true;                                                \
            len = vsf_json_write_str((__c), "{", 1);                            \
            if (len < 0) break;                                                 \
            __VA_ARGS__;                                                        \
            len = vsf_json_write_str((__c), "}", 1);                            \
            if (len < 0) break;                                                 \
        } while (0)


#define vsf_json_set_array(__c, __key, ...)                                     \
        do {                                                                    \
            int len = vsf_json_set_key((__c), (__key));                         \
            if (len < 0) break;                                                 \
            (__c)->first = true;                                                \
            len = vsf_json_write_str((__c), "[", 1);                            \
            if (len < 0) break;                                                 \
            __VA_ARGS__;                                                        \
            len = vsf_json_write_str((__c), "]", 1);                            \
            if (len < 0) break;                                                 \
        } while (0)

/*============================ TYPES =========================================*/

declare_simple_class(vsf_json_enumerator_t)
declare_simple_class(vsf_json_constructor_t)

enum vsf_json_type_t {
    VSF_JSON_TYPE_INVALID,
    VSF_JSON_TYPE_OBJECT,
    VSF_JSON_TYPE_ARRAY,
    VSF_JSON_TYPE_STRING,
    VSF_JSON_TYPE_NUMBER,
    VSF_JSON_TYPE_BOOLEAN,
    VSF_JSON_TYPE_NULL,
};
typedef enum vsf_json_type_t vsf_json_type_t;

def_simple_class(vsf_json_enumerator_t) {
    private_member(
        char *ptr;
        vsf_json_type_t type;
        bool first;
    )
};

def_simple_class(vsf_json_constructor_t) {
    public_member(
        union {
            void *param;
            uint32_t len;
        };
        int (*write_str)(void *, char *, int);

        bool first;
        bool result;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_json_type_t vsf_json_enumerate_start(vsf_json_enumerator_t *e, const char *json);
extern char * vsf_json_enumerate_next(vsf_json_enumerator_t *e);
extern char * vsf_json_get(const char *json, const char *key);
extern int vsf_json_num_of_entry(const char *json);

extern vsf_json_type_t vsf_json_get_type(const char *json);
extern int vsf_json_get_string(const char *json, char *result, int len);
extern int vsf_json_get_number(const char *json, double *result);
extern int vsf_json_get_boolean(const char *json, bool *result);



extern void vsf_json_constructor_init(vsf_json_constructor_t *c, void *param,
        int (*write_str)(void *, char *, int));
// param is vsf_mem_t *
extern int vsf_json_constructor_buffer_write_str(void *param, char *str, int len);

extern int vsf_json_write_str(vsf_json_constructor_t *c, char *buf, int len);
extern int vsf_json_set_key(vsf_json_constructor_t *c, char *key);
extern int vsf_json_set_string(vsf_json_constructor_t *c, char *key, char *value);
extern int vsf_json_set_integer(vsf_json_constructor_t *c, char *key, int value);
extern int vsf_json_set_double(vsf_json_constructor_t *c, char *key, double value);
extern int vsf_json_set_boolean(vsf_json_constructor_t *c, char *key, bool value);
extern int vsf_json_set_null(vsf_json_constructor_t *c, char *key);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_JSON
#endif      // __VSF_JSON_H__
