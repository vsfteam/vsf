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

#ifndef __VSF_SDL2_H__
#define __VSF_SDL2_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#include "vsf.h"

#include "SDL_cpuinfo.h"
#include "SDL_endian.h"
#include "SDL_stdinc.h"
#include "SDL_version.h"

#include "SDL_log.h"
#include "SDL_rwops.h"
#include "SDL_mutex.h"

#include "SDL_events.h"
#include "SDL_keyboard.h"
#include "SDL_mouse.h"

#include "SDL_surface.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_SDL_CFG_V1_COMPATIBLE
#   define VSF_SDL_CFG_V1_COMPATIBLE    ENABLED
#endif

#define DECLSPEC
#define SDLCALL
#define SDL_INLINE                      INLINE

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_SDL_CFG_FAKE_API == ENABLED
#define SDL_memset4                     __vsf_sdl2_memset4

#define SDL_Init                        __vsf_sdl2_init
#define SDL_InitSubSystem               __vsf_sdl2_init_subsystem
#define SDL_Quit                        __vsf_sdl2_quit
#define SDL_GetPlatform                 __vsf_sdl2_get_platform

#define SDL_SetError                    __vsf_sdl2_set_error
#define SDL_GetError                    __vsf_sdl2_get_error

#define SDL_CreateWindow                __vsf_sdl2_create_window
#define SDL_DestroyWindow               __vsf_sdl2_destroy_window
#define SDL_GetWindowPosition           __vsf_sdl2_get_window_position
#define SDL_GetWindowSurface            __vsf_sdl2_get_window_surface
#define SDL_GetWindowSize               __vsf_sdl2_get_window_size
#define SDL_UpdateWindowSurface         __vsf_sdl2_update_window_surface
#define SDL_UpdateWindowSurfaceRects    __vsf_sdl2_update_window_surface_rects
#define SDL_ShowWindow                  __vsf_sdl2_show_window
#define SDL_SetWindowIcon               __vsf_sdl2_set_window_icon
#define SDL_SetWindowTitle              __vsf_sdl2_set_window_title
#define SDL_SetWindowFullscreen         __vsf_sdl2_set_window_full_screen
#define SDL_RestoreWindow               __vsf_sdl2_restore_window
#define SDL_MaximizeWindow              __vsf_sdl2_maximize_window
#define SDL_GetWindowFlags              __vsf_sdl2_get_window_flags

#define SDL_GetDesktopDisplayMode       __vsf_sdl2_get_desktop_display_mode
#define SDL_GetCurrentDisplayMode       __vsf_sdl2_get_current_display_mode

#define SDL_CreateRGBSurface                __vsf_sdl2_create_rgb_surface
#define SDL_CreateRGBSurfaceWithFormat      __vsf_sdl2_create_rgb_surface_with_format
#define SDL_CreateRGBSurfaceWithFormatFrom  __vsf_sdl2_create_rgb_surface_with_format_from
#define SDL_CreateRGBSurfaceFrom        __vsf_sdl2_create_rgb_surface_from
#define SDL_FreeSurface                 __vsf_sdl2_free_surface
#define SDL_LockSurface                 __vsf_sdl2_lock_surface
#define SDL_UnlockSurface               __vsf_sdl2_unlock_surface
#define SDL_SetSurfaceBlendMode         __vsf_sdl2_set_surface_blend_mode
#define SDL_SetColorKey                 __vsf_sdl2_set_color_key
#define SDL_FillRect                    __vsf_sdl2_fill_rect
#define SDL_BlitSurface                 __vsf_sdl2_blit_surface
#define SDL_ConvertSurfaceFormat        __vsf_sdl2_convert_surface_format

#define SDL_CreateRenderer              __vsf_sdl2_create_renderer
#define SDL_DestroyRenderer             __vsf_sdl2_destroy_renderer
#define SDL_RenderClear                 __vsf_sdl2_render_clear
#define SDL_RenderCopy                  __vsf_sdl2_render_copy
#define SDL_RenderPresent               __vsf_sdl2_render_present
#define SDL_SetRenderDrawColor          __vsf_sdl2_set_render_draw_color
#define SDL_RenderDrawPoint             __vsf_sdl2_render_point
#define SDL_RenderSetLogicalSize(...)

