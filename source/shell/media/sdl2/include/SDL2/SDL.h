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

#ifndef __VSF_SDL2_H__
#define __VSF_SDL2_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#include "vsf.h"

/*============================ MACROS ========================================*/

#ifndef VSF_SDL_CFG_V1_COMPATIBLE
#   define VSF_SDL_CFG_V1_COMPATIBLE    ENABLED
#endif

/*============================ INCLUDES ======================================*/

#include "SDL_cpuinfo.h"
#include "SDL_endian.h"
#include "SDL_stdinc.h"
#include "SDL_version.h"

#include "SDL_log.h"
#include "SDL_rwops.h"
#include "SDL_mutex.h"
#include "SDL_thread.h"
#include "SDL_timer.h"
#include "SDL_error.h"

#include "SDL_events.h"
#include "SDL_keyboard.h"
#include "SDL_mouse.h"
#include "SDL_gamecontroller.h"
#include "SDL_haptic.h"

#include "SDL_audio.h"

#include "SDL_surface.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_SDL_CFG_WRAPPER == ENABLED
#define SDL_memset4                     VSF_SDL_WRAPPER(SDL_memset4)

#define SDL_Init                        VSF_SDL_WRAPPER(SDL_Init)
#define SDL_InitSubSystem               VSF_SDL_WRAPPER(SDL_InitSubSystem)
#define SDL_QuitSubSystem               VSF_SDL_WRAPPER(SDL_QuitSubSystem)
#define SDL_Quit                        VSF_SDL_WRAPPER(SDL_Quit)
#define SDL_GetPlatform                 VSF_SDL_WRAPPER(SDL_GetPlatform)

#define SDL_SetError                    VSF_SDL_WRAPPER(SDL_SetError)
#define SDL_GetError                    VSF_SDL_WRAPPER(SDL_GetError)

#define SDL_CreateWindow                VSF_SDL_WRAPPER(SDL_CreateWindow)
#define SDL_DestroyWindow               VSF_SDL_WRAPPER(SDL_DestroyWindow)
#define SDL_GetWindowPosition           VSF_SDL_WRAPPER(SDL_GetWindowPosition)
#define SDL_GetWindowSurface            VSF_SDL_WRAPPER(SDL_GetWindowSurface)
#define SDL_GetWindowSize               VSF_SDL_WRAPPER(SDL_GetWindowSize)
#define SDL_UpdateWindowSurface         VSF_SDL_WRAPPER(SDL_UpdateWindowSurface)
#define SDL_UpdateWindowSurfaceRects    VSF_SDL_WRAPPER(SDL_UpdateWindowSurfaceRects)
#define SDL_ShowWindow                  VSF_SDL_WRAPPER(SDL_ShowWindow)
#define SDL_SetWindowIcon               VSF_SDL_WRAPPER(SDL_SetWindowIcon)
#define SDL_SetWindowTitle              VSF_SDL_WRAPPER(SDL_SetWindowTitle)
#define SDL_SetWindowFullscreen         VSF_SDL_WRAPPER(SDL_SetWindowFullscreen)
#define SDL_RestoreWindow               VSF_SDL_WRAPPER(SDL_RestoreWindow)
#define SDL_MaximizeWindow              VSF_SDL_WRAPPER(SDL_MaximizeWindow)
#define SDL_MinimizeWindow              VSF_SDL_WRAPPER(SDL_MinimizeWindow)
#define SDL_GetWindowFlags              VSF_SDL_WRAPPER(SDL_GetWindowFlags)

#define SDL_GetDesktopDisplayMode       VSF_SDL_WRAPPER(SDL_GetDesktopDisplayMode)
#define SDL_GetCurrentDisplayMode       VSF_SDL_WRAPPER(SDL_GetCurrentDisplayMode)

#define SDL_CreateRGBSurface                VSF_SDL_WRAPPER(SDL_CreateRGBSurface)
#define SDL_CreateRGBSurfaceWithFormat      VSF_SDL_WRAPPER(SDL_CreateRGBSurfaceWithFormat)
#define SDL_CreateRGBSurfaceWithFormatFrom  VSF_SDL_WRAPPER(SDL_CreateRGBSurfaceWithFormatFrom)
#define SDL_CreateRGBSurfaceFrom        VSF_SDL_WRAPPER(SDL_CreateRGBSurfaceFrom)
#define SDL_FreeSurface                 VSF_SDL_WRAPPER(SDL_FreeSurface)
#define SDL_LockSurface                 VSF_SDL_WRAPPER(SDL_LockSurface)
#define SDL_UnlockSurface               VSF_SDL_WRAPPER(SDL_UnlockSurface)
#define SDL_SetSurfaceBlendMode         VSF_SDL_WRAPPER(SDL_SetSurfaceBlendMode)
#define SDL_SetColorKey                 VSF_SDL_WRAPPER(SDL_SetColorKey)
#define SDL_FillRect                    VSF_SDL_WRAPPER(SDL_FillRect)
#define SDL_BlitSurface                 VSF_SDL_WRAPPER(SDL_BlitSurface)
#define SDL_ConvertSurfaceFormat        VSF_SDL_WRAPPER(SDL_ConvertSurfaceFormat)
#define SDL_ConvertSurface              VSF_SDL_WRAPPER(SDL_ConvertSurface)
#define SDL_SetClipRect                 VSF_SDL_WRAPPER(SDL_SetClipRect)
#define SDL_GetClipRect                 VSF_SDL_WRAPPER(SDL_GetClipRect)

#define SDL_CreateRenderer              VSF_SDL_WRAPPER(SDL_CreateRenderer)
#define SDL_DestroyRenderer             VSF_SDL_WRAPPER(SDL_DestroyRenderer)
#define SDL_RenderClear                 VSF_SDL_WRAPPER(SDL_RenderClear)
#define SDL_RenderCopy                  VSF_SDL_WRAPPER(SDL_RenderCopy)
#define SDL_RenderCopyEx                VSF_SDL_WRAPPER(SDL_RenderCopyEx)
#define SDL_RenderPresent               VSF_SDL_WRAPPER(SDL_RenderPresent)
#define SDL_SetRenderDrawColor          VSF_SDL_WRAPPER(SDL_SetRenderDrawColor)
#define SDL_RenderDrawPoint             VSF_SDL_WRAPPER(SDL_RenderDrawPoint)
#define SDL_RenderDrawRect              VSF_SDL_WRAPPER(SDL_RenderDrawRect)
#define SDL_RenderGetScale              VSF_SDL_WRAPPER(SDL_RenderGetScale)
#define SDL_RenderSetScale              VSF_SDL_WRAPPER(SDL_RenderSetScale)
#define SDL_RenderGetLogicalSize        VSF_SDL_WRAPPER(SDL_RenderGetLogicalSize)
#define SDL_RenderSetLogicalSize        VSF_SDL_WRAPPER(SDL_RenderSetLogicalSize)
#define SDL_RenderGetViewport           VSF_SDL_WRAPPER(SDL_RenderGetViewport)
#define SDL_RenderSetViewport           VSF_SDL_WRAPPER(SDL_RenderSetViewport)
#define SDL_SetRenderTarget             VSF_SDL_WRAPPER(SDL_SetRenderTarget)
#define SDL_GetRenderTarget             VSF_SDL_WRAPPER(SDL_GetRenderTarget)
#define SDL_SetRenderDrawBlendMode      VSF_SDL_WRAPPER(SDL_SetRenderDrawBlendMode)
#define SDL_GetRenderDrawBlendMode      VSF_SDL_WRAPPER(SDL_GetRenderDrawBlendMode)
#define SDL_RenderFillRect              VSF_SDL_WRAPPER(SDL_RenderFillRect)

