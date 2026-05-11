#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED

#define __VSF_FBCON_CLASS_IMPLEMENT
#define __VSF_DISP_CLASS_INHERIT__
#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__

#include "component/ui/disp/vsf_disp.h"
#include "service/simple_stream/vsf_simple_stream.h"
#include "hal/vsf_hal.h"

#include "./vsf_fbcon.h"
#include "./vsf_fbcon_font.h"

#include <string.h>

/*============================ MACROS ========================================*/

#define FBCON_ANSI_STATE_NORMAL      0
#define FBCON_ANSI_STATE_ESC         1
#define FBCON_ANSI_STATE_CSI         2

#ifndef dimof
#   define dimof(a)                  (sizeof(a) / sizeof((a)[0]))
#endif

/*============================ TYPES =========================================*/

typedef struct vsf_fbcon_cell_t {
    uint8_t ch;
    uint8_t fg;     // palette index
    uint8_t bg;     // palette index
} vsf_fbcon_cell_t;

/*============================ LOCAL VARIABLES ===============================*/

// Base ANSI 16-color palette in RGB888 (R,G,B packed into uint32_t)
static const uint32_t __vsf_fbcon_palette_rgb888[16] = {
    0x000000, 0xC00000, 0x00C000, 0xC0C000,
    0x0000C0, 0xC000C0, 0x00C0C0, 0xC0C0C0,
    0x808080, 0xFF0000, 0x00FF00, 0xFFFF00,
    0x0000FF, 0xFF00FF, 0x00FFFF, 0xFFFFFF,
};

/*============================ PROTOTYPES ====================================*/

static void __vsf_fbcon_try_kick(vsf_fbcon_t *fbcon);
static void __vsf_fbcon_flush_next_row(vsf_fbcon_t *fbcon);
static void __vsf_fbcon_on_stream_rx(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt);
static void __vsf_fbcon_on_disp_ready(vk_disp_t *disp);

/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * Internal: color format conversion                                          *
 *----------------------------------------------------------------------------*/

static uint32_t __vsf_fbcon_rgb888_to_pixel(uint32_t rgb888, vk_disp_color_type_t color, uint8_t bytesize)
{
    uint8_t r = (rgb888 >> 16) & 0xFF;
    uint8_t g = (rgb888 >> 8) & 0xFF;
    uint8_t b = rgb888 & 0xFF;

    switch (bytesize) {
    case 1:     // RGB332
        return ((r >> 5) << 5) | ((g >> 5) << 2) | (b >> 6);
    case 2:     // RGB565
        return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    case 4:     // ARGB8888 / RGB888_32
        return 0xFF000000u | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    default:
        return 0;
    }
}

static void __vsf_fbcon_init_palette(vsf_fbcon_t *fbcon)
{
    vk_disp_color_type_t color = vsf_disp_get_pixel_format(fbcon->disp);
    uint8_t bytesize = vsf_disp_get_pixel_bytesize(fbcon->disp);
    for (int i = 0; i < 16; i++) {
        fbcon->palette[i] = __vsf_fbcon_rgb888_to_pixel(
            __vsf_fbcon_palette_rgb888[i], color, bytesize);
    }
}

/*----------------------------------------------------------------------------*
 * Internal: pixel write helper                                               *
 *----------------------------------------------------------------------------*/

static inline void __vsf_fbcon_write_pixel(void *buf, int offset,
    uint32_t pixel_val, uint8_t bytesize)
{
    switch (bytesize) {
    case 1:
        ((uint8_t *)buf)[offset] = (uint8_t)pixel_val;
        break;
    case 2:
        ((uint16_t *)buf)[offset] = (uint16_t)pixel_val;
        break;
    case 4:
        ((uint32_t *)buf)[offset] = pixel_val;
        break;
    }
}

/*----------------------------------------------------------------------------*
 * Internal: render one character cell into line_buf                           *
 *                                                                            *
 * line_buf holds pixels for one text row (pixel_w * font_height * bytesize). *
 * col specifies which character column to render within that line_buf.        *
 *----------------------------------------------------------------------------*/

