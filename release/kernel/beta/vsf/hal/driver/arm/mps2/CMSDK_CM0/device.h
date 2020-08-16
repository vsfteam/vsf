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

#ifndef __HAL_DEVICE_ARM_CMSDK_CM0_H__
#define __HAL_DEVICE_ARM_CMSDK_CM0_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
//#include "CMSDK_CM0.h"
/*============================ MACROS ========================================*/

/*\note first define basic info for arch. */
#if defined(__VSF_HEADER_ONLY_SHOW_ARCH_INFO__)
#   undef __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
#endif
//! arch info
#   define VSF_ARCH_PRI_NUM         4
#   define VSF_ARCH_PRI_BIT         2


// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_NUM             8
#define VSF_DEV_SWI_LIST            24,25,26,27,28,29,30,31

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where __common.h is included.*/
#include "../common/__common.h"

/*============================ MACROS ========================================*/

#define __def_idx(__name, __no)     __CONNECT2(__name, _idx) = (__no)
#define __def_msk(__name)           __CONNECT2(__name, _msk) = _BV(__CONNECT2(__name, _idx) & 0x1F)

#define GPIO_COUNT                  1

#define PIO_PORTA
#define PIO_PORTA_PIN_NUM           7
/*
#define PIO_PORTB
#define PIO_PORTB_PIN_NUM           16

#define PIO_PORTC
#define PIO_PORTC_PIN_NUM           16

#define PIO_PORTD
#define PIO_PORTD_PIN_NUM           16


#define PIO_PORTE
#define PIO_PORTF
#define PIO_PORTG
#define PIO_PORTH
*/

/*============================ MACROFIED FUNCTIONS ===========================*/

// bit0 - bit13:    clksrc bitfield
// bit14- bit27:    clkdiv bitfield
// bit28- bit31:    clkdiv_remap
#define __def_periph_clk(__name, __bf_clksel, __bf_clkdiv, __clksel_map_idx)    \
        __CONNECT2(__name, _idx) = ((__bf_clksel) << 0)                            \
                            |   ((__bf_clkdiv) << 14)                           \
                            |   ((__clksel_map_idx) << 28)

#define __def_sync_clk_idx(__name, __bus_idx, __bit_idx)                          \
            __CONNECT2(__name, _idx) = ((__bit_idx) << 0) | ((__bus_idx) << 5)

#define __def_clk_src(__name, __value)      __name = (__value)

/*============================ TYPES =========================================*/

#if 0
//! \name power set index
//! @{
enum pm_power_cfg_no_t{
    __def_idx(POWER_DUMMY, 0),
};
//! @}

//! \name power set mask
//! @{
enum pm_power_cfg_msk_t {
    __def_msk(POWER_DUMMY),
} ;
//! @}

//! \name the lowpower mode
//! @{
enum pm_sleep_mode_t{
    PM_WAIT         = 0,
    PM_SLEEP,
    PM_DEEP_SLEEP,
    PM_POWER_OFF,
} ;
//! @}

enum pm_periph_clksrc_t {
    ...,
};
typedef enum pm_periph_clksrc_t pm_periph_clksrc_t;

enum pm_periph_clksel_t {
    ...
};
typedef enum pm_periph_clksel_t pm_periph_clksel_t;

//! \name peripheral clock index
//! @{
enum pm_periph_async_clk_no_t{
    ...
};
//! @}

//! \name Peripheral AHB Clock Macros
//! @{
enum pm_sync_clk_no_t { 
                        // NAME         BUS_IDX,BIT_IDX
    // AHB
    __def_sync_clk_idx(   SyncCLK_UART0,   1,      16  ),
    __def_sync_clk_idx(   SyncCLK_UART1,   1,      17  ),
    __def_sync_clk_idx(   SyncCLK_UART2,   1,      18  ),
    __def_sync_clk_idx(   SyncCLK_UART3,   1,      19  ),
};

enum pm_sync_clk_msk_t { 
    __def_msk(SyncCLK_UART0),
    __def_msk(SyncCLK_UART1),
    __def_msk(SyncCLK_UART2),
    __def_msk(SyncCLK_UART3),
};
//! @}

//! @{
enum pm_clk_src_sel_t {
    ...
};
//! @}

enum pm_pll_sel_t {
    PLL0_idx,
};

struct io_wakeup_cfg_t {
    uint32_t dummy;
};
typedef struct io_wakeup_cfg_t io_wakeup_cfg_t;

typedef enum io_port_no_t io_port_no_t;
typedef enum pm_clk_src_sel_t pm_clk_src_sel_t;

def_interface( i_pm_wakeup_t )
    struct {
        vsf_err_t (*Enable)(io_wakeup_cfg_t *pcfg, uint_fast8_t size);
        vsf_err_t (*Disable)(io_port_no_t port, uint_fast32_t msk);
    }UseIO;
end_def_interface( i_pm_wakeup_t )

struct pm_periph_asyn_clk_cfg_t {
    pm_clk_src_sel_t    clk_src;
    uint16_t            div;
};

//! \name main clock config sturct
//! @{
struct pm_main_clk_cfg_t {
    pm_clk_src_sel_t    clk_src;                //!< main clock source
    uint32_t            freq;                   //!< system oscilator frequency
    uint16_t            core_div[1];            //!< system core clock divider
    uint16_t            ahb_div[1];             //!< system AHB clock divider
    uint16_t            apb_div[2];             //!< system APB clock divider
};
//! @}
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
