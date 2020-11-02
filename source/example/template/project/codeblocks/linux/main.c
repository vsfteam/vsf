#include "vsf.h"

vk_disp_sdl2_t disp = {
    .param                  = {
        .height             = APP_DISP_SDL2_HEIGHT,
        .width              = APP_DISP_SDL2_WIDTH,
        .drv                = &vk_disp_drv_sdl2,
        .color              = APP_DISP_SDL2_COLOR,
    },
    .title                  = APP_DISP_SDL2_TITLE,
    .amplifier              = APP_DISP_SDL2_AMPLIFIER,
};

int main(void)
{
    int tick = 0;

    vsf_start_trace();
    vsf_trace(VSF_TRACE_INFO, "System started..." VSF_TRACE_CFG_LINEEND);

    vk_disp_init(&disp.use_as__vk_disp_t);

    while (1) {
        vsf_thread_delay_ms(1000);
        tick++;
        vsf_trace(VSF_TRACE_INFO, "tick %d" VSF_TRACE_CFG_LINEEND, tick);
    }
    return 0;
}
