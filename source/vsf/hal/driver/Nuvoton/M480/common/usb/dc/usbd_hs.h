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

#ifndef __HAL_DRIVER_NUVOTON_M484_USBD_HS_H__
#define __HAL_DRIVER_NUVOTON_M484_USBD_HS_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "../../../device.h"

#include "../../io/io.h"
#include "../../pm/pm.h"

#include "hal/interface/vsf_interface_io.h"
#include "hal/interface/vsf_interface_pm.h"
#include "hal/interface/vsf_interface_usb.h"

/*============================ MACROS ========================================*/

#define m480_usbd_hs_ep_number          14
#define m480_usbd_hs_ep_is_dma          false

#define M480_USBD_HS_WROKAROUND_ISO

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct m480_usbd_hs_const_t {
    m480_usbphy_t phy;
    IRQn_Type irq;
    pm_ahb_clk_no_t ahbclk;
    HSUSBD_T *reg;
};
typedef struct m480_usbd_hs_const_t m480_usbd_hs_const_t;

struct m480_usbd_hs_t {
    uint8_t index;
    uint16_t ep_buf_ptr;
#ifdef M480_USBD_HS_WROKAROUND_ISO
    uint16_t ep_tx_mask;
    uint16_t tx_size[m480_usbd_hs_ep_number - 2];
    uint8_t retry_cnt[m480_usbd_hs_ep_number - 2];
#endif

    struct {
        usb_dc_evt_handler_t evt_handler;
        void *param;
    } callback;
    const m480_usbd_hs_const_t *param;
};
typedef struct m480_usbd_hs_t m480_usbd_hs_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t m480_usbd_hs_init(m480_usbd_hs_t *usbd_hs, usb_dc_cfg_t *cfg);
extern void m480_usbd_hs_fini(m480_usbd_hs_t *usbd_hs);
extern void m480_usbd_hs_reset(m480_usbd_hs_t *usbd_hs);

extern void m480_usbd_hs_connect(m480_usbd_hs_t *usbd_hs);
extern void m480_usbd_hs_disconnect(m480_usbd_hs_t *usbd_hs);
extern void m480_usbd_hs_wakeup(m480_usbd_hs_t *usbd_hs);

extern void m480_usbd_hs_set_address(m480_usbd_hs_t *usbd_hs, uint_fast8_t addr);
extern uint_fast8_t m480_usbd_hs_get_address(m480_usbd_hs_t *usbd_hs);

extern uint_fast16_t m480_usbd_hs_get_frame_number(m480_usbd_hs_t *usbd_hs);
extern uint_fast8_t m480_usbd_hs_get_mframe_number(m480_usbd_hs_t *usbd_hs);

extern void m480_usbd_hs_get_setup(m480_usbd_hs_t *usbd_hs, uint8_t *buffer);
extern void m480_usbd_hs_status_stage(m480_usbd_hs_t *usbd_hs, bool is_in);

extern vsf_err_t m480_usbd_hs_ep_add(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);
extern uint_fast16_t m480_usbd_hs_ep_get_size(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep);

extern vsf_err_t m480_usbd_hs_ep_set_stall(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep);
extern bool m480_usbd_hs_ep_is_stalled(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep);
extern vsf_err_t m480_usbd_hs_ep_clear_stall(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep);

extern uint_fast16_t m480_usbd_hs_ep_get_data_size(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep);
extern vsf_err_t m480_usbd_hs_ep_read_buffer(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern vsf_err_t m480_usbd_hs_ep_enable_OUT(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep);

extern vsf_err_t m480_usbd_hs_ep_set_data_size(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep, uint_fast16_t size);
extern vsf_err_t m480_usbd_hs_ep_write_buffer(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);

extern void m480_usbd_hs_irq(m480_usbd_hs_t *usbd_hs);

#endif
/* EOF */
