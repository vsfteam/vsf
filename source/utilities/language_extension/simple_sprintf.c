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

#ifndef VSF_SIMPLE_SPRINTF_SUPPORT_FLOAT
#   define VSF_SIMPLE_SPRINTF_SUPPORT_FLOAT     ENABLED
#endif
#ifndef VSF_SIMPLE_SPRINTF_SUPPORT_IPMAC
#   define VSF_SIMPLE_SPRINTF_SUPPORT_IPMAC     ENABLED
#endif
#ifndef VSF_SIMPLE_SPRINTF_SUPPORT_HEX
#   define VSF_SIMPLE_SPRINTF_SUPPORT_HEX       ENABLED
#endif
#ifndef VSF_SIMPLE_SPRINTF_SUPPORT_UUID
#   define VSF_SIMPLE_SPRINTF_SUPPORT_UUID      ENABLED
#endif

#if VSF_SIMPLE_SPRINTF_SUPPORT_FLOAT == ENABLED
#   include <math.h>
#endif

/*============================ MACROS ========================================*/

#define MIN_RANK        rank_char
#define MAX_RANK        rank_longlong
#define INTMAX_RANK     rank_longlong
#define SIZE_T_RANK     rank_long
#define PTRDIFF_T_RANK  rank_long

#define __DROUND_DIGITS 16
#define __DROUND_FACTOR 1E16

/*============================ MACROFIED FUNCTIONS ===========================*/

#define EMIT(__C)       if (++realsize <= size) { *curpos++ = (__C); }

/*============================ TYPES =========================================*/

enum ranks {
    rank_char           = -2,
    rank_short          = -1,
    rank_int            =  0,
    rank_long           =  1,
    rank_longlong       =  2,
};

/*============================ PROTOTYPES ====================================*/

int snprintf(char *str, size_t size, const char *format, ...);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#   pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wpointer-to-int-cast"
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
        uint8_t *pu8;
        uint16_t *pu16;
        uint32_t *pu32;
        unsigned long long val;
#if VSF_SIMPLE_SPRINTF_SUPPORT_FLOAT == ENABLED
        double d;
#endif
    } arg;
#if VSF_SIMPLE_SPRINTF_SUPPORT_FLOAT == ENABLED
    double d_intpart, d_fractpart, dtmp, pow;
    int exp, round, fwidth, intpart_pos;
