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

#include "hal/driver/driver.h"

#ifdef VSF_VENDOR_DRIVER_HEADER
/* include vendor specified device driver header file */
#   include VSF_VENDOR_DRIVER_HEADER
#else
#   define __VSF_HAL_SHOW_VENDOR_INFO__
#   include "hal/driver/driver.h"
#   undef __VSF_HAL_SHOW_VENDOR_INFO__

#   ifndef __VSF_HEADER_ONLY_SHOW_VENDOR_INFO__
#       warning "The driver does not provide a header file of the vendor driver, \
nor does it provide vendor information"
#   endif
#endif

/* EOF */
