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

#include "./vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#define __VSF_DISP_CLASS_INHERIT__
#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
// for vsf_disp
#include "component/vsf_component.h"
// for stream
#include "service/vsf_service.h"
#include "./include/SDL2/SDL.h"

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

/*============================ MACROS ========================================*/

#if VSF_INPUT_CFG_REGISTRATION_MECHANISM != ENABLED
#   error VSF_INPUT_CFG_REGISTRATION_MECHANISM is needed for vsf_sdl2
#endif

#if VSF_KERNEL_CFG_SUPPORT_THREAD != ENABLED
#   error vsf_sdl2 can only run in thread mode
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_sdl2_fast_copy(__pdst, __psrc, __num)                             \
    uint_fast32_t __temp = ((__num) + 3) / 4;                                   \
    if (!__num) { return; }                                                     \
    switch (__num % 4) {                                                        \
        case 0: do {    *__pdst++ = *__psrc++;                                  \
        case 3:         *__pdst++ = *__psrc++;                                  \
        case 2:         *__pdst++ = *__psrc++;                                  \
        case 1:         *__pdst++ = *__psrc++;                                  \
        } while (--__temp);                                                     \
    }

#define __vsf_sdl2_fast_set(__pdst, __value, __num)                             \
    uint_fast32_t __temp = ((__num) + 3) / 4;                                   \
    if (!__num) { return; }                                                     \
    switch (__num % 4) {                                                        \
        case 0: do {    *__pdst++ = __value;                                    \
        case 3:         *__pdst++ = __value;                                    \
        case 2:         *__pdst++ = __value;                                    \
        case 1:         *__pdst++ = __value;                                    \
        } while (--__temp);                                                     \
    }

/*============================ TYPES =========================================*/

struct SDL_Window {
    const char *title;
    SDL_Rect area;
    uint32_t flags;
    vk_disp_color_type_t format;
    SDL_Surface surface;
    uint32_t pixels[0] ALIGN(4);
};

struct SDL_Renderer {
    SDL_Window *window;
    SDL_Texture *target;
    uint32_t flags;
    SDL_BlendMode blend_mode;
    uint32_t color;
    float scale_x;
    float scale_y;
    int logic_w;
    int logic_h;
    SDL_Rect view_port;
};

struct SDL_Texture {
    uint16_t w, h;
    SDL_BlendMode blend_mode;
    SDL_PixelFormat *format;
    void *pixels;

    uint32_t __pixels[0] ALIGN(4);
};

typedef struct vsf_sdl2_t {
    vk_disp_t *disp;
#if VSF_USE_AUDIO == ENABLED
    struct {
        vk_audio_dev_t *dev;
        SDL_AudioStatus state;
        vk_audio_stream_t *stream;
        vsf_stream_t *raw_stream;
        SDL_AudioSpec spec;
    } audio;
#endif
    uint32_t init_flags;
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    uint32_t start_ms;
#endif
#if VSF_SDL_CFG_V1_COMPATIBLE == ENABLED
    SDL_Surface *sdl1_screen;
#endif
    struct {
        char *text;
    } clipboard;

    char *error;
} vsf_sdl2_t;

/*============================ PROTOTYPES ====================================*/

extern void vsf_sdl2_pixel_copy(uint_fast16_t data_line_num, uint_fast32_t data_line_size,
                                uint8_t *pdst, uint_fast32_t dst_pitch,
                                uint8_t *psrc, uint_fast32_t src_pitch);
extern void vsf_sdl2_pixel_fill(uint_fast16_t data_line_num, uint_fast32_t pixel_line_size,
                                uint8_t *pbuf, uint_fast32_t dst_pitch,
                                uint32_t color, uint_fast8_t color_byte_size);

extern uint32_t __SDL_GetColorFromMask(uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask);
extern const SDL_PixelFormat * __SDL_GetFormatFromColor(uint32_t color);
extern void __SDL_InitFormatMask(SDL_PixelFormat *format, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask);

extern void __SDL_InitEvent(uint32_t flags);
extern void __SDL_FiniEvent(void);

/*============================ LOCAL VARIABLES ===============================*/

NO_INIT vsf_sdl2_t __vsf_sdl2;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

void __SDL_LogMessage(SDL_LogPriority priority, int category, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
        vsf_trace_arg((vsf_trace_level_t)priority, fmt, ap);
    va_end(ap);

    size_t len = SDL_strlen(fmt);
    if ((fmt[len - 1] != '\r') && (fmt[len - 1] != '\n')) {
        vsf_trace((vsf_trace_level_t)priority, VSF_TRACE_CFG_LINEEND);
    }
}

// mutex
SDL_mutex* SDL_CreateMutex(void)
{
    SDL_mutex *mutex = SDL_malloc(sizeof(SDL_mutex));
    if (mutex != NULL) {
        vsf_mutex_init(mutex);
    }
    return mutex;
}

void SDL_DestroyMutex(SDL_mutex * mutex)
{
    SDL_free(mutex);
}

int SDL_TryLockMutex(SDL_mutex * mutex)
{
    vsf_sync_reason_t reason = vsf_thread_mutex_enter(mutex, 0);
    return (reason == VSF_SYNC_GET) ? 0 : -1;
}

int SDL_LockMutex(SDL_mutex * mutex)
{
    vsf_sync_reason_t reason = vsf_thread_mutex_enter(mutex, -1);
    return (reason == VSF_SYNC_GET) ? 0 : -1;
}

int SDL_UnlockMutex(SDL_mutex * mutex)
{
    vsf_thread_mutex_leave(mutex);
    return 0;
}