static void __vsf_fbcon_render_cell_to_linebuf(vsf_fbcon_t *fbcon, int row, int col)
{
    if (col < 0 || col >= fbcon->cols || row < 0 || row >= fbcon->rows) {
        return;
    }

    int cell_idx = row * fbcon->cols + col;
    vsf_fbcon_cell_t *cells = (vsf_fbcon_cell_t *)fbcon->char_grid;
    uint8_t ch = cells[cell_idx].ch;
    uint32_t fg = fbcon->palette[cells[cell_idx].fg];
    uint32_t bg = fbcon->palette[cells[cell_idx].bg];

    int px = col * VSF_FBCON_CFG_FONT_WIDTH;
    const uint8_t *font_row = &__vsf_fbcon_font_8x16[ch * 16];
    uint8_t bytesize = vsf_disp_get_pixel_bytesize(fbcon->disp);
    int pixel_w = fbcon->disp->param.width;

    for (int y = 0; y < VSF_FBCON_CFG_FONT_HEIGHT; y++) {
        int poff = y * pixel_w + px;
        uint8_t bits = font_row[y];
        for (int x = 0; x < VSF_FBCON_CFG_FONT_WIDTH; x++) {
            __vsf_fbcon_write_pixel(fbcon->line_buf, poff + x,
                (bits & 0x80) ? fg : bg, bytesize);
            bits <<= 1;
        }
    }
}

/*----------------------------------------------------------------------------*
 * Internal: mark a row dirty                                                 *
 *----------------------------------------------------------------------------*/

static inline void __vsf_fbcon_mark_dirty(vsf_fbcon_t *fbcon, int row)
{
    if (row >= 0 && row < fbcon->rows) {
        fbcon->dirty_rows |= (1u << row);
    }
}

/*----------------------------------------------------------------------------*
 * Internal: scroll the screen up by one line (char_grid level)               *
 *----------------------------------------------------------------------------*/

static void __vsf_fbcon_scroll_up(vsf_fbcon_t *fbcon)
{
    vsf_fbcon_cell_t *cells = (vsf_fbcon_cell_t *)fbcon->char_grid;
    int row_size = fbcon->cols * sizeof(vsf_fbcon_cell_t);

    for (int row = 0; row < fbcon->rows - 1; row++) {
        memmove(&cells[row * fbcon->cols], &cells[(row + 1) * fbcon->cols], row_size);
    }

    // Clear last row
    int last_row_start = (fbcon->rows - 1) * fbcon->cols;
    for (int col = 0; col < fbcon->cols; col++) {
        cells[last_row_start + col].ch = ' ';
        cells[last_row_start + col].fg = fbcon->cur_fg;
        cells[last_row_start + col].bg = fbcon->cur_bg;
    }

    // All rows are now dirty
    fbcon->dirty_rows = (fbcon->rows >= 32) ? 0xFFFFFFFFu : ((1u << fbcon->rows) - 1);
}

/*----------------------------------------------------------------------------*
 * Internal: advance cursor after printing a char                             *
 *----------------------------------------------------------------------------*/

static void __vsf_fbcon_advance_cursor(vsf_fbcon_t *fbcon)
{
    fbcon->cursor_x++;
    if (fbcon->cursor_x >= fbcon->cols) {
        fbcon->cursor_x = 0;
        fbcon->cursor_y++;
        if (fbcon->cursor_y >= fbcon->rows) {
            fbcon->cursor_y = fbcon->rows - 1;
            __vsf_fbcon_scroll_up(fbcon);
        }
    }
}

/*----------------------------------------------------------------------------*
 * Internal: put a character at current cursor position                        *
 *----------------------------------------------------------------------------*/

static void __vsf_fbcon_put_char(vsf_fbcon_t *fbcon, uint8_t ch)
{
    vsf_fbcon_cell_t *cells = (vsf_fbcon_cell_t *)fbcon->char_grid;

    if (ch >= 0x20 && ch < 0x80) {
        int idx = fbcon->cursor_y * fbcon->cols + fbcon->cursor_x;
        cells[idx].ch = ch;
        cells[idx].fg = fbcon->cur_reverse ? fbcon->cur_bg : fbcon->cur_fg;
        cells[idx].bg = fbcon->cur_reverse ? fbcon->cur_fg : fbcon->cur_bg;

        __vsf_fbcon_mark_dirty(fbcon, fbcon->cursor_y);
        __vsf_fbcon_advance_cursor(fbcon);
    }
}

