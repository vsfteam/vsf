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

#if VSF_USE_SIMPLE_SPRINTF == ENABLED

#include "../compiler/compiler.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

#ifdef __WIN__
// TODO: fixme. to avoid conflict with libvcruntime(undname.obj) in the latest Visual Studio 2022
WEAK(vsnprintf)
#endif
int vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
    if (!format) {
        return 0;
    }

    int realsize = 0;
    char ch, *curpos = str;
    union {
        char ch;
        char *str;
        signed long long integer;
        unsigned long long uinteger;
    } arg;

    if (NULL == str) { size = 0; }
    if (0 == size) { curpos = NULL; }
    // reserve for '\0' terminator
    if (size > 0) { size--; }
    while (*format != '\0') {
    next_char:
        ch = *format++;
        switch (ch) {
        case '%': {
                union {
                    struct {
                        unsigned has_prefix0    : 1;
                        unsigned has_prefix     : 1;
                        unsigned align_left     : 1;
                        unsigned has_plus_minus : 1;
                        unsigned is_upper       : 1;
                        unsigned is_signed      : 1;
                        unsigned is_plus        : 1;
                        unsigned is_halfword    : 1;
                        unsigned long_cnt       : 2;
                    };
                    unsigned all;
                } flags;

                char *format_tmp;
                int radix;
                int width;
                int precision = 0;
                int actual_width;

                flags.all = 0;
                switch (*format) {
                case '0':   flags.has_prefix0 = 1;      format++;   break;
                case '-':   flags.align_left = 1;       format++;   break;
                case '+':   flags.has_plus_minus = 1;   format++;   break;
                case ' ':                               format++;   break;
                case '#':   flags.has_prefix = 1;       format++;   break;
                case '%':
                    if (++realsize <= size) {
                        *curpos++ = '%';
                    }
                    format++;
                    goto next_char;
                }

                if ('*' == *format) {
                    width = va_arg(ap, int);
                    format++;
                } else {
                    width = strtoull(format, &format_tmp, 0);
                    format = format_tmp;
                }
                if (width < 0) {
                    width = -width;
                    flags.align_left = 1;
                }
                if ('.' == *format) {
                    format++;
                    if ('*' == *format) {
                        precision = va_arg(ap, int);
                        format++;
                    } else {
                        precision = strtoull(format, &format_tmp, 0);
                        format = format_tmp;
                    }
                }

            next:
                ch = *format++;
                switch (ch) {
                case 'h':
                    if (flags.long_cnt) {
                        return 0;
                    }
                    flags.is_halfword = 1;
                    goto next;
                case 'l':
                    if (flags.is_halfword || (flags.long_cnt >= 2)) {
                        return 0;
                    }
                    flags.long_cnt++;
                    goto next;
                case 'u':
                    flags.is_signed = 0;
                    radix = 10;
                    goto print_integer;
                case 'i':
                case 'd':
                    flags.is_signed = 1;
                    radix = 10;
                    goto print_integer;
                case 'b':
                    flags.is_signed = 0;
                    radix = 2;
                    goto print_integer;
                case 'o':
                    flags.is_signed = 0;
                    radix = 8;
                    goto print_integer;
                case 'X':
                case 'P':
                    flags.is_upper = 1;
                case 'p':
                case 'x':
                    flags.is_signed = 0;
                    radix = 16;
                    goto print_integer;

                // TODO: support %llx etc
                print_integer:
                    if (flags.is_signed) {
                        if (1 == flags.long_cnt) {
                            arg.integer = va_arg(ap, long);
                        } else if (2 == flags.long_cnt) {
                            arg.integer = va_arg(ap, long long);
                        } else {
                            arg.integer = va_arg(ap, int);
                        }
                        if (flags.is_halfword) {
                            arg.integer = (short)arg.integer;
                        }
                    } else {
                        if (1 == flags.long_cnt) {
                            arg.uinteger = va_arg(ap, unsigned long);
                        } else if (2 == flags.long_cnt) {
                            arg.uinteger = va_arg(ap, unsigned long long);
                        } else {
                            arg.uinteger = va_arg(ap, unsigned int);
                        }
                        if (flags.is_halfword) {
                            arg.integer = (unsigned short)arg.integer;
                        }
                    }

                    {
                        char integer_buf[32];
                        int pos = sizeof(integer_buf) - 1;
                        int cur_integer;

                        if (flags.is_signed) {
                            flags.is_plus = arg.integer >= 0;
                            if (!flags.is_plus) {
                                arg.integer = -arg.integer;
                            }
                        } else {
                            flags.is_plus = 1;
                        }
                        if (arg.uinteger != 0) {
                            while (arg.uinteger != 0) {
                                cur_integer = arg.uinteger % radix;
                                arg.uinteger /= radix;
                                integer_buf[pos--] = (cur_integer < 10) ? '0' + cur_integer : (flags.is_upper ? 'A' : 'a') + (cur_integer - 10);
                            }
                        } else {
                            integer_buf[pos--] = '0';
                        }

                        if (!flags.is_plus) {
                            flags.has_plus_minus = 1;
                        }
                        actual_width = sizeof(integer_buf) - pos++ - 1 + (flags.has_plus_minus ? 1 : 0);
                        width -= actual_width;
                        if (!flags.align_left) {
                            while (width-- > 0) {
                                if (++realsize <= size) {
                                    *curpos++ = flags.has_prefix0 ? '0' : ' ';
                                }
                            }
                        }
                        if (flags.has_plus_minus) {
                            if (++realsize <= size) {
                                *curpos++ = flags.is_plus ? '+' : '-';
                            }
                            actual_width--;
                        }
                        while (actual_width-- > 0) {
                            if (++realsize <= size) {
                                *curpos++ = integer_buf[pos++];
                            }
                        }
                        if (flags.align_left) {
                            while (width-- > 0) {
                                if (++realsize <= size) {
                                    *curpos++ = ' ';
                                }
                            }
                        }
                    }
                    break;
                case 'c':
                case 'C':
                    arg.ch = va_arg(ap, int);
                    if (++realsize <= size) {
                        *curpos++ = arg.ch;
                    }
                    break;
                case 's':
                case 'S':
                    arg.str = va_arg(ap, char *);
                    if (!arg.str) {
                        goto end;
                    }

                    actual_width = strlen(arg.str);
                    if (width < actual_width) {
                        width = actual_width;
                    }
                    if ((precision != 0) && (width > precision)) {
                        width = precision;
                    }
                    if (!flags.align_left) {
                        while (actual_width < width) {
                            width--;
                            if (++realsize <= size) {
                                *curpos++ = ' ';
                            }
                        }
                    }
                    while ((*arg.str != '\0') && (width-- > 0)) {
                        if (++realsize <= size) {
                            *curpos++ = *arg.str++;
                        }
                    }
                    if (flags.align_left) {
                        while (width-- > 0) {
                            if (++realsize <= size) {
                                *curpos++ = ' ';
                            }
                        }
                    }
                    break;
                default:
                    goto end;
                }
            }
            break;
        default:
            if (++realsize <= size) {
                *curpos++ = ch;
            }
            break;
        }
    }
end:
    if (curpos != NULL) {
        *curpos = '\0';
    }
    return realsize;
}

#ifdef __WIN__
// TODO: fixme. to avoid conflict with libvcruntime(undname.obj) in the latest Visual Studio 2022
WEAK(snprintf)
#endif
int snprintf(char *str, size_t size, const char *format, ...)
{
    int real_size;
    va_list ap;
    va_start(ap, format);
        real_size = vsnprintf(str, size, format, ap);
    va_end(ap);
    return real_size;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat-truncation"
#endif

int vsprintf(char *str, const char *format, va_list ap)
{
    return vsnprintf(str, (size_t)-1, format, ap);
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

int sprintf(char *str, const char *format, ...)
{
    int real_size;
    va_list ap;
    va_start(ap, format);
        real_size = vsprintf(str, format, ap);
    va_end(ap);
    return real_size;
}

#endif      // VSF_USE_SIMPLE_SPRINTF

/* EOF */
