/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

/*============================ INCLUDES ======================================*/

#include "component/script/python/vsf_python.h"

#if VSF_USE_MICROPYTHON == ENABLED || VSF_USE_PIKAPYTHON == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef int (*vsf_pyal_arg_parser_t)(vsf_pyal_arg_t arg, void *ptr);
typedef int (*vsf_pyal_arg_sized_parser_t)(vsf_pyal_arg_t arg, int size, void *ptr);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

int __vsf_pyal_parse_tuple_keyword(__vsf_pyal_keyword_parser_arg(arg), const char *format, ...)
{
    bool is_forced = true, is_done = false;
    vsf_pyal_arg_t arg;
    va_list ap;
#if     defined(vsf_pyal_keyword_is_str)
    char *key;
#elif   defined(vsf_pyal_keyword_is_int)
    int key;
#else
#   error please define one of vsf_pyal_keyword_is_str and vsf_pyal_keyword_is_int
#endif
    int parsed = 0;
    union {
        int *value_int;
        float *value_float;
        double *value_double;
        bool *value_bool;
        char **value_str;
        void *value_void;
    } u;

    va_start(ap, format);
    while ((*format != '\0') && !is_done) {

        if ((*format != '|') && (*format != ';') && (*format != ':')) {
#if     defined(vsf_pyal_keyword_is_str)
            key = va_arg(ap, char *);
#elif   defined(vsf_pyal_keyword_is_int)
            key = va_arg(ap, int);
#endif
            u.value_void = va_arg(ap, void *);
            if (NULL == u.value_void) {
                return -1;
            }

            arg = __vsf_pyal_keyword_getarg(arg, key, parsed);
        }

        switch (*format++) {
        case '|':
            is_forced = false;
            continue;
        case ';':
        case ':':
            is_done = true;
            continue;
        case 's':
            if (NULL == arg) {
                if (is_forced) {
                    return -1;
                }
            } else {
                *u.value_str = (char *)vsf_pyal_arg_getstr(arg);
            }
            break;
        case 'f':
            if (NULL == arg) {
                if (is_forced) {
                    return -1;
                }
            } else {
                *u.value_float = vsf_pyal_arg_getfloat(arg);
            }
            break;
        case 'i':
            *u.value_int = __vsf_pyal_keyword_getint(arg, key, parsed);
            break;
        case 'p':
            *u.value_bool = __vsf_pyal_keyword_getbool(arg, key, parsed);
            break;
        case 'O': {
                union {
                    vsf_pyal_arg_sized_parser_t fn_parser_sized;
                    vsf_pyal_arg_parser_t fn_parser;
                } fn_u;
                int size = *format == '#' ? 0 : -1;
                if (size >= 0) {
                    format++;
                    size = va_arg(ap, int);
                    fn_u.fn_parser_sized = va_arg(ap, vsf_pyal_arg_sized_parser_t);
                } else {
                    fn_u.fn_parser = va_arg(ap, vsf_pyal_arg_parser_t);
                }

                if (NULL == arg) {
                    if (is_forced) {
                        return -1;
                    }
                } else if (size >= 0) {
                    if (fn_u.fn_parser_sized(arg, size, u.value_void) < 0) {
                        return -1;
                    }
                } else {
                    if (fn_u.fn_parser(arg, u.value_void) < 0) {
                        return -1;
                    }
                }
            }
            break;
        }
        parsed++;
    }
    return parsed;
}

#endif

/* EOF */
