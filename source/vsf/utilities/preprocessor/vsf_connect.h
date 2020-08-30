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

#ifndef __CONNECT_H__
#define __CONNECT_H__

#include "../ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

#define __CONNECT0()                                    
#define __CONNECT1( a)                                  a
#define __CONNECT2( a, b)                               a##b
#define __CONNECT3( a, b, c)                            a##b##c
#define __CONNECT4( a, b, c, d)                         a##b##c##d
#define __CONNECT5( a, b, c, d, e)                      a##b##c##d##e
#define __CONNECT6( a, b, c, d, e, f)                   a##b##c##d##e##f
#define __CONNECT7( a, b, c, d, e, f, g)                a##b##c##d##e##f##g
#define __CONNECT8( a, b, c, d, e, f, g, h)             a##b##c##d##e##f##g##h
#define __CONNECT9( a, b, c, d, e, f, g, h, i)          a##b##c##d##e##f##g##h##i
#define __CONNECT10(a, b, c, d, e, f, g, h, i, j)       a##b##c##d##e##f##g##h##i##j
#define __CONNECT11(a, b, c, d, e, f, g, h, i, j, k)                            \
            a##b##c##d##e##f##g##h##i##j##k
#define __CONNECT12(a, b, c, d, e, f, g, h, i, j, k, l)                         \
            a##b##c##d##e##f##g##h##i##j##k##l
#define __CONNECT13(a, b, c, d, e, f, g, h, i, j, k, l, m)                      \
            a##b##c##d##e##f##g##h##i##j##k##l##m
#define __CONNECT14(a, b, c, d, e, f, g, h, i, j, k, l, m, n)                   \
            a##b##c##d##e##f##g##h##i##j##k##l##m##n
#define __CONNECT15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o)                \
            a##b##c##d##e##f##g##h##i##j##k##l##m##n##o
#define __CONNECT16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)             \
            a##b##c##d##e##f##g##h##i##j##k##l##m##n##o##p
#define __CONNECT17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q)          \
            a##b##c##d##e##f##g##h##i##j##k##l##m##n##o##p##q

#define CONNECT0()                                  __CONNECT1()
#define CONNECT1( a )                               __CONNECT1( a )
#define CONNECT2( a, b)                             __CONNECT2( a, b)
#define CONNECT3( a, b, c)                          __CONNECT3( a, b, c)
#define CONNECT4( a, b, c, d)                       __CONNECT4( a, b, c, d)
#define CONNECT5( a, b, c, d, e)                    __CONNECT5( a, b, c, d, e)
#define CONNECT6( a, b, c, d, e, f)                 __CONNECT6( a, b, c, d, e, f)
#define CONNECT7( a, b, c, d, e, f, g)              __CONNECT7( a, b, c, d, e, f, g)
#define CONNECT8( a, b, c, d, e, f, g, h)           __CONNECT8( a, b, c, d, e, f, g, h)
#define CONNECT9( a, b, c, d, e, f, g, h, i)        __CONNECT9( a, b, c, d, e, f, g, h, i)
#define CONNECT10(a, b, c, d, e, f, g, h, i, j)     __CONNECT10(a, b, c, d, e, f, g, h, i, j)
#define CONNECT11(a, b, c, d, e, f, g, h, i, j, k)                              \
            __CONNECT11(a, b, c, d, e, f, g, h, i, j, k)
#define CONNECT12(a, b, c, d, e, f, g, h, i, j, k, l)                           \
            __CONNECT12(a, b, c, d, e, f, g, h, i, j, k, l)
#define CONNECT13(a, b, c, d, e, f, g, h, i, j, k, l, m)                        \
            __CONNECT13(a, b, c, d, e, f, g, h, i, j, k, l, m)
#define CONNECT14(a, b, c, d, e, f, g, h, i, j, k, l, m, n)                     \
            __CONNECT14(a, b, c, d, e, f, g, h, i, j, k, l, m, n)
#define CONNECT15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o)                  \
            __CONNECT15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o)
#define CONNECT16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)               \
            __CONNECT16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)
#define CONNECT17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q)            \
            __CONNECT17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q)

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
//! overload connect
#   define CONNECT(...)     __PLOOC_EVAL(CONNECT, __VA_ARGS__)(__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif 
