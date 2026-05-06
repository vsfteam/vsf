/*****************************************************************************
 *   Copyright(C)2009-2026 by VSF Team                                       *
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
 ****************************************************************************/

/*
 * Port implementation for esp_lcd panel API on VSF.
 *
 * A single factory, esp_lcd_new_panel_from_vsf_disp(), creates an
 * esp_lcd_panel_handle_t that wraps a board-supplied vk_disp_t.  All
 * panel ops (init / del / draw_bitmap / etc.) dispatch to the VSF
 * display driver through the vk_disp_t abstraction.
 *
 * The IO layer (esp_lcd_panel_io_*) is not bridged; on VSF the board
 * owns the display hardware lifecycle.
 */

/*============================ INCLUDES ======================================*/

#include "../vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED && VSF_ESPIDF_CFG_USE_LCD == ENABLED

#include "../vsf_espidf.h"

#include "esp_lcd_types.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_io_i2c.h"
#include "esp_lcd_panel_vsf.h"

#include "component/ui/vsf_ui_cfg.h"
#include "component/ui/disp/vsf_disp.h"

#include <stdlib.h>
#include <string.h>

/*============================ TYPES =========================================*/

struct esp_lcd_panel_t {
    esp_err_t (*reset)(struct esp_lcd_panel_t *panel);
    esp_err_t (*init)(struct esp_lcd_panel_t *panel);
    esp_err_t (*del)(struct esp_lcd_panel_t *panel);
    esp_err_t (*draw_bitmap)(struct esp_lcd_panel_t *panel,
        int x_start, int y_start, int x_end, int y_end, const void *color_data);
    esp_err_t (*draw_bitmap_2d)(struct esp_lcd_panel_t *panel,
        int x_start, int y_start, int x_end, int y_end,
        const void *src_data, size_t src_x_size, size_t src_y_size,
        int src_x_start, int src_y_start, int src_x_end, int src_y_end);
    esp_err_t (*mirror)(struct esp_lcd_panel_t *panel,
        bool x_axis, bool y_axis);
    esp_err_t (*swap_xy)(struct esp_lcd_panel_t *panel, bool swap_axes);
    esp_err_t (*set_gap)(struct esp_lcd_panel_t *panel,
        int x_gap, int y_gap);
    esp_err_t (*invert_color)(struct esp_lcd_panel_t *panel,
        bool invert_color_data);
    esp_err_t (*disp_on_off)(struct esp_lcd_panel_t *panel, bool on_off);
    esp_err_t (*disp_sleep)(struct esp_lcd_panel_t *panel, bool sleep);
    esp_err_t (*set_brightness)(struct esp_lcd_panel_t *panel, int brightness);
    void *user_data;
};

typedef struct {
    struct esp_lcd_panel_t       base;
    vk_disp_t                   *vsf_disp;
    vsf_eda_t                   *eda;
} esp_lcd_panel_impl_t;

/*============================ LOCAL VARIABLES ================================*/

static void __esp_lcd_on_ready(vk_disp_t *disp)
{
    esp_lcd_panel_impl_t *panel = (esp_lcd_panel_impl_t *)disp->ui_data;
    if (panel && panel->eda) {
        vsf_eda_post_evt(panel->eda, VSF_EVT_USER);
    }
}

/*============================ PANEL LAYER VTABLE =============================*/

static esp_err_t __panel_reset(struct esp_lcd_panel_t *panel)
{
    (void)panel;
    return ESP_OK;
}

static esp_err_t __panel_init(struct esp_lcd_panel_t *panel)
{
    esp_lcd_panel_impl_t *impl = (esp_lcd_panel_impl_t *)panel;
    vk_disp_t *disp = impl->vsf_disp;
    if (!disp) return ESP_ERR_INVALID_STATE;

    if (disp->ui_on_ready == __esp_lcd_on_ready) {
        return ESP_OK;      // already inited through this port
    }

    impl->eda = vsf_eda_get_cur();

    disp->ui_data     = impl;
    disp->ui_on_ready = __esp_lcd_on_ready;

    vk_disp_init(disp);
    vsf_thread_wfe(VSF_EVT_USER);
    return ESP_OK;
}

static esp_err_t __panel_del(struct esp_lcd_panel_t *panel)
{
    esp_lcd_panel_impl_t *impl = (esp_lcd_panel_impl_t *)panel;
    vk_disp_t *disp = impl->vsf_disp;
    if (disp) {
        impl->eda = vsf_eda_get_cur();
        vk_disp_fini(disp);
        vsf_thread_wfe(VSF_EVT_USER);
    }
    free(panel);
    return ESP_OK;
}

