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

#ifndef _TPASTE_H_
#define _TPASTE_H_


/*! \name Token Paste
 *
 * Paste N preprocessing tokens together, these tokens being allowed to be \#defined.
 *
 * May be used only within macros with the tokens passed as arguments if the tokens are \#defined.
 *
 * For example, writing __CONNECT2(U, WIDTH) within a macro \#defined by
 * UTYPE(WIDTH) and invoked as UTYPE(UL_WIDTH) with UL_WIDTH \#defined as 32 is
 * equivalent to writing U32.
 */
//! @{
#define __CONNECT2( a, b)                            a##b
#define __CONNECT3( a, b, c)                         a##b##c
#define __CONNECT4( a, b, c, d)                      a##b##c##d
#define __CONNECT5( a, b, c, d, e)                   a##b##c##d##e
#define __CONNECT6( a, b, c, d, e, f)                a##b##c##d##e##f
#define __CONNECT7( a, b, c, d, e, f, g)             a##b##c##d##e##f##g
#define __CONNECT8( a, b, c, d, e, f, g, h)          a##b##c##d##e##f##g##h
#define __CONNECT9( a, b, c, d, e, f, g, h, i)       a##b##c##d##e##f##g##h##i
#define __CONNECT10(a, b, c, d, e, f, g, h, i, j)    a##b##c##d##e##f##g##h##i##j
//! @}

/*! \name Absolute Token Paste
 *
 * Paste N preprocessing tokens together, these tokens being allowed to be \#defined.
 *
 * No restriction of use if the tokens are \#defined.
 *
 * For example, writing CONNECT2(U, UL_WIDTH) anywhere with UL_WIDTH \#defined
 * as 32 is equivalent to writing U32.
 */
//! @{
#define CONNECT2( a, b)                           __CONNECT2( a, b)
#define CONNECT3( a, b, c)                        __CONNECT3( a, b, c)
#define CONNECT4( a, b, c, d)                     __CONNECT4( a, b, c, d)
#define CONNECT5( a, b, c, d, e)                  __CONNECT5( a, b, c, d, e)
#define CONNECT6( a, b, c, d, e, f)               __CONNECT6( a, b, c, d, e, f)
#define CONNECT7( a, b, c, d, e, f, g)            __CONNECT7( a, b, c, d, e, f, g)
#define CONNECT8( a, b, c, d, e, f, g, h)         __CONNECT8( a, b, c, d, e, f, g, h)
#define CONNECT9( a, b, c, d, e, f, g, h, i)      __CONNECT9( a, b, c, d, e, f, g, h, i)
#define CONNECT10(a, b, c, d, e, f, g, h, i, j)   __CONNECT10(a, b, c, d, e, f, g, h, i, j)
//! @}


#endif  // _TPASTE_H_