static void __vsf_sdl2_fast_memcpy8(uint8_t *pdst, uint8_t *psrc, uint_fast32_t num)
{
    __vsf_sdl2_fast_copy(pdst, psrc, num)
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe177
#endif
// __vsf_sdl2_fast_memcpy16 and __vsf_sdl2_fast_memcpy32 are reserved for future use
static void __vsf_sdl2_fast_memcpy16(uint16_t *pdst, uint16_t *psrc, uint_fast32_t num)
{
    __vsf_sdl2_fast_copy(pdst, psrc, num)
}
static void __vsf_sdl2_fast_memcpy32(uint32_t *pdst, uint32_t *psrc, uint_fast32_t num)
{
    __vsf_sdl2_fast_copy(pdst, psrc, num)
}

static void __vsf_sdl2_fast_memset8(uint8_t *pdst, uint8_t value, uint_fast32_t num)
{
    __vsf_sdl2_fast_set(pdst, value, num)
}
static void __vsf_sdl2_fast_memset16(uint16_t *pdst, uint16_t value, uint_fast32_t num)
{
    __vsf_sdl2_fast_set(pdst, value, num)
}
static void __vsf_sdl2_fast_memset32(uint32_t *pdst, uint32_t value, uint_fast32_t num)
{
    __vsf_sdl2_fast_set(pdst, value, num)
}

void SDL_memset4(void *dst, int val, size_t dwords)
{
    __vsf_sdl2_fast_memset32(dst, (uint32_t)val, dwords);
}

#ifndef WEAK_SDL2_PIXEL_COPY
WEAK(vsf_sdl2_pixel_copy)
void vsf_sdl2_pixel_copy(   uint_fast16_t data_line_num, uint_fast32_t data_line_size,
                            uint8_t *pdst, uint_fast32_t dst_pitch,
                            uint8_t *psrc, uint_fast32_t src_pitch)
{
    for (uint_fast16_t i = 0; i < data_line_num; i++) {
        __vsf_sdl2_fast_memcpy8(pdst, psrc, data_line_size);
        pdst += dst_pitch;
        psrc += src_pitch;
    }
}
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif

#ifndef WEAK_SDL2_PIXEL_FILL
WEAK(vsf_sdl2_pixel_fill)
void vsf_sdl2_pixel_fill(   uint_fast16_t data_line_num, uint_fast32_t pixel_line_size,
                            uint8_t *pbuf, uint_fast32_t dst_pitch,
                            uint32_t color, uint_fast8_t color_byte_size)
{
    for (uint_fast16_t i = 0; i < data_line_num; i++) {
        switch (color_byte_size) {
        case 1: __vsf_sdl2_fast_memset8((uint8_t *)pbuf, (uint8_t)color, pixel_line_size);      break;
        case 2: __vsf_sdl2_fast_memset16((uint16_t *)pbuf, (uint16_t)color, pixel_line_size);   break;
        case 4: __vsf_sdl2_fast_memset32((uint32_t *)pbuf, (uint32_t)color, pixel_line_size);   break;
        }
        pbuf += dst_pitch;
    }
}
#endif

static void __SDL_BlendWithFormat(
                            uint_fast16_t h, uint_fast32_t w,
                            uint8_t *pdst, uint_fast32_t dst_pitch,
                            uint8_t *psrc, uint_fast32_t src_pitch,
                            SDL_PixelFormat * dst_fmt, SDL_PixelFormat * src_fmt)
{
    VSF_SDL2_ASSERT((dst_fmt != NULL) && (src_fmt != NULL));
    uint_fast8_t dst_pixel_size = vsf_disp_get_pixel_format_bytesize(dst_fmt->format);
    uint_fast8_t src_pixel_size = vsf_disp_get_pixel_format_bytesize(src_fmt->format);

    uint8_t dst_rshift  = dst_fmt->Rshift;
    uint8_t dst_gshift  = dst_fmt->Gshift;
    uint8_t dst_bshift  = dst_fmt->Bshift;
//    uint8_t dst_ashift  = dst_fmt->Ashift;
    uint8_t dst_rloss  = dst_fmt->Rloss;
    uint8_t dst_gloss  = dst_fmt->Gloss;
    uint8_t dst_bloss  = dst_fmt->Bloss;
//    uint8_t dst_aloss  = dst_fmt->Aloss;
    uint32_t dst_rmask  = dst_fmt->Rmask;
    uint32_t dst_gmask  = dst_fmt->Gmask;
    uint32_t dst_bmask  = dst_fmt->Bmask;
    uint32_t dst_amask  = dst_fmt->Amask;

    uint8_t src_rshift  = src_fmt->Rshift;
    uint8_t src_gshift  = src_fmt->Gshift;
    uint8_t src_bshift  = src_fmt->Bshift;
    uint8_t src_ashift  = src_fmt->Ashift;
    uint8_t src_rloss  = src_fmt->Rloss;
    uint8_t src_gloss  = src_fmt->Gloss;
    uint8_t src_bloss  = src_fmt->Bloss;
    uint8_t src_aloss  = src_fmt->Aloss;
    uint32_t src_rmask  = src_fmt->Rmask;
    uint32_t src_gmask  = src_fmt->Gmask;
    uint32_t src_bmask  = src_fmt->Bmask;
    uint32_t src_amask  = src_fmt->Amask;

    uint32_t color, dcolor, da;
    uint8_t a, r, g, b;
    uint8_t dr, dg, db;
    uint8_t *dst_tmp, *src_tmp;
    for (uint_fast16_t i = 0; i < h; i++) {
        dst_tmp = pdst;
        src_tmp = psrc;
        for (uint_fast16_t j = 0; j < w; j++) {
            switch (src_pixel_size) {
            case 1: color = *(uint8_t *)src_tmp;    src_tmp += 1;   break;
            case 2: color = *(uint16_t *)src_tmp;   src_tmp += 2;   break;
            case 3: color = *(uint32_t *)src_tmp;   src_tmp += 3;   break;
            case 4: color = *(uint32_t *)src_tmp;   src_tmp += 4;   break;
            default:VSF_SDL2_ASSERT(false);
            }
            r = ((color & src_rmask) >> src_rshift) << src_rloss;
            g = ((color & src_gmask) >> src_gshift) << src_gloss;
            b = ((color & src_bmask) >> src_bshift) << src_bloss;
            if (src_amask) {
                a = ((color & src_amask) >> src_ashift) << src_aloss;
            } else {
                a = 0xFF;
            }

            switch (dst_pixel_size) {
            case 1: dcolor = *(uint8_t *)dst_tmp;                   break;
            case 2: dcolor = *(uint16_t *)dst_tmp;                  break;
            case 3: dcolor = *(uint32_t *)dst_tmp;                  break;
            case 4: dcolor = *(uint32_t *)dst_tmp;                  break;
            default:VSF_SDL2_ASSERT(false);
            }
            dr = ((dcolor & dst_rmask) >> dst_rshift) << dst_rloss;
            dg = ((dcolor & dst_gmask) >> dst_gshift) << dst_gloss;
            db = ((dcolor & dst_bmask) >> dst_bshift) << dst_bloss;
            if (dst_amask) {
                da = dcolor & dst_amask;
            } else {
                da = 0xFF;
            }

            r = (((r - dr) * a) >> 8) + dr;
            g = (((g - dg) * a) >> 8) + dg;
            b = (((b - db) * a) >> 8) + db;
            color =     ((r >> dst_rloss) << dst_rshift)
                    |   ((g >> dst_gloss) << dst_gshift)
                    |   ((b >> dst_bloss) << dst_bshift)
                    |   da;
            switch (dst_pixel_size) {
            case 1: *(uint8_t *)dst_tmp = color;    dst_tmp += 1;   break;
            case 2: *(uint16_t *)dst_tmp = color;   dst_tmp += 2;   break;
            case 3: dst_tmp[0] = color & 0x0000FF;
                    dst_tmp[1] = color & 0x00FF00;
                    dst_tmp[2] = color & 0xFF0000;  dst_tmp += 3;   break;
            case 4: *(uint32_t *)dst_tmp = color;   dst_tmp += 4;   break;
            default:VSF_SDL2_ASSERT(false);
            }
        }
        pdst += dst_pitch;
        psrc += src_pitch;
    }
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

const SDL_version * SDL_Linked_Version(void)
{
    static const SDL_version __sdl2_version = {
        .major = SDL_MAJOR_VERSION,
        .minor = SDL_MINOR_VERSION,
        .patch = SDL_PATCHLEVEL,
    };
    return &__sdl2_version;
}

void SDL_GetVersion(SDL_version *ver)
{
    ver->major = SDL_MAJOR_VERSION;
    ver->minor = SDL_MINOR_VERSION;
    ver->patch = SDL_PATCHLEVEL;
}

static void __vsf_sdl2_disp_on_ready(vk_disp_t *disp)
{
    vsf_eda_t *eda = __vsf_sdl2.disp->ui_data;
    VSF_SDL2_ASSERT(eda != NULL);
    vsf_eda_post_evt(eda, VSF_EVT_RETURN);
}

static inline bool __vsf_sdl2_cord_intersect(int *start_out, int *width_out, int start0, int width0, int start1, int width1)
{
    int start = vsf_max(start0, start1);
    int end0 = start0 + width0;
    int end1 = start1 + width1;
    int end = vsf_min(end0, end1);

    *start_out  = start;
    *width_out = end - start;
    return end > start;
}

SDL_bool SDL_IntersectRect(const SDL_Rect * rect0, const SDL_Rect * rect1, SDL_Rect * rect_out)
{
    VSF_SDL2_ASSERT((rect_out != NULL) && (rect0 != NULL) && (rect1 != NULL));
    bool x_intersect = __vsf_sdl2_cord_intersect(&rect_out->x, &rect_out->w, rect0->x, rect0->w, rect1->x, rect1->w);
    bool y_intersect = __vsf_sdl2_cord_intersect(&rect_out->y, &rect_out->h, rect0->y, rect0->h, rect1->y, rect1->h);
    return x_intersect && y_intersect;
}

static void __vsf_sdl2_disp_refresh(vk_disp_area_t *area, void *pixels)
{
    __vsf_sdl2.disp->ui_data = vsf_eda_get_cur();
    vk_disp_refresh(__vsf_sdl2.disp, area, pixels);
    vsf_thread_wfe(VSF_EVT_RETURN);
}

void vsf_sdl2_init(vsf_sdl2_cfg_t *cfg)
{
    __vsf_sdl2.init_flags = 0;
    __vsf_sdl2.disp = cfg->disp_dev;
#if VSF_USE_AUDIO == ENABLED
    __vsf_sdl2.audio.dev = cfg->audio_dev;
    __vsf_sdl2.audio.state = SDL_AUDIO_STOPPED;
    __vsf_sdl2.audio.stream = NULL;
    __vsf_sdl2.audio.raw_stream = NULL;
#endif
    __vsf_sdl2.sdl1_screen = NULL;
    __vsf_sdl2.error = NULL;
}

int SDL_InitSubSystem(uint32_t flags)
{
    if (flags & SDL_INIT_GAMECONTROLLER) {
        flags |= SDL_INIT_JOYSTICK;
    }
    if (flags & (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
        flags |= SDL_INIT_EVENTS;
    }

    flags &= ~__vsf_sdl2.init_flags;
    __vsf_sdl2.init_flags |= flags;
    if (flags & SDL_INIT_EVENTS) {
        __SDL_InitEvent(flags);
    }

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    if (flags & SDL_INIT_TIMER) {
        __vsf_sdl2.start_ms = vsf_systimer_get_ms();
    }
#endif

    if (flags & SDL_INIT_VIDEO) {
        __vsf_sdl2.disp->ui_data = vsf_eda_get_cur();
        __vsf_sdl2.disp->ui_on_ready = __vsf_sdl2_disp_on_ready;
        vk_disp_init(__vsf_sdl2.disp);
        vsf_thread_wfe(VSF_EVT_RETURN);
    }

#if VSF_USE_AUDIO == ENABLED
    if (flags & SDL_INIT_AUDIO) {
        vk_audio_dev_t *audio_dev = __vsf_sdl2.audio.dev;
        vk_audio_init(audio_dev);
        for (uint_fast8_t i = 0; i < audio_dev->stream_num; i++) {
            if (0 == audio_dev->stream[i].dir_in1out0) {
                __vsf_sdl2.audio.stream = &audio_dev->stream[i];
                break;
            }
        }
    }
#endif
    return 0;
}

void SDL_QuitSubSystem(uint32_t flags)
{
    __vsf_sdl2.init_flags &= ~flags;
}

int SDL_Init(uint32_t flags)
{
    return SDL_InitSubSystem(flags);
}

uint32_t SDL_WasInit(uint32_t flags)
{
    return __vsf_sdl2.init_flags & flags;
}

void SDL_Quit(void)
{
    uint32_t flags = __vsf_sdl2.init_flags;
    if (flags & SDL_INIT_EVENTS) {
        __SDL_FiniEvent();
    }

    if (flags & SDL_INIT_VIDEO) {
        // TODO:
    }
}

const char * SDL_GetPlatform(void)
{
    return "vsf";
}

int SDL_GetDesktopDisplayMode(int display_index, SDL_DisplayMode *mode)
{
    if (NULL == __vsf_sdl2.disp) {
        return -1;
    }

    if (mode != NULL) {
        mode->format = vsf_disp_get_pixel_format(__vsf_sdl2.disp);
#ifdef VSF_SDL_CFG_DISPLAY_HEIGHT
        mode->h = VSF_SDL_CFG_DISPLAY_HEIGHT;
#else
        mode->h = __vsf_sdl2.disp->param.height;
#endif
#ifdef VSF_SDL_CFG_DISPLAY_WIDTH
        mode->w = VSF_SDL_CFG_DISPLAY_WIDTH;
#else
        mode->w = __vsf_sdl2.disp->param.width;
#endif
    }
    return 0;
}

int SDL_GetCurrentDisplayMode(int display_index, SDL_DisplayMode *mode)
{
    return SDL_GetDesktopDisplayMode(display_index, mode);
}


SDL_RWops * SDL_RWFromFile(const char * file, const char * mode)
{
    return fopen(file, mode);
}
int64_t SDL_RWsize(SDL_RWops * context)
{
    fseek(context, 0, SEEK_END);
    off64_t off64 = ftello64(context);
    fseek(context, 0, SEEK_SET);
    return off64;
}
int SDL_RWclose(SDL_RWops * context)
{
    return fclose(context);
}
int64_t SDL_RWseek(SDL_RWops * context, int64_t offset, int whence)
{
    return fseek(context, offset, whence);
}
int64_t SDL_RWtell(SDL_RWops * context)
{
    return ftello64(context);
}
size_t SDL_RWread(SDL_RWops * context, void * ptr, size_t size, size_t maxnum)
{
    return fread(ptr, size, maxnum, context);
}
size_t SDL_RWwrite(SDL_RWops * context, const void * ptr, size_t size, size_t num)
{
    return fwrite(ptr, size, num, context);
}
uint8_t SDL_ReadU8(SDL_RWops * context)
{
    uint8_t value = 0;
    SDL_RWread(context, &value, sizeof(value), 1);
    return value;
}
uint16_t SDL_ReadLE16(SDL_RWops * context)
{
    uint16_t value = 0;
    SDL_RWread(context, &value, sizeof(value), 1);
    return SDL_SwapLE16(value);
}
uint16_t SDL_ReadBE16(SDL_RWops * context)
{
    uint16_t value = 0;
    SDL_RWread(context, &value, sizeof(value), 1);
    return SDL_SwapBE16(value);
}
uint32_t SDL_ReadLE32(SDL_RWops * context)
{
    uint32_t value = 0;
    SDL_RWread(context, &value, sizeof(value), 1);
    return SDL_SwapLE32(value);
}
uint32_t SDL_ReadBE32(SDL_RWops * context)
{
    uint32_t value = 0;
    SDL_RWread(context, &value, sizeof(value), 1);
    return SDL_SwapBE32(value);
}
uint64_t SDL_ReadLE64(SDL_RWops * context)
{
    uint64_t value = 0;
    SDL_RWread(context, &value, sizeof(value), 1);
    return SDL_SwapLE64(value);
}

uint64_t SDL_ReadBE64(SDL_RWops * context)
{
    uint64_t value = 0;
    SDL_RWread(context, &value, sizeof(value), 1);
    return SDL_SwapBE64(value);
}
uint8_t SDL_WriteU8(SDL_RWops * context, uint8_t value)
{
    return SDL_RWwrite(context, &value, sizeof(value), 1);
}
uint16_t SDL_WriteLE16(SDL_RWops * context, uint16_t value)
{
    value = SDL_SwapLE16(value);
    return SDL_RWwrite(context, &value, sizeof(value), 1);
}
uint16_t SDL_WriteBE16(SDL_RWops * context, uint16_t value)
{
    value = SDL_SwapBE16(value);
    return SDL_RWwrite(context, &value, sizeof(value), 1);
}
uint32_t SDL_WriteLE32(SDL_RWops * context, uint32_t value)
{
    value = SDL_SwapLE32(value);
    return SDL_RWwrite(context, &value, sizeof(value), 1);
}
uint32_t SDL_WriteBE32(SDL_RWops * context, uint32_t value)
{
    value = SDL_SwapBE32(value);
    return SDL_RWwrite(context, &value, sizeof(value), 1);
}
uint64_t SDL_WriteLE64(SDL_RWops * context, uint64_t value)
{
    value = SDL_SwapLE64(value);
    return SDL_RWwrite(context, &value, sizeof(value), 1);
}
uint64_t SDL_WriteBE64(SDL_RWops * context, uint64_t value)
{
    value = SDL_SwapBE64(value);
    return SDL_RWwrite(context, &value, sizeof(value), 1);
}


SDL_Window * SDL_CreateWindow(const char *title, int x, int y, int w, int h, uint32_t flags)
{
    uint_fast8_t pixel_byte_size = vsf_disp_get_pixel_bytesize(__vsf_sdl2.disp);

#ifdef VSF_SDL_CFG_WINDOW_PTR
    static bool __is_allocated = false;
    VSF_SDL2_ASSERT(!__is_allocated);
    SDL_Window *window = __is_allocated ? NULL : (SDL_Window *)VSF_SDL_CFG_WINDOW_PTR;
    __is_allocated = true;
#else
    SDL_Window *window = vsf_heap_malloc(sizeof(SDL_Window) + pixel_byte_size * w * h);
#endif
    if (window != NULL) {
        window->title   = title;
        window->area.x  = x;
        window->area.y  = y;
        window->area.w  = w;
        window->area.h  = h;
        window->flags   = flags;
        window->format  = vsf_disp_get_pixel_format(__vsf_sdl2.disp);
        // TODO: initialize pixels according to format of disp

        SDL_Surface *surface = &window->surface;
        surface->format = (SDL_PixelFormat *)__SDL_GetFormatFromColor(window->format);
        // default windows formats MUST be supported, because here is no masks to generate format
        VSF_SDL2_ASSERT(surface->format != NULL);
        surface->w = w;
        surface->h = h;
        surface->pitch = w * pixel_byte_size;
        surface->pixels = window->pixels;
    }
    return window;
}

void SDL_DestroyWindow(SDL_Window *window)
{
    VSF_SDL2_ASSERT(window != NULL);
    vsf_heap_free(window);
}

void SDL_GetWindowPosition(SDL_Window * window, int * x, int * y)
{
    if (x != NULL) {
        *x = window->area.x;
    }
    if (y != NULL) {
        *y = window->area.y;
    }
}

SDL_Surface * SDL_GetWindowSurface(SDL_Window *window)
{
    return &window->surface;
}

void SDL_GetWindowSize(SDL_Window* window, int *w, int *h)
{
    if (w != NULL) {
        *w = window->area.w;
    }
    if (h != NULL) {
        *h = window->area.h;
    }
}

int SDL_UpdateWindowSurface(SDL_Window * window)
{
    vk_disp_area_t area = {
        .pos.x          = 0,
        .pos.y          = 0,
        .size.x         = window->area.w,
        .size.y         = window->area.h,
    };
    __vsf_sdl2_disp_refresh(&area, window->pixels);
    return 0;
}

int SDL_UpdateWindowSurfaceRects(SDL_Window *window, const SDL_Rect *rects, int numrects)
{
    // TODO: update area according to rects
    return SDL_UpdateWindowSurface(window);
}

void SDL_ShowWindow(SDL_Window *window) {}
void SDL_SetWindowIcon(SDL_Window * window, SDL_Surface * icon) {}
void SDL_SetWindowTitle(SDL_Window * window, const char * title) {}
int SDL_SetWindowFullscreen(SDL_Window * window, uint32_t flags) { return 0; }
void SDL_RestoreWindow(SDL_Window * window) {}
void SDL_MaximizeWindow(SDL_Window * window) {}
void SDL_MinimizeWindow(SDL_Window * window) {}
void SDL_SetWindowBordered(SDL_Window * window, SDL_bool bordered) {}
void SDL_SetWindowPosition(SDL_Window * window, int x, int y) {}
int SDL_SetWindowOpacity(SDL_Window * window, float opacity) { return 0; }
uint32_t SDL_GetWindowFlags(SDL_Window * window)
{
    return  SDL_WINDOW_FULLSCREEN
        |   SDL_WINDOW_SHOWN
        |   SDL_WINDOW_MAXIMIZED
        |   SDL_WINDOW_INPUT_GRABBED
        |   SDL_WINDOW_INPUT_FOCUS
        |   SDL_WINDOW_MOUSE_FOCUS
        |   SDL_WINDOW_MOUSE_CAPTURE
        |   SDL_WINDOW_ALWAYS_ON_TOP
        |   SDL_WINDOW_SKIP_TASKBAR;
}

SDL_Surface * __SDL_CreateRGBSurfaceWithFormat(int w, int h, int depth, uint32_t format, uint_fast8_t pixel_size)
{
    SDL_Surface *surface = vsf_heap_malloc(sizeof(SDL_Surface) + pixel_size * w * h);
    if (surface != NULL) {
        if (format == SDL_PIXELFORMAT_PALETTE) {
            surface->__format       = *(SDL_PixelFormat *)__SDL_GetFormatFromColor(format);
            surface->__format.palette = vsf_heap_malloc(sizeof(SDL_Palette) + 256 * sizeof(SDL_Color));
            if (NULL == surface->__format.palette) {
                vsf_heap_free(surface);
                return NULL;
            }
            surface->__format.palette->ncolors = 256;
            surface->__format.palette->colors = (SDL_Color *)&surface->__format.palette[1];

            surface->format         = &surface->__format;
        } else {
            surface->__format.palette = NULL;
            surface->format         = (SDL_PixelFormat *)__SDL_GetFormatFromColor(format);
        }
        surface->w                  = w;
        surface->h                  = h;
        SDL_SetClipRect(surface, NULL);
    }
    return surface;
}

SDL_Surface * SDL_CreateRGBSurfaceWithFormat(uint32_t flags, int w, int h, int depth, uint32_t format)
{
    uint_fast8_t pixel_size = vsf_disp_get_pixel_format_bytesize(format);
    SDL_Surface *surface = __SDL_CreateRGBSurfaceWithFormat(w, h, depth, format, pixel_size);
    if (surface != NULL) {
        surface->pitch              = w * pixel_size;
        surface->pixels             = &surface->__pixels;
    }
    return surface;
}

SDL_Surface * SDL_CreateRGBSurfaceWithFormatFrom(void * pixels, int w, int h, int depth, int pitch, uint32_t format)
{
    SDL_Surface * surface = __SDL_CreateRGBSurfaceWithFormat(w, h, depth, format, 0);
    if (surface != NULL) {
        surface->flags              |= SDL_PREALLOC;
        surface->pitch              = pitch;
        surface->pixels             = pixels;
    }
    return surface;
}

static void __SDL_SurfaceInitFormat(SDL_Surface * surface, uint32_t color, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask)
{
    if (NULL == surface->format) {
        SDL_PixelFormat *format = (SDL_PixelFormat *)__SDL_GetFormatFromColor(color);
        if (format != NULL) {
            surface->format             = format;
        } else {
            __SDL_InitFormatMask(&surface->__format, Rmask, Gmask, Bmask, Amask);
        }
    }
}

SDL_Surface * SDL_CreateRGBSurface(uint32_t flags, int w, int h, int depth, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask)
{
    uint32_t format = __SDL_GetColorFromMask(Rmask, Gmask, Bmask, Amask);
    VSF_SDL2_ASSERT(format != SDL_PIXELFORMAT_UNKNOWN);
    SDL_Surface * surface = SDL_CreateRGBSurfaceWithFormat(flags, w, h, depth, format);
    if (surface != NULL) {
        __SDL_SurfaceInitFormat(surface, format, Rmask, Gmask, Bmask, Amask);
    }
    return surface;
}

SDL_Surface * SDL_CreateRGBSurfaceFrom(void * pixels, int w, int h, int depth, int pitch, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask)
{
    uint32_t format = __SDL_GetColorFromMask(Rmask, Gmask, Bmask, Amask);
    VSF_SDL2_ASSERT(format != SDL_PIXELFORMAT_UNKNOWN);
    SDL_Surface * surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels, w, h, depth, pitch, format);
    if (surface != NULL) {
        __SDL_SurfaceInitFormat(surface, format, Rmask, Gmask, Bmask, Amask);
    }
    return surface;
}

void SDL_FreeSurface(SDL_Surface *surface)
{
    VSF_SDL2_ASSERT(surface != NULL);
    if (surface->__format.palette != NULL) {
        vsf_heap_free(surface->__format.palette);
    }
    vsf_heap_free(surface);
}

int SDL_LockSurface(SDL_Surface *surface)
{
    return 0;
}

void SDL_UnlockSurface(SDL_Surface *surface)
{
}

int SDL_SetSurfaceBlendMode(SDL_Surface * surface, SDL_BlendMode blendMode)
{
    surface->blend = blendMode;
    return 0;
}

int SDL_SetColorKey(SDL_Surface * surface, int flag, uint32_t key)
{
    return 0;
}

int SDL_FillRect(SDL_Surface * surface, const SDL_Rect * rect, uint32_t color)
{
    uint_fast8_t pixel_size = surface->format->BytesPerPixel;
    SDL_Rect surface_rect = {
        .w      = surface->w,
        .h      = surface->h,
    };
    SDL_Rect realrect;

    if (NULL == rect) {
        rect = &surface_rect;
    }
    if (!SDL_IntersectRect(rect, &surface_rect, &realrect)) {
        return 0;
    }
    vsf_sdl2_pixel_fill(rect->h, rect->w, surface->pixels, surface->pitch, color, pixel_size);
    return 0;
}

int SDL_BlitSurface(SDL_Surface * src, const SDL_Rect * srcrect, SDL_Surface * dst, SDL_Rect * dstrect)
{
    VSF_SDL2_ASSERT((src != NULL) && (dst != NULL));

    SDL_Rect src_real_area, dst_real_area;
    SDL_Rect src_area = {
        .w      = src->w,
        .h      = src->h,
    };
    SDL_Rect dst_area = dst->clip_rect;

    if (srcrect != NULL) {
        if (!SDL_IntersectRect(srcrect, &src_area, &src_real_area)) {
            return -1;
        }
    } else {
        src_real_area = src_area;
    }
    if (dstrect != NULL) {
        if (!SDL_IntersectRect(dstrect, &dst_area, &dst_real_area)) {
            return -1;
        }
    } else {
        dst_real_area = dst_area;
    }
    dst_real_area.w = src_real_area.w = vsf_min(dst_real_area.w, src_real_area.w);
    dst_real_area.h = src_real_area.h = vsf_min(dst_real_area.h, src_real_area.h);

    uint_fast8_t dst_pixel_size = vsf_disp_get_pixel_format_bytesize(dst->format->format);
    uint_fast8_t src_pixel_size = vsf_disp_get_pixel_format_bytesize(src->format->format);
    __SDL_BlendWithFormat(src_real_area.h, src_real_area.w,
                (uint8_t *)dst->pixels + dst_pixel_size * (dst_real_area.y * dst_area.w + dst_real_area.x), dst->pitch,
                (uint8_t *)src->pixels + src_pixel_size * (src_real_area.y * src_area.w + src_real_area.x), src->pitch,
                dst->format, src->format);
    return 0;
}

SDL_Surface * SDL_ConvertSurfaceFormat(SDL_Surface * src, uint32_t format, uint32_t flags)
{
    uint_fast8_t depth = vsf_disp_get_pixel_format_bitsize(format);
    SDL_Surface * surface_new = SDL_CreateRGBSurfaceWithFormat(flags, src->w, src->h, depth, format);
    if (surface_new != NULL) {
        SDL_BlitSurface(src, NULL, surface_new, NULL);
    }
    return surface_new;
}

SDL_Surface * SDL_ConvertSurface(SDL_Surface * src, const SDL_PixelFormat * format, uint32_t flags)
{
    return SDL_ConvertSurfaceFormat(src, format->format, flags);
}

SDL_bool SDL_SetClipRect(SDL_Surface * surface, const SDL_Rect * rect)
{
    SDL_Rect full_rect = {
        .x      = 0,
        .y      = 0,
        .w      = surface->w,
        .h      = surface->h,
    };

    if (!rect) {
        surface->clip_rect = full_rect;
        return SDL_TRUE;
    }
    return SDL_IntersectRect(rect, &full_rect, &surface->clip_rect);
}

void SDL_GetClipRect(SDL_Surface * surface, SDL_Rect * rect)
{
    if (surface && rect) {
        *rect = surface->clip_rect;
    }
}




SDL_Renderer * SDL_CreateRenderer(SDL_Window *window, int index, uint32_t flags)
{
    SDL_Renderer *renderer;
    VSF_SDL2_ASSERT(window != NULL);
    renderer = vsf_heap_malloc(sizeof(SDL_Renderer));
    if (renderer != NULL) {
        renderer->scale_x   = 1.0;
        renderer->scale_y   = 1.0;
        renderer->window    = window;
        renderer->flags     = flags;
        SDL_RenderClear(renderer);
    }
    return renderer;
}

void SDL_DestroyRenderer(SDL_Renderer *renderer)
{
    VSF_SDL2_ASSERT(renderer != NULL);
    vsf_heap_free(renderer);
}

int SDL_RenderClear(SDL_Renderer *renderer)
{
    VSF_SDL2_ASSERT(renderer != NULL);
    SDL_Window *window = renderer->window;

    SDL_PixelFormat *format = (SDL_PixelFormat *)__SDL_GetFormatFromColor(window->format);
    VSF_SDL2_ASSERT(format != NULL);
    SDL_FillRect(&window->surface, NULL, format->Amask);
    return 0;
}

int SDL_RenderCopy(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_Rect *srcrect, const SDL_Rect *dstrect)
{
    VSF_SDL2_ASSERT((renderer != NULL) && (texture != NULL));

    SDL_Window *window = renderer->window;
    SDL_Rect src_area, dst_area;
    SDL_Rect texture_area = {
        .w = texture->w,
        .h = texture->h,
    };

    if (srcrect != NULL) {
        if (!SDL_IntersectRect(srcrect, &texture_area, &src_area)) {
            return -1;
        }
    } else {
        src_area = texture_area;
    }
    if (dstrect != NULL) {
        if (!SDL_IntersectRect(dstrect, &window->area, &dst_area)) {
            return -1;
        }
    } else {
        dst_area = window->area;
    }
    dst_area.w = src_area.w = vsf_min(dst_area.w, src_area.w);
    dst_area.h = src_area.h = vsf_min(dst_area.h, src_area.h);

    uint_fast8_t dst_pixel_size = vsf_disp_get_pixel_format_bytesize(renderer->window->format);
    uint_fast8_t src_pixel_size = vsf_disp_get_pixel_format_bytesize(texture->format->format);
    SDL_PixelFormat *dst_fmt = (SDL_PixelFormat *)__SDL_GetFormatFromColor(renderer->window->format);
    __SDL_BlendWithFormat(src_area.h, src_area.w,
                (uint8_t *)window->pixels + dst_pixel_size * (dst_area.y * window->area.w + dst_area.x), dst_pixel_size * window->area.w,
                (uint8_t *)texture->pixels + src_pixel_size * (src_area.y * texture->w + src_area.x), src_pixel_size * texture->w,
                dst_fmt, texture->format);
    return 0;
}

int SDL_RenderCopyEx(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect,
            const double angle, const SDL_Point *center, const SDL_RendererFlip flip)
{
    VSF_SDL2_ASSERT(false);
    return -1;
}

void SDL_RenderPresent(SDL_Renderer *renderer)
{
    vk_disp_area_t area = {
        .pos.x          = 0,
        .pos.y          = 0,
        .size.x         = renderer->window->area.w,
        .size.y         = renderer->window->area.h,
    };
    __vsf_sdl2_disp_refresh(&area, renderer->window->pixels);
}

int SDL_SetRenderDrawBlendMode(SDL_Renderer * renderer, SDL_BlendMode blendMode)
{
    renderer->blend_mode = blendMode;
    return 0;
}

int SDL_GetRenderDrawBlendMode(SDL_Renderer * renderer, SDL_BlendMode *blendMode)
{
    if (blendMode != NULL) {
        *blendMode = renderer->blend_mode;
    }
    return 0;
}

int SDL_RenderFillRect(SDL_Renderer * renderer, const SDL_Rect * rect)
{
    VSF_SDL2_ASSERT(false);
    return -1;
}

int SDL_SetRenderDrawColor(SDL_Renderer * renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    renderer->color = SDL_MapRGBA(renderer->window->surface.format, r, g, b, a);
    return 0;
}

int SDL_RenderDrawPoint(SDL_Renderer * renderer, int x, int y)
{
    const SDL_Rect rect = { .x = x, .y = y, .w = 1, .h = 1 };
    SDL_FillRect(&renderer->window->surface, &rect, renderer->color);
    return 0;
}

int SDL_RenderDrawRect(SDL_Renderer * renderer, const SDL_Rect * rect)
{
    VSF_SDL2_ASSERT(false);
    return -1;
}

int SDL_SetRenderTarget(SDL_Renderer *renderer, SDL_Texture *texture)
{
    renderer->target = texture;
    return 0;
}

SDL_Texture * SDL_GetRenderTarget(SDL_Renderer *renderer)
{
    return renderer->target;
}

void SDL_RenderGetViewport(SDL_Renderer * renderer, SDL_Rect * rect)
{
    if (rect != NULL) {
        *rect = renderer->view_port;
    }
}

int SDL_RenderSetViewport(SDL_Renderer * renderer, const SDL_Rect * rect)
{
    if (rect != NULL) {
        renderer->view_port = *rect;
    }
    return 0;
}

void SDL_RenderGetLogicalSize(SDL_Renderer * renderer, int *w, int *h)
{
    if (w != NULL) {
        *w = renderer->logic_w;
    }
    if (h != NULL) {
        *h = renderer->logic_h;
    }
}

int SDL_RenderSetLogicalSize(SDL_Renderer * renderer, int w, int h)
{
    renderer->logic_w = w;
    renderer->logic_h = h;
    return 0;
}

void SDL_RenderGetScale(SDL_Renderer * renderer, float *scaleX, float *scaleY)
{
    if (scaleX != NULL) {
        *scaleX = renderer->scale_x;
    }
    if (scaleY != NULL) {
        *scaleY = renderer->scale_y;
    }
}

int SDL_RenderSetScale(SDL_Renderer * renderer, float scaleX, float scaleY)
{
    renderer->scale_x = scaleX;
    renderer->scale_y = scaleY;
    return 0;
}


SDL_Texture * SDL_CreateTexture(SDL_Renderer *renderer, uint32_t format, int access, int w, int h)
{
    uint_fast8_t pixel_size = vsf_disp_get_pixel_format_bytesize(format);
    SDL_Texture *texture = vsf_heap_malloc(sizeof(SDL_Texture) + pixel_size * w * h);
    if (texture != NULL) {
        texture->format = (SDL_PixelFormat *)__SDL_GetFormatFromColor(format);
        VSF_SDL2_ASSERT(texture->format != NULL);
        texture->w      = w;
        texture->h      = h;
        texture->pixels = &texture->__pixels;
        // TODO: initialize pixels according to format
    }
    return texture;
}

SDL_Texture * SDL_CreateTextureFromSurface(SDL_Renderer *renderer, SDL_Surface *surface)
{
    SDL_Texture *texture = vsf_heap_malloc(sizeof(SDL_Texture));
    if (texture != NULL) {
        texture->format = surface->format;
        texture->w      = surface->w;
        texture->h      = surface->h;
        texture->pixels = surface->pixels;
        // TODO: initialize pixels according to format
    }
    return texture;
}

void SDL_DestroyTexture(SDL_Texture *texture)
{
    VSF_SDL2_ASSERT(texture != NULL);
    vsf_heap_free(texture);
}

int SDL_UpdateTexture(SDL_Texture *texture, const SDL_Rect *rect, const void *pixels, int pitch)
{
    SDL_Rect area;
    VSF_SDL2_ASSERT(texture != NULL);

    if (rect != NULL) {
        area    = *rect;
    } else {
        area.x  = 0;
        area.y  = 0;
        area.w  = texture->w;
        area.h  = texture->h;
    }

    uint_fast8_t pixel_size = vsf_disp_get_pixel_format_bytesize(texture->format->format);
    vsf_sdl2_pixel_copy(area.h, pixel_size * area.w,
                (uint8_t *)texture->pixels + pixel_size * (area.y * texture->w + area.x), pixel_size * texture->w,
                (uint8_t *)pixels, pitch);
    return 0;
}

int SDL_LockTexture(SDL_Texture * texture, const SDL_Rect * rect, void **pixels, int *pitch)
{
    uint_fast8_t pixel_size = vsf_disp_get_pixel_format_bytesize(texture->format->format);
    uint8_t *p = texture->pixels;
    SDL_Rect area;

    if (rect != NULL) {
        area    = *rect;
    } else {
        area.x  = 0;
        area.y  = 0;
        area.w  = texture->w;
        area.h  = texture->h;
    }

    if (pitch != NULL) {
        *pitch = pixel_size * texture->w;
    }
    if (pixels != NULL) {
        *pixels = (void *)&p[pixel_size * (area.y * texture->w + area.x)];
    }
    return 0;
}

void SDL_UnlockTexture(SDL_Texture * texture)
{
}

int SDL_SetTextureBlendMode(SDL_Texture * texture, SDL_BlendMode blendMode)
{
    texture->blend_mode = blendMode;
    return 0;
}

int SDL_GetTextureBlendMode(SDL_Texture * texture, SDL_BlendMode *blendMode)
{
    if (blendMode != NULL) {
        *blendMode = texture->blend_mode;
    }
    return 0;
}

int SDL_SetTextureColorMod(SDL_Texture * texture, uint8_t r, uint8_t g, uint8_t b) { return -1; }
int SDL_GetTextureColorMod(SDL_Texture * texture, uint8_t * r, uint8_t * g, uint8_t * b) { return -1; }
int SDL_SetTextureAlphaMod(SDL_Texture * texture, uint8_t alpha) { return -1; }
int SDL_GetTextureAlphaMod(SDL_Texture * texture, uint8_t * alpha) { return -1; }



uint32_t SDL_MapRGB(const SDL_PixelFormat * format, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t color =    ((r >> format->Rloss) << format->Rshift)
                    |   ((g >> format->Gloss) << format->Gshift)
                    |   ((b >> format->Bloss) << format->Bshift);
    return color;
}

uint32_t SDL_MapRGBA(const SDL_PixelFormat * format, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    uint32_t color = SDL_MapRGB(format, r, g, b);
    if (format->Amask != 0) {
        color |= (a >> format->Aloss) << format->Ashift;
    }
    return color;
}

SDL_PixelFormat * SDL_AllocFormat(uint32_t format)
{
    return (SDL_PixelFormat *)__SDL_GetFormatFromColor(format);
}
void SDL_FreeFormat(SDL_PixelFormat *format)
{
}




SDL_sem * SDL_CreateSemaphore(uint32_t initial_value)
{
    return NULL;
}
void SDL_DestroySemaphore(SDL_sem *sem)
{
}
int __vsf_sdl2_sem_wait(SDL_sem *sem, int32_t ms)
{
    return -1;
}
int SDL_SemPost(SDL_sem *sem)
{
    return -1;
}

// timer
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
void SDL_Delay(uint32_t ms)
{
    if (ms > 0) {
        vsf_teda_set_timer_ms(ms);
        vsf_thread_wfe(VSF_EVT_TIMER);
    }
}
uint32_t SDL_GetTicks(void)
{
    uint32_t cur_ms = vsf_systimer_get_ms();
    return cur_ms - __vsf_sdl2.start_ms;
}
uint64_t SDL_GetPerformanceCounter(void)
{
    return vsf_systimer_get_tick();
}
uint64_t SDL_GetPerformanceFrequency(void)
{
    return vsf_systimer_get_freq();
}

typedef struct SDL_Timer {
    vsf_callback_timer_t timer;

    SDL_TimerCallback callback;
    uint32_t interval;
    void *param;
} SDL_Timer;
static void __SDL_OnTimer(vsf_callback_timer_t *cbtimer)
{
    SDL_Timer *timer = container_of(cbtimer, SDL_Timer, timer);
    uint32_t interval = timer->callback(timer->interval, timer->param);
    if (interval != 0) {
        vsf_callback_timer_add(&timer->timer, vsf_systimer_ms_to_tick(interval));
    }
}
SDL_TimerID SDL_AddTimer(uint32_t interval, SDL_TimerCallback callback, void *param)
{
    SDL_Timer *timer = malloc(sizeof(SDL_Timer));
    if (NULL == timer) {
        return (SDL_TimerID)0;
    }

    timer->callback = callback;
    timer->interval = interval;
    timer->param = param;
    timer->timer.on_timer = __SDL_OnTimer;
    vsf_callback_timer_add(&timer->timer, vsf_systimer_ms_to_tick(interval));
    return (SDL_TimerID)timer;
}
SDL_bool SDL_RemoveTimer(SDL_TimerID id)
{
    SDL_Timer *timer = (SDL_Timer *)id;
    vsf_callback_timer_remove(&timer->timer);
    free(timer);
    return SDL_TRUE;
}
#endif

int SDL_GetDisplayDPI(int displayIndex, float *ddpi, float *hdpi, float *vdpi)
{
    if (ddpi != NULL) {
        *ddpi = 1.0;
    }
    if (hdpi != NULL) {
        *hdpi = 1.0;
    }
    if (vdpi != NULL) {
        *vdpi = 1.0;
    }
    return 0;
}

void SDL_EnableScreenSaver(void)
{
}

SDL_bool SDL_HasClipboardText(void)
{
    return (NULL == __vsf_sdl2.clipboard.text) ? SDL_FALSE : SDL_TRUE;
}

int SDL_SetClipboardText(const char * text)
{
    if (__vsf_sdl2.clipboard.text != NULL) {
        SDL_free(__vsf_sdl2.clipboard.text);
        __vsf_sdl2.clipboard.text = NULL;
    }
    __vsf_sdl2.clipboard.text = SDL_malloc(strlen(text) + 1);
    if (__vsf_sdl2.clipboard.text != NULL) {
        strcpy(__vsf_sdl2.clipboard.text, text);
        return 0;
    }
    return -1;
}

char * SDL_GetClipboardText(void)
{
    if (NULL == __vsf_sdl2.clipboard.text) {
        return NULL;
    }

    char *result = SDL_malloc(strlen(__vsf_sdl2.clipboard.text) + 1);
    strcpy(result, __vsf_sdl2.clipboard.text);
    return result;
}

// audio
#if VSF_USE_AUDIO == ENABLED
static void __sdl_audio_playback_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    uint_fast32_t cur_size;
    uint8_t *cur_buff;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_OUT:
        if (__vsf_sdl2.audio.state == SDL_AUDIO_PLAYING) {
            cur_size = vsf_stream_get_wbuf(stream, &cur_buff);
            if (cur_size && __vsf_sdl2.audio.spec.callback != NULL) {
                __vsf_sdl2.audio.spec.callback(__vsf_sdl2.audio.spec.userdata,
                    cur_buff, cur_size);
                vsf_stream_write(stream, cur_buff, cur_size);
            }
        }
        break;
    }
}
#endif

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained)
{
#if VSF_USE_AUDIO == ENABLED
    vk_audio_format_t fmt = {
        .channel_num        = desired->channels,
        .sample_bit_width   = SDL_AUDIO_BITSIZE(desired->format),
        .sample_rate        = desired->freq,
    };
    // double buffer mode
    desired->size = (fmt.sample_bit_width >> 3) * desired->samples * 2;
    desired->silence = 0;

    if (    (__vsf_sdl2.audio.raw_stream != NULL)
        ||  (!desired->samples)) {
        return -1;
    }

    vsf_mem_stream_t *raw_stream = calloc(1, sizeof(vsf_mem_stream_t) + desired->size);
    if (NULL == raw_stream) {
        return -1;
    }
    raw_stream->op = &vsf_mem_stream_op;
    raw_stream->buffer = (uint8_t *)&raw_stream[1];
    raw_stream->size = desired->size;
    raw_stream->is_ticktock_read = true;
    VSF_STREAM_INIT(raw_stream);
    raw_stream->tx.param = NULL;
    raw_stream->tx.evthandler = __sdl_audio_playback_evthandler;
    VSF_STREAM_CONNECT_TX(raw_stream);
    __vsf_sdl2.audio.raw_stream = &raw_stream->use_as__vsf_stream_t;

    vsf_err_t err = vk_audio_start(
                __vsf_sdl2.audio.dev,
                __vsf_sdl2.audio.stream->stream_index,
                __vsf_sdl2.audio.raw_stream,
                &fmt);
    if (VSF_ERR_NONE != err) {
        SDL_CloseAudio();
        return -1;
    }

    __vsf_sdl2.audio.spec = *desired;
    __vsf_sdl2.audio.state = SDL_AUDIO_PAUSED;
    return 0;
#else
    return -1;
#endif
}
void SDL_PauseAudio(int pause_on)
{
#if VSF_USE_AUDIO == ENABLED
    if (pause_on) {
        if (__vsf_sdl2.audio.state == SDL_AUDIO_PLAYING) {
            __vsf_sdl2.audio.state = SDL_AUDIO_PAUSED;
        }
    } else {
        if (__vsf_sdl2.audio.state == SDL_AUDIO_PAUSED) {
            __vsf_sdl2.audio.state = SDL_AUDIO_PLAYING;
            __sdl_audio_playback_evthandler(__vsf_sdl2.audio.raw_stream,
                NULL, VSF_STREAM_ON_OUT);
        }
    }
#else
#endif
}
SDL_AudioStatus SDL_GetAudioStatus(void)
{
#if VSF_USE_AUDIO == ENABLED
    return __vsf_sdl2.audio.state;
#else
    return SDL_AUDIO_STOPPED;
#endif
}
void SDL_CloseAudio(void)
{
#if VSF_USE_AUDIO == ENABLED
    __vsf_sdl2.audio.state = SDL_AUDIO_STOPPED;
    vk_audio_stop(__vsf_sdl2.audio.dev, __vsf_sdl2.audio.stream->stream_index);
    if (__vsf_sdl2.audio.raw_stream != NULL) {
        free(__vsf_sdl2.audio.raw_stream);
        __vsf_sdl2.audio.raw_stream = NULL;
    }
#else
#endif
}
void SDL_LockAudio(void)
{
}
void SDL_UnlockAudio(void)
{
}

