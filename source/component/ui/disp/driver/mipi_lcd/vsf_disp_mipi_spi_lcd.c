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

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_MIPI_SPI_LCD == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_DISP_CLASS_INHERIT__
#define __VSF_DISP_MIPI_SPI_LCD_CLASS_IMPLEMENT

#include "kernel/vsf_kernel.h"
#include "../../vsf_disp.h"
#include "./vsf_disp_mipi_spi_lcd.h"

/*============================ MACROS ========================================*/

#ifndef VSF_DISP_MIPI_SPI_LCD_ARCH_PRIO
#   define VSF_DISP_MIPI_SPI_LCD_ARCH_PRIO              vsf_arch_prio_1
#endif

#ifndef VSF_DISP_MIPI_SPI_LCD_CFG
#   if VSF_DISP_MIPI_LCD_SPI_MODE == VSF_DISP_MIPI_LCD_SPI_8BITS_MODE
#       define VSF_DISP_MIPI_SPI_LCD_CFG_DATA_SIZE      VSF_SPI_DATASIZE_8
#   elif VSF_DISP_MIPI_LCD_SPI_MODE == VSF_DISP_MIPI_LCD_SPI_9BITS_MODE
#       define VSF_DISP_MIPI_SPI_LCD_CFG_DATA_SIZE      VSF_SPI_DATASIZE_9
#   endif
#   define VSF_DISP_MIPI_SPI_LCD_CFG    (VSF_SPI_MASTER | VSF_SPI_MODE_3 | VSF_SPI_MSB_FIRST | VSF_DISP_MIPI_SPI_LCD_CFG_DATA_SIZE | VSF_SPI_CS_SOFTWARE_MODE)
#endif

#ifndef VSF_DISP_MIPI_SPI_LCD_RESET_LOW_PULSE_TIME
#   define VSF_DISP_MIPI_SPI_LCD_RESET_LOW_PULSE_TIME  1       // Reset low pulse width 1ms
#endif

#ifndef VSF_DISP_MIPI_SPI_LCD_RESET_COMPLETION_TIME
#   define VSF_DISP_MIPI_SPI_LCD_RESET_COMPLETION_TIME  120     // Reset completion time
#endif

#ifndef VSF_DISP_MIPI_LCD_SUPPORT_HARDWARE_RESET
#   define VSF_DISP_MIPI_LCD_SUPPORT_HARDWARE_RESET     DISABLED
#endif

#define __MIPI_LCD_BUFFER_TYPE                          0xFF

#ifndef VSF_DISP_MIPI_SPI_LCD_CFG_TRACE_DEBUG
#   define VSF_DISP_MIPI_SPI_LCD_CFG_TRACE_DEBUG        DISABLED  /* ENABLED: vsf_trace_debug for seq/init/refresh */
#endif
#ifndef VSF_DISP_MIPI_SPI_LCD_CFG_TRACE_ERROR
#   define VSF_DISP_MIPI_SPI_LCD_CFG_TRACE_ERROR        DISABLED  /* ENABLED: vsf_trace_error for seq/area */
#endif

#if (VSF_DISP_MIPI_SPI_LCD_CFG_TRACE_DEBUG == ENABLED)
#   define __vsf_disp_mipi_spi_lcd_trace_debug(...)     vsf_trace_debug(__VA_ARGS__)
#else
#   define __vsf_disp_mipi_spi_lcd_trace_debug(...)
#endif

#if (VSF_DISP_MIPI_SPI_LCD_CFG_TRACE_ERROR == ENABLED)
#   define __vsf_disp_mipi_spi_lcd_trace_error(...)     vsf_trace_error(__VA_ARGS__)
#else
#   define __vsf_disp_mipi_spi_lcd_trace_error(...)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_EVT_REFRESH             = VSF_EVT_USER + 0,
    VSF_EVT_WAIT_TE_LINE        = VSF_EVT_USER + 1,
    VSF_EVT_REFRESHING          = VSF_EVT_USER + 2,
};

