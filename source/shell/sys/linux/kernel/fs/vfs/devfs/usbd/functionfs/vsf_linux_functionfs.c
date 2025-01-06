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

#include "shell/sys/linux/vsf_linux_cfg.h"

#if     VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_DEVFS == ENABLED              \
    &&  VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_CFG_RAW_MODE != ENABLED

#define __VSF_USBD_CLASS_INHERIT__
#define __VSF_FS_CLASS_INHERIT__
#define __VSF_LINUX_FS_CLASS_INHERIT__
#define __VSF_LINUX_CLASS_INHERIT__

#include <vsf.h>

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "shell/sys/linux/include/unistd.h"
#   include "shell/sys/linux/include/fcntl.h"
#   include "shell/sys/linux/include/poll.h"
#   include "shell/sys/linux/include/errno.h"
#else
#   include <unistd.h>
#   include <fcntl.h>
#   include <poll.h>
#   include <errno.h>
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_linux_usbd_ep_priv_t {
    implement(vsf_linux_fs_priv_t)
    vk_usbd_trans_t trans;
} vsf_linux_usbd_ep_priv_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static vk_usbd_desc_t * __vsf_linux_usbdop_get_desc(vk_usbd_dev_t *dev, uint_fast8_t type,
                uint_fast8_t index, uint_fast16_t langid);
static vsf_err_t __vsf_linux_usbdop_request_prepare(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);
static vsf_err_t __vsf_linux_usbdop_request_process(vk_usbd_dev_t *dev, vk_usbd_ifs_t *ifs);

static void __vsf_linux_usbdep_init(vsf_linux_fd_t *sfd);
static ssize_t __vsf_linux_usbdep_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static ssize_t __vsf_linux_usbdep_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);

/*============================ LOCAL VARIABLES ===============================*/

static const vk_usbd_class_op_t __vsf_linux_functionfs = {
    .get_desc           = __vsf_linux_usbdop_get_desc,
    .request_prepare    = __vsf_linux_usbdop_request_prepare,
    .request_process    = __vsf_linux_usbdop_request_process,
};

static const vsf_linux_fd_op_t __vsf_linux_usbd_fdop = {
    .priv_size = sizeof(vsf_linux_usbd_ep_priv_t),
    .feature = VSF_LINUX_FDOP_FEATURE_FS,
    .fn_init = __vsf_linux_usbdep_init,
    .fn_read = __vsf_linux_usbdep_read,
    .fn_write = __vsf_linux_usbdep_write,
};

/*============================ IMPLEMENTATION ================================*/

static vk_usbd_desc_t * __vsf_linux_usbdop_get_desc(vk_usbd_dev_t *dev, uint_fast8_t type,
                uint_fast8_t index, uint_fast16_t langid)
{
    struct usb_ctrlrequest_t *request = &dev->ctrl_handler.request;
    vk_usbd_ifs_t *usbd_ifs = vk_usbd_get_ifs(dev, request->wIndex & 0xFF);
    vsf_linux_usbd_ifs_t *ifs = (vsf_linux_usbd_ifs_t *)usbd_ifs->class_param;

    if (ifs->get_desc != NULL) {
        return ifs->get_desc(dev, ifs, type, index, langid);
    }
    return NULL;
}

static vsf_err_t __vsf_linux_usbdop_request_prepare(vk_usbd_dev_t *dev, vk_usbd_ifs_t *usbd_ifs)
{
    vsf_linux_usbd_ifs_t *ifs = (vsf_linux_usbd_ifs_t *)usbd_ifs->class_param;

    if (ifs->request_prepare != NULL) {
        return ifs->request_prepare(dev, ifs, &dev->ctrl_handler.request);
    }
    return VSF_ERR_NOT_SUPPORT;
}

static vsf_err_t __vsf_linux_usbdop_request_process(vk_usbd_dev_t *dev, vk_usbd_ifs_t *usbd_ifs)
{
    vsf_linux_usbd_ifs_t *ifs = (vsf_linux_usbd_ifs_t *)usbd_ifs->class_param;

    if (ifs->request_process != NULL) {
        return ifs->request_process(dev, ifs, &dev->ctrl_handler.request);
    }
    return VSF_ERR_NOT_SUPPORT;
}