static esp_err_t __panel_draw_bitmap(
    struct esp_lcd_panel_t *panel,
    int x_start, int y_start,
    int x_end, int y_end,
    const void *color_data)
{
    esp_lcd_panel_impl_t *impl = (esp_lcd_panel_impl_t *)panel;
    vk_disp_t *disp = impl->vsf_disp;
    if (!disp || !color_data) return ESP_ERR_INVALID_STATE;

    vk_disp_area_t area = {
        .pos  = { .x = (vk_disp_coord_t)x_start,
                   .y = (vk_disp_coord_t)y_start },
        .size = { .x = (vk_disp_coord_t)(x_end - x_start + 1),
                   .y = (vk_disp_coord_t)(y_end - y_start + 1) },
    };

    impl->eda = vsf_eda_get_cur();

    vk_disp_refresh(disp, &area, (void *)color_data);
    vsf_thread_wfe(VSF_EVT_USER);
    return ESP_OK;
}

static esp_err_t __panel_draw_bitmap_2d(
    struct esp_lcd_panel_t *panel,
    int x_start, int y_start, int x_end, int y_end,
    const void *src_data,
    size_t src_x_size, size_t src_y_size,
    int src_x_start, int src_y_start,
    int src_x_end, int src_y_end)
{
    (void)panel; (void)x_start; (void)y_start; (void)x_end; (void)y_end;
    (void)src_data; (void)src_x_size; (void)src_y_size;
    (void)src_x_start; (void)src_y_start; (void)src_x_end; (void)src_y_end;
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t __panel_mirror(
    struct esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y)
{
    (void)panel; (void)mirror_x; (void)mirror_y;
    return ESP_OK;
}

static esp_err_t __panel_swap_xy(
    struct esp_lcd_panel_t *panel, bool swap_axes)
{
    (void)panel; (void)swap_axes;
    return ESP_OK;
}

static esp_err_t __panel_set_gap(
    struct esp_lcd_panel_t *panel, int x_gap, int y_gap)
{
    (void)panel; (void)x_gap; (void)y_gap;
    return ESP_OK;
}

static esp_err_t __panel_invert_color(
    struct esp_lcd_panel_t *panel, bool invert_color_data)
{
    (void)panel; (void)invert_color_data;
    return ESP_OK;
}

static esp_err_t __panel_disp_on_off(
    struct esp_lcd_panel_t *panel, bool on_off)
{
    (void)panel; (void)on_off;
    return ESP_OK;
}

static esp_err_t __panel_disp_sleep(
    struct esp_lcd_panel_t *panel, bool sleep)
{
    (void)panel; (void)sleep;
    return ESP_OK;
}

static esp_err_t __panel_set_brightness(
    struct esp_lcd_panel_t *panel, int brightness)
{
    (void)panel; (void)brightness;
    return ESP_OK;
}

static const struct esp_lcd_panel_t __panel_vtable = {
    .reset           = __panel_reset,
    .init            = __panel_init,
    .del             = __panel_del,
    .draw_bitmap     = __panel_draw_bitmap,
    .draw_bitmap_2d  = __panel_draw_bitmap_2d,
    .mirror          = __panel_mirror,
    .swap_xy         = __panel_swap_xy,
    .set_gap         = __panel_set_gap,
    .invert_color    = __panel_invert_color,
    .disp_on_off     = __panel_disp_on_off,
    .disp_sleep      = __panel_disp_sleep,
    .set_brightness  = __panel_set_brightness,
};

/*============================ PANEL LAYER PUBLIC API =========================*/

esp_err_t esp_lcd_panel_reset(esp_lcd_panel_handle_t panel)
{
    return panel->reset(panel);
}

esp_err_t esp_lcd_panel_init(esp_lcd_panel_handle_t panel)
{
    return panel->init(panel);
}

esp_err_t esp_lcd_panel_del(esp_lcd_panel_handle_t panel)
{
    return panel->del(panel);
}

esp_err_t esp_lcd_panel_draw_bitmap(
    esp_lcd_panel_handle_t panel,
    int x_start, int y_start,
    int x_end, int y_end,
    const void *color_data)
{
    return panel->draw_bitmap(panel, x_start, y_start,
                               x_end, y_end, color_data);
}

esp_err_t esp_lcd_panel_mirror(
    esp_lcd_panel_handle_t panel, bool mirror_x, bool mirror_y)
{
    return panel->mirror(panel, mirror_x, mirror_y);
}

esp_err_t esp_lcd_panel_swap_xy(
    esp_lcd_panel_handle_t panel, bool swap_axes)
{
    return panel->swap_xy(panel, swap_axes);
}

esp_err_t esp_lcd_panel_set_gap(
    esp_lcd_panel_handle_t panel, int x_gap, int y_gap)
{
    return panel->set_gap(panel, x_gap, y_gap);
}

esp_err_t esp_lcd_panel_invert_color(
    esp_lcd_panel_handle_t panel, bool invert_color_data)
{
    return panel->invert_color(panel, invert_color_data);
}

esp_err_t esp_lcd_panel_disp_on_off(
    esp_lcd_panel_handle_t panel, bool on_off)
{
    return panel->disp_on_off(panel, on_off);
}

esp_err_t esp_lcd_panel_disp_sleep(
    esp_lcd_panel_handle_t panel, bool sleep)
{
    return panel->disp_sleep(panel, sleep);
}

esp_err_t esp_lcd_panel_set_brightness(
    esp_lcd_panel_handle_t panel, int brightness)
{
    return panel->set_brightness(panel, brightness);
}

esp_err_t esp_lcd_panel_draw_bitmap_2d(
    esp_lcd_panel_handle_t panel,
    int x_start, int y_start, int x_end, int y_end,
    const void *src_data,
    size_t src_x_size, size_t src_y_size,
    int src_x_start, int src_y_start,
    int src_x_end, int src_y_end)
{
    return panel->draw_bitmap_2d(panel, x_start, y_start,
                                  x_end, y_end, src_data,
                                  src_x_size, src_y_size,
                                  src_x_start, src_y_start,
                                  src_x_end, src_y_end);
}

/*============================ IO LAYER STUBS ================================*/

esp_err_t esp_lcd_new_panel_io_spi(
    esp_lcd_spi_bus_handle_t            bus,
    const esp_lcd_panel_io_spi_config_t *io_config,
    esp_lcd_panel_io_handle_t           *ret_io)
{
    (void)bus; (void)io_config; (void)ret_io;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_lcd_new_panel_io_i2c(
    i2c_master_bus_handle_t             bus,
    const esp_lcd_panel_io_i2c_config_t *io_config,
    esp_lcd_panel_io_handle_t           *ret_io)
{
    (void)bus; (void)io_config; (void)ret_io;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_lcd_panel_io_rx_param(
    esp_lcd_panel_io_handle_t io, int lcd_cmd,
    void *param, size_t param_size)
{
    (void)io; (void)lcd_cmd; (void)param; (void)param_size;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_lcd_panel_io_tx_param(
    esp_lcd_panel_io_handle_t io, int lcd_cmd,
    const void *param, size_t param_size)
{
    (void)io; (void)lcd_cmd; (void)param; (void)param_size;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_lcd_panel_io_tx_color(
    esp_lcd_panel_io_handle_t io, int lcd_cmd,
    const void *color, size_t color_size)
{
    (void)io; (void)lcd_cmd; (void)color; (void)color_size;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_lcd_panel_io_del(esp_lcd_panel_io_handle_t io)
{
    (void)io;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_lcd_panel_io_register_event_callbacks(
    esp_lcd_panel_io_handle_t io,
    const esp_lcd_panel_io_callbacks_t *cbs,
    void *user_ctx)
{
    (void)io; (void)cbs; (void)user_ctx;
    return ESP_ERR_NOT_SUPPORTED;
}

/*============================ PANEL FACTORY =================================*/

esp_err_t esp_lcd_new_panel_from_vsf_disp(
    vk_disp_t                          *disp,
    const esp_lcd_panel_dev_config_t   *panel_dev_config,
    esp_lcd_panel_handle_t             *ret_panel)
{
    if (!disp || !panel_dev_config || !ret_panel) return ESP_ERR_INVALID_ARG;

    esp_lcd_panel_impl_t *panel =
        (esp_lcd_panel_impl_t *)calloc(1, sizeof(*panel));
    if (!panel) return ESP_ERR_NO_MEM;

    panel->base     = __panel_vtable;
    panel->vsf_disp = disp;

    *ret_panel = &panel->base;
    return ESP_OK;
}

#endif /* VSF_USE_ESPIDF && VSF_ESPIDF_CFG_USE_LCD */