enum {
#if VSF_DISP_MIPI_LCD_SUPPORT_HARDWARE_RESET == ENABLED
    LCD_STATE_HW_RESET,
    LCD_STATE_HW_RESET_DONE,
#endif
    LCD_STATE_EXIT_SLEEP_MODE_DONE,
    LCD_STATE_INIT_SEQ,
    LCD_STATE_INIT_SEQ_DONE,
    LCD_STATE_REFRESH,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __lcd_init(vk_disp_t *pthis);
static void __lcd_fini(vk_disp_t *pthis);
static vsf_err_t __lcd_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);
static void __lcd_evthandler(vsf_eda_t *teda, vsf_evt_t evt);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_disp_drv_t vk_disp_drv_mipi_spi_lcd = {
    .init           = __lcd_init,
    .fini           = __lcd_fini,
    .refresh        = __lcd_refresh,
};

/*============================ IMPLEMENTATION ================================*/

// avoid access private member
void vsf_disp_mipi_te_line_ready(vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd)
{
    vsf_eda_post_evt(&disp_mipi_spi_lcd->teda.use_as__vsf_eda_t, VSF_EVT_REFRESHING);
}

VSF_CAL_WEAK(vsf_disp_mipi_spi_lcd_wait_te_line_ready)
bool vsf_disp_mipi_spi_lcd_wait_te_line_ready(vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd)
{
    // If we want to avoid tearing, then we need to wait for the TE signal to be ready.
    // The default behavior is refresh immediately, which returns true.
    // vsf_disp_mipi_te_line_ready(disp_mipi_spi_lcd);

    return true;
}

#if VSF_DISP_MIPI_LCD_SUPPORT_HARDWARE_RESET == ENABLED
VSF_CAL_WEAK(vk_disp_mipi_spi_lcd_hw_reset_io_write)
void vk_disp_mipi_spi_lcd_hw_reset_io_write(vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd, bool level)
{
#if VSF_DISP_MIPI_SPI_LCD_USING_VSF_GPIO == ENABLED
    vsf_gpio_write(disp_mipi_spi_lcd->reset.gpio,
                   disp_mipi_spi_lcd->reset.pin_mask,
                   level ? disp_mipi_spi_lcd->reset.pin_mask : 0);
#endif
}
#endif

VSF_CAL_WEAK(vsf_disp_mipi_spi_lcd_dcx_io_write)
void vsf_disp_mipi_spi_lcd_dcx_io_write(vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd, bool level)
{
#if VSF_DISP_MIPI_SPI_LCD_USING_VSF_GPIO == ENABLED
    vsf_gpio_write(disp_mipi_spi_lcd->dcx.gpio,
                   disp_mipi_spi_lcd->dcx.pin_mask,
                   level ? disp_mipi_spi_lcd->dcx.pin_mask : 0);
#endif
}

VSF_CAL_WEAK(vsf_disp_mipi_spi_lcd_io_init)
void vsf_disp_mipi_spi_lcd_io_init(vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd)
{
#if VSF_DISP_MIPI_SPI_LCD_USING_VSF_GPIO == ENABLED
    vsf_gpio_cfg_t cfg = {
        .mode = VSF_GPIO_PULL_UP | VSF_GPIO_OUTPUT_PUSH_PULL,
    };
#if VSF_DISP_MIPI_LCD_SUPPORT_HARDWARE_RESET == ENABLED
    vsf_gpio_port_config_pins(disp_mipi_spi_lcd->reset.gpio,
                        disp_mipi_spi_lcd->reset.pin_mask,
                        &cfg);
    vsf_gpio_set_output(disp_mipi_spi_lcd->reset.gpio,
                        disp_mipi_spi_lcd->reset.pin_mask);
#endif
    vsf_gpio_port_config_pins(disp_mipi_spi_lcd->dcx.gpio,
                        disp_mipi_spi_lcd->dcx.pin_mask,
                        &cfg);
    vsf_gpio_set_output(disp_mipi_spi_lcd->dcx.gpio,
                        disp_mipi_spi_lcd->dcx.pin_mask);
#endif
}