#define SDL_CreateTexture               __vsf_sdl2_create_texture
#define SDL_CreateTextureFromSurface    __vsf_sdl2_create_texture_from_surface
#define SDL_UpdateTexture               __vsf_sdl2_update_texture
#define SDL_DestroyTexture              __vsf_sdl2_destroy_texture
#define SDL_LockTexture                 __vsf_sdl2_lock_texture
#define SDL_UnlockTexture               __vsf_sdl2_unlock_texture

#define SDL_MapRGBA                     __vsf_sdl2_map_rgba
#define SDL_MapRGB                      __vsf_sdl2_map_rgb

#define SDL_CreateSemaphore             __vsf_sdl2_create_sem
#define SDL_DestroySemaphore            __vsf_sdl2_destroy_sem
#define SDL_SemPost                     __vsf_sdl2_sem_post

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   define SDL_Delay                    __vsf_sdl2_delay
#   define SDL_GetTicks                 __vsf_sdl2_get_ticks
#endif

#define SDL_GetDisplayDPI               __vsf_sdl2_get_display_dpi
#define SDL_EnableScreenSaver           __vsf_sdl2_enable_screen_saver
#define SDL_HasClipboardText            __vsf_sdl2_has_clipboard_text
#define SDL_GetClipboardText            __vsf_sdl2_get_clipboard_text
#define SDL_SetClipboardText            __vsf_sdl2_set_clipboard_text
#define SDL_GetPerformanceCounter       __vsf_sdl2_get_performance_counter
#define SDL_GetPerformanceFrequency     __vsf_sdl2_get_performance_frequency

#define SDL_OpenAudio                   __vsf_sdl2_open_audio
#define SDL_PauseAudio                  __vsf_sdl2_pause_audio
#define SDL_GetAudioStatus              __vsf_sdl2_get_audio_status
#define SDL_CloseAudio                  __vsf_sdl2_close_audio

#define SDL_CreateCursor                __vsf_sdl2_create_cursor
#define SDL_CreateColorCursor           __vsf_sdl2_create_color_curosr
#define SDL_CreateSystemCursor          __vsf_sdl2_create_system_cursor
#define SDL_SetCursor                   __vsf_sdl2_set_curser
#define SDL_GetCursor                   __vsf_sdl2_get_cursor
#define SDL_GetDefaultCursor            __vsf_sdl2_get_default_cursor
#define SDL_FreeCursor                  __vsf_sdl2_free_curser
#define SDL_ShowCursor                  __vsf_sdl2_show_curser

#define SDL_GetKeyName                  __vsf_sdl2_get_key_name

#define SDL_NumJoysticks                __vsf_sdl2_num_joysticks
#define SDL_JoystickOpen                __vsf_sdl2_joystick_open
#define SDL_JoystickClose               __vsf_sdl2_joystick_close
#define SDL_JoystickNumButtons          __vsf_sdl2_joystick_num_buttons
#define SDL_JoystickNumAxes             __vsf_sdl2_joystick_num_axes
#define SDL_JoystickNumBalls            __vsf_sdl2_joystick_num_balls
#define SDL_JoystickNumHats             __vsf_sdl2_joystick_num_hats
#define SDL_JoystickEventState          __vsf_sdl2_joystick_event_state

#define SDL_CaptureMouse                __vsf_sdl2_capture_mouse
#define SDL_GetGlobalMouseState         __vsf_sdl2_get_global_mouse_state

#if VSF_SDL_CFG_V1_COMPATIBLE == ENABLED
#define SDL_CreateYUVOverlay            __vsf_sdl_create_yuv_overlay
#define SDL_FreeYUVOverlay              __vsf_sdl_free_yuv_overlay
#define SDL_LockYUVOverlay              __vsf_sdl_lock_yuv_overlay
#define SDL_UnlockYUVOverlay            __vsf_sdl_unlock_yuv_overlay
#define SDL_DisplayYUVOverlay           __vsf_sdl_display_yuv_overlay

#define SDL_SetVideoMode                __vsf_sdl_set_video_mode
#define SDL_Flip                        __vsf_sdl_flip

#define SDL_WM_SetCaption               __vsf_sdl_wm_set_caption
#endif
#endif      // VSF_SDL_CFG_FAKE_API

#define SDL_SemWait(__sem)              __vsf_sdl2_sem_wait((__sem), -1)
#define SDL_SemWaitTimeout(__sem, __ms) __vsf_sdl2_sem_wait((__sem), (__ms))
#define SDL_SemTryWait(__sem)           __vsf_sdl2_sem_wait((__sem), 0)

#define SDL_SetHint(...)
#define SDL_ShowSimpleMessageBox(...)
#define SDL_SaveBMP(...)

#define SDL_SetWindowSize(...)
#define SDL_SetWindowMinimumSize(...)
#define SDL_SetWindowMaximumSize(...)
#define SDL_GetRendererInfo(__renderer, __info)                                 \
            do {                                                                \
                SDL_memset((__info), 0, sizeof(*(__info)));                     \
                (__info)->name          = "vsf";                                \
            } while (0)

/*============================ MACROS ========================================*/

#define SDL_INIT_TIMER          (1 << 0)
#define SDL_INIT_AUDIO          (1 << 1)
#define SDL_INIT_VIDEO          (1 << 2)
#define SDL_INIT_JOYSTICK       (1 << 3)
#define SDL_INIT_HAPTIC         (1 << 4)
#define SDL_INIT_GAMECONTROLLER (1 << 5)
#define SDL_INIT_EVENTS         (1 << 6)
#define SDL_INIT_NOPARACHUTE    (1 << 7)
#define SDL_INIT_EVERYTHING     (0xFFFFFFFF)

#define SDL_ALPHA_OPAQUE        255
#define SDL_ALPHA_TRANSPARENT   0

/*============================ TYPES =========================================*/

// basic types
typedef union SDL_Color {
    struct {
        uint8_t b, g, r, a;
    };
    uint32_t value;
} SDL_Color;

typedef struct SDL_Rect {
    int x, y;
    int w, h;
} SDL_Rect;

enum {
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_UNDEFINED,
};

typedef enum SDL_WindowFlags {
    SDL_WINDOW_FULLSCREEN       = (1 <<0),
    SDL_WINDOW_OPENGL           = (1 << 1),
    SDL_WINDOW_SHOWN            = (1 << 2),
    SDL_WINDOW_HIDDEN           = (1 << 3),
    SDL_WINDOW_BORDERLESS       = (1 << 4),
    SDL_WINDOW_RESIZABLE        = (1 << 5),
    SDL_WINDOW_MINIMIZED        = (1 << 6),
    SDL_WINDOW_MAXIMIZED        = (1 << 7),
    SDL_WINDOW_INPUT_GRABBED    = (1 << 8),
    SDL_WINDOW_INPUT_FOCUS      = (1 << 9),
    SDL_WINDOW_MOUSE_FOCUS      = (1 << 10),
    SDL_WINDOW_DESKTOP          = (1 << 12),
    SDL_WINDOW_FULLSCREEN_DESKTOP = ( SDL_WINDOW_FULLSCREEN | SDL_WINDOW_DESKTOP ),

    SDL_WINDOW_ALLOW_HIGHDPI    = (1 << 13),
    SDL_WINDOW_MOUSE_CAPTURE    = (1 << 14),
    SDL_WINDOW_ALWAYS_ON_TOP    = (1 << 15),
    SDL_WINDOW_SKIP_TASKBAR     = (1 << 16),
    SDL_WINDOW_UTILITY          = (1 << 17),
    SDL_WINDOW_TOOLTIP          = (1 << 18),
    SDL_WINDOW_POPUP_MENU       = (1 << 19),
    SDL_WINDOW_VULKAN           = (1 << 20),
} SDL_WindowFlags;

typedef struct SDL_Window SDL_Window;

typedef enum {
    SDL_TEXTUREACCESS_STATIC,
    SDL_TEXTUREACCESS_STREAMING,
    SDL_TEXTUREACCESS_TARGET,
} SDL_TextureAccess;

typedef enum {
    SDL_PIXELFORMAT_UNKNOWN     = VSF_DISP_COLOR_INVALID,
    SDL_PIXELFORMAT_BYMASK_IDX  = 0xFF,

    SDL_PIXELFORMAT_RGBA8888    = VSF_DISP_COLOR_RGBA8888,
    SDL_PIXELFORMAT_RGBA32      = SDL_PIXELFORMAT_RGBA8888,
    SDL_PIXELFORMAT_ABGR8888    = VSF_DISP_COLOR_ABGR8888,

    SDL_PIXELFORMAT_ARGB8888    = VSF_DISP_COLOR_ARGB8888,
    SDL_PIXELFORMAT_RGB565      = VSF_DISP_COLOR_RGB565,

    SDL_PIXELFORMAT_RGB666      = VSF_DISP_COLOR_RGB666_32,

    SDL_PIXELFORMAT_RGB888      = VSF_DISP_COLOR_RGB888_32,
    SDL_PIXELFORMAT_BGR888      = VSF_DISP_COLOR_BGR888_32,

    SDL_PIXELFORMAT_RGB24       = VSF_DISP_COLOR_RGB888_24,
    SDL_PIXELFORMAT_BGR24       = VSF_DISP_COLOR_BGR888_24,

    VSF_DISP_COLOR_IDX_DEF(INDEX8)  = 100,
    VSF_DISP_COLOR_DEF(INDEX8, 8, 1, 0),
    SDL_PIXELFORMAT_INDEX8      = VSF_DISP_COLOR_INDEX8,
} SDL_PixelFormatEnum;

typedef enum SDL_BlendMode {
    SDL_BLENDMODE_NONE,
    SDL_BLENDMODE_BLEND,
    SDL_BLENDMODE_ADD,
    SDL_LBENDMODE_MOD,
} SDL_BlendMode;

typedef struct SDL_Texture SDL_Texture;

typedef struct SDL_Palette {
    int ncolors;
    SDL_Color * colors;
} SDL_Palette;

typedef struct SDL_PixelFormat {
    uint32_t format;

    SDL_Palette *palette;
    uint32_t Rmask;
    uint32_t Gmask;
    uint32_t Bmask;
    uint32_t Amask;
    uint8_t Rshift;
    uint8_t Gshift;
    uint8_t Bshift;
    uint8_t Ashift;
    uint8_t Rloss;
    uint8_t Gloss;
    uint8_t Bloss;
    uint8_t Aloss;
    uint8_t BitsPerPixel;
    uint8_t BytesPerPixel;
} SDL_PixelFormat;
typedef struct SDL_Surface {
    SDL_PixelFormat *format;
    int w, h;
    int pitch;

    enum {
        SDL_PREALLOC            = 1 << 0,
        SDL_SIMD_ALIGNED        = 1 << 1,
    } flags;
    SDL_BlendMode blend;

    SDL_PixelFormat __format;
    void *pixels;

    uint32_t __pixels[0] ALIGN(4);
} SDL_Surface;

typedef struct SDL_DisplayMode {
    uint32_t format;
    int w, h;
} SDL_DisplayMode;

#if VSF_SDL_CFG_V1_COMPATIBLE == ENABLED
enum {
    SDL_YV12_OVERLAY            = 0x32315659,
    SDL_IYUV_OVERLAY            = 0x56555949,
    SDL_YUY2_OVERLAY            = 0x32595559,
    SDL_UYVY_OVERLAY            = 0x59565955,
    SDL_YVYU_OVERLAY            = 0x55595659,
};
typedef struct SDL_Overlay {
    uint32_t format;
    uint8_t planes;
    bool hw_overlay;
    int w, h;
    uint16_t pitches[4];
    uint8_t ** pixels;
} SDL_Overlay;

