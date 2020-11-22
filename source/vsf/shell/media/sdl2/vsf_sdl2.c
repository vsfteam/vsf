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

#include "./vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#define __VSF_DISP_CLASS_INHERIT__
#include "component/ui/vsf_ui.h"
#include "./include/SDL2/SDL.h"

/*============================ MACROS ========================================*/

#if VSF_INPUT_CFG_REGISTRATION_MECHANISM != ENABLED
#   error VSF_INPUT_CFG_REGISTRATION_MECHANISM is needed for vsf_sdl2
#endif

#if VSF_KERNEL_CFG_SUPPORT_THREAD != ENABLED
#   error vsf_sdl2 can only run in thread mode
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct SDL_Window {
    const char *title;
    SDL_Rect area;
    uint32_t flags;
    vk_disp_color_type_t format;
    uint32_t pixels[0] ALIGN(4);
};

struct SDL_Renderer {
    SDL_Window *window;
    uint32_t flags;
};

struct SDL_Texture {
    uint16_t w, h;
    vk_disp_color_type_t format;
    void *pixels;

    uint32_t __pixels[0] ALIGN(4);
};

typedef struct vsf_sdl2_t {
    vk_disp_t *disp;
    uint32_t init_flags;
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    uint32_t start_ms;
#endif
#if VSF_SDL_CFG_V1_COMPATIBLE == ENABLED
    SDL_Surface *sdl1_screen;
#endif
    vk_input_notifier_t notifier;
} vsf_sdl2_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

NO_INIT vsf_sdl2_t __vsf_sdl2;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_sdl2_on_input(vk_input_type_t type, vk_input_evt_t *evt)
{
}

static void __vsf_sdl2_disp_on_ready(vk_disp_t *disp)
{
    vsf_eda_t *eda = __vsf_sdl2.disp->ui_data;
    VSF_SDL2_ASSERT(eda != NULL);
    vsf_eda_post_evt(eda, VSF_EVT_RETURN);
}

static void __vsf_sdl2_pixel_copy(  uint_fast16_t data_line_num, uint_fast32_t data_line_size,
                                    uint8_t *pdst, uint_fast32_t dst_pitch,
                                    uint8_t *psrc, uint_fast32_t src_pitch)
{
    for (uint_fast16_t i = 0; i < data_line_num; i++) {
        memcpy(pdst, psrc, data_line_size);
        pdst += dst_pitch;
        psrc += src_pitch;
    }
}

static inline bool __vsf_sdl2_cord_intersect(int *start_out, int *width_out, int start0, int width0, int start1, int width1)
{
    int start = max(start0, start1);
    int end0 = start0 + width0;
    int end1 = start1 + width1;
    int end = min(end0, end1);

    *start_out  = start;
    *width_out = end - start;
    return end > start;
}

static bool __vsf_sdl2_rect_intersect(SDL_Rect *rect_out, const SDL_Rect *rect0, const SDL_Rect *rect1)
{
    VSF_SDL2_ASSERT((rect_out != NULL) && (rect0 != NULL) && (rect1 != NULL));
    bool x_intersect = __vsf_sdl2_cord_intersect(&rect_out->x, &rect_out->w, rect0->x, rect0->w, rect1->x, rect1->w);
    bool y_intersect = __vsf_sdl2_cord_intersect(&rect_out->y, &rect_out->h, rect0->y, rect0->h, rect1->y, rect1->h);
    return x_intersect && y_intersect;
}

void vsf_sdl2_init(vk_disp_t *disp)
{
    __vsf_sdl2.init_flags = 0;
    __vsf_sdl2.disp = disp;
    __vsf_sdl2.sdl1_screen = NULL;
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
    __vsf_sdl2.init_flags = flags;
    if (flags & SDL_INIT_EVENTS) {
        __vsf_sdl2.notifier.mask = 1 << VSF_INPUT_TYPE_TOUCHSCREEN;
        __vsf_sdl2.notifier.on_evt = (vk_input_on_evt_t)__vsf_sdl2_on_input;
        vk_input_notifier_register(&__vsf_sdl2.notifier);
    }

    if (flags & SDL_INIT_VIDEO) {
        __vsf_sdl2.disp->ui_data = NULL;
        __vsf_sdl2.disp->ui_on_ready = __vsf_sdl2_disp_on_ready;
        vk_disp_init(__vsf_sdl2.disp);
    }
    return 0;
}

int SDL_Init(uint32_t flags)
{
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    __vsf_sdl2.start_ms = vsf_systimer_get_ms();
#endif
    return SDL_InitSubSystem(flags);
}

