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
#ifndef __VSF_DISP_SOLOMON_SYSTECH_COMMON_H__
#define __VSF_DISP_SOLOMON_SYSTECH_COMMON_H__

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if     VSF_USE_UI == ENABLED                                                   \
    &&  (   (VSF_DISP_USE_SSD1306 == ENABLED)                                   \
        ||  (VSF_DISP_USE_SSD1316 == ENABLED))

#include "osa_hal/driver/customised/multiplex_hal/iic/vsf_multiplex_iic.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_I(__INSTR)                           \
            0x80, (__INSTR)
#define VSF_DISP_SOLOMON_SYSTECH_IIC_WRITE_D(__DATA)                            \
            0xC0, (__DATA)

/*============================ TYPES =========================================*/

typedef struct vsf_disp_solomon_systech_hw_iic_t {
    void *port;
} vsf_disp_solomon_systech_hw_iic_t;

typedef struct vsf_disp_solomon_systech_hw_spi_t {
    void *port;
    struct {
        vsf_gpio_t *port;
        int8_t pin;         // -1 for unused
    } cs;
    struct {
        vsf_gpio_t *port;
        uint8_t pin;
    } dc;
} vsf_disp_solomon_systech_hw_spi_t;

typedef struct vsf_disp_solomon_systech_hw_ebi_t {
    void *addr;
    struct {
        vsf_gpio_t *port;
        int8_t pin;         // -1 for unused
    } cs;
    struct {
        vsf_gpio_t *port;
        uint8_t pin;
    } dc;
} vsf_disp_solomon_systech_hw_ebi_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_UI
#endif  // __VSF_DISP_SOLOMON_SYSTECH_COMMON_H__