/*----------------------------------------------------------------------------*
 * Internal: clear a range of cells and mark affected rows dirty              *
 *----------------------------------------------------------------------------*/

static void __vsf_fbcon_clear_rows(vsf_fbcon_t *fbcon, int r0, int c0, int r1, int c1)
{
    vsf_fbcon_cell_t *cells = (vsf_fbcon_cell_t *)fbcon->char_grid;
    for (int r = r0; r <= r1; r++) {
        int col_start = (r == r0) ? c0 : 0;
        int col_end   = (r == r1) ? c1 : fbcon->cols - 1;
        for (int c = col_start; c <= col_end; c++) {
            cells[r * fbcon->cols + c].ch = ' ';
            cells[r * fbcon->cols + c].fg = fbcon->cur_fg;
            cells[r * fbcon->cols + c].bg = fbcon->cur_bg;
        }
        __vsf_fbcon_mark_dirty(fbcon, r);
    }
}

/*----------------------------------------------------------------------------*
 * Internal: ANSI escape sequence parser                                      *
 *----------------------------------------------------------------------------*/

static void __vsf_fbcon_ansi_process(vsf_fbcon_t *fbcon, uint8_t ch)
{
    switch (fbcon->ansi_state) {
    case FBCON_ANSI_STATE_NORMAL:
        if (ch == 0x1B) {
            fbcon->ansi_state = FBCON_ANSI_STATE_ESC;
            fbcon->ansi_idx = 0;
            fbcon->ansi_param_count = 0;
            fbcon->ansi_params[0] = 0;
        } else {
            __vsf_fbcon_put_char(fbcon, ch);
        }
        break;

    case FBCON_ANSI_STATE_ESC:
        if (ch == '[') {
            fbcon->ansi_state = FBCON_ANSI_STATE_CSI;
            fbcon->ansi_param_count = 0;
            memset(fbcon->ansi_params, 0, sizeof(fbcon->ansi_params));
        } else {
            fbcon->ansi_state = FBCON_ANSI_STATE_NORMAL;
        }
        break;

    case FBCON_ANSI_STATE_CSI:
        if (ch >= '0' && ch <= '9') {
            if (fbcon->ansi_param_count < 4) {
                fbcon->ansi_params[fbcon->ansi_param_count] =
                    fbcon->ansi_params[fbcon->ansi_param_count] * 10 + (ch - '0');
            }
        } else if (ch == ';') {
            if (fbcon->ansi_param_count < 3) {
                fbcon->ansi_param_count++;
                fbcon->ansi_params[fbcon->ansi_param_count] = 0;
            }
        } else if (ch >= 0x40 && ch <= 0x7E) {
            uint8_t p0 = fbcon->ansi_params[0];
            uint8_t p1 = fbcon->ansi_params[1];

            switch (ch) {
            case 'A':
                fbcon->cursor_y -= (p0 > 0 ? p0 : 1);
                if (fbcon->cursor_y < 0) fbcon->cursor_y = 0;
                break;
            case 'B':
                fbcon->cursor_y += (p0 > 0 ? p0 : 1);
                if (fbcon->cursor_y >= fbcon->rows) fbcon->cursor_y = fbcon->rows - 1;
                break;
            case 'C':
                fbcon->cursor_x += (p0 > 0 ? p0 : 1);
                if (fbcon->cursor_x >= fbcon->cols) fbcon->cursor_x = fbcon->cols - 1;
                break;
            case 'D':
                fbcon->cursor_x -= (p0 > 0 ? p0 : 1);
                if (fbcon->cursor_x < 0) fbcon->cursor_x = 0;
                break;
            case 'H':
            case 'f':
                fbcon->cursor_y = (p0 > 0 ? p0 - 1 : 0);
                fbcon->cursor_x = (p1 > 0 ? p1 - 1 : 0);
                if (fbcon->cursor_y < 0) fbcon->cursor_y = 0;
                if (fbcon->cursor_y >= fbcon->rows) fbcon->cursor_y = fbcon->rows - 1;
                if (fbcon->cursor_x < 0) fbcon->cursor_x = 0;
                if (fbcon->cursor_x >= fbcon->cols) fbcon->cursor_x = fbcon->cols - 1;
                break;
            case 'J':
                if (p0 == 2) {
                    __vsf_fbcon_clear_rows(fbcon, 0, 0, fbcon->rows - 1, fbcon->cols - 1);
                    fbcon->cursor_x = 0;
                    fbcon->cursor_y = 0;
                } else {
                    __vsf_fbcon_clear_rows(fbcon, fbcon->cursor_y, fbcon->cursor_x,
                        fbcon->rows - 1, fbcon->cols - 1);
                }
                break;
            case 'K':
                if (p0 == 2) {
                    __vsf_fbcon_clear_rows(fbcon, fbcon->cursor_y, 0,
                        fbcon->cursor_y, fbcon->cols - 1);
                } else {
                    __vsf_fbcon_clear_rows(fbcon, fbcon->cursor_y, fbcon->cursor_x,
                        fbcon->cursor_y, fbcon->cols - 1);
                }
                break;
            case 'm': {
                    uint8_t count = fbcon->ansi_param_count + 1;
                    for (uint8_t i = 0; i < count; i++) {
                        uint8_t param = fbcon->ansi_params[i];
                        if (param == 0) {
                            fbcon->cur_fg = 7;  // default fg: white/light gray
                            fbcon->cur_bg = 0;  // default bg: black
                            fbcon->cur_bold = false;
                            fbcon->cur_reverse = false;
                        } else if (param == 1) {
                            fbcon->cur_bold = true;
                        } else if (param == 7) {
                            fbcon->cur_reverse = true;
                        } else if (param == 27) {
                            fbcon->cur_reverse = false;
                        } else if (param >= 30 && param <= 37) {
                            int idx = param - 30;
                            if (fbcon->cur_bold) idx += 8;
                            fbcon->cur_fg = (uint8_t)idx;
                        } else if (param >= 40 && param <= 47) {
                            fbcon->cur_bg = (uint8_t)(param - 40);
                        } else if (param >= 90 && param <= 97) {
                            fbcon->cur_fg = (uint8_t)(param - 90 + 8);
                        } else if (param >= 100 && param <= 107) {
                            fbcon->cur_bg = (uint8_t)(param - 100 + 8);
                        }
                    }
                }
                break;
            default:
                break;
            }
            fbcon->ansi_state = FBCON_ANSI_STATE_NORMAL;
        }
        break;
    }
}

