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

#ifndef __HAL_DRIVER_NUVOTON_M480_COMMON_H__
#define __HAL_DRIVER_NUVOTON_M480_COMMON_H__

/* \note __common.h should only be included by device.h */

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "./vendor/Include/M480.h"
#include "hal/arch/vsf_arch.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

//typedef void vsf_swi_handler_t(void *p);

enum m480_usbphy_t {
    M480_USBPHY_FS      = 0,
    M480_USBPHY_HS      = 16,
};
typedef enum m480_usbphy_t m480_usbphy_t;

enum m480_usbphy_role_t {
    M480_USBPHY_DEV     = 0,
    M480_USBPHY_HOST    = 1,
    M480_USBPHY_OTG     = 2,
};
typedef enum m480_usbphy_role_t m480_usbphy_role_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
extern bool m480_reg_unlock(void);
extern void m480_reg_lock(bool islocked);

extern uint_fast32_t m480_bit_field_get(uint_fast16_t bf, uint32_t *ptr);
extern void m480_bit_field_set(uint_fast16_t bf, uint32_t *ptr, uint_fast32_t value);
extern void m480_bit_field_set_atom(uint_fast16_t bf, uint32_t *ptr, uint_fast32_t value);
extern void m480_bit_field_set_wprotect(uint_fast16_t bf, uint32_t *ptr, uint_fast32_t value);

extern void m480_enable_usbphy(m480_usbphy_t phy, m480_usbphy_role_t role);
extern void m480_disable_usbphy(m480_usbphy_t phy);

/* todo: should remove this but keep it for a while
extern vsf_err_t vsf_drv_swi_init(uint_fast8_t idx, vsf_arch_prio_t priority,
                            vsf_swi_handler_t *handler, void *pparam);
extern void vsf_drv_swi_trigger(uint_fast8_t idx);
*/
#endif
/* EOF */
