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

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_WINGDI == ENABLED && defined(__WIN__)

#define __VSF_DISP_CLASS_INHERIT__
#define __VSF_DISP_WINGDI_CLASS_IMPLEMENT

#include "../../vsf_disp.h"

#include "component/input/vsf_input.h"

#include <Windows.h>

/*============================ MACROS ========================================*/

#ifndef VSF_DISP_WINGDI_CFG_HW_PRIORITY
#   define VSF_DISP_WINGDI_CFG_HW_PRIORITY              vsf_arch_prio_0
#endif

#ifndef VSF_DISP_WINGDI_CFG_WINDOW_TITLE
#   define VSF_DISP_WINGDI_CFG_WINDOW_TITLE             "vsf display wingdi"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_vsf_bitmap(vsf_disp_wingdi_key_state_map, 256);

typedef struct vsf_disp_wingdi_t {
    vsf_arch_irq_thread_t thread;
    bool is_inited;
    bool is_notified;
    vk_disp_wingdi_t *disp;

    BITMAPINFO bmi;
    HWND hWnd;
    HDC hFrameDC;
    HBITMAP hFrameBitmap;
    void *pixels;

    struct {
        vsf_bitmap(vsf_disp_wingdi_key_state_map) state;
    } kb;
} vsf_disp_wingdi_t;

/*============================ LOCAL VARIABLES ===============================*/

static vsf_disp_wingdi_t __vk_disp_wingdi;

/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_disp_wingdi_init(vk_disp_t *pthis);
static vsf_err_t __vk_disp_wingdi_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);
static void __vk_disp_wingdi_event_thread(void *arg);

#if VSF_USE_INPUT == ENABLED
extern void vsf_input_on_mouse(vk_mouse_evt_t *mouse_evt);
extern void vsf_input_on_touchscreen(vk_touchscreen_evt_t *ts_evt);
extern void vsf_input_on_gamepad(vk_gamepad_evt_t *gamepad_evt);
extern void vsf_input_on_keyboard(vk_keyboard_evt_t *keyboard_evt);
#endif

/*============================ GLOBAL VARIABLES ==============================*/

const vk_disp_drv_t vk_disp_drv_wingdi = {
    .init           = __vk_disp_wingdi_init,
    .refresh        = __vk_disp_wingdi_refresh,
};

/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_INPUT == ENABLED
static uint_fast16_t __vk_disp_win_keymod(void)
{
    uint_fast16_t keymod = 0;
    if (vsf_bitmap_get(&__vk_disp_wingdi.kb.state, VK_SHIFT)) {
        keymod |= VSF_KM_LEFT_SHIFT;
    }
    if (vsf_bitmap_get(&__vk_disp_wingdi.kb.state, VK_CONTROL)) {
        keymod |= VSF_KM_LEFT_CTRL;
    }
    if (vsf_bitmap_get(&__vk_disp_wingdi.kb.state, VK_MENU)) {
        keymod |= VSF_KM_LEFT_ALT;
    }
    if (vsf_bitmap_get(&__vk_disp_wingdi.kb.state, VK_LWIN)) {
        keymod |= VSF_KM_LEFT_GUI;
    }
    if (vsf_bitmap_get(&__vk_disp_wingdi.kb.state, VK_RWIN)) {
        keymod |= VSF_KM_RIGHT_GUI;
    }

    if (GetKeyState(VK_NUMLOCK) & 1) {
        keymod |= VSF_KM_NUMLOCK;
    }
    if (GetKeyState(VK_CAPITAL) & 1) {
        keymod |= VSF_KM_CAPSLOCK;
    }
    if (GetKeyState(VK_SCROLL) & 1) {
        keymod |= VSF_KM_SCROLLLOCK;
    }
    return keymod;
}

