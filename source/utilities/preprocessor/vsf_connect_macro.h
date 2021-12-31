/****************************************************************************
*  Copyright 2020 by Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)    *
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

#ifndef __CONNECT_H__
#define __CONNECT_H__

#include "../ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

#define __VSF_MCONNECT0()
#define __VSF_MCONNECT1( a)                                  a
#define __VSF_MCONNECT2( a, b)                               a##b
#define __VSF_MCONNECT3( a, b, c)                            a##b##c
#define __VSF_MCONNECT4( a, b, c, d)                         a##b##c##d
#define __VSF_MCONNECT5( a, b, c, d, e)                      a##b##c##d##e
#define __VSF_MCONNECT6( a, b, c, d, e, f)                   a##b##c##d##e##f
#define __VSF_MCONNECT7( a, b, c, d, e, f, g)                a##b##c##d##e##f##g
#define __VSF_MCONNECT8( a, b, c, d, e, f, g, h)             a##b##c##d##e##f##g##h
#define __VSF_MCONNECT9( a, b, c, d, e, f, g, h, i)          a##b##c##d##e##f##g##h##i
#define __VSF_MCONNECT10(a, b, c, d, e, f, g, h, i, j)       a##b##c##d##e##f##g##h##i##j
#define __VSF_MCONNECT11(a, b, c, d, e, f, g, h, i, j, k)                       \
            a##b##c##d##e##f##g##h##i##j##k
#define __VSF_MCONNECT12(a, b, c, d, e, f, g, h, i, j, k, l)                    \
            a##b##c##d##e##f##g##h##i##j##k##l
#define __VSF_MCONNECT13(a, b, c, d, e, f, g, h, i, j, k, l, m)                 \
            a##b##c##d##e##f##g##h##i##j##k##l##m
#define __VSF_MCONNECT14(a, b, c, d, e, f, g, h, i, j, k, l, m, n)              \
            a##b##c##d##e##f##g##h##i##j##k##l##m##n
#define __VSF_MCONNECT15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o)           \
            a##b##c##d##e##f##g##h##i##j##k##l##m##n##o
#define __VSF_MCONNECT16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)        \
            a##b##c##d##e##f##g##h##i##j##k##l##m##n##o##p
#define __VSF_MCONNECT17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q)     \
            a##b##c##d##e##f##g##h##i##j##k##l##m##n##o##p##q

#define VSF_MCONNECT0()                                  __VSF_MCONNECT1()
#define VSF_MCONNECT1( a )                               __VSF_MCONNECT1( a )
#define VSF_MCONNECT2( a, b)                             __VSF_MCONNECT2( a, b)
#define VSF_MCONNECT3( a, b, c)                          __VSF_MCONNECT3( a, b, c)
#define VSF_MCONNECT4( a, b, c, d)                       __VSF_MCONNECT4( a, b, c, d)
#define VSF_MCONNECT5( a, b, c, d, e)                    __VSF_MCONNECT5( a, b, c, d, e)
#define VSF_MCONNECT6( a, b, c, d, e, f)                 __VSF_MCONNECT6( a, b, c, d, e, f)
#define VSF_MCONNECT7( a, b, c, d, e, f, g)              __VSF_MCONNECT7( a, b, c, d, e, f, g)
#define VSF_MCONNECT8( a, b, c, d, e, f, g, h)           __VSF_MCONNECT8( a, b, c, d, e, f, g, h)
#define VSF_MCONNECT9( a, b, c, d, e, f, g, h, i)        __VSF_MCONNECT9( a, b, c, d, e, f, g, h, i)
#define VSF_MCONNECT10(a, b, c, d, e, f, g, h, i, j)     __VSF_MCONNECT10(a, b, c, d, e, f, g, h, i, j)
#define VSF_MCONNECT11(a, b, c, d, e, f, g, h, i, j, k)                         \
            __VSF_MCONNECT11(a, b, c, d, e, f, g, h, i, j, k)
#define VSF_MCONNECT12(a, b, c, d, e, f, g, h, i, j, k, l)                      \
            __VSF_MCONNECT12(a, b, c, d, e, f, g, h, i, j, k, l)
#define VSF_MCONNECT13(a, b, c, d, e, f, g, h, i, j, k, l, m)                   \
            __VSF_MCONNECT13(a, b, c, d, e, f, g, h, i, j, k, l, m)
#define VSF_MCONNECT14(a, b, c, d, e, f, g, h, i, j, k, l, m, n)                \
            __VSF_MCONNECT14(a, b, c, d, e, f, g, h, i, j, k, l, m, n)
#define VSF_MCONNECT15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o)             \
            __VSF_MCONNECT15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o)
#define VSF_MCONNECT16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)          \
            __VSF_MCONNECT16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)
#define VSF_MCONNECT17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q)       \
            __VSF_MCONNECT17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q)

#if defined(__cplusplus) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
//! overload VSF_MCONNECT
#   define VSF_MCONNECT(...)     __PLOOC_EVAL(VSF_MCONNECT, __VA_ARGS__)(__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif
