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

#ifndef __DEFAULT_COMPILER_DETECT_H__
#define __DEFAULT_COMPILER_DETECT_H__

//! default compiler supports only gcc/clang

//! \name The macros to identify the compiler
//! @{

#ifdef __IS_COMPILER_LLVM__
#   undef  __IS_COMPILER_LLVM__
#endif
#if defined(__clang__)
#   define __IS_COMPILER_LLVM__                1
#else
//! \note for gcc
#ifdef __IS_COMPILER_GCC__
#   undef __IS_COMPILER_GCC__
#endif
#if defined(__GNUC__) && !__IS_COMPILER_LLVM__
#   define __IS_COMPILER_GCC__                 1
#else
//! \note for tcc
#ifdef __IS_COMPILER_TCC__
#   undef __IS_COMPILER_TCC__
#endif
#if defined(__TINYC__)
#   define __IS_COMPILER_TCC__                 1
#endif
#endif
#endif
//! @}

#endif      // __DEFAULT_COMPILER_DETECT_H__