static void __vsf_linux_usbd_on_trans_finish(void *p)
{
    vsf_linux_usbd_ep_priv_t *ep_priv = (vsf_linux_usbd_ep_priv_t *)p;
    vk_vfs_file_t *file = (vk_vfs_file_t *)ep_priv->file;
    vsf_linux_usbd_ep_t *ep = (vsf_linux_usbd_ep_t *)file->f.param;
    vk_usbd_trans_t *trans = &ep_priv->trans;

    if ((ep->ep_no & USB_DIR_MASK) == USB_DIR_IN) {
        vsf_linux_fd_set_status(&ep_priv->use_as__vsf_linux_fd_priv_t, POLLOUT, vsf_protect_sched());
    } else {
        trans->size = ep->mts - trans->size;
        vsf_linux_fd_set_status(&ep_priv->use_as__vsf_linux_fd_priv_t, POLLIN, vsf_protect_sched());
    }
}

static void __vsf_linux_usbdep_init(vsf_linux_fd_t *sfd)
{
    vsf_linux_usbd_ep_priv_t *ep_priv = (vsf_linux_usbd_ep_priv_t *)sfd->priv;
    vk_vfs_file_t *file = (vk_vfs_file_t *)ep_priv->file;
    vsf_linux_usbd_ep_t *ep = (vsf_linux_usbd_ep_t *)file->f.param;
    vsf_linux_usbd_ifs_t *ifs = ep->ifs;
    vk_usbd_trans_t *trans = &ep_priv->trans;

    file->attr |= VSF_FILE_ATTR_EXCL;
    trans->ep = ep->ep_no;
    trans->buffer = (void *)ep->buffer;
    trans->size = 0;
    trans->on_finish = __vsf_linux_usbd_on_trans_finish;
    trans->param = ep_priv;
    trans->zlp = ep->zlp;

    if ((ep->ep_no & USB_DIR_MASK) == USB_DIR_IN) {
        vsf_linux_fd_set_status(&ep_priv->use_as__vsf_linux_fd_priv_t, POLLOUT, vsf_protect_sched());
    } else {
        ep->cur_pos = 0;
        trans->size = ep->mts;
        vk_usbd_ep_recv(ifs->dev, trans);
    }
}

static ssize_t __vsf_linux_usbdep_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_usbd_ep_priv_t *ep_priv = (vsf_linux_usbd_ep_priv_t *)sfd->priv;
    vk_vfs_file_t *file = (vk_vfs_file_t *)ep_priv->file;
    vsf_linux_usbd_ep_t *ep = (vsf_linux_usbd_ep_t *)file->f.param;
    vsf_linux_usbd_ifs_t *ifs = ep->ifs;
    vk_usbd_trans_t *trans = &ep_priv->trans;

    vsf_linux_trigger_t trig;
    vsf_linux_trigger_init(&trig);

    vsf_protect_t orig = vsf_protect_sched();
    if (!vsf_linux_fd_get_status(&ep_priv->use_as__vsf_linux_fd_priv_t, POLLIN)) {
        if (vsf_linux_fd_is_block(sfd)) {
            if (!vsf_linux_fd_pend_events(&ep_priv->use_as__vsf_linux_fd_priv_t, POLLIN, &trig, orig)) {
                // triggered by signal
                return -1;
            }
        } else {
            vsf_unprotect_sched(orig);
            errno = EAGAIN;
            return -1;
        }
    } else {
        vsf_unprotect_sched(orig);
    }

    uint8_t *cur_buffer = ep->buffer + ep->cur_pos;
    ssize_t remain_size = vsf_min(trans->size, count);
    if (buf != NULL) {
        memcpy(buf, cur_buffer, remain_size);
    }

    trans->size -= remain_size;
    if (0 == trans->size) {
        vsf_linux_fd_clear_status(&ep_priv->use_as__vsf_linux_fd_priv_t, POLLIN, vsf_protect_sched());

        ep->cur_pos = 0;
        trans->size = ep->mts;
        vk_usbd_ep_recv(ifs->dev, trans);
    } else {
        ep->cur_pos += remain_size;
    }

    return remain_size;
}

