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

#ifndef __SDL_H__
#define __SDL_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#include "vsf.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_SDL_CFG_V1_COMPATIBLE
#   define VSF_SDL_CFG_V1_COMPATIBLE            ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define SDL_assert                      VSF_SDL2_ASSERT

#define SDL_Init                        __vsf_sdl2_init
#define SDL_InitSubSystem               __vsf_sdl2_init_subsystem
#define SDL_Quit                        __vsf_sdl2_quit

#define SDL_GetError                    __vsf_sdl2_get_error
#define SDL_SetHint(...)

#define SDL_CreateWindow                __vsf_sdl2_create_window
#define SDL_DestroyWindow               __vsf_sdl2_destroy_window

#define SDL_GetDesktopDisplayMode       __vsf_sdl2_get_desktop_display_mode

#define SDL_CreateRGBSurfaceWithFormat  __vsf_sdl2_create_rgb_sruface_with_format
#define SDL_FreeSurface                 __vsf_sdl2_free_surface

#define SDL_DestroyTexture              __vsf_sdl2_destroy_texture

#define SDL_CreateRenderer              __vsf_sdl2_create_renderer
#define SDL_DestroyRenderer             __vsf_sdl2_destroy_renderer
#define SDL_RenderClear                 __vsf_sdl2_render_clear
#define SDL_RenderCopy                  __vsf_sdl2_render_copy
#define SDL_RenderPresent               __vsf_sdl2_render_present

#define SDL_CreateTexture               __vsf_sdl2_create_texture
#define SDL_CreateTextureFromSurface    __vsf_sdl2_create_texture_from_surface
#define SDL_UpdateTexture               __vsf_sdl2_update_texture

#define SDL_LockSurface                 __vsf_sdl2_lock_surface
#define SDL_UnlockSurface               __vsf_sdl2_unlock_surface

#define SDL_CreateSemaphore             __vsf_sdl2_create_sem
#define SDL_DestroySemaphore            __vsf_sdl2_destroy_sem
#define SDL_SemWait(__sem)              __vsf_sdl2_sem_wait((__sem), -1)
#define SDL_SemWaitTimeout(__sem, __ms) __vsf_sdl2_sem_wait((__sem), (__ms))
#define SDL_SemTryWait(__sem)           __vsf_sdl2_sem_wait((__sem), 0)
#define SDL_SemPost                     __vsf_sdl2_sem_post

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   define SDL_Delay                    __vsf_sdl2_delay
#   define SDL_GetTicks                 __vsf_sdl2_get_ticks
#endif

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

#define SDL_PollEvent                   __vsf_sdl2_poll_event

#define SDL_NumJoysticks                __vsf_sdl2_num_joysticks
#define SDL_JoystickOpen                __vsf_sdl2_joystick_open
#define SDL_JoystickNumButtons          __vsf_sdl2_joystick_num_buttons
#define SDL_JoystickNumAxes             __vsf_sdl2_joystick_num_axes
#define SDL_JoystickNumBalls            __vsf_sdl2_joystick_num_balls
#define SDL_JoystickNumHats             __vsf_sdl2_joystick_num_hats
#define SDL_JoystickEventState          __vsf_sdl2_joystick_event_state

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

/*============================ TYPES =========================================*/

// basic types
typedef enum SDL_bool {
    SDL_FALSE = 0,
    SDL_TRUE = 1
} SDL_bool;

#define SDL_MAX_SINT8   INT8_MAX
#define SDL_MIN_SINT8   INT8_MIN
typedef int8_t Sint8;

#define SDL_MAX_UINT8   UINT8_MAX
#define SDL_MIN_UINT8   0
typedef uint8_t Uint8;

#define SDL_MAX_SINT16  INT16_MAX
#define SDL_MIN_SINT16  INT16_MIN
typedef int16_t Sint16;

#define SDL_MAX_UINT16  UINT16_MAX
#define SDL_MIN_UINT16  0
typedef uint16_t Uint16;

#define SDL_MAX_SINT32  INT32_MAX
#define SDL_MIN_SINT32  INT32_MIN
typedef int32_t Sint32;

#define SDL_MAX_UINT32  UINT32_MAX
#define SDL_MIN_UINT32  0
typedef uint32_t Uint32;


#define SDL_MAX_SINT64  INT64_MAX
#define SDL_MIN_SINT64  INT64_MIN
typedef int64_t Sint64;

