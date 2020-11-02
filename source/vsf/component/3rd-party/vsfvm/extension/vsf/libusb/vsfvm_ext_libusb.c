/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#include "vsf_vm_cfg.h"

#if     ((VSFVM_CFG_RUNTIME_EN == ENABLED) || (VSFVM_CFG_COMPILER_EN == ENABLED))\
    &&  (VSF_USE_USB_HOST == ENABLED) && (VSF_USBH_USE_LIBUSB == ENABLED)

#define __VSFVM_RUNTIME_CLASS_INHERIT__
#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__

#include "kernel/vsf_kernel.h"
#include "component/usb/vsf_usb.h"

#include "./common/vsfvm_common.h"
#include "./runtime/vsfvm_runtime.h"
#include "./extension/std/vsfvm_ext_std.h"
#include "./vsfvm_ext_libusb.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE != ENABLED
#   error vsfvm_ext_libusb requires VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsfvm_ext_libusb_dev_t {
    vk_usbh_libusb_dev_t *ldev;
    vsfvm_instance_t *inst;
    vsf_slist_node_t dev_node;
    vk_usbh_urb_t urb;
    uint8_t cur_address;
} vsfvm_ext_libusb_dev_t;

typedef struct vsfvm_ext_libusb_t {
    implement(vsfvm_ext_t)
    implement(vsf_eda_t)
    vsf_slist_t dev_list;
    vsfvm_runtime_callback_t callback;
    bool callback_registered;
} vsfvm_ext_libusb_t;

enum {
    VSFVM_LIBUSB_EXTFUNC_START = 0,
    VSFVM_LIBUSB_EXTFUNC_RESET,
    VSFVM_LIBUSB_EXTFUNC_TRANSFER,
    VSFVM_LIBUSB_EXTFUNC_TRANSFER_EX,
    VSFVM_LIBUSB_EXTFUNC_ADDRESS,
    VSFVM_LIBUSB_EXTFUNC_PID,
    VSFVM_LIBUSB_EXTFUNC_VID,
    VSFVM_LIBUSB_EXTFUNC_CLASS,
    VSFVM_LIBUSB_EXTFUNC_SUBCLASS,
    VSFVM_LIBUSB_EXTFUNC_PROTOCOL,
    VSFVM_LIBUSB_EXTFUNC_NUM,
};

enum {
    VSFVM_LIBUSB_EVT_ON_ARRIVED = VSF_EVT_USER,
    VSFVM_LIBUSB_EVT_ON_LEFT,
};

/*============================ PROTOTYPES ====================================*/

#if VSFVM_CFG_RUNTIME_EN == ENABLED
static void __vsfvm_ext_libusb_dev_print(vsfvm_instance_t *inst);
static void __vsfvm_ext_libusb_dev_destroy(vsfvm_instance_t *inst);

static vsfvm_ret_t __vsfvm_ext_libusb_start(vsfvm_thread_t *thread);
static vsfvm_ret_t __vsfvm_ext_libusb_reset(vsfvm_thread_t *thread);
static vsfvm_ret_t __vsfvm_ext_libusb_transfer(vsfvm_thread_t *thread);
static vsfvm_ret_t __vsfvm_ext_libusb_transfer_ex(vsfvm_thread_t *thread);
static vsfvm_ret_t __vsfvm_ext_libusb_address(vsfvm_thread_t *thread);
static vsfvm_ret_t __vsfvm_ext_libusb_pid(vsfvm_thread_t *thread);
static vsfvm_ret_t __vsfvm_ext_libusb_vid(vsfvm_thread_t *thread);
static vsfvm_ret_t __vsfvm_ext_libusb_class(vsfvm_thread_t *thread);
static vsfvm_ret_t __vsfvm_ext_libusb_subclass(vsfvm_thread_t *thread);
static vsfvm_ret_t __vsfvm_ext_libusb_protocol(vsfvm_thread_t *thread);
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT vsfvm_ext_libusb_t __vsfvm_ext_libusb;

