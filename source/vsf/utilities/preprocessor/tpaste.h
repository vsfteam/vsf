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
 * For example, writing TPASTE2(U, WIDTH) within a macro \#defined by
 * UTYPE(WIDTH) and invoked as UTYPE(UL_WIDTH) with UL_WIDTH \#defined as 32 is
 * equivalent to writing U32.
 */
//! @{
#define TPASTE2( a, b)                            a##b
#define TPASTE3( a, b, c)                         a##b##c
#define TPASTE4( a, b, c, d)                      a##b##c##d
#define TPASTE5( a, b, c, d, e)                   a##b##c##d##e
#define TPASTE6( a, b, c, d, e, f)                a##b##c##d##e##f
#define TPASTE7( a, b, c, d, e, f, g)             a##b##c##d##e##f##g
#define TPASTE8( a, b, c, d, e, f, g, h)          a##b##c##d##e##f##g##h
#define TPASTE9( a, b, c, d, e, f, g, h, i)       a##b##c##d##e##f##g##h##i
#define TPASTE10(a, b, c, d, e, f, g, h, i, j)    a##b##c##d##e##f##g##h##i##j
//! @}

/*! \name Absolute Token Paste
 *
 * Paste N preprocessing tokens together, these tokens being allowed to be \#defined.
 *
 * No restriction of use if the tokens are \#defined.
 *
 * For example, writing ATPASTE2(U, UL_WIDTH) anywhere with UL_WIDTH \#defined
 * as 32 is equivalent to writing U32.
 */
//! @{
#define ATPASTE2( a, b)                           TPASTE2( a, b)
#define ATPASTE3( a, b, c)                        TPASTE3( a, b, c)
#define ATPASTE4( a, b, c, d)                     TPASTE4( a, b, c, d)
#define ATPASTE5( a, b, c, d, e)                  TPASTE5( a, b, c, d, e)
#define ATPASTE6( a, b, c, d, e, f)               TPASTE6( a, b, c, d, e, f)
#define ATPASTE7( a, b, c, d, e, f, g)            TPASTE7( a, b, c, d, e, f, g)
#define ATPASTE8( a, b, c, d, e, f, g, h)         TPASTE8( a, b, c, d, e, f, g, h)
#define ATPASTE9( a, b, c, d, e, f, g, h, i)      TPASTE9( a, b, c, d, e, f, g, h, i)
#define ATPASTE10(a, b, c, d, e, f, g, h, i, j)   TPASTE10(a, b, c, d, e, f, g, h, i, j)
//! @}


#endif  // _TPASTE_H_