static uint_fast16_t __vk_disp_win_keycode_remap(uint8_t keycode)
{
    uint_fast16_t ext = vsf_bitmap_get(&__vk_disp_wingdi.kb.state, VK_SHIFT) ? VSF_KB_EXT : 0;

    // https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
    if ((keycode >= '1') && (keycode <= '9')) {
        return (VSF_KB_1 | ext) + (keycode - '1');
    } else if (keycode == '0') {
        return VSF_KB_0 | ext;
    } else if ((keycode >= 'A') && (keycode <= 'Z')) {
        if (GetKeyState(VK_CAPITAL) & 0x0001) {
            ext ^= VSF_KB_EXT;
        }
        return (VSF_KB_a | ext) + (keycode - 0x41);
    } else if ((keycode >= VK_F1) && (keycode <= VK_F24)) {
        return (keycode <= VK_F12) ? VSF_KB_F1 + keycode - VK_F1 : 0;
    }
    switch (keycode) {
    case VK_SHIFT:              return VSF_KB_LSHIFT;
    case VK_CONTROL:            return VSF_KB_LCTRL;
    case VK_MENU:               return VSF_KB_LALT;
    case VK_CAPITAL:            return VSF_KB_CAPSLOCK;
    case VK_RETURN:             return VSF_KB_ENTER;
    case VK_ESCAPE:             return VSF_KB_ESCAPE;
    case VK_BACK:               return VSF_KB_BACKSPACE;
    case VK_TAB:                return VSF_KB_TAB;
    case VK_SPACE:              return VSF_KB_SPACE;
    case VK_OEM_PLUS:           return VSF_KB_EQUAL | ext;
    case VK_OEM_COMMA:          return VSF_KB_COMMA | ext;
    case VK_OEM_MINUS:          return VSF_KB_MINUS | ext;
    case VK_OEM_PERIOD:         return VSF_KB_DOT | ext;
    case VK_OEM_1:              return VSF_KB_SEMICOLON | ext;
    case VK_OEM_2:              return VSF_KB_SLASH | ext;
    case VK_OEM_3:              return VSF_KB_GRAVE | ext;
    case VK_OEM_4:              return VSF_KB_LEFT_BRACKET | ext;
    case VK_OEM_5:              return VSF_KB_BACKSLASH | ext;
    case VK_OEM_6:              return VSF_KB_RIGHT_BRACKET | ext;
    case VK_OEM_7:              return VSF_KB_SINGLE_QUOTE | ext;

    case VK_SNAPSHOT:           return VSF_KB_PRINT_SCREEN;
    case VK_SCROLL:             return VSF_KB_SCROLL_LOCK;
    case VK_PAUSE:              return VSF_KB_PAUSE;
    case VK_INSERT:             return VSF_KB_INSERT;
    case VK_HOME:               return VSF_KB_HOME;
    case VK_PRIOR:              return VSF_KB_PAGE_UP;
    case VK_DELETE:             return VSF_KB_DELETE;
    case VK_END:                return VSF_KB_END;
    case VK_NEXT:               return VSF_KB_PAGE_DOWN;
    case VK_RIGHT:              return VSF_KB_RIGHT;
    case VK_LEFT:               return VSF_KB_LEFT;
    case VK_DOWN:               return VSF_KB_DOWN;
    case VK_UP:                 return VSF_KB_UP;

    case VK_NUMLOCK:            return VSF_KP_NUMLOCK;
    case VK_DIVIDE:             return VSF_KP_DIVIDE;
    case VK_MULTIPLY:           return VSF_KP_MULTIPLY;
    case VK_SUBTRACT:           return VSF_KP_MINUS;
    case VK_ADD:                return VSF_KP_PLUS;
//    case VK_SEPARATOR           return VSF_KP_??;
    case VK_DECIMAL:            return VSF_KP_DOT;
    case VK_LWIN:               return VSF_KB_LGUI;
    case VK_RWIN:               return VSF_KB_RGUI;
    case VK_NUMPAD0:            return VSF_KP_0;
    case VK_NUMPAD1:            return VSF_KP_1;
    case VK_NUMPAD2:            return VSF_KP_2;
    case VK_NUMPAD3:            return VSF_KP_3;
    case VK_NUMPAD4:            return VSF_KP_4;
    case VK_NUMPAD5:            return VSF_KP_5;
    case VK_NUMPAD6:            return VSF_KP_6;
    case VK_NUMPAD7:            return VSF_KP_7;
    case VK_NUMPAD8:            return VSF_KP_8;
    case VK_NUMPAD9:            return VSF_KP_9;
    }
    return 0;
}
#endif

static LRESULT CALLBACK __WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    vk_disp_wingdi_t *disp_wingdi = __vk_disp_wingdi.disp;
    vsf_arch_irq_thread_t *irq_thread = &__vk_disp_wingdi.thread;