// parameter for SDL_SetVideoMode
enum {
    SDL_SWSURFACE               = 0 << 0,            // Just here for compatibility
    SDL_ANYFORMAT               = 1 << 0,
    SDL_HWSURFACE               = 1 << 1,
    SDL_DOUBLEBUF               = 1 << 2,
    SDL_FULLSCREEN              = 1 << 3,
};
#endif

typedef enum {
    SDL_RENDERER_SOFTWARE       = (1 << 0),
    SDL_RENDERER_ACCELERATED    = (1 << 1),
    SDL_RENDERER_PRESENTVSYNC   = (1 << 2),
    SDL_RENDERER_TARGETTEXTURE  = (1 << 3),
} SDL_RendererFlags;

typedef struct SDL_RendererInfo {
    const char                  *name;
} SDL_RendererInfo;

typedef struct SDL_Renderer SDL_Renderer;


// semaphore
typedef vsf_sem_t SDL_sem;

// audio
typedef enum SDL_AudioStatus {
    SDL_AUDIO_STOPPED = 0,
    SDL_AUDIO_PLAYING,
    SDL_AUDIO_PAUSED
} SDL_AudioStatus;
typedef uint8_t SDL_AudioFormat;
typedef void (*SDL_AudioCallback)(void * userdata, uint8_t * stream, int len);
enum {
    AUDIO_8                     = 0x0008,
    AUDIO_16                    = 0x0010,
    AUDIO_32                    = 0x0020,

    // LSB is default
    AUDIO_LSB                   = 0x0000,
    AUDIO_MSB                   = 0x4000,
    AUDIO_SIGNED                = 0x8000,
    AUDIO_UNSIGNED              = 0x0000,
    AUDIO_FLOAT                 = 0x0100,

    AUDIO_U8                    = AUDIO_8 | AUDIO_UNSIGNED,
    AUDIO_S8                    = AUDIO_8 | AUDIO_SIGNED,
    AUDIO_U16                   = AUDIO_16 | AUDIO_UNSIGNED,
    AUDIO_S16                   = AUDIO_16 | AUDIO_SIGNED,
    AUDIO_F16                   = AUDIO_16 | AUDIO_FLOAT,
    AUDIO_U32                   = AUDIO_32 | AUDIO_UNSIGNED,
    AUDIO_S32                   = AUDIO_32 | AUDIO_SIGNED,
    AUDIO_F32                   = AUDIO_32 | AUDIO_FLOAT,

    AUDIO_U16LSB                = AUDIO_U16 | AUDIO_LSB,
    AUDIO_U16MSB                = AUDIO_U16 | AUDIO_MSB,
    AUDIO_S16LSB                = AUDIO_S16 | AUDIO_LSB,
    AUDIO_S16MSB                = AUDIO_S16 | AUDIO_MSB,
    AUDIO_U32LSB                = AUDIO_U32 | AUDIO_LSB,
    AUDIO_U32MSB                = AUDIO_U32 | AUDIO_MSB,
    AUDIO_S32LSB                = AUDIO_S32 | AUDIO_LSB,
    AUDIO_S32MSB                = AUDIO_S32 | AUDIO_MSB,
    AUDIO_F32LSB                = AUDIO_F32 | AUDIO_UNSIGNED,
    AUDIO_F32MSB                = AUDIO_F32 | AUDIO_SIGNED,

#if __BYTE_ORDER == __BIG_ENDIAN
    AUDIO_U16SYS                = AUDIO_U16MSB,
    AUDIO_S16SYS                = AUDIO_S16MSB,
    AUDIO_S32SYS                = AUDIO_S32MSB,
    AUDIO_F32SYS                = AUDIO_F32MSB,
#else
    AUDIO_U16SYS                = AUDIO_U16LSB,
    AUDIO_S16SYS                = AUDIO_S16LSB,
    AUDIO_S32SYS                = AUDIO_S32LSB,
    AUDIO_F32SYS                = AUDIO_F32LSB,
#endif
};
typedef struct SDL_AudioSpec {
    uint32_t freq;
    SDL_AudioFormat format;
    uint8_t channels;
    uint16_t samples;
    uint32_t size;
    SDL_AudioCallback callback;
    void * userdata;
} SDL_AudioSpec;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_sdl2_init(vk_disp_t * disp);