#define SDL_CreateTexture               VSF_SDL_WRAPPER(SDL_CreateTexture)
#define SDL_CreateTextureFromSurface    VSF_SDL_WRAPPER(SDL_CreateTextureFromSurface)
#define SDL_UpdateTexture               VSF_SDL_WRAPPER(SDL_UpdateTexture)
#define SDL_DestroyTexture              VSF_SDL_WRAPPER(SDL_DestroyTexture)
#define SDL_LockTexture                 VSF_SDL_WRAPPER(SDL_LockTexture)
#define SDL_UnlockTexture               VSF_SDL_WRAPPER(SDL_UnlockTexture)
#define SDL_SetTextureBlendMode         VSF_SDL_WRAPPER(SDL_SetTextureBlendMode)
#define SDL_GetTextureBlendMode         VSF_SDL_WRAPPER(SDL_GetTextureBlendMode)
#define SDL_SetTextureColorMod          VSF_SDL_WRAPPER(SDL_SetTextureColorMod)
#define SDL_GetTextureColorMod          VSF_SDL_WRAPPER(SDL_GetTextureColorMod)
#define SDL_SetTextureAlphaMod          VSF_SDL_WRAPPER(SDL_SetTextureAlphaMod)
#define SDL_GetTextureAlphaMod          VSF_SDL_WRAPPER(SDL_GetTextureAlphaMod)

#define SDL_MapRGBA                     VSF_SDL_WRAPPER(SDL_MapRGBA)
#define SDL_MapRGB                      VSF_SDL_WRAPPER(SDL_MapRGB)
#define SDL_AllocFormat                 VSF_SDL_WRAPPER(SDL_AllocFormat)
#define SDL_FreeFormat                  VSF_SDL_WRAPPER(SDL_FreeFormat)

#define SDL_CreateSemaphore             VSF_SDL_WRAPPER(SDL_CreateSemaphore)
#define SDL_DestroySemaphore            VSF_SDL_WRAPPER(SDL_DestroySemaphore)
#define SDL_SemPost                     VSF_SDL_WRAPPER(SDL_SemPost)

#define SDL_GetDisplayDPI               VSF_SDL_WRAPPER(SDL_GetDisplayDPI)
#define SDL_EnableScreenSaver           VSF_SDL_WRAPPER(SDL_EnableScreenSaver)
#define SDL_HasClipboardText            VSF_SDL_WRAPPER(SDL_HasClipboardText)
#define SDL_GetClipboardText            VSF_SDL_WRAPPER(SDL_GetClipboardText)
#define SDL_SetClipboardText            VSF_SDL_WRAPPER(SDL_SetClipboardText)
#define SDL_GetPerformanceCounter       VSF_SDL_WRAPPER(SDL_GetPerformanceCounter)
#define SDL_GetPerformanceFrequency     VSF_SDL_WRAPPER(SDL_GetPerformanceFrequency)

#define SDL_CreateCursor                VSF_SDL_WRAPPER(SDL_CreateCursor)
#define SDL_CreateColorCursor           VSF_SDL_WRAPPER(SDL_CreateColorCursor)
#define SDL_CreateSystemCursor          VSF_SDL_WRAPPER(SDL_CreateSystemCursor)
#define SDL_SetCursor                   VSF_SDL_WRAPPER(SDL_SetCursor)
#define SDL_GetCursor                   VSF_SDL_WRAPPER(SDL_GetCursor)
#define SDL_GetDefaultCursor            VSF_SDL_WRAPPER(SDL_GetDefaultCursor)
#define SDL_FreeCursor                  VSF_SDL_WRAPPER(SDL_FreeCursor)
#define SDL_ShowCursor                  VSF_SDL_WRAPPER(SDL_ShowCursor)

#define SDL_GetScancodeName             VSF_SDL_WRAPPER(SDL_GetScancodeName)
#define SDL_GetKeyName                  VSF_SDL_WRAPPER(SDL_GetKeyName)

