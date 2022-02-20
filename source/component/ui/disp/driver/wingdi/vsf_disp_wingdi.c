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
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_vsf_bitmap(vsf_disp_wingdi_key_state_map, 256);

typedef struct vsf_disp_wingdi_t {
    vsf_arch_irq_thread_t thread;
    bool is_inited;
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
static uint_fast8_t __vk_disp_win_keymod(void)
{
    uint_fast8_t keymod = 0;
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
    return keymod;
}

static uint_fast16_t __vk_disp_win_keycode_remap(uint8_t keycode)
{
    uint_fast16_t ext = vsf_bitmap_get(&__vk_disp_wingdi.kb.state, VK_SHIFT) ? VSF_KB_EXT : 0;

    // https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
    if ((keycode >= '0') && (keycode <= '9')) {
        return (VSF_KB_0 | ext) + (keycode - 0x30);
    } else if ((keycode >= 'A') && (keycode <= 'Z')) {
        if (GetKeyState(VK_CAPITAL) & 0x0001) {
            ext ^= VSF_KB_EXT;
        }
        return (VSF_KB_a | ext) + (keycode - 0x41);
    } else if ((keycode >= VK_F1) && (keycode <= VK_F24)) {
        return (keycode <= VK_F12) ? VSF_KB_F1 + keycode - VK_F1 : 0;
    }
    switch (keycode) {
    case VK_CAPITAL:            return VSF_KB_CAPSLOCK;
    case VK_RETURN:             return VSF_KB_ENTER;
    case VK_ESCAPE:             return VSF_KB_ESCAPE;
    case VK_BACK:               return VSF_KB_BACKSPACE;
    case VK_TAB:                return VSF_KB_TAB;
    case VK_SPACE:              return VSF_KB_SPACE;
    case VK_OEM_PLUS:           return VSF_KB_PLUS | ext;
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
//    case SDLK_KP_ENTER:         return VSF_KP_ENTER;
    case VK_DECIMAL:            return VSF_KP_DOT;
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
                vk_disp_on_ready(&disp_wingdi->use_as__vk_disp_t);
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
    case WM_KEYDOWN:
        if (vsf_bitmap_get(&__vk_disp_wingdi.kb.state, wParam & 0xFF)) {
            break;
        }
        vsf_bitmap_set(&__vk_disp_wingdi.kb.state, wParam & 0xFF);
        goto issue_kb_event;
    case WM_KEYUP:
        vsf_bitmap_clear(&__vk_disp_wingdi.kb.state, wParam & 0xFF);
    issue_kb_event: {
            is_event_triggered = true;
            evt_type = VSF_INPUT_TYPE_KEYBOARD;
            vsf_input_keyboard_set(&evt.keyboard_evt,
                __vk_disp_win_keycode_remap(wParam & 0xFF),
                WM_KEYDOWN == msg,
                __vk_disp_win_keymod());
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
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
    __vk_disp_wingdi.bmi.bmiHeader.biBitCount = vsf_disp_get_pixel_bitsize(disp_wingdi);
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
        TEXT("vsf display wingdi"),
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
        __vk_disp_wingdi.disp = (vk_disp_wingdi_t *)pthis;
        __vsf_arch_irq_init(&__vk_disp_wingdi.thread, "disp_windgi", __vk_disp_wingdi_thread, vsf_arch_prio_0);
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
    uint32_t byteoffset = ((disp_wingdi->param.height - 1 - area->pos.y) * disp_wingdi->param.width + area->pos.x) * bytesize_pixel;
    uint32_t bytesize_line = disp_wingdi->param.width * bytesize_pixel;
    uint32_t bytesize_line_area = area->size.x * bytesize_pixel;
    void *ptr = (void *)((uint8_t *)__vk_disp_wingdi.pixels + byteoffset);

    for (uint16_t i = 0; i < area->size.y; i++) {
        memcpy(ptr, disp_buff, bytesize_line_area);
        ptr = (void *)((uint8_t *)ptr - bytesize_line);
        disp_buff = (void *)((uint8_t *)disp_buff + bytesize_line_area);
    }
    RECT rect = {
        .left   = area->pos.x,
        .top    = area->pos.y,
        .right  = area->pos.x + area->size.x,
        .bottom = area->pos.y + area->size.y,
    };
    InvalidateRect(__vk_disp_wingdi.hWnd, &rect, false);
    return VSF_ERR_NONE;
}

#endif

/* EOF */
