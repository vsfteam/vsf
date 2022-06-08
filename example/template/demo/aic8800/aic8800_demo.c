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

#define __VSF_HEAP_CLASS_INHERIT__
#include "vsf.h"

#ifdef __AIC8800__

#include "rtos_al.h"
#include "sysctrl_api.h"
#include "psram_api.h"
#include "dbg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_USB_HOST
// redefine usbh memory allocation, memory MUST be in 0x001A0000 - 0x001C7FFF
struct __usbh_heap_t {
    implement(vsf_heap_t)
    uint8_t memory[32 * 1024];
    // one more as terminator
    vsf_dlist_t freelist[2];
} static __usbh_heap;

static vsf_dlist_t * __usbh_heap_get_freelist(uint_fast32_t size)
{
    return &__usbh_heap.freelist[0];
}

static void __usbh_heap_init(void)
{
    memset(&__usbh_heap.use_as__vsf_heap_t, 0, sizeof(__usbh_heap.use_as__vsf_heap_t));
    for (uint_fast8_t i = 0; i < dimof(__usbh_heap.freelist); i++) {
        vsf_dlist_init(&__usbh_heap.freelist[i]);
    }
    __usbh_heap.get_freelist = __usbh_heap_get_freelist;
    __vsf_heap_add_buffer(&__usbh_heap.use_as__vsf_heap_t, __usbh_heap.memory, sizeof(__usbh_heap.memory));
}

void * __vsf_usbh_malloc_aligned(uint_fast32_t size, uint_fast32_t alignment)
{
    return __vsf_heap_malloc_aligned(&__usbh_heap.use_as__vsf_heap_t, size, alignment);
}

void * __vsf_usbh_malloc(uint_fast32_t size)
{
    return __vsf_heap_malloc_aligned(&__usbh_heap.use_as__vsf_heap_t, size, 0);
}

void __vsf_usbh_free(void *buffer)
{
    __vsf_heap_free(&__usbh_heap.use_as__vsf_heap_t, buffer);
}
#endif

void aic8800_demo_init(void)
{
    dbg("\r\n    dsp_clock: %dM, sys_clock: %dM, pclk: %dM, flash_clock: %dM\r\n",
            DSPSysCoreClock / 1000000, SystemCoreClock / 1000000,
            PeripheralClock / 1000000, sysctrl_clock_get(PER_FLASH) / 1000000);

    psram_init();
    uint32_t ram_size = psram_size_get();
    if (ram_size > 0) {
        dbg("    psram: %dM bytes\r\n", ram_size / (1024 * 1024));
    }

#if VSF_USE_USB_HOST
    __usbh_heap_init();
#endif

    // currently known dependency on rtos_al: lwip from vendor
    if (rtos_init()) {
        VSF_HAL_ASSERT(false);
    }

#if AIC8800_APP_USE_WIFI_DEMO == ENABLED
    extern void aic8800_wifi_start(void);
    aic8800_wifi_start();
#endif
#if AIC8800_APP_USE_BT_DEMO == ENABLED && APP_USE_BTSTACK_DEMO == ENABLED
    extern vsf_err_t aic8800_bt_start(void);
    aic8800_bt_start();
#endif
}

#endif      // __AIC8800__

/* EOF */
