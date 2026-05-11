#ifndef __VSF_FBCON_H__
#define __VSF_FBCON_H__

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED

#include "component/ui/disp/vsf_disp.h"
#include "service/simple_stream/vsf_simple_stream.h"

#if     defined(__VSF_FBCON_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_FBCON_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_FBCON_CFG_FONT_WIDTH
#   define VSF_FBCON_CFG_FONT_WIDTH         8
#endif
#ifndef VSF_FBCON_CFG_FONT_HEIGHT
#   define VSF_FBCON_CFG_FONT_HEIGHT        16
#endif

#ifndef VSF_FBCON_CFG_TAB_WIDTH
#   define VSF_FBCON_CFG_TAB_WIDTH          8
#endif

// Maximum rows supported by the dirty-row bitmask (uint32_t)
#define VSF_FBCON_MAX_ROWS                  32

/*============================ TYPES =========================================*/

/**
 * vsf_fbcon_t - callback-driven framebuffer console
 *
 * fbcon renders incoming byte stream data to a display panel using a built-in
 * 8x16 bitmap font.  It owns no task/thread; rendering is driven purely by
 * stream rx events and display ready callbacks.
 *
 * Memory-efficient design:
 *   - Uses a single-text-row line buffer instead of a full-frame pixel buffer.
 *   - Dirty rows are tracked with a bitmask and refreshed one at a time
 *     (async chain via ui_on_ready).
 *   - char_grid stores palette indices (1 byte each) rather than raw pixel
 *     colors, keeping it format-independent and compact (3 bytes/cell).
 *   - Pixel format is auto-detected from disp->param.color; supported:
 *     RGB332 (1B/px), RGB565 (2B/px), RGB888_32/ARGB8888 (4B/px).
 *
 * Before calling vsf_fbcon_init(), the caller MUST initialize the following
 * public members:
 *
 *   .disp   - pointer to a vk_disp_t instance (display panel driver).
 *             fbcon will call vk_disp_init() on it internally.
 *   .stream - pointer to a vsf_stream_t instance that provides the input
 *             byte stream (e.g. a vsf_fifo_stream_t used as debug output).
 *             fbcon subscribes to VSF_STREAM_ON_RX on this stream.
 */
vsf_class(vsf_fbcon_t) {
    public_member(
        vk_disp_t *disp;            // display device, MUST be set before init
        vsf_stream_t *stream;       // input byte stream, MUST be set before init
    )
    private_member(
        bool disp_ready;
        volatile bool refresh_busy;
        uint32_t dirty_rows;        // bitmask of rows that need refresh

        void *line_buf;             // one-text-row pixel buffer

        int cols, rows;
        int cursor_x, cursor_y;

        uint8_t ansi_state;
        uint8_t ansi_buf[16];
        uint8_t ansi_idx;
        uint8_t ansi_params[4];
        uint8_t ansi_param_count;

        uint8_t cur_fg;             // palette index (0-15)
        uint8_t cur_bg;             // palette index (0-15)
        bool cur_bold;
        bool cur_reverse;

        uint32_t palette[16];       // format-specific pixel values

        struct {
            uint8_t ch;
            uint8_t fg;             // palette index
            uint8_t bg;             // palette index
        } *char_grid;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

/*============================ PROTOTYPES ====================================*/

/**
 * vsf_fbcon_init - initialize the framebuffer console
 * @param fbcon     pointer to a vsf_fbcon_t instance whose public members
 *                  (disp, stream) have been set by the caller.
 * @return VSF_ERR_NONE on success.
 *
 * Typical usage:
 *   vsf_fbcon_t con;
 *   con.disp   = &my_display.use_as__vk_disp_t;
 *   con.stream = &my_fifo_stream.use_as__vsf_stream_t;
 *   vsf_fbcon_init(&con);
 */
extern vsf_err_t vsf_fbcon_init(vsf_fbcon_t *fbcon);

/**
 * vsf_fbcon_fini - finalize and release fbcon resources
 * @param fbcon     pointer to a previously initialized vsf_fbcon_t instance.
 *
 * Disconnects the stream rx subscription, frees the internal pixel buffer
 * and character grid.  The caller-owned disp and stream are NOT touched.
 */
extern void vsf_fbcon_fini(vsf_fbcon_t *fbcon);

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_UI
#endif  // __VSF_FBCON_H__
