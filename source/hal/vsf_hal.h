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

#ifdef __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

#   include "./driver/driver.h"
#   undef __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

#else

#   ifndef __VSF_HAL_H__
#   define __VSF_HAL_H__

#   include "hal/vsf_hal_cfg.h"
#   include "./arch/vsf_arch.h"
#   include "./driver/driver.h"
// DO NOT INCLUDE COMMON.h, so that use can use their own hal API standard
//#   include "./driver/common/common.h"

#   include "./utilities/io_mapper/vsf_io_mapper.h"
#   include "./utilities/stream/vsf_hal_stream.h"
#   include "./utilities/remap/vsf_hal_remap.h"

#   if VSF_HAL_USE_I2C == ENABLED
#       include "./utilities/i2c_util/vsf_i2c_util.h"
#   endif

#   ifdef __cplusplus
extern "C" {
#   endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_HAL_IRQ(__IRQHANDLER, ...)                                          \
    void __IRQHANDLER(void)                                                     \
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        do {                                                                    \
            __VA_ARGS__                                                         \
        } while (0);                                                            \
        vsf_hal_irq_leave(ctx);                                                 \
    }

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*! \note initialize level 0/1 hardware abstract layer
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
extern bool vsf_hal_init(void);

extern uintptr_t vsf_hal_irq_enter(void);

extern void vsf_hal_irq_leave(uintptr_t ctx);

/*! \note initialize level 2 hardware abstract layer
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
extern bool vsf_osa_hal_init(void);

#   ifdef __cplusplus
}
#   endif

#endif      // __VSF_HAL_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
