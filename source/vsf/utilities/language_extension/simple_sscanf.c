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


/*============================ INCLUDES ======================================*/

#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

static char * skip_space(char *str, size_t *size)
{
    while (isspace(*str) && *size) {
        str++;
        (*size)--;
    }
    return str;
}

static int vsnscanf(const char *str, size_t size, const char *format, va_list ap)
{
    char ch;
    char *strtmp;
    int result = 0;

    while (*format != '\0') {
        ch = *format++;
        switch (ch) {
        case '%': {
                int width = 0;
                union {
                    struct {
                        int is_signed       : 1;
                    };
                    int all;
                } flags;
                int radix;

                flags.all = 0;
                if ('*' == *format) {
                    // TODO:
                } else {
                    width = strtoull(format, &strtmp, 0);
                    if (format == strtmp) {
                        width = -1;
                    }
                    format = strtmp;
                }

                ch = *format++;
                switch (ch) {
                case 'u':
                    flags.is_signed = 0;
                    radix = 10;
                    width = sizeof(unsigned int);
                    goto parse_integer;
                case 'i':
                case 'd':
                    flags.is_signed = 1;
                    radix = 10;
                    width = sizeof(int);
                    goto parse_integer;
                case 'o':
                    flags.is_signed = 0;
                    radix = 8;
                    width = sizeof(unsigned int);
                    goto parse_integer;
                case 'x':
                case 'X':
                    flags.is_signed = 0;
                    radix = 16;
                    width = sizeof(unsigned int);
                    goto parse_integer;

                parse_integer:
                    str = skip_space((char *)str, &size);
                    if (!size) { goto end; }
                    if (flags.is_signed) {
                        *va_arg(ap, int *) = strtol(str, &strtmp, radix);;
                    } else {
                        *va_arg(ap, unsigned int *) = strtoul(str, &strtmp, radix);;
                    }
                    size -= strtmp - str;
                    str = strtmp;
                    result++;
                    break;
                case 'f':
                    str = skip_space((char *)str, &size);
                    if (!size) { goto end; }
                    *va_arg(ap, float *) = strtof(str, &strtmp);
                    size -= strtmp - str;
                    str = strtmp;
                    result++;
                    break;
                case 'c':
                    str = skip_space((char *)str, &size);
                    if (!size) { goto end; }
                    *va_arg(ap, char *) = *str++;
                    size--;
                    result++;
                    break;
                case 's': {
                        char *ptr = va_arg(ap, char *);
                        while (!isspace(*str) && (!width || (width-- > 0))) {
                            *ptr++ = *str++;
                            size--;
                        }
                        result++;
                    }
                    break;
                }
            }
            break;
        case ' ':
            str = skip_space((char *)str, &size);
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

/* EOF */