#define SDL_NumJoysticks                VSF_SDL_WRAPPER(SDL_NumJoysticks)
#define SDL_JoystickOpen                VSF_SDL_WRAPPER(SDL_JoystickOpen)
#define SDL_JoystickClose               VSF_SDL_WRAPPER(SDL_JoystickClose)
#define SDL_JoystickNumButtons          VSF_SDL_WRAPPER(SDL_JoystickNumButtons)
#define SDL_JoystickNumAxes             VSF_SDL_WRAPPER(SDL_JoystickNumAxes)
#define SDL_JoystickNumBalls            VSF_SDL_WRAPPER(SDL_JoystickNumBalls)
#define SDL_JoystickNumHats             VSF_SDL_WRAPPER(SDL_JoystickNumHats)
#define SDL_JoystickEventState          VSF_SDL_WRAPPER(SDL_JoystickEventState)

#define SDL_CaptureMouse                VSF_SDL_WRAPPER(SDL_CaptureMouse)
#define SDL_GetGlobalMouseState         VSF_SDL_WRAPPER(SDL_GetGlobalMouseState)

#define SDL_SetWindowHitTest            VSF_SDL_WRAPPER(SDL_SetWindowHitTest)

#if VSF_SDL_CFG_V1_COMPATIBLE == ENABLED
#define SDL_CreateYUVOverlay            VSF_SDL_WRAPPER(SDL_CreateYUVOverlay)
#define SDL_FreeYUVOverlay              VSF_SDL_WRAPPER(SDL_FreeYUVOverlay)
#define SDL_LockYUVOverlay              VSF_SDL_WRAPPER(SDL_LockYUVOverlay)
#define SDL_UnlockYUVOverlay            VSF_SDL_WRAPPER(SDL_UnlockYUVOverlay)
#define SDL_DisplayYUVOverlay           VSF_SDL_WRAPPER(SDL_DisplayYUVOverlay)

#define SDL_SetVideoMode                VSF_SDL_WRAPPER(SDL_SetVideoMode)
#define SDL_Flip                        VSF_SDL_WRAPPER(SDL_Flip)

#define SDL_EnableUNICODE               VSF_SDL_WRAPPER(SDL_EnableUNICODE)
#define SDL_EnableKeyRepeat             VSF_SDL_WRAPPER(SDL_EnableKeyRepeat)

#define SDL_WM_SetCaption               VSF_SDL_WRAPPER(SDL_WM_SetCaption)
#endif
#endif      // VSF_SDL_CFG_WRAPPER

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
            ({                                                                  \
                SDL_memset((__info), 0, sizeof(*(__info)));                     \
                (__info)->name          = "vsf";                                \
                0;                                                              \
            })

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

typedef struct SDL_Point {
    int x, y;
} SDL_Point;

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

    SDL_PIXELFORMAT_PALETTE     = VSF_DISP_COLOR_VALUE(0, 8, 1, 0),

    SDL_PIXELFORMAT_RGBA8888    = VSF_DISP_COLOR_RGBA8888,
    SDL_PIXELFORMAT_RGBA32      = SDL_PIXELFORMAT_RGBA8888,
    SDL_PIXELFORMAT_ABGR8888    = VSF_DISP_COLOR_ABGR8888,

    SDL_PIXELFORMAT_ARGB8888    = VSF_DISP_COLOR_ARGB8888,
    SDL_PIXELFORMAT_RGB565      = VSF_DISP_COLOR_RGB565,
    SDL_PIXELFORMAT_RGB332      = VSF_DISP_COLOR_RGB332,

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
    SDL_BLENDMODE_MOD,
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
    SDL_Rect clip_rect;
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
typedef enum {
    SDL_FLIP_NONE               = 0,
    SDL_FLIP_HORIZONTAL         = (1 << 0),
    SDL_FLIP_VERTICAL           = (1 << 1),
} SDL_RendererFlip;

typedef struct SDL_RendererInfo {
    const char                  *name;
    uint32_t                    flags;
} SDL_RendererInfo;

typedef struct SDL_Renderer SDL_Renderer;


// semaphore
typedef vsf_sem_t SDL_sem;

