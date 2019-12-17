/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless requir by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#define __VSF_EDA_CLASS_INHERIT
#include "vsf.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_HCD_LIBUSB == ENABLED
#   include "component/usb/driver/hcd/libusb_hcd/vsf_libusb_hcd.h"
#endif

#include "fakefat32.h"

/*============================ MACROS ========================================*/

#ifndef USRAPP_CFG_FAT32_SIZE
#   define USRAPP_CFG_FAT32_SIZE                (512 * 0x10000)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSF_USE_USB_HOST == ENABLED
struct usrapp_const_t {
#   if VSF_USE_USB_HOST == ENABLED
    struct {
#       if VSF_USE_USB_HOST_HCD_OHCI == ENABLED
        vsf_ohci_param_t ohci_param;
#       elif VSF_USE_USB_HOST_HCD_LIBUSB == ENABLED
        vsf_libusb_hcd_param_t libusb_hcd_param;
#       endif
    } usbh;
#   endif
};
typedef struct usrapp_const_t usrapp_const_t;
#endif

struct usrapp_t {
#if VSF_USE_USB_HOST == ENABLED
    struct {
        vk_usbh_t host;
        vk_usbh_class_t msc;
    } usbh;
#endif

    struct {
        vk_fakefat32_mal_t fakefat32;
    } mal;

    struct {
        uint8_t buffer[1024];
        vsf_mem_stream_t mem_stream;
    } stream;

    struct {
        vsf_teda_t task;
        vk_memfs_info_t memfs_info;
#if VSF_USE_WINFS == ENABLED
        vk_winfs_info_t winfs_info;
#endif
    } fs;
};
typedef struct usrapp_t usrapp_t;

struct usrapp_eda_state_t {
    uint_fast8_t kernel : 2;
    uint_fast8_t state  : 2;
    uint_fast8_t idx    : 4;
};
typedef struct usrapp_eda_state_t usrapp_eda_state_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#if VSF_USE_USB_HOST == ENABLED
static const usrapp_const_t __usrapp_const = {
#   if VSF_USE_USB_HOST == ENABLED
#       if VSF_USE_USB_HOST_HCD_OHCI == ENABLED
    .usbh.ohci_param        = {
        .op                 = &VSF_USB_HC0_IP,
        .priority           = vsf_arch_prio_0,
    },
#       elif VSF_USE_USB_HOST_HCD_LIBUSB == ENABLED
    .usbh.libusb_hcd_param  = {
        .priority = vsf_arch_prio_0,
    },
#       endif
#   endif
};
#endif

