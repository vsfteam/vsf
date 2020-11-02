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

#define VSF_EDA_CLASS_INHERIT
#include "vsf.h"

// define USRAPP_CF_XXXX and include usrapp_common.h
#if VSF_FS_USE_MEMFS == ENABLED
#   include "fakefat32.h"
#   define USRAPP_CFG_MEMFS_ROOT    __fakefat32_root
#endif
#if VSF_FS_USE_WINFS == ENABLED
#   define USRAPP_CFG_WINFS_ROOT    "winfs_root"
#endif
#include "../usrapp_common.h"

/*============================ MACROS ========================================*/

#ifndef USRAPP_CFG_FAT32_SIZE
#   define USRAPP_CFG_FAT32_SIZE                (512 * 0x10000)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_t {
    struct {
        vk_fakefat32_mal_t fakefat32;
    } mal;

    struct {
        uint8_t buffer[1024];
        vsf_mem_stream_t mem_stream;
    } stream;

    struct {
        vsf_teda_t task;
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

static usrapp_t __usrapp = {
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
    int32_t result;

    static int __cur_indent = 0;
    static vk_file_t *__cur_file = NULL;
    static uint8_t __buff[256];
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
                if (VSF_ERR_NONE != vk_file_read(file, 0, sizeof(__buff), __buff)) {
                    goto do_return;
                }
            } else {
                goto do_return;
            }
        }
        break;
    case VSF_EVT_RETURN:
        result = (int32_t)vsf_eda_get_return_value();
        if (file->attr & VSF_FILE_ATTR_DIRECTORY) {
            switch (state->state) {
            case LISTALL_STATE_LOOKUP:
                if ((result < 0) || (NULL == __cur_file)) {
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
            if (result < 0) {
                __usrapp_print_indent(__cur_indent);
                vsf_trace(VSF_TRACE_INFO, "fail to read %s, err is %d" VSF_TRACE_CFG_LINEEND,
                        file->name, result);
            } else {
                vsf_trace_buffer(VSF_TRACE_INFO, __buff, result, VSF_TRACE_DF_DEFAULT);
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
#if VSF_FS_USE_WINFS == ENABLED
        USRAPP_STATE_CREATED_WINFS,
        USRAPP_STATE_OPENED_WINFS,
        USRAPP_STATE_MOUNTED_WINFS,
#endif
        USRAPP_STATE_LISTALL,
        USRAPP_STATE_OPENED_CONTROL,
        USRAPP_STATE_WRITTEN_CONTROL,
    } static __state;
    static vk_file_t *__froot, *__fmemfs, *__fcontrol;
#if VSF_FS_USE_WINFS == ENABLED
    static vk_file_t *__fwinfs;
#endif
    static uint8_t __control = 0;
    int32_t result;

    switch (evt) {
    case VSF_EVT_INIT:
        vk_fs_init();
        __state = USRAPP_STATE_OPENED_ROOT;
        __froot = NULL;
        vk_file_open(NULL, "\\", 0, &__froot);
        break;
    case VSF_EVT_RETURN:
        result = (int32_t)vsf_eda_get_return_value();
        switch (__state) {
        case USRAPP_STATE_OPENED_ROOT:
            ASSERT(__froot != NULL);
            vk_file_create(__froot, "memfs", VSF_FILE_ATTR_DIRECTORY, 0);
            break;
        case USRAPP_STATE_CREATED_MEMFS:
            ASSERT(result == VSF_ERR_NONE);
            __fmemfs = NULL;
            vk_file_open(__froot, "memfs", 0, &__fmemfs);
            break;
        case USRAPP_STATE_OPENED_MEMFS:
            ASSERT(result == VSF_ERR_NONE);
            ASSERT(__fmemfs != NULL);
            vk_fs_mount(__fmemfs, &vk_memfs_op, &__usrapp_common.fs.memfs_info);
            break;
        case USRAPP_STATE_MOUNTED_MEMFS:
            ASSERT(result == VSF_ERR_NONE);
#if VSF_FS_USE_WINFS == ENABLED
            vk_file_create(__froot, "winfs", VSF_FILE_ATTR_DIRECTORY, 0);
            break;
        case USRAPP_STATE_CREATED_WINFS:
            ASSERT(result == VSF_ERR_NONE);
            __fwinfs = NULL;
            vk_file_open(__froot, "winfs", 0, &__fwinfs);
            break;
        case USRAPP_STATE_OPENED_WINFS:
            ASSERT(result == VSF_ERR_NONE);
            ASSERT(__fwinfs != NULL);
            vk_fs_mount(__fwinfs, &vk_winfs_op, &__usrapp_common.fs.winfs_info);
            break;
        case USRAPP_STATE_MOUNTED_WINFS:
            ASSERT(result == VSF_ERR_NONE);
#endif
            if (VSF_ERR_NONE == vsf_eda_call_param_eda(__usrapp_fs_listall_evthandler, __froot)) {
                break;
            }
        case USRAPP_STATE_LISTALL:
            __fcontrol = NULL;
            vk_file_open(__fmemfs, "control.bin", 0, &__fcontrol);
            break;
        case USRAPP_STATE_OPENED_CONTROL:
            ASSERT(result == VSF_ERR_NONE);
            ASSERT(__fcontrol != NULL);
        case USRAPP_STATE_WRITTEN_CONTROL:
            ASSERT(result == 1);
            vsf_teda_set_timer_ms(1000);
            return;
        }
        __state++;
        break;
    case VSF_EVT_TIMER:
        vk_file_write(__fcontrol, 0, 1, &__control);
        __control++;
        break;
    }
}

// TODO: SDL require that main need argc and argv
int main(int argc, char *argv[])
{
    __usrapp_common_init();

    vsf_eda_set_evthandler(&__usrapp.fs.task.use_as__vsf_eda_t, __usrapp_fs_evthandler);
    vsf_teda_init(&__usrapp.fs.task, vsf_prio_0, false);
    return 0;
}

/* EOF */
