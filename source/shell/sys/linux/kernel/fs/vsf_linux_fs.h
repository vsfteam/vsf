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

#ifndef __VSF_LINUX_FS_INTERNAL_H__
#define __VSF_LINUX_FS_INTERNAL_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"

#if VSF_USE_LINUX == ENABLED

#if     defined(__VSF_LINUX_FS_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_LINUX_FS_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_dcl_class(vsf_linux_fd_t)
vsf_dcl_class(vsf_linux_trigger_t)
vsf_dcl_class(vsf_linux_process_t)

typedef struct vsf_linux_fd_op_t {
    int priv_size;
    int (*fn_fcntl)(vsf_linux_fd_t *sfd, int cmd, long arg);
    ssize_t (*fn_read)(vsf_linux_fd_t *sfd, void *buf, size_t count);
    ssize_t (*fn_write)(vsf_linux_fd_t *sfd, const void *buf, size_t count);
    int (*fn_close)(vsf_linux_fd_t *sfd);
    int (*fn_eof)(vsf_linux_fd_t *sfd);
} vsf_linux_fd_op_t;

vsf_class(vsf_linux_fd_priv_t) {
    protected_member(
        void *target;
        int flags;
        vsf_linux_trigger_t *trigger;
        short events_pending;
        short events_triggered;

        short status;
        short events;
        // sticky_events will not be cleared
        //  use for eg pipe, pipe_tx is closed, pipe_rx will always POLLIN
        short sticky_events;
    )
    private_member(
        int ref;
    )
};

vsf_class(vsf_linux_fd_t) {
    protected_member(
        int fd;
        int fd_flags;
        const vsf_linux_fd_op_t *op;
    )

    private_member(
        vsf_dlist_node_t fd_node;
    )

    protected_member(
        int unget_buff;
        vsf_linux_fd_priv_t *priv;

        // used while binding fd to something
        //  eg: in popen, fd is binded with the target pid
        union {
            pid_t pid;
        } binding;
    )
};

#if defined(__VSF_LINUX_FS_CLASS_IMPLEMENT) || defined(__VSF_LINUX_FS_CLASS_INHERIT__)
vsf_dcl_class(vsf_linux_stream_priv_t)
typedef void (*vsf_linux_stream_on_evt_t)(vsf_linux_stream_priv_t *priv, vsf_protect_t orig, short event, bool is_ready);

vsf_class(vsf_linux_stream_priv_t) {
    public_member(
        implement(vsf_linux_fd_priv_t)
    )
    protected_member(
        vsf_stream_t *stream_rx;
        vsf_stream_t *stream_tx;
        vsf_linux_stream_on_evt_t on_evt;
    )
};

vsf_dcl_class(vsf_linux_pipe_tx_priv_t)
vsf_class(vsf_linux_pipe_rx_priv_t) {
    protected_member(
        implement(vsf_linux_stream_priv_t)
        bool is_to_free_stream;
        vsf_linux_pipe_tx_priv_t *pipe_tx_priv;
    )
};

vsf_class(vsf_linux_pipe_tx_priv_t) {
    protected_member(
        implement(vsf_linux_stream_priv_t)
        vsf_linux_pipe_rx_priv_t *pipe_rx_priv;
    )
};

typedef struct vsf_linux_fs_priv_t {
    implement(vsf_linux_fd_priv_t)
    vk_file_t *file;

    struct dirent dir;
    vk_file_t *child;
} vsf_linux_fs_priv_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if defined(__VSF_LINUX_FS_CLASS_IMPLEMENT) || defined(__VSF_LINUX_FS_CLASS_INHERIT__)
extern const vsf_linux_fd_op_t __vsf_linux_fs_fdop;
extern const vsf_linux_fd_op_t vsf_linux_pipe_rx_fdop;
extern const vsf_linux_fd_op_t vsf_linux_pipe_tx_fdop;
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#if defined(__VSF_LINUX_FS_CLASS_IMPLEMENT) || defined(__VSF_LINUX_FS_CLASS_INHERIT__)
extern int vsf_linux_fd_bind_target(int fd, void *target,
        vsf_param_eda_evthandler_t peda_read,
        vsf_param_eda_evthandler_t peda_write);
extern int vsf_linux_fd_get_target(int fd, void **target);
extern int vsf_linux_fs_bind_target(const char *pathname, void *target,
        vsf_param_eda_evthandler_t peda_read,
        vsf_param_eda_evthandler_t peda_write);
int vsf_linux_fs_bind_target_ex(const char *pathname, void *target,
        vsf_param_eda_evthandler_t peda_read,
        vsf_param_eda_evthandler_t peda_write,
        uint_fast32_t feature, uint64_t size);
extern int vsf_linux_fs_get_target(const char *pathname, void **target);

extern int vsf_linux_fs_bind_buffer(const char *pathname, void *buffer,
        uint_fast32_t feature, uint64_t size);

extern int vsf_linux_fd_create(vsf_linux_fd_t **sfd, const vsf_linux_fd_op_t *op);
extern vsf_linux_fd_t * vsf_linux_fd_get(int fd);
extern void vsf_linux_fd_delete(int fd);
extern bool vsf_linux_fd_is_block(vsf_linux_fd_t *sfd);

extern int vsf_linux_fd_get_feature(int fd, uint_fast32_t *feature);
extern int vsf_linux_fd_set_feature(int fd, uint_fast32_t feature);
extern int vsf_linux_fd_add_feature(int fd, uint_fast32_t feature);

extern int vsf_linux_fd_set_size(int fd, uint64_t size);

// vsf_linux_fd_xx_trigger/vsf_linux_fd_xx_pend MUST be called scheduler protected
extern short vsf_linux_fd_pend_events(vsf_linux_fd_priv_t *priv, short events, vsf_linux_trigger_t *trig, vsf_protect_t orig);
extern void vsf_linux_fd_set_events(vsf_linux_fd_priv_t *priv, short events, vsf_protect_t orig);
extern void vsf_linux_fd_set_status(vsf_linux_fd_priv_t *priv, short status, vsf_protect_t orig);
extern void vsf_linux_fd_clear_status(vsf_linux_fd_priv_t *priv, short status, vsf_protect_t orig);
extern short vsf_linux_fd_get_status(vsf_linux_fd_priv_t *priv, short status);

// stream
// IMPORTANT: priority of stream MUST be within scheduler priorities
extern vsf_linux_fd_t * vsf_linux_stream(vsf_stream_t *stream_rx, vsf_stream_t *stream_tx);
extern vsf_linux_fd_t * vsf_linux_rx_stream(vsf_stream_t *stream);
extern vsf_linux_fd_t * vsf_linux_tx_stream(vsf_stream_t *stream);
extern vsf_stream_t * vsf_linux_get_rx_stream(vsf_linux_fd_t *sfd);
extern vsf_stream_t * vsf_linux_get_tx_stream(vsf_linux_fd_t *sfd);

// pipe
extern vsf_linux_fd_t * vsf_linux_rx_pipe(vsf_queue_stream_t *queue_stream);
extern vsf_linux_fd_t * vsf_linux_tx_pipe(vsf_linux_pipe_rx_priv_t *priv_rx);
#endif

#ifdef __cplusplus
}
#endif

#undef __VSF_LINUX_FS_CLASS_IMPLEMENT
#undef __VSF_LINUX_FS_CLASS_INHERIT__

/*============================ INCLUDES ======================================*/

#include "./vfs/vsf_linux_vfs.h"

#endif      // VSF_USE_LINUX
#endif      // __VSF_LINUX_FS_INTERNAL_H__
/* EOF */