/*----------------------------------------------------------------------------*
 * Internal: process a single character                                        *
 *----------------------------------------------------------------------------*/

static void __vsf_fbcon_process_char(vsf_fbcon_t *fbcon, uint8_t ch)
{
    switch (ch) {
    case '\r':
        fbcon->cursor_x = 0;
        break;
    case '\n':
        fbcon->cursor_x = 0;
        fbcon->cursor_y++;
        if (fbcon->cursor_y >= fbcon->rows) {
            fbcon->cursor_y = fbcon->rows - 1;
            __vsf_fbcon_scroll_up(fbcon);
        }
        break;
    case '\b':
        if (fbcon->cursor_x > 0) {
            fbcon->cursor_x--;
        }
        break;
    case '\t': {
            int spaces = VSF_FBCON_CFG_TAB_WIDTH - (fbcon->cursor_x % VSF_FBCON_CFG_TAB_WIDTH);
            for (int i = 0; i < spaces; i++) {
                __vsf_fbcon_put_char(fbcon, ' ');
            }
        }
        break;
    case '\a':
        break;
    default:
        if (fbcon->ansi_state != FBCON_ANSI_STATE_NORMAL || ch == 0x1B
                || (ch >= 0x20 && ch < 0x80)) {
            __vsf_fbcon_ansi_process(fbcon, ch);
        }
        break;
    }
}