static ssize_t __vsf_linux_usbdep_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    vsf_linux_usbd_ep_priv_t *ep_priv = (vsf_linux_usbd_ep_priv_t *)sfd->priv;
    vk_vfs_file_t *file = (vk_vfs_file_t *)ep_priv->file;
    vsf_linux_usbd_ep_t *ep = (vsf_linux_usbd_ep_t *)file->f.param;
    vsf_linux_usbd_ifs_t *ifs = ep->ifs;
    vk_usbd_trans_t *trans = &ep_priv->trans;
    vsf_protect_t orig;
    vsf_linux_trigger_t trig;
    vsf_linux_trigger_init(&trig);

again:
    orig = vsf_protect_sched();
    if (!vsf_linux_fd_get_status(&ep_priv->use_as__vsf_linux_fd_priv_t, POLLOUT)) {
        if (vsf_linux_fd_is_block(sfd)) {
            if (!vsf_linux_fd_pend_events(&ep_priv->use_as__vsf_linux_fd_priv_t, POLLOUT, &trig, orig)) {
                // triggered by signal
                return -1;
            }
            goto again;
        } else {
            vsf_unprotect_sched(orig);
            errno = EAGAIN;
            return -1;
        }
    } else {
        vsf_linux_fd_clear_status(&ep_priv->use_as__vsf_linux_fd_priv_t, POLLOUT, orig);
    }

    VSF_LINUX_ASSERT(ep->mts >= count);
    if (buf != NULL) {
        memcpy(ep->buffer, buf, count);
    }

    trans->size = count;
    vk_usbd_ep_send(ifs->dev, trans);

    return count;
}

int vsf_linux_fs_bind_usbd_ep(char *ifs_path, vsf_linux_usbd_ep_t *ep)
{
    int result = -1;
    char epname[8] = "ep";
    itoa(ep->ep_no & 0x0F, &epname[2], 10);
    if (0 == ep->ep_no) {
        // no need to bind ep0
        return -1;
    } else if ((ep->ep_no & USB_DIR_MASK) == USB_DIR_IN) {
        strcat(epname, "in");
    } else {
        strcat(epname, "out");
    }

    int epfd, ifsfd = open(ifs_path, 0);
    VSF_LINUX_ASSERT(ifsfd >= 0);

    vsf_linux_usbd_ifs_t *ifs;
    result = vsf_linux_fd_get_target(ifsfd, (void **)&ifs);
    if (result) {
        goto close_ifs_and_exit;
    }
    VSF_LINUX_ASSERT(ifs != NULL);

    epfd = openat(ifsfd, epname, O_CREAT);
    if (epfd < 0) {
        goto close_ifs_and_exit;
    }

    ep->ifs = ifs;
    result = vsf_linux_fd_bind_target_ex(epfd, ep, &__vsf_linux_usbd_fdop, NULL, NULL, 0, 0);

    close(epfd);
close_ifs_and_exit:
    close(ifsfd);
    return result;
}

int vsf_linux_fs_bind_usbd_ifs(char *ifs_path, vsf_linux_usbd_ifs_t *ifs)
{
    VSF_LINUX_ASSERT((ifs != NULL) && (ifs->dev != NULL) && (ifs->ifs != NULL) && (NULL == ifs->ifs->class_op));

    int result = vsf_linux_fs_bind_dir_target(ifs_path, ifs);
    if (result) {
        return result;
    }

    ifs->ifs->class_param = ifs;
    ifs->ifs->class_op = &__vsf_linux_functionfs;
    return result;
}

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_DEVFS && VSF_USE_USB_DEVICE && !VSF_USBD_CFG_RAW_MODE
