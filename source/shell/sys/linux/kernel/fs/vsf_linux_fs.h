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

// avoid to include vsf.h, include necessary headers only
// #include "vsf.h"

#include "utilities/vsf_utilities.h"
#include "hal/arch/vsf_arch.h"
#include "kernel/vsf_kernel.h"

#if VSF_USE_LINUX == ENABLED

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/termios.h"
#   include "shell/sys/linux/include/sys/stat.h"
#else
#   include <termios.h>
#   include <sys/stat.h>
#endif

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

#if VSF_USE_FS != ENABLED
#   error VSF_USE_FS MUST be enabled to use fs in linux
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_dcl_class(vsf_linux_fd_t)
vsf_dcl_class(vsf_linux_trigger_t)
vsf_dcl_class(vsf_linux_process_t)
vsf_dcl_class(vsf_linux_fd_priv_t)

enum {
    VSF_LINUX_FDOP_FEATURE_FS       = 1 << 0,
};

typedef struct vsf_linux_fd_op_t {
    int priv_size;
    int feature;
    void (*fn_init)(vsf_linux_fd_t *sfd);
    void (*fn_fini)(vsf_linux_fd_t *sfd);
    int (*fn_fcntl)(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg);
    ssize_t (*fn_read)(vsf_linux_fd_t *sfd, void *buf, size_t count);
    ssize_t (*fn_write)(vsf_linux_fd_t *sfd, const void *buf, size_t count);
    int (*fn_close)(vsf_linux_fd_t *sfd);
    int (*fn_eof)(vsf_linux_fd_t *sfd);
    int (*fn_setsize)(vsf_linux_fd_t *sfd, off64_t size);
    int (*fn_stat)(vsf_linux_fd_t *sfd, struct stat *buf);

    void * (*fn_mmap)(vsf_linux_fd_t *sfd, off64_t offset, size_t len, uint_fast32_t feature);
    int (*fn_munmap)(vsf_linux_fd_t *sfd, void *buffer);
    int (*fn_msync)(vsf_linux_fd_t *sfd, void *buffer);
} vsf_linux_fd_op_t;

typedef struct vsf_linux_fd_priv_callback_t {
    short pendind_events;
    void *param;
    void (*cb)(vsf_linux_fd_priv_t *priv, void *param, short events, vsf_protect_t orig);
} vsf_linux_fd_priv_callback_t;

vsf_class(vsf_linux_fd_priv_t) {
    protected_member(
        // target is used in socket_unix to indicate message for sendmsg/recvmsg,
        //  ofc it can be used in other fd types
        void *target;
        int flags;

        short status;
        short events;
        // sticky_events will not be cleared
        //  use for eg pipe, pipe_tx is closed, pipe_rx will always POLLIN
        short sticky_events;
        short user_data;

        vsf_linux_fd_priv_callback_t events_callback[2];
    )
    private_member(
        int ref;
    )
};