VSF_CAL_WEAK(vsf_disp_mipi_spi_lcd_cs_active)
void vsf_disp_mipi_spi_lcd_cs_active(vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd)
{
    vsf_spi_cs_active(disp_mipi_spi_lcd->spi, VSF_DISP_MIPI_SPI_LCD_CFG_CS_INDEX);
}

VSF_CAL_WEAK(vsf_disp_mipi_spi_lcd_cs_inactive)
void vsf_disp_mipi_spi_lcd_cs_inactive(vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd)
{
    vsf_spi_cs_inactive(disp_mipi_spi_lcd->spi, VSF_DISP_MIPI_SPI_LCD_CFG_CS_INDEX);
}

static bool __lcd_get_next_command(vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd)
{
    VSF_UI_ASSERT(disp_mipi_spi_lcd != NULL);

    const uint8_t *command_seq = disp_mipi_spi_lcd->seq.buf;
    uint16_t cur_cnt = disp_mipi_spi_lcd->seq.cur_cnt;
    uint16_t max_cnt = disp_mipi_spi_lcd->seq.max_cnt;

    if (cur_cnt >= max_cnt) {
        __vsf_disp_mipi_spi_lcd_trace_debug("mipi_lcd seq: cur_cnt=%u >= max_cnt=%u, no more command\r\n",
                        (unsigned)cur_cnt, (unsigned)max_cnt);
        return false;
    }

    /* need at least 2 bytes: cmd + param_size */
    if (cur_cnt + 2 > max_cnt) {
        __vsf_disp_mipi_spi_lcd_trace_error("mipi_lcd seq: cur_cnt=%u max_cnt=%u, not enough for cmd+param_size\r\n",
                        (unsigned)cur_cnt, (unsigned)max_cnt);
        VSF_UI_ASSERT(false);
        return false;
    }

    disp_mipi_spi_lcd->cmd.cmd        = command_seq[cur_cnt + 0];
    disp_mipi_spi_lcd->cmd.param_size = command_seq[cur_cnt + 1];

    if (disp_mipi_spi_lcd->cmd.param_size != __MIPI_LCD_BUFFER_TYPE) {
        disp_mipi_spi_lcd->cmd.param_buffer  = (disp_mipi_spi_lcd->cmd.param_size == 0) ? NULL : (void *)&command_seq[cur_cnt + 2];
        disp_mipi_spi_lcd->seq.cur_cnt += 2 + disp_mipi_spi_lcd->cmd.param_size;
        __vsf_disp_mipi_spi_lcd_trace_debug("mipi_lcd seq: cur_cnt=%u cmd=0x%02X param_size=%u next_cur=%u max_cnt=%u\r\n",
                        (unsigned)cur_cnt, disp_mipi_spi_lcd->cmd.cmd,
                        (unsigned)disp_mipi_spi_lcd->cmd.param_size,
                        (unsigned)disp_mipi_spi_lcd->seq.cur_cnt, (unsigned)max_cnt);
        if (disp_mipi_spi_lcd->seq.cur_cnt > max_cnt) {
            __vsf_disp_mipi_spi_lcd_trace_error("mipi_lcd seq: next_cur_cnt %u > max_cnt %u (overflow)\r\n",
                            (unsigned)disp_mipi_spi_lcd->seq.cur_cnt, (unsigned)max_cnt);
            VSF_UI_ASSERT(false);
            return false;
        }
    } else {
        /* buffer type: need 2 + 4 (size) + 4 (ptr) = 10 bytes */
        if (cur_cnt + 10 > max_cnt) {
            __vsf_disp_mipi_spi_lcd_trace_error("mipi_lcd seq: cur_cnt=%u max_cnt=%u, not enough for buffer type (need 10)\r\n",
                            (unsigned)cur_cnt, (unsigned)max_cnt);
            VSF_UI_ASSERT(false);
            return false;
        }
        disp_mipi_spi_lcd->cmd.param_size    = get_unaligned_cpu32(&command_seq[cur_cnt + 2]);
        disp_mipi_spi_lcd->cmd.param_buffer  = (void *)get_unaligned_cpu32(&command_seq[cur_cnt + 6]);
        disp_mipi_spi_lcd->seq.cur_cnt += 2 + 8;
        __vsf_disp_mipi_spi_lcd_trace_debug("mipi_lcd seq: cur_cnt=%u cmd=0x%02X buffer_type size=%u ptr=%p next_cur=%u max_cnt=%u\r\n",
                        (unsigned)cur_cnt, disp_mipi_spi_lcd->cmd.cmd,
                        (unsigned)disp_mipi_spi_lcd->cmd.param_size,
                        disp_mipi_spi_lcd->cmd.param_buffer,
                        (unsigned)disp_mipi_spi_lcd->seq.cur_cnt, (unsigned)max_cnt);
        if (disp_mipi_spi_lcd->seq.cur_cnt > max_cnt) {
            __vsf_disp_mipi_spi_lcd_trace_error("mipi_lcd seq: next_cur_cnt %u > max_cnt %u (buffer type overflow)\r\n",
                            (unsigned)disp_mipi_spi_lcd->seq.cur_cnt, (unsigned)max_cnt);
            VSF_UI_ASSERT(false);
            return false;
        }
    }

    return true;
}