extern int SDL_Init(uint32_t flags);
extern int SDL_InitSubSystem(uint32_t flags);
extern void SDL_Quit(void);
extern const char * SDL_GetPlatform(void);

extern void SDL_memset4(void *dst, int val, size_t dwords);
extern const char * SDL_GetError(void);
extern int SDL_SetError(const char* fmt, ...);

extern SDL_Window * SDL_CreateWindow(const char * title, int x, int y, int w, int h, uint32_t flags);
extern void SDL_DestroyWindow(SDL_Window * window);
extern void SDL_GetWindowPosition(SDL_Window * window, int * x, int * y);
extern SDL_Surface * SDL_GetWindowSurface(SDL_Window * window);
extern void SDL_GetWindowSize(SDL_Window* window, int *w, int *h);
extern int SDL_UpdateWindowSurface(SDL_Window * window);
extern int SDL_UpdateWindowSurfaceRects(SDL_Window * window, const SDL_Rect * rects, int numrects);
extern void SDL_ShowWindow(SDL_Window *window);
extern void SDL_SetWindowIcon(SDL_Window * window, SDL_Surface * icon);
extern void SDL_SetWindowTitle(SDL_Window * window, const char * title);
extern int SDL_SetWindowFullscreen(SDL_Window * window, uint32_t flags);
extern void SDL_RestoreWindow(SDL_Window * window);
extern void SDL_MaximizeWindow(SDL_Window * window);
extern uint32_t SDL_GetWindowFlags(SDL_Window * window);

extern int SDL_GetDesktopDisplayMode(int display_index, SDL_DisplayMode * mode);
extern int SDL_GetCurrentDisplayMode(int display_index, SDL_DisplayMode * mode);

extern SDL_Surface * SDL_CreateRGBSurface(uint32_t flags, int w, int h, int depth, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask);
extern SDL_Surface * SDL_CreateRGBSurfaceWithFormat(uint32_t flags, int w, int h, int depth, uint32_t format);
extern SDL_Surface * SDL_CreateRGBSurfaceWithFormatFrom(void * pixels, int w, int h, int depth, int pitch, uint32_t format);
extern SDL_Surface * SDL_CreateRGBSurfaceFrom(void *pixels, int w, int h, int depth, int pitch, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask);
extern void SDL_FreeSurface(SDL_Surface *surface);
extern int SDL_LockSurface(SDL_Surface * surface);
extern void SDL_UnlockSurface(SDL_Surface * surface);
extern int SDL_SetSurfaceBlendMode(SDL_Surface * surface, SDL_BlendMode blendMode);
extern int SDL_SetColorKey(SDL_Surface * surface, int flag, uint32_t key);
extern int SDL_FillRect(SDL_Surface * surface, const SDL_Rect * rect, uint32_t color);
extern int SDL_BlitSurface(SDL_Surface * src, const SDL_Rect * srcrect, SDL_Surface * dst, SDL_Rect * dstrect);
extern SDL_Surface * SDL_ConvertSurfaceFormat(SDL_Surface * src, uint32_t format, uint32_t flags);