#endif
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
            unsigned float_state    : 2;
            unsigned exp_state      : 2;
            unsigned is_g           : 1;
        };
        unsigned all;
    } flags;

    char *format_tmp;
    int radix;
    int width;
    int precision;
    int actual_width;
    int rank;

    if (NULL == str) { size = 0; }
    if (0 == size) { curpos = NULL; }
    // reserve for '\0' terminator
    if (size > 0) { size--; }
    while (*format != '\0') {
    next_char:
        ch = *format++;
        switch (ch) {
        case '%':
            flags.all = 0;
            rank = 0;
        next_flag:
            switch (*format) {
            case '0':   flags.has_prefix0 = 1;      format++;   goto next_flag;
            case '-':   flags.align_left = 1;       format++;   goto next_flag;
            case '+':   flags.has_plus_minus = 1;   format++;   goto next_flag;
            case ' ':                               format++;   goto next_flag;
            case '#':   flags.has_prefix = 1;       format++;   goto next_flag;
            case '%':
                EMIT('%');
                format++;
                goto next_char;
            default:
                break;
            }

            if ('*' == *format) {
                width = va_arg(ap, int);
                format++;
            } else {
                width = strtoull(format, &format_tmp, 0);
                format = format_tmp;
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
            } else {
                precision = -1;
            }

        next:
            ch = *format++;
            switch (ch) {
            case 'j':
                rank = INTMAX_RANK;
                goto next;
            case 'z':
                rank = SIZE_T_RANK;
                goto next;
            case 't':
                rank = PTRDIFF_T_RANK;
                goto next;
            case 'h':
                rank--;
                goto check_rand;
            case 'l':
                rank++;
                goto check_rand;
            case 'L':
            case 'q':
                rank += 2;
            check_rand:
                if (rank < MIN_RANK) {
                    rank = MIN_RANK;
                } else if (rank > MAX_RANK) {
                    rank = MAX_RANK;
                }
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
            case 'P':
                flags.is_upper = 1;
            case 'p':
                arg.val = (unsigned long long)va_arg(ap, void *);
                int remain_width = size - realsize + 1;
                VSF_UNUSED_PARAM(remain_width);
#if VSF_SIMPLE_SPRINTF_SUPPORT_IPMAC == ENABLED
                // use width as size, containing 1-byte NULL terminator
                if (format[0] == 'M') {
                    // %pM : printf mac address, XX:XX:XX:XX:XX:XX
                    format++;
                    if (format[0] == 'R') {
                        format++;
                        radix = snprintf(curpos, remain_width, "%02X:%02X:%02X:%02X:%02X:%02X",
                            arg.pu8[5], arg.pu8[4], arg.pu8[3], arg.pu8[2], arg.pu8[1], arg.pu8[0]);
                    } else if (format[0] == 'F') {
                        format++;
                        radix = snprintf(curpos, remain_width, "%02X-%02X-%02X-%02X-%02X-%02X",
                            arg.pu8[0], arg.pu8[1], arg.pu8[2], arg.pu8[3], arg.pu8[4], arg.pu8[5]);
                    } else {
                        radix = snprintf(curpos, remain_width, "%02X:%02X:%02X:%02X:%02X:%02X",
                            arg.pu8[0], arg.pu8[1], arg.pu8[2], arg.pu8[3], arg.pu8[4], arg.pu8[5]);
                    }
                    realsize += radix;
                    // even if curpos overflows, realsize will protect it
                    curpos += radix;
                    break;
                } else if (format[0] == 'm') {
                    format++;
                    if (format[0] == 'R') {
                        format++;
                        radix = snprintf(curpos, remain_width, "%02X%02X%02X%02X%02X%02X",
                            arg.pu8[5], arg.pu8[4], arg.pu8[3], arg.pu8[2], arg.pu8[1], arg.pu8[0]);
                    } else {
                        radix = snprintf(curpos, remain_width, "%02X%02X%02X%02X%02X%02X",
                            arg.pu8[0], arg.pu8[1], arg.pu8[2], arg.pu8[3], arg.pu8[4], arg.pu8[5]);
                    }
                    realsize += radix;
                    // even if curpos overflows, realsize will protect it
                    curpos += radix;
                    break;
                } else if (format[0] == 'I') {
                    if (format[1] == '4') {
                        // %pI4 : printf IPv4, x.x.x.x
                        format += 2;
                        radix = snprintf(curpos, remain_width, "%d.%d.%d.%d",
                            arg.pu8[0], arg.pu8[1], arg.pu8[2], arg.pu8[3]);
                        realsize += radix;
                        // even if curpos overflows, realsize will protect it
                        curpos += radix;
                        break;
                    } else if (format[1] == '6') {
                        // %pI6 : printf IPv6, xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx
                        format += 2;
                        radix = snprintf(curpos, remain_width, "%04X:%04X:%04X:%04X:%04X:%04X:%04X:%04X",
                            arg.pu16[0], arg.pu16[1], arg.pu16[2], arg.pu16[3],
                            arg.pu16[4], arg.pu16[5], arg.pu16[6], arg.pu16[7]);
                        realsize += radix;
                        // even if curpos overflows, realsize will protect it
                        curpos += radix;
                        break;
                    }
                } else if (format[0] == 'i') {
                    if (format[1] == '4') {
                        // %pI4 : printf IPv4, x.x.x.x
                        format += 2;
                        radix = snprintf(curpos, remain_width, "%03d.%03d.%03d.%03d",
                            arg.pu8[0], arg.pu8[1], arg.pu8[2], arg.pu8[3]);
                        realsize += radix;
                        // even if curpos overflows, realsize will protect it
                        curpos += radix;
                        break;
                    } else if (format[1] == '6') {
                        // %pI6 : printf IPv6, xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx
                        format += 2;
                        radix = snprintf(curpos, remain_width, "%04X%04X%04X%04X%04X%04X%04X%04X",
                            arg.pu16[0], arg.pu16[1], arg.pu16[2], arg.pu16[3],
                            arg.pu16[4], arg.pu16[5], arg.pu16[6], arg.pu16[7]);
                        realsize += radix;
                        // even if curpos overflows, realsize will protect it
                        curpos += radix;
                        break;
                    }
                }
#endif
#if VSF_SIMPLE_SPRINTF_SUPPORT_HEX == ENABLED
                if (*format == 'h') {
                    char div;
                    format++;
                    if (*format == 'D') {
                        format++;
                        div = '-';
                    } else if (*format == 'C') {
                        format++;
                        div = ':';
                    } else if (*format == 'N') {
                        format++;
                        div = '\0';
                    } else {
                        div = ' ';
                    }

                    for (int i = 0; i < width; i++) {
                        if (remain_width < 0) { remain_width = 0; }
                        radix = snprintf(curpos, remain_width, "%02X", arg.pu8[i]);
                        remain_width -= radix;
                        realsize += radix;
                        // even if curpos overflows, realsize will protect it
                        curpos += radix;
                        if ((div != '\0') && (width - i > 1)) {
                            EMIT(div);
                            remain_width--;
                         }
                    }
                    break;
                }
#endif
#if VSF_SIMPLE_SPRINTF_SUPPORT_UUID == ENABLED
                if (*format == 'U') {
                    bool is_upper = false;
                    bool is_big_endian = true;

                    format++;
                    if ('b' == tolower(*format)) {
                        is_big_endian = true;
                        is_upper = *format < 'a';
                        format++;
                    } else if ('l' == tolower(*format)) {
                        is_big_endian = false;
                        is_upper = *format < 'a';
                        format++;
                    }

                    char *uuid_format;
                    if (is_upper) {
                        uuid_format = "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X";
                    } else {
                        uuid_format = "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x";
                    }

                    if (is_big_endian) {
                        radix = snprintf(curpos, remain_width, uuid_format,
                            arg.pu8[0], arg.pu8[1], arg.pu8[2], arg.pu8[3], arg.pu8[4], arg.pu8[5], arg.pu8[6], arg.pu8[7],
                            arg.pu8[8], arg.pu8[9], arg.pu8[10], arg.pu8[11], arg.pu8[12], arg.pu8[13], arg.pu8[14], arg.pu8[15]);
                        realsize += radix;
                        curpos += radix;
                    } else {
                        radix = snprintf(curpos, remain_width, uuid_format,
                            arg.pu8[3], arg.pu8[2], arg.pu8[1], arg.pu8[0], arg.pu8[5], arg.pu8[4], arg.pu8[7], arg.pu8[6],
                            arg.pu8[8], arg.pu8[9], arg.pu8[10], arg.pu8[11], arg.pu8[12], arg.pu8[13], arg.pu8[14], arg.pu8[15]);
                        realsize += radix;
                        curpos += radix;
                    }
                    break;
                }
#endif
                flags.is_signed = 0;
                radix = 16;
                width = 0;
                goto print_integer_do;
            case 'X':
                flags.is_upper = 1;
            case 'x':
                flags.is_signed = 0;
                radix = 16;
                goto print_integer;

            // TODO: support %llx etc
            print_integer:
                if (flags.is_signed) {
                    switch (rank) {
                    case rank_char:     arg.val = (unsigned long long)(signed long long)(signed char)va_arg(ap, signed int);    break;
                    case rank_short:    arg.val = (unsigned long long)(signed long long)(signed short)va_arg(ap, signed int);   break;
                    case rank_int:      arg.val = (unsigned long long)(signed long long)va_arg(ap, signed int);                 break;
                    case rank_long:     arg.val = (unsigned long long)(signed long long)va_arg(ap, signed long);                break;
                    case rank_longlong: arg.val = (unsigned long long)(signed long long)va_arg(ap, signed long long);           break;
                    }
                } else {
                    switch (rank) {
                    case rank_char:     arg.val = (unsigned long long)(unsigned char)va_arg(ap, unsigned int);                  break;
                    case rank_short:    arg.val = (unsigned long long)(unsigned short)va_arg(ap, unsigned int);                 break;
                    case rank_int:      arg.val = (unsigned long long)va_arg(ap, unsigned int);                                 break;
                    case rank_long:     arg.val = (unsigned long long)va_arg(ap, unsigned long);                                break;
                    case rank_longlong: arg.val = (unsigned long long)va_arg(ap, unsigned long long);                           break;
                    }
                }
                if (flags.has_prefix) {
                    EMIT('0');
                    EMIT(flags.is_upper ? 'X' : 'x');
                }

            print_integer_do: {
                    char integer_buf[32];
                    int pos = sizeof(integer_buf) - 1;
                    int cur_integer;

                    if (flags.is_signed && ((signed long long)arg.val < 0)) {
                        arg.val = (unsigned long long)(-(signed long long)arg.val);
                    } else {
                        flags.is_plus = 1;
                    }
                    if (arg.val != 0) {
                        while (arg.val != 0) {
                            cur_integer = arg.val % radix;
                            arg.val /= radix;
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
                            EMIT(flags.has_prefix0 ? '0' : ' ');
                        }
                    }
                    if (flags.has_plus_minus) {
                        EMIT(flags.is_plus ? '+' : '-');
                        actual_width--;
                    }
                    while (actual_width-- > 0) {
                        EMIT(integer_buf[pos++]);
                    }
                    if (flags.align_left) {
                        while (width-- > 0) {
                            EMIT(' ');
                        }
                    }
                }
#if VSF_SIMPLE_SPRINTF_SUPPORT_FLOAT == ENABLED
                if (flags.float_state) {
                    goto print_float;
                }
#endif
                break;
            case 'n':
                switch (rank) {
                case rank_char:     *va_arg(ap, signed char *) = realsize;      break;
                case rank_short:    *va_arg(ap, signed short *) = realsize;     break;
                case rank_int:      *va_arg(ap, signed int *) = realsize;       break;
                case rank_long:     *va_arg(ap, signed long *) = realsize;      break;
                case rank_longlong: *va_arg(ap, signed long long *) = realsize; break;
                }
                break;
#if VSF_SIMPLE_SPRINTF_SUPPORT_FLOAT == ENABLED
            case 'f':
            print_float:
                switch (flags.float_state) {
                case 0:
                    arg.d = va_arg(ap, double);

                print_float_do:
                    if (precision < 0) {
                        precision = 6;
                    }
                    d_fractpart = modf(arg.d, &d_intpart);
                    if (d_fractpart < 0) {
                        d_fractpart = -d_fractpart;
                    }
                    flags.is_signed = d_intpart < 0;

                    pow = 10;
                    for (int i = 0; i < precision;) {
                        int remain = precision - i;
                        if (remain >= 8) {
                            pow *= 100000000;
                            i += 8;
                        } else if (remain >= 4) {
                            pow *= 10000;
                            i += 4;
                        } else if (remain >= 2) {
                            pow *= 100;
                            i += 2;
                        } else if (remain >= 1) {
                            pow *= 10;
                            i += 1;
                        }
                    }
                    d_fractpart += 5 / pow;

                    if (d_fractpart >= 1) {
                        d_fractpart = modf(d_fractpart, &dtmp);
                        if (flags.is_signed) {
                            d_intpart -= 1;
                        } else {
                            d_intpart += 1;
                        }
                    }
                    d_fractpart *= pow / 10;
                    if (flags.is_g && d_fractpart != 0) {
                        while (precision && !(int)fmod(d_fractpart, 10)) {
                            d_fractpart /= 10;
                            precision--;
                        }
                    }

                    if (flags.is_signed) {
                        EMIT('-');
                        d_intpart *= -1;
                    } else if (flags.has_plus_minus) {
                        EMIT('+');
                    }
                    flags.has_plus_minus = 0;

                    flags.float_state = 1;
                    flags.is_signed = 0;
                    round = 1;
                    radix = 10;
                    while (d_intpart > __DROUND_FACTOR) {
                        round++;
                        d_intpart = __DROUND_FACTOR;
                    }

                    if ((width > 0) && (precision > 0)) {
                        if (width < precision + 1) {
                            width = 0;
                        } else {
                            width -= precision + 1;
                        }
                    }
                    fwidth = width;
                    intpart_pos = realsize;
                    width = 0;
                    // fall through
                case 1:
                    if (round-- > 0) {
                        arg.val = (unsigned long long)d_intpart;
                        d_intpart = modf(d_intpart, &dtmp);
                        d_intpart *= __DROUND_FACTOR;
                        goto print_integer_do;
                    }

                    {
                        int intpart_width_actual = realsize - intpart_pos;
                        int remain = size - intpart_pos;
                        fwidth -= intpart_width_actual;
                        if (!flags.align_left && (fwidth > 0)) {
                            realsize += fwidth;

                            if (remain > fwidth) {
                                int move_size = remain - fwidth;
                                move_size = vsf_min(move_size, intpart_width_actual);
                                memmove(&str[intpart_pos + fwidth], &str[intpart_pos], move_size);
                            }
                            if (remain > 0) {
                                int space_size = vsf_min(remain, fwidth);
                                memset(&str[intpart_pos], ' ', space_size);
                                curpos += space_size;
                            }
                            fwidth = 0;
                        }
                    }

                    if (0 == precision) {
                        goto print_float_end;
                    }
                    EMIT('.');

                    flags.float_state = 2;
                    flags.has_prefix0 = 1;
                    round = 1;
                    width = precision;
                    while (d_fractpart > __DROUND_FACTOR) {
                        round++;
                        d_fractpart /= __DROUND_FACTOR;
                        width -= __DROUND_DIGITS;
                    }
                    radix = 10;

                    arg.val = (unsigned long long)d_fractpart;
                    d_fractpart = modf(d_fractpart, &dtmp);
                    d_fractpart *= __DROUND_FACTOR;
                    flags.align_left = 0;
                    goto print_integer_do;
                case 2:
                    if (--round > 0) {
                        arg.val = (unsigned long long)d_fractpart;
                        d_fractpart = modf(d_fractpart, &dtmp);
                        d_fractpart *= __DROUND_FACTOR;
                        width = __DROUND_DIGITS;
                        goto print_integer_do;
                    }
                    while (fwidth-- > 0) {
                        EMIT(' ');
                    }
                    // fall through
                case 3:
                print_float_end:
                    if (flags.exp_state) {
                        goto print_exp;
                    }
                    break;
                }
                break;
            case 'E':
                flags.is_upper = 1;
                // fall through
            case 'e':
            print_exp:
                switch (flags.exp_state) {
                case 0:
                    dtmp = arg.d = va_arg(ap, double);
                print_exp_do:
                    d_fractpart = modf(arg.d, &d_intpart);
                    if (d_fractpart < 0) {
                        d_fractpart = -d_fractpart;
                    }

                    exp = 0;
                    if (d_intpart < 0) {
                        d_intpart = -d_intpart;
                    }
                    if (d_intpart >= 10) {
                        while (d_intpart >= 10) {
                            d_intpart /= 10;
                            arg.d /= 10;
                            exp++;
                        }
                    } else if (0 == d_fractpart) {
                        arg.d = 0;
                    } else if (0 == d_intpart) {
                        while (d_fractpart < 1) {
                            d_fractpart *= 10;
                            exp--;
                        }

                        arg.d = d_fractpart;
                    }

                    if (flags.is_g) {
                        if ((exp < -4) || (exp >= precision)) {
                            precision -= 1;
                        } else {
                            arg.d = dtmp;
                            precision -= exp + 1;
                            goto print_float_do;
                        }
                    }

                    flags.exp_state = 1;
                    goto print_float_do;
                case 1:
                    EMIT(flags.is_upper ? 'E' : 'e');
                    EMIT(exp >= 0 ? '+' : '-');
                    if (exp < 0) {
                        exp = -exp;
                    }
                    EMIT('0' + (exp / 10));
                    EMIT('0' + (exp % 10));
                    break;
                }
                break;
            case 'G':
                flags.is_upper = 1;
                // fall through
            case 'g':
                if (precision < 0) {
                    precision = 6;
                } else if (precision == 0) {
                    precision = 1;
                }
                flags.is_g = 1;
                goto print_exp;
#endif
            case 'c':
            case 'C':
                arg.ch = va_arg(ap, int);
                EMIT(arg.ch);
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
                if ((precision >= 0) && (width > precision)) {
                    width = precision;
                }
                if (!flags.align_left) {
                    while (actual_width < width) {
                        width--;
                        EMIT(' ');
                    }
                }
                while ((*arg.str != '\0') && (width-- > 0)) {
                    EMIT(*arg.str++);
                }
                if (flags.align_left) {
                    while (width-- > 0) {
                        EMIT(' ');
                    }
                }
                break;
            default:
                goto end;
            }
            break;
        default:
            EMIT(ch);
            break;
        }
    }
end:
    if (curpos != NULL) {
        *curpos = '\0';
    }
    return realsize;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
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