vsf_class(vsf_linux_fd_t) {
    protected_member(
        int fd;
        int fd_flags;
        int cur_rdflags;
        int cur_wrflags;
        const vsf_linux_fd_op_t *op;
    )

    private_member(
        vsf_dlist_node_t fd_node;
        void *mmapped_buffer;
        bool is_close_pending_on_munmap;
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
#   if VSF_LINUX_CFG_FS_CACHE_SIZE > 0
VSF_STATIC_ASSERT(VSF_LINUX_CFG_FS_CACHE_SIZE <= ((1 << (sizeof(uint16_t) << 3)) - 1));
VSF_STATIC_ASSERT(VSF_LINUX_CFG_FS_CACHE_SIZE > VSF_LINUX_CFG_FS_CACHE_THRESHOLD);
#   endif
typedef struct vk_file_t vk_file_t;
typedef struct vsf_linux_fs_priv_t {
    implement(vsf_linux_fd_priv_t)
    vk_file_t *file;

    union {
        struct dirent dir;
        struct dirent64 dir64;
    };
#   if VSF_LINUX_CFG_FS_CACHE_SIZE > 0
    uint8_t *cache_buffer;
    uint16_t cache_size;
    uint16_t cache_offset;
    uint64_t cache_pos;
#   endif
} vsf_linux_fs_priv_t;

vsf_dcl_class(vsf_linux_stream_priv_t)
typedef void (*vsf_linux_stream_on_evt_t)(vsf_linux_stream_priv_t *priv, vsf_protect_t orig, short event, bool is_ready);

vsf_class(vsf_linux_stream_priv_t) {
    public_member(
        implement(vsf_linux_fs_priv_t)
    )
    protected_member(
        vsf_stream_t *stream_rx;
        vsf_stream_t *stream_tx;
        vsf_linux_stream_on_evt_t on_evt;
        bool stream_rx_allocated;
        bool stream_tx_allocated;
    )
};

vsf_class(vsf_linux_pipe_priv_t) {
    protected_member(
        implement(vsf_linux_stream_priv_t)
        vsf_linux_pipe_priv_t *pipe_remote;
    )
};

typedef struct vsf_linux_term_priv_t {
    implement(vsf_linux_pipe_priv_t)
    const vsf_linux_fd_op_t *subop;
    struct termios termios;
    char esc_type;
    bool line_start;
    bool subop_inited;
} vsf_linux_term_priv_t;

typedef struct vsf_linux_key_t vsf_linux_key_t;
vsf_class(vsf_linux_key_priv_t) {
    public_member(
        implement(vsf_linux_fd_priv_t)
    )
    protected_member(
        union {
            struct {
                unsigned short *semadj_arr;
            } sem;
        } u;
        vsf_linux_key_t *key;
        void (*fn_fini)(vsf_linux_fd_t *sfd);
        int (*fn_close)(vsf_linux_fd_t *sfd);
    )
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if defined(__VSF_LINUX_FS_CLASS_IMPLEMENT) || defined(__VSF_LINUX_FS_CLASS_INHERIT__)
extern const vsf_linux_fd_op_t __vsf_linux_fs_fdop;
extern const vsf_linux_fd_op_t __vsf_linux_stream_fdop;
extern const vsf_linux_fd_op_t vsf_linux_pipe_fdop;
extern const vsf_linux_fd_op_t vsf_linux_term_fdop;
extern const vsf_linux_fd_op_t vsf_linux_key_fdop;
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#if defined(__VSF_LINUX_FS_CLASS_IMPLEMENT) || defined(__VSF_LINUX_FS_CLASS_INHERIT__)
extern int vsf_linux_fd_bind_target_ex(int fd,
        void *target, const vsf_linux_fd_op_t *op,
        vsf_param_eda_evthandler_t peda_read,
        vsf_param_eda_evthandler_t peda_write,
        uint_fast32_t feature, uint64_t size);
extern int vsf_linux_fd_bind_target(int fd, void *target,
        vsf_param_eda_evthandler_t peda_read,
        vsf_param_eda_evthandler_t peda_write);
// compatible with file and directory
extern int vsf_linux_fd_get_target(int fd, void **target);
extern int vsf_linux_fs_bind_target(const char *pathname, void *target,
        vsf_param_eda_evthandler_t peda_read,
        vsf_param_eda_evthandler_t peda_write);
extern int vsf_linux_fs_bind_dir_target(const char *pathname, void *target);
int vsf_linux_fs_bind_target_ex(const char *pathname,
        void *target, const vsf_linux_fd_op_t *op,
        vsf_param_eda_evthandler_t peda_read,
        vsf_param_eda_evthandler_t peda_write,
        uint_fast32_t feature, uint64_t size);
extern int vsf_linux_fs_get_target(const char *pathname, void **target);
typedef struct vk_vfs_file_t vk_vfs_file_t;
extern int vsf_linux_fs_bind_target_relative(vk_vfs_file_t *dir, const char *pathname,
        void *target, const vsf_linux_fd_op_t *op,
        uint_fast32_t feature, uint64_t size);
extern int vsf_linux_fs_bind_pipe(const char *pathname1, const char *pathname2, bool exclusive);
extern int vsf_linux_create_pty(int num);

extern int vsf_linux_fd_create(vsf_linux_fd_t **sfd, const vsf_linux_fd_op_t *op);
extern vsf_linux_fd_t * vsf_linux_fd_get(int fd);
extern void vsf_linux_fd_delete(int fd);
extern bool vsf_linux_fd_is_block(vsf_linux_fd_t *sfd);

extern int vsf_linux_fd_get_feature(int fd, uint_fast32_t *feature);
extern int vsf_linux_fd_set_feature(int fd, uint_fast32_t feature);
extern int vsf_linux_fd_add_feature(int fd, uint_fast32_t feature);

extern int vsf_linux_fd_set_size(int fd, uint64_t size);

// vsf_linux_fd_xx_trigger/vsf_linux_fd_xx_pend MUST be called scheduler protected
vsf_linux_fd_priv_callback_t * vsf_linux_fd_claim_calback(vsf_linux_fd_priv_t *priv);
void vsf_linux_fd_release_calback(vsf_linux_fd_priv_t *priv, vsf_linux_fd_priv_callback_t *callback);
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

// pipe
extern vsf_linux_fd_t * vsf_linux_rx_pipe(vsf_queue_stream_t *queue_stream);
extern vsf_linux_fd_t * vsf_linux_tx_pipe(vsf_linux_pipe_priv_t *priv_rx);
#endif

extern int vsf_linux_fs_bind_buffer(const char *pathname, void *buffer,
        uint_fast32_t feature, uint64_t size);
extern int vsf_linux_fs_bind_stream(const char *pathname, vsf_stream_t *stream_rx, vsf_stream_t *stream_tx);

#ifdef __cplusplus
}
#endif

// DO NOT undefine MACROs below, because lower header will check the same MACROs to include necessary codes
//#undef __VSF_LINUX_FS_CLASS_IMPLEMENT
//#undef __VSF_LINUX_FS_CLASS_INHERIT__

/*============================ INCLUDES ======================================*/

#include "./vfs/vsf_linux_vfs.h"

#endif      // VSF_USE_LINUX
#endif      // __VSF_LINUX_FS_INTERNAL_H__
/* EOF */