void SDL_Quit(void)
{
    uint32_t flags = __vsf_sdl2.init_flags;
    if (flags & SDL_INIT_EVENTS) {
        vk_input_notifier_unregister(&__vsf_sdl2.notifier);
    }

    if (flags & SDL_INIT_VIDEO) {
        // TODO:
    }
}

int SDL_GetDesktopDisplayMode(int display_index, SDL_DisplayMode *mode)
{
    if (NULL == __vsf_sdl2.disp) {
        return -1;
    }

    if (mode != NULL) {
        mode->format = vsf_disp_get_pixel_format(__vsf_sdl2.disp);
        mode->h = __vsf_sdl2.disp->param.height;
        mode->w = __vsf_sdl2.disp->param.width;
    }
    return 0;
}

const char *SDL_GetError(void)
{
    return NULL;
}


SDL_Window * SDL_CreateWindow(const char *title, int x, int y, int w, int h, uint32_t flags)
{
    uint_fast8_t pixel_size = vsf_disp_get_pixel_bytesize(__vsf_sdl2.disp);
    SDL_Window *window = vsf_heap_malloc(sizeof(struct SDL_Window) + pixel_size * w * h);
    if (window != NULL) {
        window->title   = title;
        window->area.x  = x;
        window->area.y  = y;
        window->area.w  = w;
        window->area.h  = h;
        window->flags   = flags;
        window->format  = vsf_disp_get_pixel_format(__vsf_sdl2.disp);
        // TODO: initialize pixels according to format of disp
    }
    return window;
}

void SDL_DestroyWindow(SDL_Window *window)
{
    VSF_SDL2_ASSERT(window != NULL);
    vsf_heap_free(window);
}

SDL_Surface * SDL_CreateRGBSurfaceWithFormat(uint32_t flags, int w, int h, int depth, uint32_t format)
{
    VSF_SDL2_ASSERT(__vsf_sdl2.disp != NULL);
    VSF_SDL2_ASSERT(format == __vsf_sdl2.disp->param.color);
    VSF_SDL2_ASSERT(depth == vsf_disp_get_pixel_bitsize(__vsf_sdl2.disp));

    uint_fast8_t pixel_size = vsf_disp_get_pixel_bytesize(__vsf_sdl2.disp);
    SDL_Surface *surface = vsf_heap_malloc(sizeof(SDL_Surface) + pixel_size * w * h);
    if (surface != NULL) {
        surface->__format.format = format;
        surface->format     = &surface->__format;

        surface->w          = w;
        surface->h          = h;

        surface->pitch      = w * pixel_size;
        surface->pixels     = &surface->__pixels;
    }
    return surface;
}

void SDL_FreeSurface(SDL_Surface *surface)
{
    VSF_SDL2_ASSERT(surface != NULL);
    vsf_heap_free(surface);
}