const vsfvm_class_t vsfvm_ext_libusb_dev = {
#if VSFVM_CFG_COMPILER_EN == ENABLED
    .name = "libusb_dev",
#endif
#if VSFVM_CFG_RUNTIME_EN == ENABLED
    .type = VSFVM_CLASS_USER,
    .op.print = __vsfvm_ext_libusb_dev_print,
    .op.destroy = __vsfvm_ext_libusb_dev_destroy,
#endif
};

#if VSFVM_CFG_COMPILER_EN == ENABLED
extern const vsfvm_ext_op_t __vsfvm_ext_libusb_op;
static const vsfvm_lexer_sym_t __vsfvm_ext_libusb_sym[] = {
    VSFVM_LEXERSYM_EXTFUNC("libusb_start", &__vsfvm_ext_libusb_op, NULL, NULL, 1, VSFVM_LIBUSB_EXTFUNC_START),
    VSFVM_LEXERSYM_CONST("USB_EVT_ON_ARRIVED", &__vsfvm_ext_libusb_op, NULL, VSF_USBH_LIBUSB_EVT_ON_ARRIVED),
    VSFVM_LEXERSYM_CONST("USB_EVT_ON_LEFT", &__vsfvm_ext_libusb_op, NULL, VSF_USBH_LIBUSB_EVT_ON_LEFT),
    VSFVM_LEXERSYM_CONST("USB_OUT", &__vsfvm_ext_libusb_op, NULL, USB_DIR_OUT),
    VSFVM_LEXERSYM_CONST("USB_IN", &__vsfvm_ext_libusb_op, NULL, USB_DIR_IN),

    // class libusb_dev
    VSFVM_LEXERSYM_CLASS("libusb_dev", &__vsfvm_ext_libusb_op, &vsfvm_ext_libusb_dev),
    VSFVM_LEXERSYM_EXTFUNC("libusb_dev_reset", &__vsfvm_ext_libusb_op, &vsfvm_ext_libusb_dev, &vsfvm_ext_libusb_dev, 1, VSFVM_LIBUSB_EXTFUNC_RESET),
    VSFVM_LEXERSYM_EXTFUNC("libusb_dev_transfer", &__vsfvm_ext_libusb_op, &vsfvm_ext_libusb_dev, &vsfvm_ext_libusb_dev, -1, VSFVM_LIBUSB_EXTFUNC_TRANSFER),
    VSFVM_LEXERSYM_EXTFUNC("libusb_dev_transfer_ex", &__vsfvm_ext_libusb_op, &vsfvm_ext_libusb_dev, &vsfvm_ext_libusb_dev, -1, VSFVM_LIBUSB_EXTFUNC_TRANSFER_EX),
    VSFVM_LEXERSYM_EXTFUNC("libusb_dev_address", &__vsfvm_ext_libusb_op, &vsfvm_ext_libusb_dev, NULL, 1, VSFVM_LIBUSB_EXTFUNC_ADDRESS),
    VSFVM_LEXERSYM_EXTFUNC("libusb_dev_pid", &__vsfvm_ext_libusb_op, &vsfvm_ext_libusb_dev, NULL, 1, VSFVM_LIBUSB_EXTFUNC_PID),
    VSFVM_LEXERSYM_EXTFUNC("libusb_dev_vid", &__vsfvm_ext_libusb_op, &vsfvm_ext_libusb_dev, NULL, 1, VSFVM_LIBUSB_EXTFUNC_VID),
    VSFVM_LEXERSYM_EXTFUNC("libusb_dev_class", &__vsfvm_ext_libusb_op, &vsfvm_ext_libusb_dev, NULL, 1, VSFVM_LIBUSB_EXTFUNC_CLASS),
    VSFVM_LEXERSYM_EXTFUNC("libusb_dev_subclass", &__vsfvm_ext_libusb_op, &vsfvm_ext_libusb_dev, NULL, 1, VSFVM_LIBUSB_EXTFUNC_SUBCLASS),
    VSFVM_LEXERSYM_EXTFUNC("libusb_dev_protocol", &__vsfvm_ext_libusb_op, &vsfvm_ext_libusb_dev, NULL, 1, VSFVM_LIBUSB_EXTFUNC_PROTOCOL),
};
#endif