// cursor
SDL_Cursor * SDL_CreateCursor(const uint8_t *data, const uint8_t *mask,
                                            int w, int h, int hot_x, int hot_y)
{
    return NULL;
}
SDL_Cursor * SDL_CreateColorCursor(SDL_Surface *surface, int hot_x, int hot_y)
{
    return NULL;
}
SDL_Cursor * SDL_CreateSystemCursor(SDL_SystemCursor id)
{
    return NULL;
}
void SDL_SetCursor(SDL_Cursor *cursor)
{
}
SDL_Cursor * SDL_GetCursor(void)
{
    return NULL;
}
SDL_Cursor * SDL_GetDefaultCursor(void)
{
    return NULL;
}
void SDL_FreeCursor(SDL_Cursor *cursor)
{
}
int SDL_ShowCursor(int toggle)
{
    return -1;
}

int SDL_SetWindowHitTest(SDL_Window * window, SDL_HitTest callback, void *callback_data)
{
    return -1;
}

// SDL_error.h
int SDL_Error(SDL_errorcode code)
{
    SDL_SetError("errcode %d", code);
    return 0;
}
const char *SDL_GetError(void)
{
    return "unknown error";
}
int SDL_SetError(const char *fmt, ...)
{
    return 0;
}
void SDL_ClearError(void)
{
}