#if VSF_USE_INPUT == ENABLED
    union {
        implement(vk_input_evt_t)
        vk_touchscreen_evt_t    ts_evt;
        vk_gamepad_evt_t        gamepad_evt;
        vk_keyboard_evt_t       keyboard_evt;
        vk_mouse_evt_t          mouse_evt;
    } evt;
    vk_input_type_t evt_type;

    uint_fast16_t x = 0, y = 0;
    uint_fast8_t pressure = 0;
    bool is_down = false, is_event_triggered = false;
    int_fast16_t wheel_x = 0, wheel_y = 0;
    int mouse_evt = 0, mouse_button = 0;

    evt.dev = disp_wingdi;
#endif

    switch (msg) {
    case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            BitBlt(hdc,
                ps.rcPaint.left, ps.rcPaint.top,
                ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top,
                __vk_disp_wingdi.hFrameDC, ps.rcPaint.left, ps.rcPaint.top,
                SRCCOPY);
            EndPaint(hWnd, &ps);

            __vsf_arch_irq_start(irq_thread);
                if (!__vk_disp_wingdi.is_notified) {
                    __vk_disp_wingdi.is_notified = true;
                    vk_disp_on_ready(&disp_wingdi->use_as__vk_disp_t);
                }
            __vsf_arch_irq_end(irq_thread, false);
        }
        break;
    case WM_SIZE:
        __vk_disp_wingdi.bmi.bmiHeader.biWidth  = LOWORD(lParam);
        __vk_disp_wingdi.bmi.bmiHeader.biHeight = HIWORD(lParam);

        if (__vk_disp_wingdi.hFrameBitmap) {
            DeleteObject(__vk_disp_wingdi.hFrameBitmap);
        }
        __vk_disp_wingdi.hFrameBitmap = CreateDIBSection(NULL, &__vk_disp_wingdi.bmi, DIB_RGB_COLORS, &__vk_disp_wingdi.pixels, 0, 0);
        SelectObject(__vk_disp_wingdi.hFrameDC, __vk_disp_wingdi.hFrameBitmap);
        break;
#if VSF_USE_INPUT == ENABLED
    case WM_SYSCHAR:
        break;
    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
        if (vsf_bitmap_get(&__vk_disp_wingdi.kb.state, wParam & 0xFF)) {
            break;
        }
        vsf_bitmap_set(&__vk_disp_wingdi.kb.state, wParam & 0xFF);
        goto issue_kb_event;
    case WM_SYSKEYUP:
    case WM_KEYUP:
        vsf_bitmap_clear(&__vk_disp_wingdi.kb.state, wParam & 0xFF);
    issue_kb_event: {
            is_event_triggered = true;
            evt_type = VSF_INPUT_TYPE_KEYBOARD;
            vsf_input_keyboard_set(&evt.keyboard_evt,
                __vk_disp_win_keycode_remap(wParam & 0xFF),
                WM_KEYDOWN == msg || WM_SYSKEYDOWN == msg,
                __vk_disp_win_keymod());
        }
        break;
#   if VSF_DISP_WINGDI_CFG_MOUSE_AS_TOUCHSCREEN == ENABLED
    case WM_LBUTTONUP:
        is_down = false;
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        is_event_triggered = true;
        evt_type = VSF_INPUT_TYPE_TOUCHSCREEN;
        break;
    case WM_LBUTTONDOWN:
        is_down = true;
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        is_event_triggered = true;
        evt_type = VSF_INPUT_TYPE_TOUCHSCREEN;
        break;
    case WM_MOUSEMOVE:
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        is_event_triggered = is_down;
        evt_type = VSF_INPUT_TYPE_TOUCHSCREEN;
        break;