#if VSFVM_CFG_RUNTIME_EN == ENABLED
static const vsfvm_extfunc_t __vsfvm_ext_libusb_func[VSFVM_LIBUSB_EXTFUNC_NUM] = {
    [VSFVM_LIBUSB_EXTFUNC_START] = VSFVM_EXTFUNC(__vsfvm_ext_libusb_start, 1),
    [VSFVM_LIBUSB_EXTFUNC_RESET] = VSFVM_EXTFUNC(__vsfvm_ext_libusb_reset, 1),
    [VSFVM_LIBUSB_EXTFUNC_TRANSFER] = VSFVM_EXTFUNC(__vsfvm_ext_libusb_transfer, -1),
    [VSFVM_LIBUSB_EXTFUNC_TRANSFER_EX] = VSFVM_EXTFUNC(__vsfvm_ext_libusb_transfer_ex, -1),
    [VSFVM_LIBUSB_EXTFUNC_ADDRESS] = VSFVM_EXTFUNC(__vsfvm_ext_libusb_address, 1),
    [VSFVM_LIBUSB_EXTFUNC_PID] = VSFVM_EXTFUNC(__vsfvm_ext_libusb_pid, 1),
    [VSFVM_LIBUSB_EXTFUNC_VID] = VSFVM_EXTFUNC(__vsfvm_ext_libusb_vid, 1),
    [VSFVM_LIBUSB_EXTFUNC_CLASS] = VSFVM_EXTFUNC(__vsfvm_ext_libusb_class, 1),
    [VSFVM_LIBUSB_EXTFUNC_SUBCLASS] = VSFVM_EXTFUNC(__vsfvm_ext_libusb_subclass, 1),
    [VSFVM_LIBUSB_EXTFUNC_PROTOCOL] = VSFVM_EXTFUNC(__vsfvm_ext_libusb_protocol, 1),
};
#endif

static const vsfvm_ext_op_t __vsfvm_ext_libusb_op = {
#if VSFVM_CFG_COMPILER_EN == ENABLED
    .name = "libusb",
    .sym = __vsfvm_ext_libusb_sym,
    .sym_num = dimof(__vsfvm_ext_libusb_sym),
#endif
#if VSFVM_CFG_RUNTIME_EN == ENABLED
    .init = NULL,
    .fini = NULL,
    .func = (vsfvm_extfunc_t *)__vsfvm_ext_libusb_func,
#endif
    .func_num = dimof(__vsfvm_ext_libusb_func),
};

/*============================ IMPLEMENTATION ================================*/

#if VSFVM_CFG_RUNTIME_EN == ENABLED

static void __vsfvm_ext_libusb_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    if (evt >= VSF_EVT_USER) {
        vk_usbh_libusb_dev_t *ldev = vsf_eda_get_cur_msg();
        vsfvm_var_t argv[2] = {
            {
                .type   = VSFVM_VAR_TYPE_VALUE,
            },
        };

        VSFVM_ASSERT(ldev != NULL);
        switch (evt) {
        case VSFVM_LIBUSB_EVT_ON_ARRIVED:
            if (__vsfvm_ext_libusb.callback_registered) {
                if (!vk_usbh_libusb_open(ldev)) {
                    argv[0].value = VSF_USBH_LIBUSB_EVT_ON_ARRIVED;
                    argv[1].type = VSFVM_VAR_TYPE_INSTANCE;
                    argv[1].inst = vsfvm_instance_alloc(sizeof(vsfvm_ext_libusb_dev_t), &vsfvm_ext_libusb_dev);
                    if (argv[1].inst != NULL) {
                        vsfvm_ext_libusb_dev_t *ext_ldev = argv[1].inst->obj_ptr;
                        ext_ldev->ldev = ldev;
                        ext_ldev->inst = argv[1].inst;
                        ext_ldev->cur_address = ldev->address;
                        vsf_slist_append(vsfvm_ext_libusb_dev_t, dev_node, &__vsfvm_ext_libusb.dev_list, ext_ldev);
                        vsfvm_runtime_call_callback(&__vsfvm_ext_libusb.callback, dimof(argv), (vsfvm_var_t *)&argv);
                    } else {
                        vk_usbh_libusb_close(ldev);
                    }
                }
            }
            break;
        case VSFVM_LIBUSB_EVT_ON_LEFT:
            __vsf_slist_foreach_unsafe(vsfvm_ext_libusb_dev_t, dev_node, &__vsfvm_ext_libusb.dev_list) {
                if (_->ldev == ldev) {
                    argv[0].value = VSF_USBH_LIBUSB_EVT_ON_LEFT;
                    argv[1].inst = _->inst;
                    argv[1].type = VSFVM_VAR_TYPE_INSTANCE;
                    vsfvm_runtime_call_callback(&__vsfvm_ext_libusb.callback, dimof(argv), (vsfvm_var_t *)&argv);
                }
            }
            break;
        }
    } else if (evt == VSF_EVT_MESSAGE) {
        vsfvm_thread_ready((vsfvm_thread_t *)eda);
    }
}