extern SDL_Renderer * SDL_CreateRenderer(SDL_Window * window, int index, uint32_t flags);
extern void SDL_DestroyRenderer(SDL_Renderer * renderer);
extern int SDL_RenderClear(SDL_Renderer * renderer);
extern int SDL_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect);
extern void SDL_RenderPresent(SDL_Renderer * renderer);
extern int SDL_SetRenderDrawColor(SDL_Renderer * renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
extern int SDL_RenderDrawPoint(SDL_Renderer * renderer, int x, int y);

extern SDL_Texture * SDL_CreateTexture(SDL_Renderer * renderer, uint32_t format, int access, int w, int h);
extern SDL_Texture * SDL_CreateTextureFromSurface(SDL_Renderer * renderer, SDL_Surface * surface);
extern void SDL_DestroyTexture(SDL_Texture * texture);
extern int SDL_UpdateTexture(SDL_Texture * texture, const SDL_Rect * rect, const void * pixels, int pitch);
extern int SDL_LockTexture(SDL_Texture * texture, const SDL_Rect * rect, void **pixels, int *pitch);
extern void SDL_UnlockTexture(SDL_Texture * texture);

extern uint32_t SDL_MapRGBA(const SDL_PixelFormat * format, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
extern uint32_t SDL_MapRGB(const SDL_PixelFormat * format, uint8_t r, uint8_t g, uint8_t b);

extern SDL_sem * SDL_CreateSemaphore(uint32_t initial_value);
extern void SDL_DestroySemaphore(SDL_sem * sem);
extern int __vsf_sdl2_sem_wait(SDL_sem * sem, int32_t ms);
extern int SDL_SemPost(SDL_sem * sem);

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
extern void SDL_Delay(uint32_t ms);
extern uint32_t SDL_GetTicks(void);
#endif

extern int SDL_GetDisplayDPI(int display_index, float * ddpi, float * hdpi, float * vdpi);
extern void SDL_EnableScreenSaver(void);
extern SDL_bool SDL_HasClipboardText(void);
extern char * SDL_GetClipboardText(void);
extern int SDL_SetClipboardText(const char * text);
extern uint64_t SDL_GetPerformanceCounter(void);
extern uint64_t SDL_GetPerformanceFrequency(void);

extern int SDL_OpenAudio(SDL_AudioSpec * desired, SDL_AudioSpec * obtained);
extern void SDL_PauseAudio(int pause_on);
extern SDL_AudioStatus SDL_GetAudioStatus(void);
extern void SDL_CloseAudio(void);

extern const char * SDL_GetKeyName(SDL_Keycode key);

extern SDL_Cursor * SDL_CreateCursor(const uint8_t * data, const uint8_t * mask,
                                            int w, int h, int hot_x, int hot_y);
extern SDL_Cursor * SDL_CreateColorCursor(SDL_Surface * surface, int hot_x, int hot_y);
extern SDL_Cursor * SDL_CreateSystemCursor(SDL_SystemCursor id);
extern void SDL_SetCursor(SDL_Cursor * cursor);
extern SDL_Cursor * SDL_GetCursor(void);
extern SDL_Cursor * SDL_GetDefaultCursor(void);
extern void SDL_FreeCursor(SDL_Cursor * cursor);
extern int SDL_ShowCursor(int toggle);

extern int SDL_NumJoysticks(void);
extern SDL_Joystick * SDL_JoystickOpen(int device_index);
extern void SDL_JoystickClose(SDL_Joystick *joystick);
extern int SDL_JoystickEventState(int state);
extern int SDL_JoystickNumButtons(SDL_Joystick * joystick);
extern int SDL_JoystickNumAxes(SDL_Joystick * joystick);
extern int SDL_JoystickNumBalls(SDL_Joystick * joystick);
extern int SDL_JoystickNumHats(SDL_Joystick * joystick);

extern int SDL_CaptureMouse(SDL_bool enabled);
extern uint32_t SDL_GetGlobalMouseState(int * x, int * y);

#if VSF_SDL_CFG_V1_COMPATIBLE == ENABLED
extern SDL_Overlay * SDL_CreateYUVOverlay(int width, int height, uint32_t format, SDL_Surface *display);
extern void SDL_FreeYUVOverlay(SDL_Overlay * overlay);
extern int SDL_LockYUVOverlay(SDL_Overlay * overlay);
extern void SDL_UnlockYUVOverlay(SDL_Overlay * overlay);
extern int SDL_DisplayYUVOverlay(SDL_Overlay * overlay, SDL_Rect * dstrect);

extern SDL_Surface * SDL_SetVideoMode(int width, int height, int bpp, uint32_t flags);
extern int SDL_Flip(SDL_Surface * screen);

extern void SDL_WM_SetCaption(const char * title, const char * icon);
#endif

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_SDL
#endif      // __VSF_SDL2_H__