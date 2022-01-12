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

typedef struct vsf_linux_fd_op_t {
    int priv_size;
    int (*fn_fcntl)(vsf_linux_fd_t *sfd, int cmd, long arg);
    ssize_t (*fn_read)(vsf_linux_fd_t *sfd, void *buf, size_t count);
    ssize_t (*fn_write)(vsf_linux_fd_t *sfd, const void *buf, size_t count);
    int (*fn_close)(vsf_linux_fd_t *sfd);
} vsf_linux_fd_op_t;

vsf_class(vsf_linux_fd_t) {
    protected_member(
        int fd;
        int flags;
        const vsf_linux_fd_op_t *op;

        vsf_trig_t *txpend, *rxpend;
        bool txrdy;
        bool txevt;
        bool rxrdy;
        bool rxevt;
    )

    private_member(
        vsf_dlist_node_t fd_node;
    )

    protected_member(
#if __IS_COMPILER_IAR__
        // make compiler happy by waisting 4 bytes
        int priv[1];
#else
        int priv[0];
#endif
    )
};

#if defined(__VSF_LINUX_FS_CLASS_IMPLEMENT) || defined(__VSF_LINUX_FS_CLASS_INHERIT__)
typedef void (*vsf_linux_stream_on_evt_t)(vsf_linux_fd_t *sfd, vsf_protect_t orig, bool is_ready);

vsf_class(vsf_linux_stream_priv_t) {
    private_member(
        vsf_stream_t *stream;
    )
    protected_member(
        vsf_linux_stream_on_evt_t on_evt;
    )
};

vsf_class(vsf_linux_pipe_rx_priv_t) {
    protected_member(
        implement(vsf_linux_stream_priv_t)
        void *target;
    )
    private_member(
        vsf_queue_stream_t queue_stream;
    )
};

vsf_class(vsf_linux_pipe_tx_priv_t) {
    protected_member(
        implement(vsf_linux_stream_priv_t)
        void *target;
    )
};

typedef struct vsf_linux_fs_priv_t {
    vk_file_t *file;
    uint64_t pos;

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

// sfd is allocated and initialized by user(clear memory, and set op),
//  vsf_linux_fd_add will add sfd to current process
extern int vsf_linux_fd_add(vsf_linux_fd_t *sfd);
extern int vsf_linux_fd_create(vsf_linux_fd_t **sfd, const vsf_linux_fd_op_t *op);
extern vsf_linux_fd_t * vsf_linux_fd_get(int fd);
extern void vsf_linux_fd_delete(int fd);
extern bool vsf_linux_fd_is_block(vsf_linux_fd_t *sfd);

extern int vsf_linux_fd_get_feature(int fd, uint_fast32_t *feature);
extern int vsf_linux_fd_set_feature(int fd, uint_fast32_t feature);
extern int vsf_linux_fd_add_feature(int fd, uint_fast32_t feature);

extern int vsf_linux_fd_set_size(int fd, uint64_t size);

extern void vsf_linux_fd_trigger_init(vsf_trig_t *trig);
// vsf_linux_fd_xx_trigger/vsf_linux_fd_xx_pend MUST be called scheduler protected
extern int vsf_linux_fd_tx_pend(vsf_linux_fd_t *sfd, vsf_trig_t *trig, vsf_protect_t orig);
extern int vsf_linux_fd_rx_pend(vsf_linux_fd_t *sfd, vsf_trig_t *trig, vsf_protect_t orig);
extern int vsf_linux_fd_tx_trigger(vsf_linux_fd_t *sfd, vsf_protect_t orig);
extern int vsf_linux_fd_rx_trigger(vsf_linux_fd_t *sfd, vsf_protect_t orig);
extern int vsf_linux_fd_tx_ready(vsf_linux_fd_t *sfd, vsf_protect_t orig);
extern int vsf_linux_fd_rx_ready(vsf_linux_fd_t *sfd, vsf_protect_t orig);
extern void vsf_linux_fd_tx_busy(vsf_linux_fd_t *sfd, vsf_protect_t orig);
extern void vsf_linux_fd_rx_busy(vsf_linux_fd_t *sfd, vsf_protect_t orig);

// stream
// IMPORTANT: priority of stream MUST be within scheduler priorities
extern vsf_linux_fd_t * vsf_linux_rx_stream(vsf_stream_t *stream);
extern vsf_linux_fd_t * vsf_linux_tx_stream(vsf_stream_t *stream);
extern vsf_stream_t * vsf_linux_get_stream(vsf_linux_fd_t *sfd);

// pipe
extern vsf_linux_fd_t * vsf_linux_rx_pipe(void);
extern vsf_linux_fd_t * vsf_linux_tx_pipe(vsf_linux_fd_t *sfd_rx);
#endif

#ifdef __cplusplus
}
#endif

/*============================ INCLUDES ======================================*/

#include "./vfs/vsf_linux_vfs.h"

#endif      // VSF_USE_LINUX
#endif      // __VSF_LINUX_FS_INTERNAL_H__
/* EOF */