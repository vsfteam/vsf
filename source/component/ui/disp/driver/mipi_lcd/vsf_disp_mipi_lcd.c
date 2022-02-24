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

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_MIPI_LCD == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_DISP_CLASS_INHERIT__
#define __VSF_DISP_MIPI_LCD_CLASS_IMPLEMENT

#include "kernel/vsf_kernel.h"
#include "../../vsf_disp.h"
#include "./vsf_disp_mipi_lcd.h"

/*============================ MACROS ========================================*/

#ifndef MIPI_LCD_SPI_PRIO
#   define MIPI_LCD_SPI_PRIO                            vsf_prio_0
#endif

#ifndef MIPI_LCD_SPI_CFG
#   if VSF_DISP_MIPI_LCD_SPI_MODE == VSF_DISP_MIPI_LCD_SPI_8BITS_MODE
#       define MIPI_LCD_SPI_CFG_DATA_SIZE               SPI_DATASIZE_8
#   elif VSF_DISP_MIPI_LCD_SPI_MODE == VSF_DISP_MIPI_LCD_SPI_9BITS_MODE
#       define MIPI_LCD_SPI_CFG_DATA_SIZE               SPI_DATASIZE_9
#   endif
#   define MIPI_LCD_SPI_CFG     (SPI_MASTER | SPI_MODE_3 | SPI_MSB_FIRST | MIPI_LCD_SPI_CFG_DATA_SIZE | SPI_AUTO_CS_DISABLE)
#endif

#ifndef MIPI_LCD_RESET_LOW_PULSE_TIME
#   define MIPI_LCD_RESET_LOW_PULSE_TIME                1       // Reset low pulse width 1ms
#endif

#ifndef MIPI_LCD_RESET_COMPLETION_TIME
#   define MIPI_LCD_RESET_COMPLETION_TIME               120     // Reset completion time
#endif

#ifndef VK_DISP_MIPI_LCD_SUPPORT_HARDWARE_RESET
#   define VK_DISP_MIPI_LCD_SUPPORT_HARDWARE_RESET      DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
enum {
    VSF_EVT_REFRESH             = VSF_EVT_USER + 0,
    VSF_EVT_WAIT_TE_LINE        = VSF_EVT_USER + 1,
    VSF_EVT_REFRESHING          = VSF_EVT_USER + 2,

