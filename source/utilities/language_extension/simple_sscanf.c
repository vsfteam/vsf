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

#include "../vsf_utilities_cfg.h"

#if VSF_USE_SIMPLE_SSCANF == ENABLED

#include "../compiler/compiler.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

static char * __skip_space(char *str, size_t *size)
{
    while (isspace(*str) && *size) {
        str++;
        (*size)--;
    }
    return str;
}

static bool __is_in_seq(char ch, const char *seq, int seq_len)
{
    for (int i = 0; i < seq_len; i++) {
        if (ch == seq[i]) {
            return true;
        }
    }
    return false;
}

static int vsnscanf(const char *str, size_t size, const char *format, va_list ap)
{
    char ch, integer_buf[32], *ptr;
    char *strtmp;
    int result = 0, cur_size;

    goto skip_space;
    while (*format != '\0') {
        ch = *format++;
        switch (ch) {
        case '%': {
                int width = 0;
                union {
                    struct {
                        unsigned is_signed      : 1;
                    };
                    unsigned all;
                } flags;
                int radix;

                int seqlen;
                const char *seq;

                flags.all = 0;
                if ('*' == *format) {
                    // TODO:
                } else {
                    width = strtoull(format, &strtmp, 0);
                    format = strtmp;
                }

                ch = *format++;
                switch (ch) {
                case '[':
                    seq = format;
                    while (true) {
                        ch = *format++;
                        if (!ch) {
                            return -1;
                        }
                        if (ch == ']') {
                            break;
                        }
                    }
                    seqlen = format - seq - 1;

                    ptr = va_arg(ap, char *);
                    if (!width) { width = -1; }
                    while (__is_in_seq(*str, seq, seqlen) && (width-- > 0)) {
                        *ptr++ = *str++;
                        size--;
                    }
                    result++;
                    break;
                case 'u':
                    flags.is_signed = 0;
                    radix = 10;
                    goto parse_integer;
                case 'i':
                case 'd':
                    flags.is_signed = 1;
                    radix = 10;
                    goto parse_integer;
                case 'o':
                    flags.is_signed = 0;
                    radix = 8;
                    goto parse_integer;
                case 'x':
                case 'X':
                    flags.is_signed = 0;
                    radix = 16;
                    goto parse_integer;

                parse_integer:
                    if (width > 0) {
                        VSF_ASSERT(width <= sizeof(integer_buf) - 1);
                        memcpy(integer_buf, str, width);
                        integer_buf[width] = '\0';
                        ptr = integer_buf;
                    } else {
                        ptr = (char *)str;
                    }
                    if (flags.is_signed) {
                        *va_arg(ap, int *) = strtol(ptr, &strtmp, radix);
                    } else {
                        *va_arg(ap, unsigned int *) = strtoul(ptr, &strtmp, radix);
                    }
                    cur_size = strtmp - ptr;
                    size -= cur_size;
                    str += cur_size;
                    result++;
                    break;
                case 'f':
                    *va_arg(ap, float *) = strtof(str, &strtmp);
                    size -= strtmp - str;
                    str = strtmp;
                    result++;
                    break;
                case 'c':
                    *va_arg(ap, char *) = *str++;
                    size--;
                    result++;
                    break;
                case 's': {
                        char *ptr = va_arg(ap, char *);
                        if (width) { width--; /* reserved for '\0' */ }
                        else { width = -1; }
                        while (!isspace(*str) && ((width < 0) || (width-- > 0))) {
                            *ptr++ = *str++;
                            size--;
                        }
                        *ptr = '\0';
                        result++;
                    }
                    break;
                }
            }
            break;
        case ' ':
        skip_space:
            str = __skip_space((char *)str, &size);
            if (!size) { goto end; }
            break;
        default:
            if (*str++ != ch) {
                goto end;
            }
            size--;
            break;
        }
    }
end:
    return result;
}

int vsscanf(const char *str, const char *format, va_list ap)
{
    return vsnscanf(str, (size_t)-1, format, ap);
}

int snscanf(const char *str, size_t size, const char *format, ...)
{
    int result;
    va_list ap;
    va_start(ap, format);
        result = vsnscanf(str, size, format, ap);
    va_end(ap);
    return result;
}

int sscanf(const char *str, const char *format, ...)
{
    int result;
    va_list ap;
    va_start(ap, format);
        result = vsscanf(str, format, ap);
    va_end(ap);
    return result;
}

#endif      // VSF_USE_SIMPLE_SSCANF

/* EOF */
