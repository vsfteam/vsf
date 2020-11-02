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

#include "./vsf_xboot_cfg.h"

#if VSF_USE_LINUX == ENABLED && VSF_USE_XBOOT == ENABLED

#define __VSF_LINUX_CLASS_INHERIT__
#include <unistd.h>
#include <xboot.h>
#include <string.h>

//#define __XBOOT_CFG_EVENT_TRACE

struct {
    struct window_manager_t wm;
    vk_input_notifier_t notifier;

    uint8_t event_rpos;
    uint8_t event_wpos;
    bool touch_is_down[8];
    struct event_t events[32];
} static __xboot = { 0 };

size_t strlcpy(char *dest, const char *src, size_t size)
{
    return strncpy(dest, src, size);
}

char *strsep(char **stringp, const char *delim)
{
    char *s;
    const char *spanp;
    int c, sc;
    char *tok;
    if ((s = *stringp)== NULL)
        return (NULL);
    for (tok = s;;) {
        c = *s++;
        spanp = delim;
        do {
            if ((sc =*spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *stringp = s;
                return (tok);
            }
        } while (sc != 0);
    }
    /* NOTREACHED */
}

// vfs
int vfs_open(const char *pathname, int flags, ...)
{
    return open(pathname, flags);
}

off_t vfs_lseek(int fd, off_t offset, int whence)
{
    return lseek(fd, offset, whence);
}

ssize_t vfs_read(int fd, void *buf, size_t count)
{
    return read(fd, buf, count);
}

ssize_t vfs_write(int fd, void *buf, size_t count)
{
    return write(fd, buf, count);
}

int vfs_close(int fd)
{
    return close(fd);
}

// xfs
struct xfs_file_t * xfs_open_read(struct xfs_context_t * ctx, const char * name)
{
    return NULL;
}

s64_t xfs_length(struct xfs_file_t * file)
{
    return 0;
}

s64_t xfs_read(struct xfs_file_t * file, void * buf, s64_t size)
{
    return 0;
}

void xfs_close(struct xfs_file_t * file)
{
}

s64_t xfs_seek(struct xfs_file_t * file, s64_t offset)
{
    return 0;
}

// arch algo
int fls(int x)
{
    return x ? sizeof(x) * 8 - vsf_clz(x) : 0;
}

// misc
#if VSF_USE_LINUX != ENABLED || VSF_LINUX_USE_SIMPLE_STDIO != ENABLED
int strcasecmp(const char *s1, const char *s2)
{
    return stricmp(s1, s2);
}
#endif

// window
static struct render_t __xboot_render_default = {
	.name	 			= "default",

	.create				= render_default_create,
	.destroy			= render_default_destroy,

	.blit				= render_default_blit,
	.fill				= render_default_fill,
	.text				= render_default_text,
	.icon				= render_default_icon,

	.shape_line			= render_default_shape_line,
	.shape_polyline		= render_default_shape_polyline,
	.shape_curve		= render_default_shape_curve,
	.shape_triangle		= render_default_shape_triangle,
	.shape_rectangle	= render_default_shape_rectangle,
	.shape_polygon		= render_default_shape_polygon,
	.shape_circle		= render_default_shape_circle,
	.shape_ellipse		= render_default_shape_ellipse,
	.shape_arc			= render_default_shape_arc,
	.shape_raster		= render_default_shape_raster,

	.filter_haldclut	= render_default_filter_haldclut,
	.filter_grayscale	= render_default_filter_grayscale,
	.filter_sepia		= render_default_filter_sepia,
	.filter_invert		= render_default_filter_invert,
	.filter_threshold	= render_default_filter_threshold,
	.filter_colorize	= render_default_filter_colorize,
	.filter_hue			= render_default_filter_hue,
	.filter_saturate	= render_default_filter_saturate,
	.filter_brightness	= render_default_filter_brightness,
	.filter_contrast	= render_default_filter_contrast,
	.filter_opacity		= render_default_filter_opacity,
	.filter_blur		= render_default_filter_blur,
};
struct window_t * window_alloc(const char * fb, const char * input, void * data)
{
    uint_fast32_t pixlen = ((__xboot.wm.disp->param.height * __xboot.wm.disp->param.width) << 2);
    struct window_t * window = malloc(sizeof(struct window_t) + sizeof(struct surface_t) + pixlen);
    if (!window) {
        return NULL;
    }

    memset(window, 0, sizeof(struct window_t) + sizeof(struct surface_t) + pixlen);
    window->wm = &__xboot.wm;
    window->priv = data;
    window->s = (struct surface_t *)&window[1];

    struct surface_t *surface = window->s;
    surface->height = __xboot.wm.disp->param.height;
    surface->width = __xboot.wm.disp->param.width;
    surface->pixels = &surface[1];
    surface->pixlen = pixlen;
    surface->stride = __xboot.wm.disp->param.width << 2;
    surface->r = &__xboot_render_default;

    spin_lock(&__xboot.wm.lock);
        vsf_dlist_add_to_head(struct window_t, window_node, &__xboot.wm.window_list, window);
    spin_unlock(&__xboot.wm.lock);

    return window;
}

void window_free(struct window_t * window)
{
    if (!window || !window->wm) {
        return;
    }

    spin_lock(&window->wm->lock);
        vsf_dlist_remove(struct window_t, window_node, &__xboot.wm.window_list, window);
    spin_unlock(&window->wm->lock);
    free(window);
}

int window_is_active(struct window_t * window)
{
    struct window_t *windows_tmp;
    vsf_dlist_peek_head(struct window_t, window_node, &__xboot.wm.window_list, windows_tmp);
    return window == windows_tmp ? 1 : 0;
}

int window_get_width(struct window_t * window)
{
    if (window != NULL && __xboot.wm.disp != NULL) {
        return __xboot.wm.disp->param.width;
    }
    return 0;
}

int window_get_height(struct window_t * window)
{
    if (window != NULL && __xboot.wm.disp != NULL) {
        return __xboot.wm.disp->param.height;
    }
    return 0;
}

void __xboot_disp_on_ready(vk_disp_t *disp)
{
    VSF_XBOOT_ASSERT(__xboot.wm.disp == disp);
    VSF_XBOOT_ASSERT(__xboot.wm.disp->ui_data != NULL);
    vsf_eda_post_evt(__xboot.wm.disp->ui_data, VSF_EVT_RETURN);
}

void window_present(struct window_t * window, struct color_t * c, void * o, void (*draw)(struct window_t *, void *))
{
    draw(window, o);
    __xboot.wm.disp->ui_data = vsf_eda_get_cur();
    vk_disp_refresh(__xboot.wm.disp, NULL, window->s->pixels);
    vsf_thread_wfe(VSF_EVT_RETURN);
}

static void __xboot_on_input(vk_input_type_t type, vk_input_evt_t *evt)
{
    if (((__xboot.event_wpos + 1) % dimof(__xboot.events)) == __xboot.event_rpos) {
        VSF_XBOOT_ASSERT(false);
        return;
    }

    struct event_t *cur_event = &__xboot.events[__xboot.event_wpos];
    switch (type) {
    case VSF_INPUT_TYPE_TOUCHSCREEN: {
            vk_touchscreen_evt_t *ts_evt = (vk_touchscreen_evt_t *)evt;
            uint_fast8_t id = vsf_input_touchscreen_get_id(ts_evt);
            uint_fast16_t x = vsf_input_touchscreen_get_x(ts_evt);
            uint_fast16_t y = vsf_input_touchscreen_get_y(ts_evt);

            if (id >= dimof(__xboot.touch_is_down)) {
                break;
            }

            if (vsf_input_touchscreen_is_down(ts_evt)) {
                if (__xboot.touch_is_down[id]) {
                    // touch move
                    cur_event->type = EVENT_TYPE_TOUCH_MOVE;
                } else {
                    // touch begin
                    __xboot.touch_is_down[id] = true;
                    cur_event->type = EVENT_TYPE_TOUCH_BEGIN;
                }
            } else {
                // touch end
                __xboot.touch_is_down[id] = false;
                cur_event->type = EVENT_TYPE_TOUCH_END;
            }
            cur_event->e.touch.id = id;
            cur_event->e.touch.x = x;
            cur_event->e.touch.y = y;
            __xboot.event_wpos = (__xboot.event_wpos + 1) % dimof(__xboot.events);
        }
        break;
    }
}

int window_pump_event(struct window_t * window, struct event_t * evt)
{
    if(window != NULL && evt != NULL) {
        if (__xboot.event_rpos != __xboot.event_wpos) {
            *evt = __xboot.events[__xboot.event_rpos];
            __xboot.event_rpos = (__xboot.event_rpos + 1) % dimof(__xboot.events);
#ifdef __XBOOT_CFG_EVENT_TRACE
            switch (evt->type) {
            case EVENT_TYPE_TOUCH_BEGIN:
                vsf_trace_debug("touch begin %d(%d, %d)" VSF_TRACE_CFG_LINEEND, evt->e.touch_begin.id, evt->e.touch_begin.x, evt->e.touch_begin.y);
                break;
            case EVENT_TYPE_TOUCH_MOVE:
                vsf_trace_debug("touch move %d(%d, %d)" VSF_TRACE_CFG_LINEEND, evt->e.touch_move.id, evt->e.touch_move.x, evt->e.touch_move.y);
                break;
            case EVENT_TYPE_TOUCH_END:
                vsf_trace_debug("touch end %d(%d, %d)" VSF_TRACE_CFG_LINEEND, evt->e.touch_end.id, evt->e.touch_end.x, evt->e.touch_end.y);
                break;
            }
#endif
            return 1;
        }
    }
    return 0;
}

// task
struct xboot_task_priv_t {
    void * data;
    task_func_t func;
    bool is_running;
};
typedef struct xboot_task_priv_t xboot_task_priv_t;

static void __xboot_task_on_run(vsf_thread_cb_t *cb);
static const vsf_linux_thread_op_t __vsf_linux_xboot_task_op = {
    .priv_size          = sizeof(xboot_task_priv_t),
    .on_run             = __xboot_task_on_run,
    .on_terminate       = vsf_linux_thread_on_terminate,
};

static void __xboot_task_on_run(vsf_thread_cb_t *cb)
{
    vsf_linux_thread_t *thread = container_of(cb, vsf_linux_thread_t, use_as__vsf_thread_cb_t);
    xboot_task_priv_t *priv = vsf_linux_thread_get_priv(thread);
    priv->func((struct task_t *)thread, priv->data);
    thread->retval = 0;
}

struct task_t * task_create(struct scheduler_t * sched, const char * name, task_func_t func, void * data, size_t stksz, int nice)
{
    vsf_linux_thread_t *thread = vsf_linux_create_thread(NULL, stksz, &__vsf_linux_xboot_task_op);
    if (!thread) {
        return NULL;
    }

    xboot_task_priv_t *priv = vsf_linux_thread_get_priv(thread);
    priv->is_running = false;
    priv->data = data;
    priv->func = func;
    return (struct task_t *)thread;
}

void task_destroy(struct task_t * task)
{
    VSF_XBOOT_ASSERT(false);
}

void task_suspend(struct task_t * task)
{
    VSF_XBOOT_ASSERT(false);
}

void task_resume(struct task_t * task)
{
    xboot_task_priv_t *priv = vsf_linux_thread_get_priv(&task->use_as__vsf_linux_thread_t);
    if (!priv->is_running) {
        priv->is_running = true;
        vsf_linux_start_thread(&task->use_as__vsf_linux_thread_t);
    }
}

void task_yield(void)
{
    vsf_eda_post_evt(vsf_eda_get_cur(), VSF_EVT_RETURN);
    vsf_thread_wfe(VSF_EVT_RETURN);
}

ktime_t ktime_get(void)
{
    ktime_t ktime = {
        .tv64 = vsf_systimer_get_us() * 1000,
    };
    return ktime;
}

void vsf_xboot_bind(vk_disp_t *disp)
{
    __xboot.wm.disp = disp;

    if (__xboot.wm.disp != NULL) {
        // unsecure code to set color because param maybe really const
        *(vk_disp_color_type_t *)&__xboot.wm.disp->param.color = VSF_DISP_COLOR_ARGB8888;
        __xboot.wm.disp->ui_on_ready = __xboot_disp_on_ready;
        vk_disp_init(__xboot.wm.disp);

        __xboot.notifier.mask = 1 << VSF_INPUT_TYPE_TOUCHSCREEN;
        __xboot.notifier.on_evt = (vk_input_on_evt_t)__xboot_on_input;
        vk_input_notifier_register(&__xboot.notifier);
    }
}

#endif      // VSF_USE_LINUX && VSF_USE_XBOOT