/*----------------------------------------------------------------------------*
 * Callback-driven engine                                                     *
 *                                                                            *
 * Two entry points drive everything:                                         *
 *   1. __vsf_fbcon_on_stream_rx   (VSF_STREAM_ON_RX on the input stream)    *
 *   2. __vsf_fbcon_on_disp_ready  (vk_disp_t::ui_on_ready)                  *
 *                                                                            *
 * try_kick is the chain STARTER: drain stream, start one-row refresh.        *
 * on_disp_ready is the chain CONTINUER: flush remaining dirty rows, then     *
 * re-drain, and release refresh_busy when everything is done.                *
 *                                                                            *
 * ui_on_ready may fire synchronously (within vk_disp_refresh) or             *
 * asynchronously (after it returns).  For sync drivers, the flush chain      *
 * forms bounded recursion (depth <= dirty_rows <= 30).  For async drivers,   *
 * each on_disp_ready is a separate call stack — no recursion.                *
 *----------------------------------------------------------------------------*/

static void __vsf_fbcon_flush_next_row(vsf_fbcon_t *fbcon)
{
    // Find lowest dirty row
    uint32_t mask = fbcon->dirty_rows;
    int row = 0;
    while (!(mask & 1u)) {
        mask >>= 1;
        row++;
    }
    fbcon->dirty_rows &= ~(1u << row);

    // Render entire row to line_buf
    for (int col = 0; col < fbcon->cols; col++) {
        __vsf_fbcon_render_cell_to_linebuf(fbcon, row, col);
    }

    // Issue partial refresh for this text row
    vk_disp_area_t area = {
        .pos  = { .x = 0, .y = (vk_disp_coord_t)(row * VSF_FBCON_CFG_FONT_HEIGHT) },
        .size = { .x = (vk_disp_coord_t)fbcon->disp->param.width,
                  .y = (vk_disp_coord_t)VSF_FBCON_CFG_FONT_HEIGHT },
    };
    vk_disp_refresh(fbcon->disp, &area, fbcon->line_buf);
}

static void __vsf_fbcon_try_kick(vsf_fbcon_t *fbcon)
{
    if (!fbcon->disp_ready) {
        return;
    }

    if (fbcon->refresh_busy) {
        return;
    }
    fbcon->refresh_busy = true;

    // Drain all available stream data into char_grid
    uint8_t ch;
    while (1 == vsf_stream_read(fbcon->stream, &ch, 1)) {
        __vsf_fbcon_process_char(fbcon, ch);
    }

    if (fbcon->dirty_rows != 0) {
        // Start the flush chain — on_disp_ready will continue it
        __vsf_fbcon_flush_next_row(fbcon);
    } else {
        fbcon->refresh_busy = false;
    }
}

static void __vsf_fbcon_on_stream_rx(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vsf_fbcon_t *fbcon = (vsf_fbcon_t *)param;
    if (fbcon == NULL) {
        return;
    }
    if (evt == VSF_STREAM_ON_RX) {
        __vsf_fbcon_try_kick(fbcon);
    }
}

static void __vsf_fbcon_on_disp_ready(vk_disp_t *disp)
{
    vsf_fbcon_t *fbcon = (vsf_fbcon_t *)disp->ui_data;
    if (fbcon == NULL) {
        return;
    }
    if (!fbcon->disp_ready) {
        // First notification: vk_disp_init completed.
        fbcon->disp_ready = true;
        fbcon->refresh_busy = false;
        __vsf_fbcon_try_kick(fbcon);
        return;
    }

    // A row refresh completed.  Continue the flush chain.
    if (fbcon->dirty_rows != 0) {
        __vsf_fbcon_flush_next_row(fbcon);
        return;
    }

    // All dirty rows flushed.  Drain any data that arrived during the chain.
    uint8_t ch;
    while (1 == vsf_stream_read(fbcon->stream, &ch, 1)) {
        __vsf_fbcon_process_char(fbcon, ch);
    }

    if (fbcon->dirty_rows != 0) {
        __vsf_fbcon_flush_next_row(fbcon);
    } else {
        fbcon->refresh_busy = false;
    }
}