#define SDL_MAX_UINT64  UINT64_MAX
#define SDL_MIN_UINT64  0
typedef uint64_t Uint64;


typedef struct SDL_Rect {
    int x, y;
    int w, h;
} SDL_Rect;

enum {
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_UNDEFINED,
};

typedef enum __sdl_window_flag_t {
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
} __sdl_window_flag_t;

typedef struct SDL_Window SDL_Window;

typedef enum {
    SDL_TEXTUREACCESS_STATIC,
    SDL_TEXTUREACCESS_STREAMING,
    SDL_TEXTUREACCESS_TARGET,
} SDL_TextureAccess;

typedef enum {
    SDL_PIXELFORMAT_UNKNOWN     = VSF_DISP_COLOR_INVALID,
    SDL_PIXELFORMAT_RGBA8888    = VSF_DISP_COLOR_INVALID,

    SDL_PIXELFORMAT_ARGB8888    = VSF_DISP_COLOR_ARGB8888,
    SDL_PIXELFORMAT_RGB565      = VSF_DISP_COLOR_RGB565,
    SDL_PIXELFORMAT_RGB666      = VSF_DISP_COLOR_RGB666_32,
} SDL_PixelFormatEnum;

typedef struct SDL_Texture SDL_Texture;

typedef struct SDL_PixelFormat {
    uint32_t format;

    uint32_t Rmask;
    uint32_t Gmask;
    uint32_t Bmask;
    uint32_t Amask;
    uint8_t BitsPerPixel;
} SDL_PixelFormat;
typedef struct SDL_Surface {
    SDL_PixelFormat *format;
    int w, h;
    int pitch;

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

// events
enum {
    SDL_QUERY                   = -1,
    SDL_IGNORE                  = 0,
    SDL_DISABLE                 = 0,
    SDL_ENABLE                  = 1,
    SDL_RELEASED                = 0,
    SDL_PRESSED                 = 1,
};

typedef enum {
    SDL_FIRSTEVENT              = 0,
    SDL_QUIT                    = 0x100,

    SDL_KEYDOWN                 = 0x300,
    SDL_KEYUP,

    SDL_MOUSEMOTION             = 0x400,
    SDL_MOUSEBUTTONDOWN,
    SDL_MOUSEBUTTONUP,
    SDL_MOUSEWHEEL,

    SDL_JOYAXISMOTION           = 0x600,
    SDL_JOYBALLMOTION,
    SDL_JOYHATMOTION,
    SDL_JOYBUTTONDOWN,
    SDL_JOYBUTTONUP,
    SDL_JOYDEVICEADDED,
    SDL_JOYDEVICEREMOVED,

    SDL_CONTROLLERAXISMOTION    = 0x650,
    SDL_CONTROLLERBUTTONDOWN,
    SDL_CONTROLLERBUTTONUP,
    SDL_CONTROLLERDEVICEADDED,
    SDL_CONTROLLERDEVICEREMOVED,
    SDL_CONTROLLERDEVICEREMAPPED,

    SDL_FINGERDOWN              = 0x700,
    SDL_FINGERUP,
    SDL_FINGERMOTION,

    SDL_AUDIODEVICEADDED        = 0x1100,
    SDL_AUDIODEVICEREMOVED,

#if VSF_SDL_CFG_V1_COMPATIBLE == ENABLED
    SDL_ACTIVEEVENT             = 0x2000,
#endif

    SDL_USEREVENT               = 0x8000,
    SDL_LASTEVENT               = 0xFFFF
} SDL_EventType;

// events::keyboard
typedef enum SDL_Keymod {
    KMOD_NONE                   = 0,
    KMOD_LSHIFT                 = 1 << 0,
    KMOD_RSHIFT                 = 1 << 1,
    KMOD_LCTRL                  = 1 << 2,
    KMOD_RCTRL                  = 1 << 3,
    KMOD_LALT                   = 1 << 4,
    KMOD_RALT                   = 1 << 5,
    KMOD_LGUI                   = 1 << 6,
    KMOD_RGUI                   = 1 << 7,
    KMOD_LMETA                  = 1 << 8,
    KMOD_RMETA                  = 1 << 9,
    KMOD_NUM                    = 1 << 12,
    KMOD_CAPS                   = 1 << 13,
    KMOD_MODE                   = 1 << 14,

    KMOD_CTRL                   = (KMOD_LCTRL | KMOD_RCTRL),
    KMOD_SHIFT                  = (KMOD_LSHIFT | KMOD_RSHIFT),
    KMOD_ALT                    = (KMOD_LALT | KMOD_RALT),
    KMOD_GUI                    = (KMOD_LGUI | KMOD_RGUI),
    KMOD_META                   = (KMOD_LMETA | KMOD_RMETA),

    KMOD_RESERVED               = 0x8000,
} SDL_Keymod;
typedef int32_t SDL_Keycode;
enum {
    SDLK_UNKNOWN                = 0,

    SDLK_BACKSPACE              = '\b',
    SDLK_RETURN                 = '\r',
    SDLK_ESCAPE                 = '\033',
    SDLK_SPACE                  = ' ',

    SDLK_1                      = '1',
    SDLK_2                      = '2',
    SDLK_3                      = '3',
    SDLK_4                      = '4',
    SDLK_5                      = '5',
    SDLK_6                      = '6',
    SDLK_7                      = '7',
    SDLK_8                      = '8',
    SDLK_9                      = '9',

    SDLK_COLON                  = ':',
    SDLK_SEMICOLON              = ';',
    SDLK_LESS                   = '<',
    SDLK_EQUALS                 = '=',
    SDLK_GREATER                = '>',
    SDLK_QUESTION               = '?',
    SDLK_AT                     = '@',

    SDLK_a                      = 'a',
    SDLK_b                      = 'b',
    SDLK_c                      = 'c',
    SDLK_d                      = 'd',
    SDLK_e                      = 'e',
    SDLK_f                      = 'f',
    SDLK_g                      = 'g',
    SDLK_h                      = 'h',
    SDLK_i                      = 'i',
    SDLK_j                      = 'j',
    SDLK_k                      = 'k',
    SDLK_l                      = 'l',
    SDLK_m                      = 'm',
    SDLK_n                      = 'n',
    SDLK_o                      = 'o',
    SDLK_p                      = 'p',
    SDLK_q                      = 'q',
    SDLK_r                      = 'r',
    SDLK_s                      = 's',
    SDLK_t                      = 't',
    SDLK_u                      = 'u',
    SDLK_v                      = 'v',
    SDLK_w                      = 'w',
    SDLK_x                      = 'x',
    SDLK_y                      = 'y',
    SDLK_z                      = 'z',

    SDLK_KP0                    = 256,
    SDLK_KP1                    = 257,
    SDLK_KP2                    = 258,
    SDLK_KP3                    = 259,
    SDLK_KP4                    = 260,
    SDLK_KP5                    = 261,
    SDLK_KP6                    = 262,
    SDLK_KP7                    = 263,
    SDLK_KP8                    = 264,
    SDLK_KP9                    = 265,
    SDLK_KP_PERIOD              = 266,
    SDLK_KP_DIVIDE              = 267,
    SDLK_KP_MULTIPLY            = 268,
    SDLK_KP_MINUS               = 269,
    SDLK_KP_PLUS                = 270,
    SDLK_KP_ENTER               = 271,
    SDLK_KP_EQUALS              = 272,

    SDLK_UP                     = 273,
    SDLK_DOWN                   = 274,
    SDLK_RIGHT                  = 275,
    SDLK_LEFT                   = 276,
    SDLK_INSERT                 = 277,
    SDLK_HOME                   = 278,
    SDLK_END                    = 279,
    SDLK_PAGEUP                 = 280,
    SDLK_PAGEDOWN               = 281,

    SDLK_F1                     = 282,
    SDLK_F2                     = 283,
    SDLK_F3                     = 284,
    SDLK_F4                     = 285,
    SDLK_F5                     = 286,
    SDLK_F6                     = 287,
    SDLK_F7                     = 288,
    SDLK_F8                     = 289,
    SDLK_F9                     = 290,
    SDLK_F10                    = 291,
    SDLK_F11                    = 292,
    SDLK_F12                    = 293,
    SDLK_F13                    = 294,
    SDLK_F14                    = 295,
    SDLK_F15                    = 296,
};

enum {
    SDL_HAT_CENTERED            = 0x00,
    SDL_HAT_UP                  = 0x01,
    SDL_HAT_RIGHT               = 0x02,
    SDL_HAT_DOWN                = 0x04,
    SDL_HAT_LEFT                = 0x08,
    SDL_HAT_RIGHTUP             = (SDL_HAT_RIGHT | SDL_HAT_UP),
    SDL_HAT_RIGHTDOWN           = (SDL_HAT_RIGHT | SDL_HAT_DOWN),
    SDL_HAT_LEFTUP              = (SDL_HAT_LEFT | SDL_HAT_UP),
    SDL_HAT_LEFTDOWN            = (SDL_HAT_LEFT | SDL_HAT_DOWN),
};

typedef struct SDL_Keysym {
//    SDL_Scancode scancode;
    SDL_Keycode sym;
    uint16_t mod;
    uint32_t unused;
} SDL_Keysym;

typedef struct SDL_KeyboardEvent {
    uint32_t type;
    uint32_t timestamp;
    uint32_t windowID;
    SDL_Keysym keysym;
    uint8_t state;
    uint8_t repeat;
} SDL_KeyboardEvent;

typedef int SDL_JoystickID;
typedef struct SDL_Joystick SDL_Joystick;
typedef struct SDL_JoyHatEvent {
    uint32_t type;
    uint32_t timestamp;
    SDL_JoystickID which;
    uint8_t hat;
    uint8_t value;
} SDL_JoyHatEvent;
typedef struct SDL_JoyAxisEvent {
    uint32_t type;
    uint32_t timestamp;
    SDL_JoystickID which;
    uint8_t axis;
    int16_t value;
} SDL_JoyAxisEvent;
typedef struct SDL_JoyBallEvent {
    uint32_t type;
    uint32_t timestamp;
    SDL_JoystickID which;
    uint8_t ball;
    int16_t xrel;
    int16_t yrel;
} SDL_JoyBallEvent;
typedef struct SDL_JoyButtonEvent {
    uint32_t type;
    uint32_t timestamp;
    SDL_JoystickID which;
    uint8_t button;
    uint8_t state;
} SDL_JoyButtonEvent;
typedef struct SDL_JoyDeviceEvent {
    uint32_t type;
    uint32_t timestamp;
    int32_t which;
} SDL_JoyDeviceEvent;
typedef struct SDL_MouseMotionEvent {
    uint32_t type;
    uint32_t timestamp;
    uint32_t windowID;
    uint32_t which;
    uint32_t state;
    int32_t x;
    int32_t y;
    int32_t xrel;
    int32_t yrel;
} SDL_MouseMotionEvent;
enum {
    SDL_BUTTON_LEFT,
    SDL_BUTTON_MIDDLE,
    SDL_BUTTON_RIGHT,
    SDL_BUTTON_X1,
    SDL_BUTTON_X2,
};
typedef struct SDL_MouseButtonEvent {
    uint32_t type;
    uint32_t timestamp;
    uint32_t windowID;
    uint32_t which;
    uint8_t button;
    uint8_t state;
    uint8_t clicks;
    int32_t x;
    int32_t y;
} SDL_MouseButtonEvent;
typedef struct SDL_MouseWheelEvent {
    uint32_t type;
    uint32_t timestamp;
    uint32_t windowID;
    uint32_t which;
    int32_t x;
    int32_t y;
    uint32_t direction;
} SDL_MouseWheelEvent;

typedef struct SDL_Cursor SDL_Cursor;
typedef enum SDL_SystemCursor {
    SDL_SYSTEM_CURSOR_ARROW,
    SDL_NUM_SYSTEM_CURSORS,
} SDL_SystemCursor;

#if VSF_SDL_CFG_V1_COMPATIBLE == ENABLED
enum {
    SDL_APPMOUSEFOCUS           = 1 << 0,
    SDL_APPINPUTFOCUS           = 1 << 1,
    SDL_APPACTIVE               = 1 << 2,
};
typedef struct SDL_ActiveEvent {
    uint8_t type;
    uint8_t gain;
    uint8_t state;
} SDL_ActiveEvent;
#endif

typedef union SDL_Event {
    uint32_t type;
    SDL_KeyboardEvent key;

    SDL_JoyAxisEvent jaxis;
    SDL_JoyBallEvent jball;
    SDL_JoyHatEvent jhat;
    SDL_JoyButtonEvent jbutton;
    SDL_JoyDeviceEvent jdevice;
    SDL_MouseMotionEvent motion;
    SDL_MouseButtonEvent button;
    SDL_MouseWheelEvent wheel;

#if VSF_SDL_CFG_V1_COMPATIBLE == ENABLED
    SDL_ActiveEvent active;
#endif
} SDL_Event;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_sdl2_init(vk_disp_t *disp);

extern int SDL_Init(uint32_t flags);
extern int SDL_InitSubSystem(uint32_t flags);
extern void SDL_Quit(void);

extern const char *SDL_GetError(void);

extern SDL_Window * SDL_CreateWindow(const char *title, int x, int y, int w, int h, uint32_t flags);
extern void SDL_DestroyWindow(SDL_Window * window);

extern int SDL_GetDesktopDisplayMode(int display_index, SDL_DisplayMode *mode);

extern SDL_Surface * SDL_CreateRGBSurfaceWithFormat(uint32_t flags, int w, int h, int depth, uint32_t format);
extern void SDL_FreeSurface(SDL_Surface *surface);

extern SDL_Renderer * SDL_CreateRenderer(SDL_Window * window, int index, uint32_t flags);
extern void SDL_DestroyRenderer(SDL_Renderer * renderer);
extern int SDL_RenderClear(SDL_Renderer * renderer);
extern int SDL_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect);
extern void SDL_RenderPresent(SDL_Renderer * renderer);