    VSF_EVT_CMD_NEXT            = VSF_EVT_USER + 0,
    VSF_EVT_SPI_CPL             = VSF_EVT_USER + 1,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_disp_mipi_lcd_init(vk_disp_t *pthis);
static vsf_err_t __vk_disp_mipi_lcd_refresh(vk_disp_t *pthis,
                                            vk_disp_area_t *area,
                                            void *disp_buff);

#if VK_DISP_MIPI_LCD_SUPPORT_HARDWARE_RESET == ENABLED
extern void vk_disp_mipi_lcd_hw_reset_io_write(vk_disp_mipi_lcd_t *disp_mipi_lcd, bool level);
static void __lcd_hardware_reset_pin(vsf_eda_t *teda, vsf_evt_t evt);
#endif

static void __lcd_exit_sleep_mode(vsf_eda_t *teda, vsf_evt_t evt);
static void __lcd_init_param_seq(vsf_eda_t *teda, vsf_evt_t evt);
static void __lcd_refresh_evthandler(vsf_eda_t *teda, vsf_evt_t evt);

extern void vk_disp_mipi_lcd_io_init(vk_disp_mipi_lcd_t *disp_mipi_lcd);
extern void vk_disp_mipi_lcd_te_line_isr_handler(vk_disp_mipi_lcd_t *disp_mipi_lcd);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_disp_drv_t vk_disp_drv_mipi_lcd = {
    .init           = __vk_disp_mipi_lcd_init,
    .refresh        = __vk_disp_mipi_lcd_refresh,
};

/*============================ IMPLEMENTATION ================================*/

// avoid access private member
void vk_disp_mipi_tearing_effect_line_ready(vk_disp_mipi_lcd_t *disp_mipi_lcd)
{
    vsf_eda_post_evt(&disp_mipi_lcd->teda.use_as__vsf_eda_t, VSF_EVT_REFRESHING);
}

#ifndef WEAK_VK_DISP_MIPI_LCD_TE_LINE_ISR_HANDLER
WEAK(vk_disp_mipi_lcd_te_line_isr_handler)
void vk_disp_mipi_lcd_te_line_isr_handler(vk_disp_mipi_lcd_t *disp_mipi_lcd)
{
    // If we want to avoid tearing, then we need to wait for the TE signal to be ready.
    // The default behavior is refresh immediately.

    vk_disp_mipi_tearing_effect_line_ready(disp_mipi_lcd);
}
#endif

#if VK_DISP_MIPI_LCD_SUPPORT_HARDWARE_RESET == ENABLED
#ifndef WEAK_VK_DISP_MIPI_LCD_RESET_IO_WRITE
WEAK(vk_disp_mipi_lcd_hw_reset_io_write)
void vk_disp_mipi_lcd_hw_reset_io_write(vk_disp_mipi_lcd_t *disp_mipi_lcd, bool level)
{
#if VSF_DISP_MIPI_LCD_USING_VSF_GPIO == ENABLED
    vsf_gpio_write(disp_mipi_lcd->reset.gpio,
                   level ? disp_mipi_lcd->reset.pin_mask : 0,
                   disp_mipi_lcd->reset.pin_mask);
#endif
}
#endif
#endif

#ifndef WEAK_VK_DISP_MIPI_LCD_DCX_WRITE
WEAK(vk_disp_mipi_lcd_dcx_io_write)
void vk_disp_mipi_lcd_dcx_io_write(vk_disp_mipi_lcd_t *disp_mipi_lcd, bool level)
{
#if VSF_DISP_MIPI_LCD_USING_VSF_GPIO == ENABLED
    vsf_gpio_write(disp_mipi_lcd->dcx.gpio,
                   level ? disp_mipi_lcd->dcx.pin_mask : 0,
                   disp_mipi_lcd->dcx.pin_mask);
#endif
}
#endif

#ifndef WEAK_VK_DISP_MIPI_LCD_IO_INIT
WEAK(vk_disp_mipi_lcd_io_init)
void vk_disp_mipi_lcd_io_init(vk_disp_mipi_lcd_t *disp_mipi_lcd)
{
#if VSF_DISP_MIPI_LCD_USING_VSF_GPIO == ENABLED
    vsf_gpio_config_pin(disp_mipi_lcd->reset.gpio,
                        disp_mipi_lcd->reset.pin_mask, IO_PULL_UP);
    vsf_gpio_set_output(disp_mipi_lcd->reset.gpio,
                        disp_mipi_lcd->reset.pin_mask);

    vsf_gpio_config_pin(disp_mipi_lcd->dcx.gpio,
                        disp_mipi_lcd->dcx.pin_mask, IO_PULL_UP);
    vsf_gpio_set_output(disp_mipi_lcd->dcx.gpio,
                        disp_mipi_lcd->dcx.pin_mask);
#endif
}
#endif



#ifndef WEAK_VK_DISP_MIPI_TE_LINE_ISR_ENABLE_ONCE
WEAK(vk_disp_mipi_te_line_isr_enable_once)
void vk_disp_mipi_te_line_isr_enable_once(vk_disp_mipi_lcd_t *disp_mipi_lcd)
{
    vk_disp_mipi_tearing_effect_line_ready(disp_mipi_lcd);
}
#endif

static void __mipi_lcd_spi_req_cpl_handler(void *target_ptr,
                                           vsf_spi_t *spi_ptr,
                                           em_spi_irq_mask_t irq_mask)
{
    if (irq_mask & SPI_IRQ_MASK_CPL) {
        vk_disp_mipi_lcd_t *disp_mipi_lcd = (vk_disp_mipi_lcd_t *)target_ptr;
        vsf_eda_post_evt(&disp_mipi_lcd->teda.use_as__vsf_eda_t, VSF_EVT_SPI_CPL);
    }
}

static vsf_err_t __mipi_lcd_spi_init(vk_disp_mipi_lcd_t * disp_mipi_lcd)
{
    vsf_err_t init_result;
    fsm_rt_t enable_status;

    VSF_UI_ASSERT(disp_mipi_lcd->spi != NULL);

    spi_cfg_t spi_cfg = {
        .mode = { MIPI_LCD_SPI_CFG },
        .clock_hz = disp_mipi_lcd->clock_hz,
        .isr = {
            .handler_fn = __mipi_lcd_spi_req_cpl_handler,
            .target_ptr = disp_mipi_lcd,
            .prio = MIPI_LCD_SPI_PRIO,
        }
    };

    init_result = vsf_spi_init(disp_mipi_lcd->spi, &spi_cfg);
    if (init_result != VSF_ERR_NONE) {
        return init_result;
    }

    do {
        enable_status = vsf_spi_enable(disp_mipi_lcd->spi);
    } while (enable_status != fsm_rt_cpl);

    vsf_spi_irq_enable(disp_mipi_lcd->spi, SPI_IRQ_MASK_CPL);

    return VSF_ERR_NONE;
}

static void __lcd_update_cur_seq(vk_disp_mipi_lcd_t *disp_mipi_lcd,
                                 const uint8_t *buf,
                                 uint16_t max_cnt)
{
    VSF_UI_ASSERT(disp_mipi_lcd != NULL);

    disp_mipi_lcd->seq.buf = buf;
    disp_mipi_lcd->seq.max_cnt = max_cnt;
    disp_mipi_lcd->seq.cur_cnt = 0;
}

static bool __lcd_get_next_command(vk_disp_mipi_lcd_t *disp_mipi_lcd)
{
    VSF_UI_ASSERT(disp_mipi_lcd != NULL);

    const uint8_t *command_seq = disp_mipi_lcd->seq.buf;
    uint16_t cur_cnt = disp_mipi_lcd->seq.cur_cnt;

    if (cur_cnt >= disp_mipi_lcd->seq.max_cnt) {
        return false;
    }

    disp_mipi_lcd->cmd.cmd        = command_seq[cur_cnt + 0];
    disp_mipi_lcd->cmd.param_size = command_seq[cur_cnt + 1];

    if (disp_mipi_lcd->cmd.param_size != 0xFF) {
        disp_mipi_lcd->cmd.param_buffer  = (disp_mipi_lcd->cmd.param_size == 0) ? NULL : (void *)&command_seq[cur_cnt + 2];
        disp_mipi_lcd->seq.cur_cnt += 2 + disp_mipi_lcd->cmd.param_size;
    } else {
        disp_mipi_lcd->cmd.param_size    = get_unaligned_cpu32(&command_seq[cur_cnt + 2]);
        disp_mipi_lcd->cmd.param_buffer  = (void *)get_unaligned_cpu32(&command_seq[cur_cnt + 6]);
        disp_mipi_lcd->seq.cur_cnt += 2 + 8;
    }

    return true;
}

static void __lcd_spi_request_cmd(vk_disp_mipi_lcd_t *disp_mipi_lcd)
{
    VSF_UI_ASSERT(disp_mipi_lcd->spi != NULL);

    vsf_spi_cs_active(disp_mipi_lcd->spi, 0);
    vk_disp_mipi_lcd_dcx_io_write(disp_mipi_lcd, false);
    vsf_spi_request_transfer(disp_mipi_lcd->spi, &disp_mipi_lcd->cmd.cmd, NULL, 1);
}

static void __lcd_spi_request_data(vk_disp_mipi_lcd_t *disp_mipi_lcd)
{
    VSF_UI_ASSERT(disp_mipi_lcd->spi != NULL);

    vk_disp_mipi_lcd_dcx_io_write(disp_mipi_lcd, true);
    vsf_spi_request_transfer(disp_mipi_lcd->spi,
                             disp_mipi_lcd->cmd.param_buffer,
                             NULL, disp_mipi_lcd->cmd.param_size);
}

static void __lcd_write_command_seq(vsf_eda_t *teda, vsf_evt_t evt)
{
    VSF_UI_ASSERT(teda != NULL);
    vk_disp_mipi_lcd_t *disp_mipi_lcd = container_of(teda, vk_disp_mipi_lcd_t, teda);
    VSF_UI_ASSERT(disp_mipi_lcd->spi != NULL);
    VSF_UI_ASSERT(disp_mipi_lcd->seq.buf != NULL);
    VSF_UI_ASSERT(disp_mipi_lcd->seq.max_cnt != 0);

    switch (evt) {
    case VSF_EVT_INIT:
    case VSF_EVT_CMD_NEXT:
        if (!__lcd_get_next_command(disp_mipi_lcd)) {
            __lcd_update_cur_seq(disp_mipi_lcd, NULL, 0);
            disp_mipi_lcd->teda.fn.evthandler = disp_mipi_lcd->evthandler;
            disp_mipi_lcd->evthandler = NULL;
            vsf_eda_post_evt(&disp_mipi_lcd->teda.use_as__vsf_eda_t, VSF_EVT_MESSAGE);
        } else {
            __lcd_spi_request_cmd(disp_mipi_lcd);
        }
        break;

    case VSF_EVT_SPI_CPL: {
            if (disp_mipi_lcd->cmd.param_size > 0) {
                __lcd_spi_request_data(disp_mipi_lcd);
                disp_mipi_lcd->cmd.param_size = 0;
            } else {
                vsf_spi_cs_inactive(disp_mipi_lcd->spi, 0);
                vsf_eda_post_evt(&disp_mipi_lcd->teda.use_as__vsf_eda_t, VSF_EVT_CMD_NEXT);
            }
            break;
        }
    }
}

static void __lcd_write_command_seq_start(vk_disp_mipi_lcd_t *disp_mipi_lcd)
{
    VSF_UI_ASSERT(disp_mipi_lcd != NULL);
    VSF_UI_ASSERT(disp_mipi_lcd->seq.buf != NULL);

    disp_mipi_lcd->evthandler = disp_mipi_lcd->teda.fn.evthandler;
    disp_mipi_lcd->teda.fn.evthandler = __lcd_write_command_seq;
    vsf_eda_post_evt(&disp_mipi_lcd->teda.use_as__vsf_eda_t, VSF_EVT_INIT);
}

#if VK_DISP_MIPI_LCD_SUPPORT_HARDWARE_RESET == ENABLED
static void __lcd_hardware_reset_pin(vsf_eda_t *teda, vsf_evt_t evt)
{
    VSF_UI_ASSERT(teda != NULL);
    vk_disp_mipi_lcd_t *disp_mipi_lcd = container_of(teda, vk_disp_mipi_lcd_t, teda);

    switch (evt) {
    case VSF_EVT_INIT:
        VSF_UI_ASSERT(disp_mipi_lcd->spi != NULL);

        vk_disp_mipi_lcd_io_init(disp_mipi_lcd);
        __mipi_lcd_spi_init(disp_mipi_lcd);

        vk_disp_mipi_lcd_hw_reset_io_write(disp_mipi_lcd, false);
        disp_mipi_lcd->reset_state = 0;
        vsf_teda_set_timer_ms(MIPI_LCD_RESET_LOW_PULSE_TIME);
        break;

    case VSF_EVT_TIMER:
        if (disp_mipi_lcd->reset_state == 0) {
            disp_mipi_lcd->reset_state++;
            vk_disp_mipi_lcd_hw_reset_io_write(disp_mipi_lcd, true);
            vsf_teda_set_timer_ms(MIPI_LCD_RESET_COMPLETION_TIME);
        } else {
            disp_mipi_lcd->teda.fn.evthandler = __lcd_exit_sleep_mode;
            vsf_eda_post_evt(&disp_mipi_lcd->teda.use_as__vsf_eda_t, VSF_EVT_INIT);
        }
        break;
    }
}
#endif

static void __lcd_exit_sleep_mode(vsf_eda_t *teda, vsf_evt_t evt)
{
    static const uint8_t __seq[] = {
        MIPI_DCS_EXIT_SLEEP_MODE,   0,
    };
    VSF_UI_ASSERT(teda != NULL);
    vk_disp_mipi_lcd_t *disp_mipi_lcd = container_of(teda, vk_disp_mipi_lcd_t, teda);

    switch (evt) {
    case VSF_EVT_INIT:
        VSF_UI_ASSERT(disp_mipi_lcd->spi != NULL);
        // exit sleep mode
        __lcd_update_cur_seq(disp_mipi_lcd, __seq, sizeof(__seq));
        __lcd_write_command_seq_start(disp_mipi_lcd);
        break;

    case VSF_EVT_MESSAGE:
        vsf_teda_set_timer_ms(MIPI_LCD_RESET_COMPLETION_TIME);
        break;

    case VSF_EVT_TIMER:
        disp_mipi_lcd->teda.fn.evthandler = __lcd_init_param_seq;
        vsf_eda_post_evt(&disp_mipi_lcd->teda.use_as__vsf_eda_t, VSF_EVT_INIT);
        break;
    }
}

static void __lcd_init_param_seq(vsf_eda_t *teda, vsf_evt_t evt)
{
    VSF_UI_ASSERT(teda != NULL);
    vk_disp_mipi_lcd_t *disp_mipi_lcd = container_of(teda, vk_disp_mipi_lcd_t, teda);
    VSF_UI_ASSERT(disp_mipi_lcd->init_seq != NULL);
    VSF_UI_ASSERT(disp_mipi_lcd->init_seq_len != 0);

    switch (evt) {
    case VSF_EVT_INIT:
        __lcd_update_cur_seq(disp_mipi_lcd,
                             disp_mipi_lcd->init_seq,
                             disp_mipi_lcd->init_seq_len);
        __lcd_write_command_seq_start(disp_mipi_lcd);
        break;

    case VSF_EVT_MESSAGE:
        disp_mipi_lcd->teda.fn.evthandler = __lcd_refresh_evthandler;
        vsf_eda_post_evt(&disp_mipi_lcd->teda.use_as__vsf_eda_t, VSF_EVT_INIT);
        break;
    }
}

static void __lcd_refresh_evthandler(vsf_eda_t *teda, vsf_evt_t evt)
{
    VSF_UI_ASSERT(teda != NULL);
    vk_disp_mipi_lcd_t *disp_mipi_lcd = container_of(teda, vk_disp_mipi_lcd_t, teda);

    switch (evt) {
    case VSF_EVT_INIT:
        // After lcd initialized, send first on ready
        vk_disp_on_ready(&disp_mipi_lcd->use_as__vk_disp_t);
        break;

    case VSF_EVT_REFRESH:
        vk_disp_mipi_lcd_te_line_isr_handler(disp_mipi_lcd);
        break;

    case VSF_EVT_REFRESHING: {
            uint16_t x_start = disp_mipi_lcd->area.pos.x;
            uint16_t x_end   = disp_mipi_lcd->area.pos.x + disp_mipi_lcd->area.size.x - 1;
            uint16_t y_start = disp_mipi_lcd->area.pos.y;
            uint16_t y_end   = disp_mipi_lcd->area.pos.y + disp_mipi_lcd->area.size.y - 1;
            uint32_t size    = disp_mipi_lcd->area.size.x * disp_mipi_lcd->area.size.y * 2;
            uint32_t address = (uint32_t)disp_mipi_lcd->cur_buffer;
            uint8_t  seq[] = {
                MIPI_DCS_SET_COLUMN_ADDRESS,   4, x_start >> 8, x_start & 0xFF, x_end >> 8, x_end & 0xFF,
                MIPI_DCS_SET_PAGE_ADDRESS,     4, y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF,
                MIPI_DCS_WRITE_MEMORY_START, 255, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            };
            put_unaligned_cpu32(size, &seq[6 + 6 + 2]);
            put_unaligned_cpu32(address, &seq[6 + 6 + 2 + 4]);

            memcpy(disp_mipi_lcd->refresh_seq, seq, sizeof(seq));

            __lcd_update_cur_seq(disp_mipi_lcd,
                                 disp_mipi_lcd->refresh_seq,
                                 sizeof(disp_mipi_lcd->refresh_seq));
            __lcd_write_command_seq_start(disp_mipi_lcd);
        }
        break;

    case VSF_EVT_MESSAGE:
        vk_disp_on_ready(&disp_mipi_lcd->use_as__vk_disp_t);
        break;
    }
}

static vsf_err_t __vk_disp_mipi_lcd_init(vk_disp_t *pthis)
{
    vk_disp_mipi_lcd_t *disp_mipi_lcd = (vk_disp_mipi_lcd_t *)pthis;
    VSF_UI_ASSERT(disp_mipi_lcd != NULL);

//    vk_disp_mipi_lcd_io_init(disp_mipi_lcd);
//    __mipi_lcd_spi_init(disp_mipi_lcd);

#if VK_DISP_MIPI_LCD_SUPPORT_HARDWARE_RESET == ENABLED
    disp_mipi_lcd->teda.fn.evthandler = __lcd_hardware_reset_pin;
#else
    disp_mipi_lcd->teda.fn.evthandler = __lcd_exit_sleep_mode;
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    disp_mipi_lcd->teda.on_terminate = NULL;
#endif

    return vsf_teda_init(&disp_mipi_lcd->teda);
}

static vsf_err_t __vk_disp_mipi_lcd_refresh(vk_disp_t *pthis,
                                            vk_disp_area_t *area,
                                            void *disp_buff)
{
    vk_disp_mipi_lcd_t *disp_mipi_lcd = (vk_disp_mipi_lcd_t *)pthis;
    VSF_UI_ASSERT(disp_mipi_lcd != NULL);

    if (disp_mipi_lcd->teda.fn.evthandler != __lcd_refresh_evthandler) {
        return VSF_ERR_NOT_READY;
    }

    if (   (area->pos.x + area->size.x > disp_mipi_lcd->param.width)
        || (area->pos.y + area->size.y > disp_mipi_lcd->param.height)) {
        vsf_trace_error("disp area [%d,%d], [%d,%d] out of bounds\r\n",
                    area->pos.x, area->pos.y, area->size.x, area->size.y);
        return VSF_ERR_INVALID_RANGE;
    }

    disp_mipi_lcd->area = *area;
    disp_mipi_lcd->cur_buffer = disp_buff;
    vsf_eda_post_evt(&disp_mipi_lcd->teda.use_as__vsf_eda_t, VSF_EVT_REFRESH);

    return VSF_ERR_NONE;
}

#endif

/* EOF */