#   else
    case WM_MBUTTONDOWN:
        is_down = true;
        mouse_button = VSF_INPUT_MOUSE_BUTTON_MIDDLE;
        goto issue_mouse_btn_event;
    case WM_MBUTTONUP:
        is_down = false;
        mouse_button = VSF_INPUT_MOUSE_BUTTON_MIDDLE;
        goto issue_mouse_btn_event;
    case WM_RBUTTONDOWN:
        is_down = true;
        mouse_button = VSF_INPUT_MOUSE_BUTTON_RIGHT;
        goto issue_mouse_btn_event;
    case WM_RBUTTONUP:
        is_down = false;
        mouse_button = VSF_INPUT_MOUSE_BUTTON_RIGHT;
        goto issue_mouse_btn_event;
    case WM_LBUTTONUP:
        is_down = false;
        mouse_button = VSF_INPUT_MOUSE_BUTTON_LEFT;
        goto issue_mouse_btn_event;
    case WM_LBUTTONDOWN:
        is_down = true;
        mouse_button = VSF_INPUT_MOUSE_BUTTON_LEFT;

    issue_mouse_btn_event:
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        mouse_evt = VSF_INPUT_MOUSE_EVT_BUTTON;
        is_event_triggered = true;
        evt_type = VSF_INPUT_TYPE_MOUSE;
        break;
    case WM_MOUSEMOVE:
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        mouse_evt = VSF_INPUT_MOUSE_EVT_MOVE;
        is_event_triggered = true;
        evt_type = VSF_INPUT_TYPE_MOUSE;
        break;
    case WM_MOUSEWHEEL:
        wheel_y = (short)HIWORD(wParam) / WHEEL_DELTA;
        mouse_evt = VSF_INPUT_MOUSE_EVT_WHEEL;
        is_event_triggered = true;
        evt_type = VSF_INPUT_TYPE_MOUSE;
        break;
#   endif
#endif
    case WM_CLOSE:
        DestroyWindow(hWnd);
// is console is hidden, exit process if window is closed
#if VSF_ARCH_CFG_HIDE_CONSOLE == ENABLED
        ExitProcess(0);
#endif
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    if (is_event_triggered) {
        __vsf_arch_irq_start(irq_thread);
        switch (evt_type) {
        case VSF_INPUT_TYPE_TOUCHSCREEN:
            evt.ts_evt.info.width = disp_wingdi->param.width;
            evt.ts_evt.info.height = disp_wingdi->param.height;
            vsf_input_touchscreen_set(&evt.ts_evt, 0, is_down, pressure, x, y);
            vsf_input_on_touchscreen(&evt.ts_evt);
            break;
        case VSF_INPUT_TYPE_GAMEPAD:
            vsf_input_on_gamepad(&evt.gamepad_evt);
            break;
        case VSF_INPUT_TYPE_KEYBOARD:
            vsf_input_on_keyboard(&evt.keyboard_evt);
            break;
        case VSF_INPUT_TYPE_MOUSE:
            switch (mouse_evt) {
            case VSF_INPUT_MOUSE_EVT_BUTTON:
                vk_input_mouse_evt_button_set(&evt.mouse_evt, mouse_button, is_down, x, y);
                break;
            case VSF_INPUT_MOUSE_EVT_MOVE:
                vk_input_mouse_evt_move_set(&evt.mouse_evt, x, y);
                break;
            case VSF_INPUT_MOUSE_EVT_WHEEL:
                vk_input_mouse_evt_wheel_set(&evt.mouse_evt, wheel_x, wheel_y);
                break;
            }
            vsf_input_on_mouse(&evt.mouse_evt);
            break;
        }
        __vsf_arch_irq_end(irq_thread, false);
    }

    return 0;
}

static void __vk_disp_wingdi_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vk_disp_wingdi_t *disp_wingdi = __vk_disp_wingdi.disp;

    static const wchar_t __ClassName[] = L"vsf_display";
    HINSTANCE hInstance = GetModuleHandle(NULL);

    __vsf_arch_irq_set_background(irq_thread);

    // step 1: Register Windows Class
    WNDCLASS wc = {
        .lpfnWndProc    = __WindowProc,
        .lpszClassName  = __ClassName,
        .hInstance      = hInstance,
    };
    if (!RegisterClass(&wc)) {
        VSF_UI_ASSERT(false);
        return;
    }

    // Step 2: Create FrameDC
    __vk_disp_wingdi.bmi.bmiHeader.biSize = sizeof(__vk_disp_wingdi.bmi.bmiHeader);
    __vk_disp_wingdi.bmi.bmiHeader.biPlanes = 1;
    __vk_disp_wingdi.bmi.bmiHeader.biBitCount = 32;
    __vk_disp_wingdi.bmi.bmiHeader.biCompression = BI_RGB;
    __vk_disp_wingdi.hFrameDC = CreateCompatibleDC(0);
    if (NULL == __vk_disp_wingdi.hFrameDC) {
        VSF_UI_ASSERT(false);
        return;
    }

    // step 3: Create Window
    RECT rect = {
        .left   = 0,
        .top    = 0,
        .right  = disp_wingdi->param.width,
        .bottom = disp_wingdi->param.height,
    };
    AdjustWindowRect(&rect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, false);
    __vk_disp_wingdi.hWnd = CreateWindow(__ClassName,
        TEXT(VSF_DISP_WINGDI_CFG_WINDOW_TITLE),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, wc.hInstance, NULL);
    if (NULL == __vk_disp_wingdi.hWnd) {
        VSF_UI_ASSERT(false);
        return;
    }

    // Step 4: Message Handling
    MSG Msg;
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    __vsf_arch_irq_fini(irq_thread);
}