extern SDL_Texture * SDL_CreateTexture(SDL_Renderer * renderer, uint32_t format, int access, int w, int h);
extern SDL_Texture * SDL_CreateTextureFromSurface(SDL_Renderer *renderer, SDL_Surface *surface);
extern void SDL_DestroyTexture(SDL_Texture * texture);
extern int SDL_UpdateTexture(SDL_Texture * texture, const SDL_Rect * rect, const void *pixels, int pitch);

extern int SDL_LockSurface(SDL_Surface * surface);
extern void SDL_UnlockSurface(SDL_Surface * surface);

extern SDL_sem * SDL_CreateSemaphore(uint32_t initial_value);
extern void SDL_DestroySemaphore(SDL_sem * sem);
extern int __vsf_sdl2_sem_wait(SDL_sem * sem, int32_t ms);
extern int SDL_SemPost(SDL_sem * sem);

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
extern void SDL_Delay(uint32_t ms);
extern uint32_t SDL_GetTicks(void);
#endif

extern int SDL_OpenAudio(SDL_AudioSpec * desired, SDL_AudioSpec * obtained);
extern void SDL_PauseAudio(int pause_on);
extern SDL_AudioStatus SDL_GetAudioStatus(void);
extern void SDL_CloseAudio(void);

extern int SDL_PollEvent(SDL_Event * event);

