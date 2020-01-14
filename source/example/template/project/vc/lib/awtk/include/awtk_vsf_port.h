#ifndef __AWTK_VSF_PORT_H__
#define __AWTK_VSF_PORT_H__

#include "vsf.h"

#include "tkc/mem.h"
#include "tkc/fs.h"
#include "lcd/lcd_mem_fragment.h"
#include "base/pixel.h"
#include "base/main_loop.h"

typedef struct vsf_awtk_op_t {
    void * (*malloc_impl)(size_t size);
    void * (*realloc_impl)(void *p, size_t size);
    void (*free_impl)(void *p);
    int (*printf_impl)(const char *format, ...);
    int (*strcasecmp_impl)(const char *s1, const char *s2);
    int (*vsnprintf_impl)(char *str, size_t size, const char *format, va_list ap);
    int (*vsscanf_impl)(const char *str, const char *format, va_list ap);

    void (*sleep_ms_impl)(uint32_t ms);

    ret_t (*platform_prepare_impl)(void);
    main_loop_t* (*main_loop_init_impl)(int w, int h);
    uint64_t (*get_time_ms64_impl)(void);
    fs_t * (*os_fs_impl)(void);

    vsf_eda_t * (*vsf_eda_get_cur_impl)(void);
    vsf_err_t (*vsf_eda_post_evt_impl)(vsf_eda_t *pthis, vsf_evt_t evt);
    vsf_err_t (*vk_disp_refresh_impl)(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);
    void (*vsf_thread_wfe_impl)(vsf_evt_t evt);
    vsf_err_t (*vk_disp_init_impl)(vk_disp_t *pthis);
} vsf_awtk_op_t;

extern lcd_t *vsf_awtk_create_lcd_mem_fragment(vk_disp_t *disp, wh_t w, wh_t h);
extern void awtk_vsf_init(vsf_awtk_op_t *op);

#endif