static void __vsfvm_ext_libusb_on_evt(void *param, vk_usbh_libusb_dev_t *ldev, vk_usbh_libusb_evt_t evt)
{
    switch (evt) {
    case VSF_USBH_LIBUSB_EVT_ON_ARRIVED:
        vsf_eda_post_evt_msg(&__vsfvm_ext_libusb.use_as__vsf_eda_t, VSFVM_LIBUSB_EVT_ON_ARRIVED, ldev);
        break;
    case VSF_USBH_LIBUSB_EVT_ON_LEFT:
        vsf_eda_post_evt_msg(&__vsfvm_ext_libusb.use_as__vsf_eda_t, VSFVM_LIBUSB_EVT_ON_LEFT, ldev);
        break;
    }
}

static vsfvm_ret_t __vsfvm_ext_libusb_start(vsfvm_thread_t *thread)
{
    vsfvm_var_t *func = vsfvm_get_func_argu_ref(thread, 0);

    if (!__vsfvm_ext_libusb.fn.evthandler) {
        vsf_slist_init(&__vsfvm_ext_libusb.dev_list);
        __vsfvm_ext_libusb.fn.evthandler = __vsfvm_ext_libusb_evthandler;
        vsf_eda_init(&__vsfvm_ext_libusb.use_as__vsf_eda_t, VSFVM_CFG_PRIORITY, false);
        vk_usbh_libusb_set_evthandler(&__vsfvm_ext_libusb, __vsfvm_ext_libusb_on_evt);
    }

    __vsfvm_ext_libusb.callback_registered = vsfvm_runtime_register_callback(&__vsfvm_ext_libusb.callback, thread, func);
    return __vsfvm_ext_libusb.callback_registered ? VSFVM_RET_FINISHED : VSFVM_RET_INVALID_PARAM;
}

static vsfvm_ret_t __vsfvm_ext_libusb_address(vsfvm_thread_t *thread)
{
    vsfvm_var_t *thiz = vsfvm_get_func_argu_ref(thread, 0);
    vsfvm_var_t *result = vsfvm_get_func_argu(thread, 0);
    vk_usbh_libusb_dev_t *ldev;

    if (!vsfvm_var_instance_of(thiz, &vsfvm_ext_libusb_dev)) {
        return VSFVM_RET_ERROR;
    }

    ldev = ((vsfvm_ext_libusb_dev_t*)thiz->inst->obj_ptr)->ldev;
    vsfvm_var_set(thread, result, VSFVM_VAR_TYPE_VALUE, ldev->address);
    return VSFVM_RET_FINISHED;
}

static vsfvm_ret_t __vsfvm_ext_libusb_pid(vsfvm_thread_t *thread)
{
    vsfvm_var_t *thiz = vsfvm_get_func_argu_ref(thread, 0);
    vsfvm_var_t *result = vsfvm_get_func_argu(thread, 0);
    vk_usbh_libusb_dev_t *ldev;

    if (!vsfvm_var_instance_of(thiz, &vsfvm_ext_libusb_dev)) {
        return VSFVM_RET_ERROR;
    }

    ldev = ((vsfvm_ext_libusb_dev_t*)thiz->inst->obj_ptr)->ldev;
    vsfvm_var_set(thread, result, VSFVM_VAR_TYPE_VALUE, ldev->pid);
    return VSFVM_RET_FINISHED;
}

