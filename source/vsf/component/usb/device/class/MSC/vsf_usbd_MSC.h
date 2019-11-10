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

#ifndef __VSF_USBD_MSC_H__
#define __VSF_USBD_MSC_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USE_USB_DEVICE_MSC == ENABLED

#include "../../../common/class/MSC/vsf_usb_MSC.h"
#include "./vsf_usbd_MSC_desc.h"

#if     defined(VSF_USBD_MSC_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef VSF_USBD_MSC_IMPLEMENT
#elif   defined(VSF_USBD_MSC_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#   undef VSF_USBD_MSC_INHERIT
#endif
#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#ifndef VSF_USBD_MSC_CFG_REPLY_SIZE
#   define VSF_USBD_MSC_CFG_REPLY_SIZE              36  // reply size for inquiry
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_class(vsf_usbd_msc_t)

union vsf_usbd_msc_scsi_ctx_t {
    usb_msc_cbw_t cbw;
    struct {
        usb_msc_csw_t csw;
        union {
            uint8_t reply[VSF_USBD_MSC_CFG_REPLY_SIZE];
            struct {
                uint64_t addr;
                uint32_t size;
                uint32_t cur_size;
                uint8_t cmd;
            };
        };
    };
};
typedef union vsf_usbd_msc_scsi_ctx_t vsf_usbd_msc_scsi_ctx_t;

struct vsf_usbd_msc_op_t {
    /*! \brief execute commands
     *! \param msc          mass storage class object
     *! \param ctx          scsi context
     *! \param mem          memory for the data phase
     *! \retval < 0         error
     *! \retval >= 0        byte size of data phase
     */
    int_fast32_t (*execute)(vsf_usbd_msc_t *msc, vsf_usbd_msc_scsi_ctx_t *ctx, vsf_mem_t *mem);

    /*! \brief read memory asynchronously, call vsf_usbd_msc_send_data when done
     *! \param msc          mass storage class object
     *! \param ctx          scsi context
     *! \param mem          memory buffer for read operation
     *! \retval             error code
     */
    vsf_err_t (*read)(vsf_usbd_msc_t *msc, vsf_usbd_msc_scsi_ctx_t *ctx, vsf_mem_t *mem);

    /*! \brief write memory asynchronously, call vsf_usbd_msc_recv_data when done
     *! \param msc          mass storage class object
     *! \param ctx          scsi context
     *! \param mem          memory buffer for read operation
     *! \retval             error code
     */
    vsf_err_t (*write)(vsf_usbd_msc_t *msc, vsf_usbd_msc_scsi_ctx_t *ctx, vsf_mem_t *mem);
};
typedef struct vsf_usbd_msc_op_t vsf_usbd_msc_op_t;

def_simple_class(vsf_usbd_msc_t) {

    public_member(
        const uint8_t ep_out;
        const uint8_t ep_in;
        const uint8_t max_lun;
        vsf_usbd_msc_op_t const * const op;
    )

    private_member(
        vsf_usbd_msc_scsi_ctx_t ctx;
        vsf_usbd_dev_t *dev;
        vsf_usbd_trans_t trans;
        int32_t reply_size;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_usbd_class_op_t vsf_usbd_msc_class;

/*============================ PROTOTYPES ====================================*/

extern void vsf_usbd_msc_send_data(vsf_usbd_msc_t *msc);
extern void vsf_usbd_msc_recv_data(vsf_usbd_msc_t *msc);

#endif      // VSF_USE_USB_DEVICE && VSF_USE_USB_DEVICE_MSC
#endif      // __VSF_USBD_MSC_H__