extern SDL_Cursor * SDL_CreateCursor(const uint8_t * data, const uint8_t * mask,
                                            int w, int h, int hot_x, int hot_y);
extern SDL_Cursor * SDL_CreateColorCursor(SDL_Surface *surface, int hot_x, int hot_y);
extern SDL_Cursor * SDL_CreateSystemCursor(SDL_SystemCursor id);
extern void SDL_SetCursor(SDL_Cursor * cursor);
extern SDL_Cursor * SDL_GetCursor(void);
extern SDL_Cursor * SDL_GetDefaultCursor(void);
extern void SDL_FreeCursor(SDL_Cursor * cursor);
extern int SDL_ShowCursor(int toggle);

extern int SDL_NumJoysticks(void);
extern SDL_Joystick * SDL_JoystickOpen(int device_index);
extern int SDL_JoystickEventState(int state);
extern int SDL_JoystickNumButtons(SDL_Joystick * joystick);
extern int SDL_JoystickNumAxes(SDL_Joystick * joystick);
extern int SDL_JoystickNumBalls(SDL_Joystick * joystick);
extern int SDL_JoystickNumHats(SDL_Joystick * joystick);

#if VSF_SDL_CFG_V1_COMPATIBLE == ENABLED
extern SDL_Overlay * SDL_CreateYUVOverlay(int width, int height, uint32_t format, SDL_Surface *display);
extern void SDL_FreeYUVOverlay(SDL_Overlay * overlay);
extern int SDL_LockYUVOverlay(SDL_Overlay *overlay);
extern void SDL_UnlockYUVOverlay(SDL_Overlay *overlay);
extern int SDL_DisplayYUVOverlay(SDL_Overlay *overlay, SDL_Rect *dstrect);

extern SDL_Surface * SDL_SetVideoMode(int width, int height, int bpp, uint32_t flags);
extern int SDL_Flip(SDL_Surface *screen);

extern void SDL_WM_SetCaption(const char *title, const char *icon);
#endif

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_SDL
#endif      // __SDL_H__