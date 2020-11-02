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
#ifndef __VSF_DISP_SITRONIX_COMMON_H__
#define __VSF_DISP_SITRONIX_COMMON_H__

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if     VSF_USE_UI == ENABLED                                                   \
    &&  (   (VSF_DISP_USE_ST7789 == ENABLED)                                    \
        ||  (VSF_DISP_USE_ST7735 == ENABLED))

#if     defined(__VSF_DISP_SITRONIX_CLASS_IMPLEMENT)
#   undef __VSF_DISP_SITRONIX_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSF_DISP_SITRONIX_CLASS_INHERIT__)
#   undef __VSF_DISP_SITRONIX_CLASS_INHERIT__
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define VSF_DISP_SITRONIX_PORT_SPI                      0
#define VSF_DISP_SITRONIX_PORT_EBI                      1

#ifndef VSF_DISP_SITRONIX_CFG_PORT
#   define VSF_DISP_SITRONIX_CFG_PORT                   VSF_DISP_SITRONIX_PORT_SPI
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_DISP_SITRONIX_WRITE(__CMD, __PARAM_LEN, ...)                        \
            (__CMD), (__PARAM_LEN), ##__VA_ARGS__

/*============================ TYPES =========================================*/

declare_simple_class(vk_disp_sitronix_t)

def_simple_class(vk_disp_sitronix_t) {
    public_member(
        implement(vk_disp_t)
        struct {
#if VSF_DISP_SITRONIX_CFG_PORT == VSF_DISP_SITRONIX_PORT_SPI
            void * port;
#else
            uint32_t * addr;
#endif
            void * gpio;
            uint8_t cs_pin;
            uint8_t dc_pin;
            uint8_t reset_pin;
        } hw;
    )
    private_member(
        struct {
            struct {
                uint8_t *param;
                uint16_t param_len;
                uint8_t cmd;
                uint8_t state;
            } reg;
            struct {
                uint8_t *seq;
                uint16_t seq_len;
            } seq;
        } ctx;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void vk_disp_sitronix_init(vk_disp_sitronix_t *disp_sitronix);
extern void vk_disp_sitronix_reset(vk_disp_sitronix_t *disp_sitronix, bool reset);
extern vsf_err_t vk_disp_sitronix_write(vk_disp_sitronix_t *disp_sitronix, uint8_t cmd, uint8_t *param, uint_fast16_t param_len);
extern vsf_err_t vk_disp_sitronix_seq(vk_disp_sitronix_t *disp_sitronix, uint8_t *seq, uint_fast16_t seq_len);

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_UI
#endif  // __VSF_DISP_SITRONIX_COMMON_H__