static vsf_err_t __vk_disp_wingdi_init(vk_disp_t *pthis)
{
    vk_disp_wingdi_t *disp_wingdi = (vk_disp_wingdi_t *)pthis;
    VSF_UI_ASSERT(disp_wingdi != NULL);

    if (!__vk_disp_wingdi.is_inited) {
        __vk_disp_wingdi.is_inited = true;
        __vk_disp_wingdi.is_notified = false;
        __vk_disp_wingdi.disp = (vk_disp_wingdi_t *)pthis;
        __vsf_arch_irq_init(&__vk_disp_wingdi.thread, "disp_windgi", __vk_disp_wingdi_thread, VSF_DISP_WINGDI_CFG_HW_PRIORITY);        
    } else {
        vk_disp_on_ready(&disp_wingdi->use_as__vk_disp_t);
    }
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_disp_wingdi_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff)
{
    vk_disp_wingdi_t *disp_wingdi = (vk_disp_wingdi_t *)pthis;
    VSF_UI_ASSERT(disp_wingdi != NULL);
    uint8_t bytesize_pixel = vsf_disp_get_pixel_bytesize(disp_wingdi);
    uint32_t bytesize_line_area = area->size.x * bytesize_pixel;

    uint32_t wingdi_bytesize_line = disp_wingdi->param.width * 4;
    uint32_t wingdi_byteoffset = ((disp_wingdi->param.height - 1 - area->pos.y) * disp_wingdi->param.width + area->pos.x) * 4;
    void *ptr = (void *)((uint8_t *)__vk_disp_wingdi.pixels + wingdi_byteoffset);

    for (uint16_t i = 0; i < area->size.y; i++) {
        switch (vsf_disp_get_pixel_format(disp_wingdi)) {
        case VSF_DISP_COLOR_RGB332: {
                // convert to RGB888
                uint32_t color;
                for (uint16_t j = 0; j < area->size.x; j++) {
                    color = (uint32_t)(((uint8_t *)disp_buff)[j]);
                    ((uint32_t *)ptr)[j] =
                            ((color & 0x03) << 6)
                        |   ((color & 0x1C) << 11)
                        |   ((color & 0xE0) << 16);
                }
            }
            break;
        case VSF_DISP_COLOR_RGB565: {
                // convert to RGB888
                uint32_t color;
                for (uint16_t j = 0; j < area->size.x; j++) {
                    color = (uint32_t)(((uint16_t *)disp_buff)[j]);
                    ((uint32_t *)ptr)[j] =
                            ((color & 0x001F) << 3)
                        |   ((color & 0x07E0) << 5)
                        |   ((color & 0xF800) << 8);
                }
            }
            break;
        case VSF_DISP_COLOR_RGB888_32:
        case VSF_DISP_COLOR_ARGB8888:
            for (uint16_t j = 0; j < area->size.x; j++) {
                ((uint32_t *)ptr)[j] = ((uint32_t *)disp_buff)[j];
            }
            break;
        default:
            // TODO: add support to other color formats
            VSF_UI_ASSERT(false);
            break;
        }

        ptr = (void *)((uint8_t *)ptr - wingdi_bytesize_line);
        disp_buff = (void *)((uint8_t *)disp_buff + bytesize_line_area);
    }
    RECT rect = {
        .left   = area->pos.x,
        .top    = area->pos.y,
        .right  = area->pos.x + area->size.x,
        .bottom = area->pos.y + area->size.y,
    };
    __vk_disp_wingdi.is_notified = false;
    InvalidateRect(__vk_disp_wingdi.hWnd, &rect, false);
    return VSF_ERR_NONE;
}

#endif

/* EOF */