#if VSF_SDL_CFG_V1_COMPATIBLE == ENABLED
// overlay
SDL_Overlay * SDL_CreateYUVOverlay(int width, int height, uint32_t format, SDL_Surface *display)
{
    return NULL;
}
void SDL_FreeYUVOverlay(SDL_Overlay *overlay)
{
}
int SDL_LockYUVOverlay(SDL_Overlay *overlay)
{
    return 0;
}
void SDL_UnlockYUVOverlay(SDL_Overlay *overlay)
{
}
int SDL_DisplayYUVOverlay(SDL_Overlay *overlay, SDL_Rect *dstrect)
{
    return -1;
}

SDL_Surface * SDL_SetVideoMode(int width, int height, int bpp, uint32_t flags)
{
    SDL_Surface *surface = NULL;

    if (__vsf_sdl2.sdl1_screen != NULL) {
        vsf_heap_free(__vsf_sdl2.sdl1_screen);
        __vsf_sdl2.sdl1_screen = NULL;
    }

    int bytes_per_pixel = vsf_disp_get_pixel_bytesize(__vsf_sdl2.disp);
    int bits_per_pixel = vsf_disp_get_pixel_bitsize(__vsf_sdl2.disp);
    VSF_UNUSED_PARAM(bits_per_pixel);
    if (bpp != 0) {
        VSF_SDL2_ASSERT(bpp == bits_per_pixel);
    }

    surface = vsf_heap_malloc(sizeof(SDL_Surface) + (width * height * bytes_per_pixel));
    if (surface != NULL) {
        surface->w = width;
        surface->h = height;
        surface->pitch = width * bytes_per_pixel;
        surface->pixels = &surface->__pixels;
        surface->__format.palette = NULL;
        SDL_SetClipRect(surface, NULL);

        vk_disp_color_type_t color_type = vsf_disp_get_pixel_format(__vsf_sdl2.disp);
        switch (color_type) {
        case VSF_DISP_COLOR_RGB332:
        case VSF_DISP_COLOR_RGB565:
        case VSF_DISP_COLOR_ARGB8888: {
                SDL_PixelFormat *format = (SDL_PixelFormat *)__SDL_GetFormatFromColor(color_type);
                VSF_SDL2_ASSERT(format != NULL);
                surface->__format = *format;
            }
            break;
        default:
            VSF_SDL2_ASSERT(false);
            vsf_heap_free(surface);
            return NULL;
        }
        surface->format = &surface->__format;
    }
    __vsf_sdl2.sdl1_screen = surface;
    return surface;
}
int SDL_Flip(SDL_Surface *screen)
{
    vk_disp_area_t area = {
        .pos.x  = 0,
        .pos.y  = 0,
        .size.x = screen->w,
        .size.y = screen->h,
    };
    __vsf_sdl2_disp_refresh(&area, screen->pixels);
    return 0;
}

int SDL_EnableUNICODE(int enable)
{
    return 0;
}

int SDL_EnableKeyRepeat(int delay, int interval)
{
    return 0;
}

void SDL_WM_SetCaption(const char *title, const char *icon)
{
}
#endif

#endif      // VSF_USE_SDL2
