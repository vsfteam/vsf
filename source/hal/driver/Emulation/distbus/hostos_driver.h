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

#if     defined(__WIN__)
#   if  defined(__CPU_X86__) || defined(__CPU_X64__)

#       define VSF_HOSTOS_USART_CFG_TX_BLOCK            ENABLED
#       define VSF_HOSTOS_USART_PREFIX                  __vsf_hw
#       define VSF_HOSTOS_USART_UPCASE_PREFIX           __VSF_HW
#       define VSF_HOSTOS_RTC_PREFIX                    __vsf_hw
#       define VSF_HOSTOS_RTC_UPCASE_PREFIX             __VSF_HW
#       define VSF_HOSTOS_RNG_PREFIX                    __vsf_hw
#       define VSF_HOSTOS_RNG_UPCASE_PREFIX             __VSF_HW
#	    define __VSF_HW_USART_COUNT						32

#       include "hal/driver/x86/driver.h"

#   else
#       error hostos not supported
#   endif
//#elif   defined(__LINUX__)
//#elif   defined(__MACOS__)
#else
#   error hostos not supported
#endif