// SDL_video.h
typedef enum {
    SDL_HITTEST_NORMAL,
    SDL_HITTEST_DRAGGABLE,
    SDL_HITTEST_RESIZE_TOPLEFT,
    SDL_HITTEST_RESIZE_TOP,
    SDL_HITTEST_RESIZE_TOPRIGHT,
    SDL_HITTEST_RESIZE_RIGHT,
    SDL_HITTEST_RESIZE_BOTTOMRIGHT,
    SDL_HITTEST_RESIZE_BOTTOM,
    SDL_HITTEST_RESIZE_BOTTOMLEFT,
    SDL_HITTEST_RESIZE_LEFT
} SDL_HitTestResult;
typedef SDL_HitTestResult (SDLCALL *SDL_HitTest)(SDL_Window *win, const SDL_Point *area, void *data);

typedef struct vsf_sdl2_cfg_t {
    vk_disp_t *disp_dev;
#if VSF_USE_AUDIO == ENABLED
    vk_audio_dev_t *audio_dev;
#endif
} vsf_sdl2_cfg_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_sdl2_init(vsf_sdl2_cfg_t *cfg);

extern int SDL_Init(uint32_t flags);
extern int SDL_InitSubSystem(uint32_t flags);
extern void SDL_QuitSubSystem(uint32_t flags);
extern void SDL_Quit(void);
extern const char * SDL_GetPlatform(void);

extern void SDL_memset4(void *dst, int val, size_t dwords);
extern const char * SDL_GetError(void);
extern int SDL_SetError(const char* fmt, ...);

extern SDL_bool SDL_IntersectRect(const SDL_Rect * rect0, const SDL_Rect * rect1, SDL_Rect * rect_out);

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
extern void SDL_MinimizeWindow(SDL_Window * window);
extern uint32_t SDL_GetWindowFlags(SDL_Window * window);
extern void SDL_SetWindowBordered(SDL_Window * window, SDL_bool bordered);
extern void SDL_SetWindowPosition(SDL_Window * window, int x, int y);
extern int SDL_SetWindowOpacity(SDL_Window * window, float opacity);

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
extern SDL_Surface * SDL_ConvertSurface(SDL_Surface * src, const SDL_PixelFormat * format, uint32_t flags);
extern SDL_bool SDL_SetClipRect(SDL_Surface * surface, const SDL_Rect * rect);
extern void SDL_GetClipRect(SDL_Surface * surface, SDL_Rect * rect);

extern SDL_Renderer * SDL_CreateRenderer(SDL_Window * window, int index, uint32_t flags);
extern void SDL_DestroyRenderer(SDL_Renderer * renderer);
extern int SDL_RenderClear(SDL_Renderer * renderer);
extern int SDL_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect);
extern int SDL_RenderCopyEx(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect,
            const double angle, const SDL_Point *center, const SDL_RendererFlip flip);
