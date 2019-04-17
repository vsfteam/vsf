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

#ifndef _USE_COMPILER_H_
#define _USE_COMPILER_H_            


#include "./type.h"
#include "./preprocessor/mrepeat.h"
     
//! \brief CPU io

#if     defined(__CPU_AVR__)                    //!< Atmel AVR series
#   include "./compiler/avr/avr_compiler.h"
#elif   defined(__CPU_ARM__)                //!< ARM series
#   include "./compiler/arm/arm_compiler.h"
#else
//#warning No specified MCU type! use arm as default
#   include "./compiler/arm/arm_compiler.h"
#endif

#endif
