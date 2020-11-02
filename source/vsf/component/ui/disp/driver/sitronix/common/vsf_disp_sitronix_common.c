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

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if     VSF_USE_UI == ENABLED                                                   \
    &&  (   (VSF_DISP_USE_ST7789 == ENABLED)                                    \
        ||  (VSF_DISP_USE_ST7735 == ENABLED))

#define __VSF_DISP_SITRONIX_CLASS_IMPLEMENT
#include "../../vsf_disp.h"
#include "./vsf_disp_sitronix_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

static void __vk_disp_sitronix_write(uintptr_t target, vsf_evt_t evt)
{
    vk_disp_sitronix_t *disp_sitronix = (vk_disp_sitronix_t *)target;
    vsf_gpio_t *gpio = disp_sitronix->hw.gpio;

    switch (evt) {
    case VSF_EVT_INIT:
    case VSF_EVT_RETURN:
        switch (disp_sitronix->ctx.reg.state++) {
        case 0:
            vsf_gpio_clear(gpio, 1 << disp_sitronix->hw.cs_pin);
            vsf_gpio_clear(gpio, 1 << disp_sitronix->hw.dc_pin);
            vsf_multiplex_spi_transact(disp_sitronix->hw.port, &disp_sitronix->ctx.reg.cmd, NULL, 1);
            break;
        case 1:
            if (disp_sitronix->ctx.reg.param != NULL) {
                vsf_gpio_set(gpio, 1 << disp_sitronix->hw.dc_pin);
                vsf_multiplex_spi_transact(disp_sitronix->hw.port, disp_sitronix->ctx.reg.param, NULL, disp_sitronix->ctx.reg.param_len);
                break;
            }
            // fall through
        case 2:
            vsf_gpio_set(gpio, 1 << disp_sitronix->hw.cs_pin);
            vsf_eda_return();
            break;
        }
        break;
    }
}

void vk_disp_sitronix_init(vk_disp_sitronix_t *disp_sitronix)
{
    VSF_UI_ASSERT(disp_sitronix != NULL);
    vsf_gpio_t *gpio = disp_sitronix->hw.gpio;

    vsf_gpio_set(gpio, 1 << disp_sitronix->hw.cs_pin);
    vsf_gpio_config_pin(gpio, 1 << disp_sitronix->hw.cs_pin, IO_OUTPUT_PP);
    vsf_gpio_config_pin(gpio, 1 << disp_sitronix->hw.dc_pin, IO_OUTPUT_PP);
    vsf_gpio_config_pin(gpio, 1 << disp_sitronix->hw.reset_pin, IO_OUTPUT_PP);
}

void vk_disp_sitronix_reset(vk_disp_sitronix_t *disp_sitronix, bool reset)
{
    VSF_UI_ASSERT(disp_sitronix != NULL);
    vsf_gpio_t *gpio = disp_sitronix->hw.gpio;

    if (reset) {
        vsf_gpio_clear(gpio, 1 << disp_sitronix->hw.reset_pin);
    } else {
        vsf_gpio_set(gpio, 1 << disp_sitronix->hw.reset_pin);
    }
}

vsf_err_t vk_disp_sitronix_write(vk_disp_sitronix_t *disp_sitronix, uint8_t cmd, uint8_t *param, uint_fast16_t param_len)
{
    VSF_UI_ASSERT(disp_sitronix != NULL);
    disp_sitronix->ctx.reg.cmd = cmd;
    disp_sitronix->ctx.reg.param = param;
    disp_sitronix->ctx.reg.param_len = param_len;
    disp_sitronix->ctx.reg.state = 0;
    return vsf_call_eda((uintptr_t)__vk_disp_sitronix_write, (uintptr_t)disp_sitronix);
}

static void __vk_disp_sitronix_seq(uintptr_t target, vsf_evt_t evt)
{
    vk_disp_sitronix_t *disp_sitronix = (vk_disp_sitronix_t *)target;

    switch (evt) {
    case VSF_EVT_INIT:
    case VSF_EVT_RETURN: {
            uint8_t *seq = disp_sitronix->ctx.seq.seq;
            uint_fast16_t param_len = seq[1];
            if (disp_sitronix->ctx.seq.seq_len > 0) {
                vk_disp_sitronix_write(disp_sitronix, seq[0], param_len ? &seq[2] : NULL, param_len);
                disp_sitronix->ctx.seq.seq_len -= 2 + param_len;
                disp_sitronix->ctx.seq.seq += 2 + param_len;
            } else {
                vsf_eda_return();
            }
        }
        break;
    }
}

vsf_err_t vk_disp_sitronix_seq(vk_disp_sitronix_t *disp_sitronix, uint8_t *seq, uint_fast16_t seq_len)
{
    VSF_UI_ASSERT(disp_sitronix != NULL);
    disp_sitronix->ctx.seq.seq = seq;
    disp_sitronix->ctx.seq.seq_len = seq_len;
    return vsf_call_eda((uintptr_t)__vk_disp_sitronix_seq, (uintptr_t)disp_sitronix);
}

#endif

/* EOF */
