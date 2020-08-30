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

int vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
    if(!str || !format) {
        return 0;
    }

    char ch, *curpos = str;
    union {
        char ch;
        char *str;
        signed long long integer;
        unsigned long long uinteger;
    } arg;

    if (!size) {
        goto end;
    }

    size--;     // reserve for '\0' terminator
    while (*format != '\0') {
        ch = *format++;
        switch (ch) {
        case '%': {
                union {
                    struct {
                        int has_prefix0     : 1;
                        int has_prefix      : 1; 
                        int align_left      : 1;
                        int has_plus_minus  : 1;
                        int is_upper        : 1;
                        int is_signed       : 1;
                        int is_plus         : 1;
                    };
                    int all;
                } flags;

                char *format_tmp;
                int radix;
                int width;
                int actual_width;
                int integer_wordsize;

                flags.all = 0;
                switch (*format) {
                case '0':   flags.has_prefix0 = 1;      format++;   break;
                case '-':   flags.align_left = 1;       format++;   break;
                case '+':   flags.has_plus_minus = 1;   format++;   break;
                case ' ':                               format++;   break;
                case '#':   flags.has_prefix = 1;       format++;   break;
                }
                width = strtoull(format, &format_tmp, 0);
                if (format == format_tmp) {
                    width = -1;
                }
                format = format_tmp;

            next:
                ch = *format++;
                flags.is_upper = isupper(ch) ? 1 : 0;
                switch (ch) {
                case 'u':
                    integer_wordsize = 1;
                    flags.is_signed = 0;
                    radix = 10;
                    goto print_integer;
                case 'i':
                case 'd':
                    integer_wordsize = 1;
                    flags.is_signed = 1;
                    radix = 10;
                    goto print_integer;
                case 'b':
                    integer_wordsize = 1;
                    flags.is_signed = 0;
                    radix = 2;
                    goto print_integer;
                case 'o':
                    integer_wordsize = 1;
                    flags.is_signed = 0;
                    radix = 8;
                    goto print_integer;
                case 'x':
                case 'X':
                    integer_wordsize = 1;
                    flags.is_signed = 0;
                    radix = 16;
                    goto print_integer;
                case '*':
                    width = va_arg(ap, int);
                    goto next;

                // TODO: support %llx etc
                print_integer:
                    if (1 == integer_wordsize) {
                        arg.uinteger = va_arg(ap, unsigned int);
                        if (flags.is_signed) {
                            arg.integer <<= 32;
                            arg.integer >>= 32;
                        }
                    } else if (2 == integer_wordsize) {
                        // endian??, not tested
                        arg.uinteger = va_arg(ap, unsigned int);
                        arg.uinteger += ((unsigned long long)va_arg(ap, unsigned int) << 32);
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
                                if (!size--) { goto end; }
                                *curpos++ = flags.has_prefix0 ? '0' : ' ';
                            }
                        }
                        if (flags.has_plus_minus) {
                            if (!size--) { goto end; }
                            *curpos++ = flags.is_plus ? '+' : '-';
                            actual_width--;
                        }
                        while (actual_width-- > 0) {
                            if (!size--) { goto end; }
                            *curpos++ = integer_buf[pos++];
                        }
                        if (flags.align_left) {
                            while (width-- > 0) {
                                if (!size--) { goto end; }
                                *curpos++ = ' ';
                            }
                        }
                    }
                    break;
                case 'c':
                case 'C':
                    arg.ch = va_arg(ap, int);
                    if (!size--) { goto end; }
                    *curpos++ = arg.ch;
                    break;
                case 's':
                case 'S':
                    arg.str = va_arg(ap, char *);
                    if (!arg.str) {
                        goto end;
                    }

                    actual_width = strlen(arg.str);
                    if (width < 0) {
                        width = actual_width;
                    }
                    if (!flags.align_left) {
                        while (actual_width < width) {
                            width--;
                            if (!size--) { goto end; }
                            *curpos++ = ' ';
                        }
                    }
                    while ((*arg.str != '\0') && (width-- > 0)) {
                        if (!size--) { goto end; }
                        *curpos++ = *arg.str++;
                    }
                    if (flags.align_left) {
                        while (width-- > 0) {
                            if (!size--) { goto end; }
                            *curpos++ = ' ';
                        }
                    }
                    break;
                default:
                    goto end;
                }
            }
            break;
        default:
            if (!size--) { goto end; }
            *curpos++ = ch;
            break;
        }
    }
end:
    *curpos = '\0';
    return curpos - str;
}

int snprintf(char *str, size_t size, const char *format, ...)
{
    int real_size;
    va_list ap;
    va_start(ap, format);
        real_size = vsnprintf(str, size, format, ap);
    va_end(ap);
    return real_size;
}

int vsprintf(char *str, const char *format, va_list ap)
{
    return vsnprintf(str, (size_t)-1, format, ap);
}

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
