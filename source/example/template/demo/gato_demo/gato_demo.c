/*============================ INCLUDES ======================================*/

#define __VSF_DISP_CLASS_INHERIT__
#include "vsf.h"

#if VSF_USE_UI == ENABLED && VSF_USE_GATO == ENABLED && APP_USE_GATO_DEMO == ENABLED

#include "../common/usrapp_common.h"

#include "render.h"
#include "surface.h"
#include "image.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct usrapp_gato_t {
    struct {
        int x;
        int y;
    } mouse;
    vk_input_notifier_t notifier;
} usrapp_gato_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static usrapp_gato_t __usrapp_gato;

/*============================ PROTOTYPES ====================================*/

extern void sample(surface_t *base, float fps);

/*============================ IMPLEMENTATION ================================*/

void motion_get_xy(int *x, int *y)
{
    *x = __usrapp_gato.mouse.x;
    *y = __usrapp_gato.mouse.y;
}

static void __gato_on_evt(vk_input_type_t type, vk_input_evt_t *evt)
{
    switch (type) {
    case VSF_INPUT_TYPE_TOUCHSCREEN: {
            vk_touchscreen_evt_t *ts_evt = (vk_touchscreen_evt_t *)evt;
            if (0 == vsf_input_touchscreen_get_id(ts_evt)) {
                __usrapp_gato.mouse.x = vsf_input_touchscreen_get_x(ts_evt);
                __usrapp_gato.mouse.y = vsf_input_touchscreen_get_y(ts_evt);
            }
        }
        break;
    }
}

static void __vsf_gato_disp_on_ready(vk_disp_t *disp)
{
    vsf_eda_post_evt(disp->ui_data, VSF_EVT_USER);
}

#if APP_USE_LINUX_DEMO == ENABLED
int gato_main(int argc, char *argv[])
{
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#   endif
#endif

    vk_disp_t *disp = &usrapp_ui_common.disp.use_as__vk_disp_t;
    // TODO: ugly and dangerous code to fix color to ARGB8888, only valid on disp_sdl2
    *(vk_disp_color_type_t *)&disp->param.color = VSF_DISP_COLOR_ARGB8888;

    float fps = 50;
    surface_t surface = { 0 };
    color_t *pixels = vsf_heap_malloc(disp->param.height * disp->param.width * vsf_disp_get_pixel_bytesize(disp));
    if (NULL == pixels) {
        return -1;
    }

    disp->ui_data = vsf_eda_get_cur();
    disp->ui_on_ready = __vsf_gato_disp_on_ready;
    vk_disp_init(disp);

    surface_wrap(&surface, pixels, disp->param.width, disp->param.height);

    __usrapp_gato.notifier.mask = 1 << VSF_INPUT_TYPE_TOUCHSCREEN;
    __usrapp_gato.notifier.on_evt = (vk_input_on_evt_t)__gato_on_evt;
    vk_input_notifier_register(&__usrapp_gato.notifier);

    while (1) {
        sample(&surface, fps);
        vk_disp_refresh(disp, NULL, pixels);
        vsf_thread_wfe(VSF_EVT_USER);
        vsf_thread_delay_ms(20);
    }
    return 0;
}

#endif