static vsfvm_ret_t __vsfvm_ext_libusb_vid(vsfvm_thread_t *thread)
{
    vsfvm_var_t *thiz = vsfvm_get_func_argu_ref(thread, 0);
    vsfvm_var_t *result = vsfvm_get_func_argu(thread, 0);
    vk_usbh_libusb_dev_t *ldev;

    if (!vsfvm_var_instance_of(thiz, &vsfvm_ext_libusb_dev)) {
        return VSFVM_RET_ERROR;
    }

    ldev = ((vsfvm_ext_libusb_dev_t*)thiz->inst->obj_ptr)->ldev;
    vsfvm_var_set(thread, result, VSFVM_VAR_TYPE_VALUE, ldev->vid);
    return VSFVM_RET_FINISHED;
}

static vsfvm_ret_t __vsfvm_ext_libusb_class(vsfvm_thread_t *thread)
{
    vsfvm_var_t *thiz = vsfvm_get_func_argu_ref(thread, 0);
    vsfvm_var_t *result = vsfvm_get_func_argu(thread, 0);
    vk_usbh_libusb_dev_t *ldev;

    if (!vsfvm_var_instance_of(thiz, &vsfvm_ext_libusb_dev)) {
        return VSFVM_RET_ERROR;
    }

    ldev = ((vsfvm_ext_libusb_dev_t*)thiz->inst->obj_ptr)->ldev;
    vsfvm_var_set(thread, result, VSFVM_VAR_TYPE_VALUE, ldev->c);
    return VSFVM_RET_FINISHED;
}

static vsfvm_ret_t __vsfvm_ext_libusb_subclass(vsfvm_thread_t *thread)
{
    vsfvm_var_t *thiz = vsfvm_get_func_argu_ref(thread, 0);
    vsfvm_var_t *result = vsfvm_get_func_argu(thread, 0);
    vk_usbh_libusb_dev_t *ldev;

    if (!vsfvm_var_instance_of(thiz, &vsfvm_ext_libusb_dev)) {
        return VSFVM_RET_ERROR;
    }

    ldev = ((vsfvm_ext_libusb_dev_t*)thiz->inst->obj_ptr)->ldev;
    vsfvm_var_set(thread, result, VSFVM_VAR_TYPE_VALUE, ldev->subc);
    return VSFVM_RET_FINISHED;
}

static vsfvm_ret_t __vsfvm_ext_libusb_protocol(vsfvm_thread_t *thread)
{
    vsfvm_var_t *thiz = vsfvm_get_func_argu_ref(thread, 0);
    vsfvm_var_t *result = vsfvm_get_func_argu(thread, 0);
    vk_usbh_libusb_dev_t *ldev;

    if (!vsfvm_var_instance_of(thiz, &vsfvm_ext_libusb_dev)) {
        return VSFVM_RET_ERROR;
    }

    ldev = ((vsfvm_ext_libusb_dev_t*)thiz->inst->obj_ptr)->ldev;
    vsfvm_var_set(thread, result, VSFVM_VAR_TYPE_VALUE, ldev->protocol);
    return VSFVM_RET_FINISHED;
}

static vsfvm_ret_t __vsfvm_ext_libusb_reset(vsfvm_thread_t *thread)
{
    vsfvm_var_t *thiz = vsfvm_get_func_argu_ref(thread, 0);
    vsfvm_ext_libusb_dev_t *ext_ldev;
    vk_usbh_libusb_dev_t *ldev;

    if (!vsfvm_var_instance_of(thiz, &vsfvm_ext_libusb_dev)) {
        return VSFVM_RET_ERROR;
    }

    ext_ldev = thiz->inst->obj_ptr;
    ext_ldev->cur_address = 0;
    ldev = ext_ldev->ldev;
    vk_usbh_reset_dev(ldev->usbh, ldev->dev);
    return VSFVM_RET_FINISHED;
}