/*----------------------------------------------------------------------------*
 * Public API                                                                 *
 *----------------------------------------------------------------------------*/

vsf_err_t vsf_fbcon_init(vsf_fbcon_t *fbcon)
{
    if (fbcon == NULL || fbcon->disp == NULL || fbcon->stream == NULL) {
        return VSF_ERR_FAIL;
    }

    vk_disp_t *disp = fbcon->disp;
    vsf_stream_t *stream = fbcon->stream;

    // Initialize private state
    fbcon->disp_ready = false;
    fbcon->refresh_busy = false;
    fbcon->dirty_rows = 0;
    fbcon->line_buf = NULL;
    fbcon->char_grid = NULL;
    fbcon->cursor_x = 0;
    fbcon->cursor_y = 0;
    fbcon->ansi_state = 0;

    fbcon->cols = disp->param.width / VSF_FBCON_CFG_FONT_WIDTH;
    fbcon->rows = disp->param.height / VSF_FBCON_CFG_FONT_HEIGHT;
    if (fbcon->rows > VSF_FBCON_MAX_ROWS) {
        fbcon->rows = VSF_FBCON_MAX_ROWS;
    }

    fbcon->cur_fg = 7;      // default: light gray
    fbcon->cur_bg = 0;      // default: black
    fbcon->cur_bold = false;
    fbcon->cur_reverse = false;

    // Build format-specific palette
    __vsf_fbcon_init_palette(fbcon);

    // Allocate line buffer: one text row of pixels
    size_t line_buf_size = (size_t)disp->param.width * VSF_FBCON_CFG_FONT_HEIGHT
                         * vsf_disp_get_pixel_bytesize(disp);
    fbcon->line_buf = vsf_heap_malloc(line_buf_size);
    if (fbcon->line_buf == NULL) {
        return VSF_ERR_FAIL;
    }
    memset(fbcon->line_buf, 0, line_buf_size);

    // Allocate char_grid
    size_t grid_size = (size_t)fbcon->rows * fbcon->cols * sizeof(vsf_fbcon_cell_t);
    fbcon->char_grid = vsf_heap_malloc(grid_size);
    if (fbcon->char_grid == NULL) {
        vsf_heap_free(fbcon->line_buf);
        fbcon->line_buf = NULL;
        return VSF_ERR_FAIL;
    }

    vsf_fbcon_cell_t *cells = (vsf_fbcon_cell_t *)fbcon->char_grid;
    for (int i = 0; i < fbcon->rows * fbcon->cols; i++) {
        cells[i].ch = ' ';
        cells[i].fg = fbcon->cur_fg;
        cells[i].bg = fbcon->cur_bg;
    }

    // Subscribe to incoming byte stream
    stream->rx.param = fbcon;
    stream->rx.evthandler = __vsf_fbcon_on_stream_rx;
    vsf_stream_connect_rx(stream);

    // Start the display panel
    disp->ui_data = fbcon;
    disp->ui_on_ready = __vsf_fbcon_on_disp_ready;
    if (vk_disp_init(disp) != VSF_ERR_NONE) {
        vsf_stream_disconnect_rx(stream);
        vsf_heap_free(fbcon->char_grid);
        vsf_heap_free(fbcon->line_buf);
        fbcon->char_grid = NULL;
        fbcon->line_buf = NULL;
        return VSF_ERR_FAIL;
    }

    return VSF_ERR_NONE;
}

void vsf_fbcon_fini(vsf_fbcon_t *fbcon)
{
    if (fbcon == NULL) return;

    if (fbcon->stream != NULL) {
        vsf_stream_disconnect_rx(fbcon->stream);
    }
    if (fbcon->line_buf != NULL) {
        vsf_heap_free(fbcon->line_buf);
        fbcon->line_buf = NULL;
    }
    if (fbcon->char_grid != NULL) {
        vsf_heap_free(fbcon->char_grid);
        fbcon->char_grid = NULL;
    }
}

#endif  // VSF_USE_UI