extern void SDL_RenderPresent(SDL_Renderer * renderer);
extern int SDL_SetRenderDrawColor(SDL_Renderer * renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
extern int SDL_RenderDrawPoint(SDL_Renderer * renderer, int x, int y);
extern int SDL_RenderDrawRect(SDL_Renderer * renderer, const SDL_Rect * rect);
extern void SDL_RenderGetScale(SDL_Renderer * renderer, float *scaleX, float *scaleY);
extern int SDL_RenderSetScale(SDL_Renderer * renderer, float scaleX, float scaleY);
extern void SDL_RenderGetLogicalSize(SDL_Renderer * renderer, int *w, int *h);
extern int SDL_RenderSetLogicalSize(SDL_Renderer * renderer, int w, int h);
extern void SDL_RenderGetViewport(SDL_Renderer * renderer, SDL_Rect * rect);
extern int SDL_RenderSetViewport(SDL_Renderer * renderer, const SDL_Rect * rect);
extern int SDL_SetRenderTarget(SDL_Renderer *renderer, SDL_Texture *texture);
extern SDL_Texture * SDL_GetRenderTarget(SDL_Renderer *renderer);
extern int SDL_SetRenderDrawBlendMode(SDL_Renderer * renderer, SDL_BlendMode blendMode);
extern int SDL_GetRenderDrawBlendMode(SDL_Renderer * renderer, SDL_BlendMode *blendMode);
extern int SDL_RenderFillRect(SDL_Renderer * renderer, const SDL_Rect * rect);

extern SDL_Texture * SDL_CreateTexture(SDL_Renderer * renderer, uint32_t format, int access, int w, int h);
extern SDL_Texture * SDL_CreateTextureFromSurface(SDL_Renderer * renderer, SDL_Surface * surface);
extern void SDL_DestroyTexture(SDL_Texture * texture);
extern int SDL_UpdateTexture(SDL_Texture * texture, const SDL_Rect * rect, const void * pixels, int pitch);
extern int SDL_LockTexture(SDL_Texture * texture, const SDL_Rect * rect, void **pixels, int *pitch);
extern void SDL_UnlockTexture(SDL_Texture * texture);
extern int SDL_SetTextureBlendMode(SDL_Texture * texture, SDL_BlendMode blendMode);
extern int SDL_GetTextureBlendMode(SDL_Texture * texture, SDL_BlendMode *blendMode);
extern int SDL_SetTextureColorMod(SDL_Texture * texture, uint8_t r, uint8_t g, uint8_t b);
extern int SDL_GetTextureColorMod(SDL_Texture * texture, uint8_t * r, uint8_t * g, uint8_t * b);
extern int SDL_SetTextureAlphaMod(SDL_Texture * texture, uint8_t alpha);
extern int SDL_GetTextureAlphaMod(SDL_Texture * texture, uint8_t * alpha);

extern uint32_t SDL_MapRGBA(const SDL_PixelFormat * format, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
extern uint32_t SDL_MapRGB(const SDL_PixelFormat * format, uint8_t r, uint8_t g, uint8_t b);
extern SDL_PixelFormat * SDL_AllocFormat(uint32_t format);
extern void SDL_FreeFormat(SDL_PixelFormat *format);

extern SDL_sem * SDL_CreateSemaphore(uint32_t initial_value);
extern void SDL_DestroySemaphore(SDL_sem * sem);
extern int __vsf_sdl2_sem_wait(SDL_sem * sem, int32_t ms);
extern int SDL_SemPost(SDL_sem * sem);

extern int SDL_GetDisplayDPI(int display_index, float * ddpi, float * hdpi, float * vdpi);
extern void SDL_EnableScreenSaver(void);
extern SDL_bool SDL_HasClipboardText(void);
extern char * SDL_GetClipboardText(void);
extern int SDL_SetClipboardText(const char * text);
extern uint64_t SDL_GetPerformanceCounter(void);
extern uint64_t SDL_GetPerformanceFrequency(void);

extern const char * SDL_GetScancodeName(SDL_Scancode scancode);
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

// SDL_video.h
extern int SDL_SetWindowHitTest(SDL_Window * window, SDL_HitTest callback, void *callback_data);

#if VSF_SDL_CFG_V1_COMPATIBLE == ENABLED
extern SDL_Overlay * SDL_CreateYUVOverlay(int width, int height, uint32_t format, SDL_Surface *display);
extern void SDL_FreeYUVOverlay(SDL_Overlay * overlay);
extern int SDL_LockYUVOverlay(SDL_Overlay * overlay);
extern void SDL_UnlockYUVOverlay(SDL_Overlay * overlay);
extern int SDL_DisplayYUVOverlay(SDL_Overlay * overlay, SDL_Rect * dstrect);

extern SDL_Surface * SDL_SetVideoMode(int width, int height, int bpp, uint32_t flags);
extern int SDL_Flip(SDL_Surface * screen);

extern int SDL_EnableUNICODE(int enable);
extern int SDL_EnableKeyRepeat(int delay, int interval);

extern void SDL_WM_SetCaption(const char * title, const char * icon);
#endif

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_SDL
#endif      // __VSF_SDL2_H__