static vsfvm_ret_t __vsfvm_ext_libusb_transfer_ex(vsfvm_thread_t *thread)
{
    uint_fast8_t argc = thread->func.argc;
    vsfvm_var_t *thiz = vsfvm_get_func_argu_ref(thread, 0);
    vsfvm_var_t *ep = vsfvm_get_func_argu_ref(thread, 1);
    vsfvm_var_t *result = vsfvm_get_func_argu_ref(thread, 2);
    uint_fast8_t epnum;
    vsfvm_ext_libusb_dev_t *ext_ldev;
    vk_usbh_urb_t *urb;

    if (    (NULL == ep) || (argc != (ep->uval8 ? 6 : 10))
        ||  !vsfvm_var_instance_of(thiz, &vsfvm_ext_libusb_dev)) {
        return VSFVM_RET_ERROR;
    }

    ext_ldev = thiz->inst->obj_ptr;
    epnum = ep->uval8 & ~USB_DIR_MASK;
    urb = epnum > 0 ? &ext_ldev->urb : &ext_ldev->ldev->dev->ep0.urb;

    if (!thread->fn.evthandler) {
        vk_usbh_libusb_dev_t *ldev = ext_ldev->ldev;
        bool is_in = (ep->uval8 & USB_DIR_MASK) == USB_DIR_IN;

        vsfvm_var_t *length;
        vsfvm_var_t *buffer;

        if (!epnum) {
            length = vsfvm_get_func_argu_ref(thread, 7);
            buffer = vsfvm_get_func_argu_ref(thread, 8);
        } else {
            length = vsfvm_get_func_argu_ref(thread, 3);
            buffer = vsfvm_get_func_argu_ref(thread, 4);
        }

        if (epnum != 0) {
            vk_usbh_pipe_t pipe = vk_usbh_urb_get_pipe(urb);
            if ((pipe.endpoint == epnum) && ((bool)pipe.dir_in1out0 == is_in)) {
                vk_usbh_free_urb(ldev->usbh, urb);
                // TODO: implement pipe array in ldev
                //      vk_usbh_urb_prepare_by_pipe(urb, ldev->dev, ldev->pipe[(is_in ? 16 : 0) + epnum]);
                if (vk_usbh_alloc_urb(ldev->usbh, ldev->dev, urb)) {
                    goto ret_fail;
                }
            }
        }

        if (length->uval32 > 0) {
            if (!buffer->inst) { return VSFVM_RET_ERROR; }
            vk_usbh_urb_set_buffer(urb, buffer->inst->buffer, length->uval32);
        } else {
            vk_usbh_urb_set_buffer(urb, NULL, 0);
        }

        // update address
        if (ext_ldev->cur_address != ldev->dev->devnum) {
            vk_usbh_pipe_t pipe = vk_usbh_urb_get_pipe(urb);
            ldev->dev->devnum = pipe.address = ext_ldev->cur_address;
            vk_usbh_urb_set_pipe(urb, pipe);
        }

        thread->fn.evthandler = __vsfvm_ext_libusb_evthandler;
        vsf_teda_init(&thread->use_as__vsf_teda_t, VSFVM_CFG_PRIORITY, false);

        if (!epnum) {
            // control transfer
            vsfvm_var_t *type = vsfvm_get_func_argu_ref(thread, 3);
            vsfvm_var_t *request = vsfvm_get_func_argu_ref(thread, 4);
            vsfvm_var_t *value = vsfvm_get_func_argu_ref(thread, 5);
            vsfvm_var_t *index = vsfvm_get_func_argu_ref(thread, 6);
            struct usb_ctrlrequest_t req = {
                .bRequestType   = type->uval8,
                .bRequest       = request->uval8,
                .wValue         = value->uval16,
                .wIndex         = index->uval16,
                .wLength        = length->uval16,
            };

            vsfvm_var_t *flags = vsfvm_get_func_argu_ref(thread, 9);
            if (vk_usbh_control_msg_ex(ldev->usbh, ldev->dev, &req, flags->uval16, &thread->use_as__vsf_eda_t)) {
                goto ret_fail;
            } else if ( ((USB_RECIP_DEVICE | USB_DIR_OUT) == req.bRequestType)
                    &&  (USB_REQ_SET_ADDRESS == req.bRequest)) {
                // set address
                ext_ldev->cur_address = req.wValue;
            }
        } else {
            vsfvm_var_t *flags = vsfvm_get_func_argu_ref(thread, 5);
            if (vk_usbh_submit_urb_ex(ldev->usbh, urb, flags->uval16, &thread->use_as__vsf_eda_t)) {
                goto ret_fail;
            }
        }
        return VSFVM_RET_PEND;
    } else {
        vsf_eda_fini(&thread->use_as__vsf_eda_t);
        thread->fn.evthandler = NULL;

        vsfvm_var_set(thread, result, VSFVM_VAR_TYPE_VALUE, vk_usbh_urb_get_status(urb));
        if (URB_OK == result->value) {
            vsfvm_var_set(thread, result, VSFVM_VAR_TYPE_VALUE, vk_usbh_urb_get_actual_length(urb));
        }

        return VSFVM_RET_FINISHED;
    }

ret_fail:
    vsfvm_var_set(thread, result, VSFVM_VAR_TYPE_VALUE, VSF_ERR_FAIL);
    return VSFVM_RET_FINISHED;
}