static vsf_err_t __lcd_spi_request_cmd(vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd)
{
    vsf_disp_mipi_spi_lcd_cs_active(disp_mipi_spi_lcd);
    vsf_disp_mipi_spi_lcd_dcx_io_write(disp_mipi_spi_lcd, false);
    vsf_err_t err = vsf_spi_request_transfer(disp_mipi_spi_lcd->spi, &disp_mipi_spi_lcd->cmd.cmd, NULL, 1);
    __vsf_disp_mipi_spi_lcd_trace_debug("mipi_lcd req_cmd: cmd=0x%02X err=%d\r\n",
                                        disp_mipi_spi_lcd->cmd.cmd, err);
    return err;
}

static vsf_err_t __lcd_start_cmdseq(vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd,
                                 const uint8_t *buf,
                                 uint16_t max_cnt)
{
    VSF_UI_ASSERT(disp_mipi_spi_lcd != NULL);
    VSF_UI_ASSERT(NULL == disp_mipi_spi_lcd->seq.buf);

    __vsf_disp_mipi_spi_lcd_trace_debug("mipi_lcd seq start: %s buf=%p max_cnt=%u\r\n",
                    (buf == disp_mipi_spi_lcd->init_seq) ? "init_seq" : "cmdseq",
                    (void *)buf, (unsigned)max_cnt);

    disp_mipi_spi_lcd->seq.buf = buf;
    disp_mipi_spi_lcd->seq.max_cnt = max_cnt;
    disp_mipi_spi_lcd->seq.cur_cnt = 0;

    bool valid = __lcd_get_next_command(disp_mipi_spi_lcd);
    if (!valid) {
        __vsf_disp_mipi_spi_lcd_trace_error("mipi_lcd seq start: __lcd_get_next_command failed (buf=%p max_cnt=%u)\r\n",
                        (void *)buf, (unsigned)max_cnt);
        VSF_UI_ASSERT(false);
    }
    VSF_UI_ASSERT(valid);

    return __lcd_spi_request_cmd(disp_mipi_spi_lcd);
}