SDL_Renderer * SDL_CreateRenderer(SDL_Window *window, int index, uint32_t flags)
{
    SDL_Renderer *renderer;
    VSF_SDL2_ASSERT(window != NULL);
    renderer = vsf_heap_malloc(sizeof(struct SDL_Renderer));
    if (renderer != NULL) {
        renderer->window    = window;
        renderer->flags     = flags;
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
    uint_fast8_t pixel_size = vsf_disp_get_pixel_bytesize(__vsf_sdl2.disp);
    SDL_Window *window = renderer->window;

    // TODO: set to default color instead of 0
    memset(window->pixels, 0, pixel_size * window->area.w * window->area.h);
    return 0;
}

int SDL_RenderCopy(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_Rect *srcrect, const SDL_Rect *dstrect)
{
    VSF_SDL2_ASSERT((renderer != NULL) && (texture != NULL));
    // not support format conversion now
    VSF_SDL2_ASSERT(renderer->window->format == texture->format);

    SDL_Window *window = renderer->window;
    SDL_Rect src_area, dst_area;
    SDL_Rect texture_area = {
        .x = 0,
        .y = 0,
        .w = texture->w,
        .h = texture->h,
    };

    if (srcrect != NULL) {
        if (!__vsf_sdl2_rect_intersect(&src_area, srcrect, &texture_area)) {
            return -1;
        }
    } else {
        src_area = texture_area;
    }
    if (dstrect != NULL) {
        if (!__vsf_sdl2_rect_intersect(&dst_area, dstrect, &window->area)) {
            return -1;
        }
    } else {
        dst_area = window->area;
    }
    dst_area.w = src_area.w = min(dst_area.w, src_area.w);
    dst_area.h = src_area.h = min(dst_area.h, src_area.h);

    uint_fast8_t pixel_size = vsf_disp_get_pixel_format_bytesize(texture->format);
    __vsf_sdl2_pixel_copy(src_area.h, pixel_size * src_area.w,
                (uint8_t *)window->pixels + pixel_size * (dst_area.y * window->area.w + dst_area.x), pixel_size * window->area.w,
                (uint8_t *)texture->pixels + pixel_size * (src_area.y * texture->w + src_area.x), pixel_size * texture->w);
    return 0;
}

static void __vsf_sdl2_disp_refresh(vk_disp_area_t *area, void *pixels)
{
    __vsf_sdl2.disp->ui_data = vsf_eda_get_cur();
    vk_disp_refresh(__vsf_sdl2.disp, area, pixels);
    vsf_thread_wfe(VSF_EVT_RETURN);
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


SDL_Texture * SDL_CreateTexture(SDL_Renderer *renderer, uint32_t format, int access, int w, int h)
{
    uint_fast8_t pixel_size = vsf_disp_get_pixel_format_bytesize(format);
    SDL_Texture *texture = vsf_heap_malloc(sizeof(struct SDL_Renderer) + pixel_size * w * h);
    if (texture != NULL) {
        texture->format = format;
        texture->w      = w;
        texture->h      = h;
        texture->pixels = &texture->__pixels;
        // TODO: initialize pixels according to format
    }
    return texture;
}

SDL_Texture * SDL_CreateTextureFromSurface(SDL_Renderer *renderer, SDL_Surface *surface)
{
    SDL_Texture *texture = vsf_heap_malloc(sizeof(struct SDL_Renderer));
    if (texture != NULL) {
        texture->format = surface->format->format;
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

    uint_fast8_t pixel_size = vsf_disp_get_pixel_format_bytesize(texture->format);
    __vsf_sdl2_pixel_copy(area.h, pixel_size * area.w,
                (uint8_t *)texture->pixels + pixel_size * (area.y * texture->w + area.x), pixel_size * texture->w,
                (uint8_t *)pixels, pitch);
    return 0;
}


int SDL_LockSurface(SDL_Surface *surface)
{
    return 0;
}
void SDL_UnlockSurface(SDL_Surface *surface)
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
    vsf_teda_set_timer_ms(ms);
    vsf_thread_wfe(VSF_EVT_TIMER);
}
uint32_t SDL_GetTicks(void)
{
    uint32_t cur_ms = vsf_systimer_get_ms();
    return cur_ms - __vsf_sdl2.start_ms;
}
#endif

// audio
int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained)
{
    return -1;
}
void SDL_PauseAudio(int pause_on)
{
}
SDL_AudioStatus SDL_GetAudioStatus(void)
{
    return SDL_AUDIO_STOPPED;
}
void SDL_CloseAudio(void)
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
    if (!bpp) {
        bpp = vsf_disp_get_pixel_bitsize(__vsf_sdl2.disp);
    }
    surface = vsf_heap_malloc(sizeof(SDL_Surface) + (width * height * bpp >> 3));
    if (surface != NULL) {
        surface->w = width;
        surface->h = height;
        surface->pitch = width * bpp >> 3;

        surface->__format.BitsPerPixel = bpp;
        switch (vsf_disp_get_pixel_format(__vsf_sdl2.disp)) {
        case VSF_DISP_COLOR_RGB565:
            surface->__format.Rmask = ((1UL << 5) - 1) << 0;
            surface->__format.Gmask = ((1UL << 6) - 1) << 5;
            surface->__format.Bmask = ((1UL << 5) - 1) << 11;
            surface->__format.Amask = 0;
            break;
        case VSF_DISP_COLOR_ARGB8888:
            surface->__format.Rmask = ((1UL << 8) - 1) << 0;
            surface->__format.Gmask = ((1UL << 8) - 1) << 8;
            surface->__format.Bmask = ((1UL << 8) - 1) << 16;
            surface->__format.Amask = ((1UL << 8) - 1) << 24;
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

void SDL_WM_SetCaption(const char *title, const char *icon)
{
}
#endif


// event
// joysticks
int SDL_NumJoysticks(void)
{
    return 0;
}
SDL_Joystick * SDL_JoystickOpen(int device_index)
{
    return NULL;
}
int SDL_JoystickEventState(int state)
{
    return -1;
}
int SDL_JoystickNumButtons(SDL_Joystick *joystick)
{
    return 0;
}
int SDL_JoystickNumAxes(SDL_Joystick *joystick)
{
    return 0;
}
int SDL_JoystickNumBalls(SDL_Joystick *joystick)
{
    return 0;
}
int SDL_JoystickNumHats(SDL_Joystick *joystick)
{
    return 0;
}

int SDL_PollEvent(SDL_Event *event)
{
    VSF_SDL2_ASSERT(event != NULL);


    return 0;
}


#endif      // VSF_USE_SDL