static vsfvm_ret_t __vsfvm_ext_libusb_transfer(vsfvm_thread_t *thread)
{
    vsfvm_ret_t ret;

    if (!thread->fn.evthandler) {
        // push default flags 0
        if (vsfvm_thread_stack_push(thread, 0, VSFVM_VAR_TYPE_VALUE, 1)) {
            return VSFVM_RET_ERROR;
        }
        thread->func.argc++;
    }

    ret = __vsfvm_ext_libusb_transfer_ex(thread);
    if (VSFVM_RET_FINISHED == ret) {
        VSFVM_ASSERT(!thread->fn.evthandler);

        // pop default flags
        if (!vsfvm_thread_stack_pop(thread, 1)) {
            return VSFVM_RET_ERROR;
        }
        thread->func.argc--;
    }

    return ret;
}

static void __vsfvm_ext_libusb_dev_print(vsfvm_instance_t *inst)
{
    if (vsfvm_instance_of(inst, &vsfvm_ext_libusb_dev)) {
        vsfvm_ext_libusb_dev_t *ext_ldev = inst->obj_ptr;
        vk_usbh_libusb_dev_t *ldev = ext_ldev->ldev;

        vsf_trace_info("libusb_dev@(%02X_%04X:%04X)",
                    ldev->address, ldev->vid, ldev->pid);
    }
}

static void __vsfvm_ext_libusb_dev_destroy(vsfvm_instance_t *inst)
{
    VSFVM_ASSERT(vsfvm_instance_of(inst, &vsfvm_ext_libusb_dev));
    vsfvm_ext_libusb_dev_t *ext_ldev = inst->obj_ptr;
    VSFVM_ASSERT(vsf_slist_is_in(vsfvm_ext_libusb_dev_t, dev_node, &__vsfvm_ext_libusb.dev_list, ext_ldev));

    vsf_slist_remove(vsfvm_ext_libusb_dev_t, dev_node, &__vsfvm_ext_libusb.dev_list, ext_ldev);
    if (ext_ldev->ldev != NULL) {
        vk_usbh_libusb_close(ext_ldev->ldev);
    }
}

#endif

void vsfvm_ext_register_libusb(void)
{
    memset(&__vsfvm_ext_libusb, 0, sizeof(__vsfvm_ext_libusb));

    __vsfvm_ext_libusb.op = &__vsfvm_ext_libusb_op;
    vsfvm_register_ext(&__vsfvm_ext_libusb.use_as__vsfvm_ext_t);
}

#endif      // (VSFVM_CFG_RUNTIME_EN || VSFVM_CFG_COMPILER_EN) && VSF_USE_USB_HOST && VSF_USBH_USE_LIBUSB