static usrapp_t __usrapp = {
#if VSF_USE_USB_HOST == ENABLED
    .usbh                   = {
#   if VSF_USE_USB_HOST_HCD_OHCI == ENABLED
        .host.drv           = &vsf_ohci_drv,
        .host.param         = (void *)&__usrapp_const.usbh.ohci_param,
#   elif VSF_USE_USB_HOST_HCD_LIBUSB == ENABLED
        .host.drv           = &vsf_libusb_hcd_drv,
        .host.param         = (void*)&__usrapp_const.usbh.libusb_hcd_param,
#   endif
//        .msc.drv            = &vk_usbh_msc_drv,
    },
#endif

    .mal                    = {
        .fakefat32          = {
            .drv                = &VK_FAKEFAT32_MAL_DRV,
            .sector_size        = 512,
            .sector_number      = USRAPP_CFG_FAT32_SIZE / 512,
            .sectors_per_cluster= 8,
            .volume_id          = 0x12345678,
            .disk_id            = 0x9ABCEF01,
            .root               = {
                .name           = "ROOT",
                .d.child        = (vk_memfs_file_t *)__fakefat32_root,
                .d.child_num    = dimof(__fakefat32_root),
            },
        },
    },

    .fs.memfs_info          = {
        .root               = {
            .d.child        = (vk_memfs_file_t *)__fakefat32_root,
            .d.child_num    = dimof(__fakefat32_root),
            .d.child_size   = sizeof(vk_fakefat32_file_t),
        },
    },
#if VSF_USE_WINFS == ENABLED
    .fs.winfs_info          = {
        .root               = {
            .name           = "winfs_root",
        },
    },
#endif
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_input_on_touchscreen(vk_touchscreen_evt_t *ts_evt)
{
}

static void __usrapp_print_indent(int indent)
{
    while (indent--) {
        vsf_trace_string(VSF_TRACE_INFO, "  ");
    }
}

static void __usrapp_fs_listall_evthandler(uintptr_t target, vsf_evt_t evt)
{
    usrapp_eda_state_t *state = (usrapp_eda_state_t *)&vsf_eda_get_cur()->fn.frame->state;
    vk_file_t *file = (vk_file_t *)target;
    vsf_err_t err;

    static int __cur_indent = 0;
    static vk_file_t *__cur_file = NULL;
    static uint8_t __buff[256];
    static int32_t __rsize = 0;
    enum {
        // for directory
        LISTALL_STATE_LOOKUP,
        LISTALL_STATE_RECURSION,
        // for file
        LISTALL_STATE_READ,
        // common
        LISTALL_STATE_CLOSE,
    };

    if (LISTALL_STATE_CLOSE == state->state) {
        goto do_return;
    }

    switch (evt) {
    case VSF_EVT_INIT:
        {
            char *name = file->name;
            if (NULL == name) {
                name = "root";
            }
            __usrapp_print_indent(__cur_indent);
            vsf_trace(VSF_TRACE_INFO, "%s(%d):" VSF_TRACE_CFG_LINEEND,
                    name, file->size);
        }

        if (file->attr & VSF_FILE_ATTR_DIRECTORY) {
            state->idx = 0;
            state->state = LISTALL_STATE_LOOKUP;
            if (VSF_ERR_NONE != vk_file_open(file, NULL, state->idx, &__cur_file)) {
                goto do_return;
            }
        } else {
            if (file->attr & VSF_FILE_ATTR_READ) {
                state->state = LISTALL_STATE_READ;
                if (VSF_ERR_NONE != vk_file_read(file, 0, sizeof(__buff), __buff, &__rsize)) {
                    goto do_return;
                }
            } else {
                goto do_return;
            }
        }
        break;
    case VSF_EVT_RETURN:
        err = vk_file_get_errcode(file);
        if (file->attr & VSF_FILE_ATTR_DIRECTORY) {
            switch (state->state) {
            case LISTALL_STATE_LOOKUP:
                if ((err != VSF_ERR_NONE) || (NULL == __cur_file)) {
                    goto do_close;
                }

                __cur_indent++;
                state->idx++;
                state->state = LISTALL_STATE_RECURSION;
                if (VSF_ERR_NONE != vsf_eda_call_param_eda(__usrapp_fs_listall_evthandler, __cur_file)) {
                    goto do_return;
                }
                break;
            case LISTALL_STATE_RECURSION:
                state->state = LISTALL_STATE_LOOKUP;
                if (VSF_ERR_NONE != vk_file_open(file, NULL, state->idx, &__cur_file)) {
                    goto do_return;
                }
                break;
            }
        } else {
            if (VSF_ERR_NONE != err) {
                __usrapp_print_indent(__cur_indent);
                vsf_trace(VSF_TRACE_INFO, "fail to read %s, err is %d" VSF_TRACE_CFG_LINEEND,
                        file->name, err);
            } else {
                vsf_trace_buffer(VSF_TRACE_INFO, __buff, __rsize, VSF_TRACE_DF_DEFAULT);
            }
        do_close:
            state->state = LISTALL_STATE_CLOSE;
            if (VSF_ERR_NONE != vk_file_close(file)) {
                goto do_return;
            }
        }
        break;
    }
    return;
do_return:
    __cur_indent--;
    vsf_eda_return();
}

static void __usrapp_fs_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    enum {
        USRAPP_STATE_OPENED_ROOT,
        USRAPP_STATE_CREATED_MEMFS,
        USRAPP_STATE_OPENED_MEMFS,
        USRAPP_STATE_MOUNTED_MEMFS,
#if VSF_USE_WINFS == ENABLED
        USRAPP_STATE_CREATED_WINFS,
        USRAPP_STATE_OPENED_WINFS,
        USRAPP_STATE_MOUNTED_WINFS,
#endif
        USRAPP_STATE_LISTALL,
        USRAPP_STATE_OPENED_CONTROL,
        USRAPP_STATE_WRITTEN_CONTROL,
    } static __state;
    static vk_file_t *__froot, *__fmemfs, *__fcontrol;
#if VSF_USE_WINFS == ENABLED
    static vk_file_t *__fwinfs;
#endif
    static uint8_t __control = 0;
    static int32_t __size;

    switch (evt) {
    case VSF_EVT_INIT:
        vk_fs_init();
        __state = USRAPP_STATE_OPENED_ROOT;
        __froot = NULL;
        vk_file_open(NULL, "\\", 0, &__froot);
        break;
    case VSF_EVT_RETURN:
        switch (__state) {
        case USRAPP_STATE_OPENED_ROOT:
            ASSERT(__froot != NULL);
            vk_file_create(__froot, "memfs", VSF_FILE_ATTR_DIRECTORY, 0);
            break;
        case USRAPP_STATE_CREATED_MEMFS:
            ASSERT(vk_file_get_errcode(__froot) == VSF_ERR_NONE);
            __fmemfs = NULL;
            vk_file_open(__froot, "memfs", 0, &__fmemfs);
            break;
        case USRAPP_STATE_OPENED_MEMFS:
            ASSERT(vk_file_get_errcode(__froot) == VSF_ERR_NONE);
            ASSERT(__fmemfs != NULL);
            vk_fs_mount(__fmemfs, &vk_memfs_op, &__usrapp.fs.memfs_info);
            break;
        case USRAPP_STATE_MOUNTED_MEMFS:
            ASSERT(vk_file_get_errcode(__fmemfs) == VSF_ERR_NONE);
#if VSF_USE_WINFS == ENABLED
            vk_file_create(__froot, "winfs", VSF_FILE_ATTR_DIRECTORY, 0);
            break;
        case USRAPP_STATE_CREATED_WINFS:
            ASSERT(vk_file_get_errcode(__froot) == VSF_ERR_NONE);
            __fwinfs = NULL;
            vk_file_open(__froot, "winfs", 0, &__fwinfs);
            break;
        case USRAPP_STATE_OPENED_WINFS:
            ASSERT(vk_file_get_errcode(__froot) == VSF_ERR_NONE);
            ASSERT(__fwinfs != NULL);
            vk_fs_mount(__fwinfs, &vk_winfs_op, &__usrapp.fs.winfs_info);
            break;
        case USRAPP_STATE_MOUNTED_WINFS:
            ASSERT(vk_file_get_errcode(__fwinfs) == VSF_ERR_NONE);
#endif
            if (VSF_ERR_NONE == vsf_eda_call_param_eda(__usrapp_fs_listall_evthandler, __froot)) {
                break;
            }
        case USRAPP_STATE_LISTALL:
            __fcontrol = NULL;
            vk_file_open(__fmemfs, "control.bin", 0, &__fcontrol);
            break;
        case USRAPP_STATE_OPENED_CONTROL:
            ASSERT(vk_file_get_errcode(__fmemfs) == VSF_ERR_NONE);
            ASSERT(__fcontrol != NULL);
        case USRAPP_STATE_WRITTEN_CONTROL:
            ASSERT(vk_file_get_errcode(__fcontrol) == VSF_ERR_NONE);
            vsf_teda_set_timer_ms(1000);
            return;
        }
        __state++;
        break;
    case VSF_EVT_TIMER:
        vk_file_write(__fcontrol, 0, 1, &__control, NULL);
        __control++;
        break;
    }
}

// TODO: SDL require that main need argc and argv
int main(int argc, char *argv[])
{
#if VSF_USE_TRACE == ENABLED
    vsf_trace_init(NULL);
    vsf_stdio_init();
#endif

#if VSF_USE_USB_HOST == ENABLED
    vk_usbh_init(&__usrapp.usbh.host);
//    vk_usbh_register_class(&usrapp.usbh.host, &usrapp.usbh.msc);
#endif

    vsf_eda_set_evthandler(&__usrapp.fs.task.use_as__vsf_eda_t, __usrapp_fs_evthandler);
    vsf_teda_init(&__usrapp.fs.task, vsf_prio_0, false);
    return 0;
}

/* EOF */
