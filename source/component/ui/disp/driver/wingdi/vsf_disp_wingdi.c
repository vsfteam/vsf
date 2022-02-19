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

typedef struct vsf_disp_wingdi_t {
    vsf_arch_irq_thread_t thread;
    bool is_inited;
    vk_disp_wingdi_t *disp;

    BITMAPINFO bmi;
    HWND hWnd;
    HDC hFrameDC;
    HBITMAP hFrameBitmap;
    void *pixels;
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

static LRESULT CALLBACK __WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
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

            vsf_arch_irq_thread_t *irq_thread = &__vk_disp_wingdi.thread;
            vk_disp_wingdi_t *disp_wingdi = __vk_disp_wingdi.disp;
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
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
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
