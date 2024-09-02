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

//#ifndef __USE_COMPILER_H__
//#define __USE_COMPILER_H__

//! \brief CPU io

// extract arch info only to include different compiler header
#define __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
#include "hal/arch/vsf_arch.h"

#if   defined(__CPU_ARM__)                                                      //!< ARM series
#   include "./arm/arm_compiler.h"
#elif   defined(__CPU_MCS51__)                                                  //!< MCS-51 series
#   include "./mcs51/mcs51_compiler.h"
#elif   defined(__CPU_X86__) || defined(__CPU_X64__) || defined(__CPU_WEBASSEMBLY__)
#   include "./x86/x86_compiler.h"
#elif   defined(__CPU_RV__) || defined(__CPU_RISCV__)
#   include "./rv/rv_compiler.h"
#else
//#warning No specified MCU type! use default which support gcc/clang only
#   include "./default/default_compiler.h"
#endif

#ifndef __VSF_HEADER_ONLY_SHOW_COMPILER_INFO__
#   include "../preprocessor/vsf_preprocessor.h"

#   ifndef VSF_APPLET_USE_COMPILER
#       define VSF_APPLET_USE_COMPILER              ENABLED
#   endif
#   if VSF_APPLET_USE_COMPILER == ENABLED && !VSF_APPLET_COMPILER_VPLT_DEFINED
#       define VSF_APPLET_COMPILER_VPLT_DEFINED     1
// APIs for compilers:
//  GCC, refer to: https://gcc.gnu.org/onlinedocs/gccint/Integer-library-routines.html
typedef struct vsf_compiler_vplt_t {
    vsf_vplt_info_t info;

    // Routines for integer arithmetic
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__ashlsi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__ashldi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__ashlti3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__ashrsi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__ashrdi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__ashrti3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__divsi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__divdi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__divti3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__lshrsi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__lshrdi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__lshrti3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__modsi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__moddi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__modti3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__mulsi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__muldi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__multi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__negdi2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__negti2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__udivsi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__udivdi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__udivti3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__udivmoddi4);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__udivmodti4);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__umodsi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__umoddi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__umodti3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__cmpdi2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__cmpti2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__ucmpdi2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__ucmpti2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__absvsi2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__absvdi2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__addvsi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__addvdi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__mulvsi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__mulvdi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__negvsi2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__negvdi2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__subvsi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__subvdi3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__clzsi2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__clzdi2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__clzti2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__ctzsi2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__ctzdi2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__ctzti2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__ffsdi2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__ffsti2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__paritysi2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__paritydi2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__parityti2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__popcountsi2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__popcountdi2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__popcountti2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__bswapsi2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__bswapdi2);

    // Routines for floating point emulation
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__addsf3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__adddf3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__addtf3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__addxf3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__subsf3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__subdf3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__subtf3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__subxf3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__mulsf3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__muldf3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__multf3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__mulxf3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__divsf3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__divdf3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__divtf3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__divxf3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__negsf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__negdf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__negtf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__negxf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__extendsfdf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__extendsftf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__extendsfxf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__extenddftf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__extenddfxf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__truncxfdf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__trunctfdf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__truncxfsf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__trunctfsf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__truncdfsf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixsfsi);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixdfsi);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixtfsi);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixxfsi);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixsfdi);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixdfdi);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixtfdi);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixxfdi);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixsfti);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixdfti);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixtfti);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixxfti);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixunssfsi);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixunsdfsi);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixunstfsi);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixunsxfsi);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixunssfdi);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixunsdfdi);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixunstfdi);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixunsxfdi);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixunssfti);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixunsdfti);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixunstfti);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixunsxfti);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatsisf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatsidf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatsitf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatsixf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatdisf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatdidf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatditf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatdixf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floattisf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floattidf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floattitf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floattixf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatunsisf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatunsidf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatunsitf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatunsixf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatundisf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatundidf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatunditf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatundixf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatuntisf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatuntidf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatuntitf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatuntixf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixsfbitint);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixdfbitint);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixxfbitint);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__fixtfbitint);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatbitintsf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatbitintdf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatbitintxf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatbitinttf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatbitinthf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__floatbitintbf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__cmpsf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__cmpdf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__cmptf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__unordsf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__unorddf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__unordtf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__eqsf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__eqdf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__eqtf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__nesf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__nedf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__netf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__gesf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__gedf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__getf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__ltsf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__ltdf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__lttf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__lesf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__ledf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__letf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__gtsf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__gtdf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__gttf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__powisf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__powidf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__powitf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__powixf2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__mulsc3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__muldc3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__multc3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__mulxc3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__divsc3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__divdc3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__divtc3);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__divxc3);
} vsf_compiler_vplt_t;
#       ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_compiler_vplt_t vsf_compiler_vplt;
#       endif
#   endif
#else
#   undef __VSF_HEADER_ONLY_SHOW_COMPILER_INFO__
#endif

#ifdef __VSF_CPP__
// vsf_compiler_cpp_startup returns -1 if not implemented,
//  otherwise, it returns 0.
#ifdef __cplusplus
extern "C" {
#endif
extern int vsf_compiler_cpp_startup(void);
#ifdef __cplusplus
}
#endif
#endif

//#endif