static void __mipi_lcd_spi_req_cpl_handler(void *target_ptr,
                                            vsf_spi_t *spi_ptr,
                                            vsf_spi_irq_mask_t irq_mask)
{
    if (irq_mask & VSF_SPI_IRQ_MASK_CPL) {
        vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd = (vk_disp_mipi_spi_lcd_t *)target_ptr;
        __vsf_disp_mipi_spi_lcd_trace_debug(
            "mipi_lcd cpl: spi=%p cmd=0x%02X param_size=%u cur=%u max=%u\r\n",
            spi_ptr,
            disp_mipi_spi_lcd->cmd.cmd,
            (unsigned)disp_mipi_spi_lcd->cmd.param_size,
            (unsigned)disp_mipi_spi_lcd->seq.cur_cnt,
            (unsigned)disp_mipi_spi_lcd->seq.max_cnt);
        if (disp_mipi_spi_lcd->cmd.param_size > 0) {
            uint16_t param_size = disp_mipi_spi_lcd->cmd.param_size;
            vsf_disp_mipi_spi_lcd_dcx_io_write(disp_mipi_spi_lcd, true);
            vsf_err_t err = vsf_spi_request_transfer(disp_mipi_spi_lcd->spi,
                                            disp_mipi_spi_lcd->cmd.param_buffer, NULL, param_size);
            __vsf_disp_mipi_spi_lcd_trace_debug(
                "mipi_lcd cpl: send params size=%u err=%d\r\n",
                (unsigned)param_size, err);
            disp_mipi_spi_lcd->cmd.param_size = 0;
        } else {
            vsf_disp_mipi_spi_lcd_cs_inactive(disp_mipi_spi_lcd);
            __vsf_disp_mipi_spi_lcd_trace_debug("mipi_lcd cpl: cs_inactive, next command\r\n");
            if (!__lcd_get_next_command(disp_mipi_spi_lcd)) {
                __vsf_disp_mipi_spi_lcd_trace_debug("mipi_lcd seq: command sequence done, post VSF_EVT_MESSAGE\r\n");
                disp_mipi_spi_lcd->seq.buf = NULL;
                vsf_eda_post_evt(&disp_mipi_spi_lcd->teda.use_as__vsf_eda_t, VSF_EVT_MESSAGE);
            } else {
                __vsf_disp_mipi_spi_lcd_trace_debug(
                    "mipi_lcd cpl: continue cmd=0x%02X param_size=%u cur=%u/%u\r\n",
                    disp_mipi_spi_lcd->cmd.cmd,
                    (unsigned)disp_mipi_spi_lcd->cmd.param_size,
                    (unsigned)disp_mipi_spi_lcd->seq.cur_cnt,
                    (unsigned)disp_mipi_spi_lcd->seq.max_cnt);
                __lcd_spi_request_cmd(disp_mipi_spi_lcd);
            }
        }
    }
}

static vsf_err_t __mipi_lcd_spi_init(vk_disp_mipi_spi_lcd_t * disp_mipi_spi_lcd)
{
    vsf_err_t init_result;

    VSF_UI_ASSERT(disp_mipi_spi_lcd->spi != NULL);

    vsf_spi_cfg_t spi_cfg = {
        .mode = VSF_DISP_MIPI_SPI_LCD_CFG,
        .clock_hz = disp_mipi_spi_lcd->clock_hz,
        .isr = {
            .handler_fn = __mipi_lcd_spi_req_cpl_handler,
            .target_ptr = disp_mipi_spi_lcd,
            .prio = VSF_DISP_MIPI_SPI_LCD_ARCH_PRIO,
        }
    };

    init_result = vsf_spi_init(disp_mipi_spi_lcd->spi, &spi_cfg);
    if (init_result != VSF_ERR_NONE) {
        return init_result;
    }

    while (fsm_rt_on_going == vsf_spi_enable(disp_mipi_spi_lcd->spi));
    vsf_spi_irq_enable(disp_mipi_spi_lcd->spi, VSF_SPI_IRQ_MASK_CPL);

    return VSF_ERR_NONE;
}

static void __lcd_evthandler(vsf_eda_t *teda, vsf_evt_t evt)
{
    static const uint8_t __exit_sleep_mode_seq[] = {
        MIPI_DCS_CMD_EXIT_SLEEP_MODE
    };
    VSF_UI_ASSERT(teda != NULL);
    vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd = vsf_container_of(teda, vk_disp_mipi_spi_lcd_t, teda);
    uint8_t state = vsf_eda_get_user_value();

    switch (evt) {
    case VSF_EVT_INIT:
#if VSF_DISP_MIPI_LCD_SUPPORT_HARDWARE_RESET == ENABLED
        vk_disp_mipi_spi_lcd_hw_reset_io_write(disp_mipi_spi_lcd, false);
        vsf_eda_set_user_value(LCD_STATE_HW_RESET);
        vsf_teda_set_timer_ms(VSF_DISP_MIPI_SPI_LCD_RESET_LOW_PULSE_TIME);
#else
        vsf_eda_set_user_value(LCD_STATE_EXIT_SLEEP_MODE_DONE);
        __lcd_start_cmdseq(disp_mipi_spi_lcd, __exit_sleep_mode_seq, sizeof(__exit_sleep_mode_seq));
#endif
        break;

    case VSF_EVT_MESSAGE:
        switch (state) {
        case LCD_STATE_EXIT_SLEEP_MODE_DONE:
            vsf_eda_set_user_value(LCD_STATE_INIT_SEQ);
            vsf_teda_set_timer_ms(VSF_DISP_MIPI_SPI_LCD_RESET_COMPLETION_TIME);
            break;
        case LCD_STATE_INIT_SEQ_DONE:
            vsf_eda_set_user_value(LCD_STATE_REFRESH);
            // fall through
        case LCD_STATE_REFRESH:
            vk_disp_on_ready(&disp_mipi_spi_lcd->use_as__vk_disp_t);
            break;
        }
        break;

    case VSF_EVT_TIMER:
        switch (state) {
#if VSF_DISP_MIPI_LCD_SUPPORT_HARDWARE_RESET == ENABLED
        case LCD_STATE_HW_RESET:
            vk_disp_mipi_spi_lcd_hw_reset_io_write(disp_mipi_spi_lcd, true);
            vsf_eda_set_user_value(LCD_STATE_HW_RESET_DONE);
            vsf_teda_set_timer_ms(VSF_DISP_MIPI_SPI_LCD_RESET_COMPLETION_TIME);
            break;
        case LCD_STATE_HW_RESET_DONE:
            vsf_eda_set_user_value(LCD_STATE_EXIT_SLEEP_MODE_DONE);
            __lcd_start_cmdseq(disp_mipi_spi_lcd, __exit_sleep_mode_seq, sizeof(__exit_sleep_mode_seq));
            break;
#endif
        case LCD_STATE_INIT_SEQ: {
            const uint8_t *init_seq = disp_mipi_spi_lcd->init_seq;
            uint16_t init_len = disp_mipi_spi_lcd->init_seq_len;
            __vsf_disp_mipi_spi_lcd_trace_debug("mipi_lcd init_seq start: init_seq=%p init_seq_len=%u\r\n",
                            (void *)init_seq, (unsigned)init_len);
            vsf_eda_set_user_value(LCD_STATE_INIT_SEQ_DONE);
            __lcd_start_cmdseq(disp_mipi_spi_lcd, init_seq, init_len);
            break;
        }
        }
        break;
    case VSF_EVT_REFRESH:
        VSF_UI_ASSERT(LCD_STATE_REFRESH == state);
        if (!vsf_disp_mipi_spi_lcd_wait_te_line_ready(disp_mipi_spi_lcd)) {
            break;
        }
        // fall through
    case VSF_EVT_REFRESHING: {
            uint16_t x_start = disp_mipi_spi_lcd->area.pos.x + (uint16_t)VSF_DISP_MIPI_SPI_LCD_CFG_X_OFFSET;
            uint16_t x_end   = disp_mipi_spi_lcd->area.pos.x + disp_mipi_spi_lcd->area.size.x - 1 + (uint16_t)VSF_DISP_MIPI_SPI_LCD_CFG_X_OFFSET;
            uint16_t y_start = disp_mipi_spi_lcd->area.pos.y + (uint16_t)VSF_DISP_MIPI_SPI_LCD_CFG_Y_OFFSET;
            uint16_t y_end   = disp_mipi_spi_lcd->area.pos.y + disp_mipi_spi_lcd->area.size.y - 1 + (uint16_t)VSF_DISP_MIPI_SPI_LCD_CFG_Y_OFFSET;
            uint32_t size    = disp_mipi_spi_lcd->area.size.x * disp_mipi_spi_lcd->area.size.y * 2;
            uint32_t address = (uint32_t)disp_mipi_spi_lcd->cur_buffer;
            uint8_t  seq[] = {
                MIPI_DCS_CMD_SET_COLUMN_ADDRESS(x_start, x_end),
                MIPI_DCS_CMD_SET_PAGE_ADDRESS(y_start, y_end),
                MIPI_DCS_CMD_WRITE_MEMORY_START(__MIPI_LCD_BUFFER_TYPE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
            };
            put_unaligned_cpu32(size, &seq[6 + 6 + 2]);
            put_unaligned_cpu32(address, &seq[6 + 6 + 2 + 4]);

            memcpy(disp_mipi_spi_lcd->refresh_seq, seq, sizeof(seq));

            __vsf_disp_mipi_spi_lcd_trace_debug("mipi_lcd refresh_seq: len=%u x=[%u,%u] y=[%u,%u] size=%u addr=%p\r\n",
                            (unsigned)sizeof(disp_mipi_spi_lcd->refresh_seq),
                            (unsigned)x_start, (unsigned)x_end, (unsigned)y_start, (unsigned)y_end,
                            (unsigned)size, (void *)address);

            __lcd_start_cmdseq(disp_mipi_spi_lcd,
                                 disp_mipi_spi_lcd->refresh_seq,
                                 sizeof(disp_mipi_spi_lcd->refresh_seq));
        }
        break;
    }
}

static vsf_err_t __lcd_init(vk_disp_t *pthis)
{
    vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd = (vk_disp_mipi_spi_lcd_t *)pthis;

    vsf_err_t err;
    VSF_UI_ASSERT(disp_mipi_spi_lcd != NULL);

    vsf_disp_mipi_spi_lcd_io_init(disp_mipi_spi_lcd);
    err = __mipi_lcd_spi_init(disp_mipi_spi_lcd);
    if (err != VSF_ERR_NONE) {
        return err;
    }

    disp_mipi_spi_lcd->teda.fn.evthandler = __lcd_evthandler;
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    disp_mipi_spi_lcd->teda.on_terminate = NULL;
#endif

    err = vsf_teda_init(&disp_mipi_spi_lcd->teda);
    if (err != VSF_ERR_NONE) {
        return err;
    }

    return VSF_ERR_NONE;
}

static void __lcd_fini(vk_disp_t *pthis)
{
    vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd = (vk_disp_mipi_spi_lcd_t *)pthis;
    VSF_UI_ASSERT(disp_mipi_spi_lcd != NULL);

    vsf_spi_irq_disable(disp_mipi_spi_lcd->spi, VSF_SPI_IRQ_MASK_CPL);
    while (fsm_rt_on_going == vsf_spi_disable(disp_mipi_spi_lcd->spi));

    vsf_spi_fini(disp_mipi_spi_lcd->spi);

    disp_mipi_spi_lcd->teda.fn.evthandler = NULL;
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    disp_mipi_spi_lcd->teda.on_terminate = NULL;
#endif

    vsf_eda_fini(&disp_mipi_spi_lcd->teda.use_as__vsf_eda_t);
}

static vsf_err_t __lcd_refresh(vk_disp_t *pthis,
                                            vk_disp_area_t *area,
                                            void *disp_buff)
{
    vk_disp_mipi_spi_lcd_t *disp_mipi_spi_lcd = (vk_disp_mipi_spi_lcd_t *)pthis;
    VSF_UI_ASSERT(disp_mipi_spi_lcd != NULL);

    if (   (area->pos.x + area->size.x > disp_mipi_spi_lcd->param.width)
        || (area->pos.y + area->size.y > disp_mipi_spi_lcd->param.height)) {
        __vsf_disp_mipi_spi_lcd_trace_error("disp area [%d,%d], [%d,%d] out of bounds\r\n",
                    area->pos.x, area->pos.y, area->size.x, area->size.y);
        VSF_UI_ASSERT(false);
        return VSF_ERR_INVALID_RANGE;
    }

    disp_mipi_spi_lcd->area = *area;
    disp_mipi_spi_lcd->cur_buffer = disp_buff;
    return vsf_eda_post_evt(&disp_mipi_spi_lcd->teda.use_as__vsf_eda_t, VSF_EVT_REFRESH);
}

#endif

/* EOF */
