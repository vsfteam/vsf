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

#ifndef __VSF_USBD_H__
#define __VSF_USBD_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED

#include "../common/usb_common.h"

#if     defined(VSF_USBD_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_USBD_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#ifndef VSF_USBD_CFG_AUTOSETUP
#   define VSF_USBD_CFG_AUTOSETUP           ENABLED
#endif

#ifndef VSF_USBD_CFG_USE_EDA
#   define VSF_USBD_CFG_USE_EDA             ENABLED
#endif

#define VSF_USBD_DESC_DEVICE(__LANID, __DESC, __SIZE)                           \
    {USB_DT_DEVICE, 0, (__LANID), (__SIZE), (uint8_t*)(__DESC)}
#define VSF_USBD_DESC_CONFIG(__LANID, __INDEX, __DESC, __SIZE)                  \
    {USB_DT_CONFIG, (__INDEX), (__LANID), (__SIZE), (uint8_t*)(__DESC)}
#define VSF_USBD_DESC_STRING(__LANID, __INDEX, __DESC, __SIZE)                  \
    {USB_DT_STRING, (__INDEX), (__LANID), (__SIZE), (uint8_t*)(__DESC)}



#if defined(VSF_USBD_CFG_DRV_LV0)
#   undef VSF_USBD_CFG_DRV_INTERFACE
#   undef VSF_USBD_DRV_PREPARE
#   define VSF_USBD_DRV_PREPARE(__dev)

#   define vsf_usbd_drv_func_name(__header, __func) TPASTE3(__header, _, __func)

// lv0 API used as usbd driver
#   define vsf_usbd_drv_init(__cfg)                                             \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, init)                      \
            (&VSF_USBD_CFG_DRV_OBJ, (__cfg))

#   define vsf_usbd_drv_fini()                                                  \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, fini)                      \
            (&VSF_USBD_CFG_DRV_OBJ)

#   define vsf_usbd_drv_reset()                                                 \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, reset)                     \
            (&VSF_USBD_CFG_DRV_OBJ)

#   define vsf_usbd_drv_connect()                                               \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, connect)                   \
            (&VSF_USBD_CFG_DRV_OBJ)

#   define vsf_usbd_drv_disconnect()                                            \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, disconnect)                \
            (&VSF_USBD_CFG_DRV_OBJ)

#   define vsf_usbd_drv_wakeup()                                                \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, wakeup)                    \
            (&VSF_USBD_CFG_DRV_OBJ)

#   define vsf_usbd_drv_set_address(__addr)                                     \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, set_address)               \
            (&VSF_USBD_CFG_DRV_OBJ, (__addr))

#   define vsf_usbd_drv_get_setup(__request)                                    \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, get_setup)                 \
            (&VSF_USBD_CFG_DRV_OBJ, (uint8_t *)(__request))

#   define vsf_usbd_drv_status_stage(__is_in)                                   \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, status_stage)              \
            (&VSF_USBD_CFG_DRV_OBJ, (__is_in))

#   define vsf_usbd_drv_ep_add(__addr, __attr, __size)                          \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_add)                    \
            (&VSF_USBD_CFG_DRV_OBJ, (__addr), (__attr), (__size))

#   define vsf_usbd_drv_ep_is_dma                                               \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_is_dma)

#   define vsf_usbd_drv_ep_number                                               \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_number)

#   define vsf_usbd_drv_ep_get_size(__ep)                                       \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_get_size)               \
            (&VSF_USBD_CFG_DRV_OBJ, (__ep))

#   define vsf_usbd_drv_ep_is_stalled(__ep)                                     \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_is_stalled)             \
            (&VSF_USBD_CFG_DRV_OBJ, (__ep))

#   define vsf_usbd_drv_ep_set_stall(__ep)                                      \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_set_stall)              \
            (&VSF_USBD_CFG_DRV_OBJ, (__ep))

#   define vsf_usbd_drv_ep_clear_stall(__ep)                                    \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_clear_stall)            \
            (&VSF_USBD_CFG_DRV_OBJ, (__ep))

#   define vsf_usbd_drv_ep_read_buffer(__ep, __buf, __size)                     \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_read_buffer)            \
            (&VSF_USBD_CFG_DRV_OBJ, (__ep), (__buf), (__size))

#   define vsf_usbd_drv_ep_write_buffer(__ep, __buf, __size)                    \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_write_buffer)           \
            (&VSF_USBD_CFG_DRV_OBJ, (__ep), (__buf), (__size))

#   define vsf_usbd_drv_ep_get_data_size(__ep)                                  \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_get_data_size)          \
            (&VSF_USBD_CFG_DRV_OBJ, (__ep))

#   define vsf_usbd_drv_ep_set_data_size(__ep, __size)                          \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_set_data_size)          \
            (&VSF_USBD_CFG_DRV_OBJ, (__ep), (__size))

#   define vsf_usbd_drv_ep_enable_out(__ep)                                     \
        vsf_usbd_drv_func_name(VSF_USBD_CFG_DRV_LV0, ep_enable_OUT)             \
            (&VSF_USBD_CFG_DRV_OBJ, (__ep))

#elif defined(VSF_USBD_CFG_DRV_LV1)
#   undef VSF_USBD_CFG_DRV_INTERFACE
// lv1 API used as usbd driver
#   error "not implemented now!!!"

#   define vsf_usbd_drv_init(__cfg)
#   define vsf_usbd_drv_fini()
#   define vsf_usbd_drv_reset()
#   define vsf_usbd_drv_connect()
#   define vsf_usbd_drv_disconnect()
#   define vsf_usbd_drv_wakeup()
#   define vsf_usbd_drv_set_address(__addr)
#   define vsf_usbd_drv_get_setup(__request)
#   define vsf_usbd_drv_status_stage(__is_in)
#   define vsf_usbd_drv_ep_add(__addr, __attr, __size)

#   define vsf_usbd_drv_ep_is_dma
#   define vsf_usbd_drv_ep_num
#   define vsf_usbd_drv_ep_get_size(__ep)
#   define vsf_usbd_drv_ep_is_stalled(__ep)
#   define vsf_usbd_drv_ep_set_stall(__ep)
#   define vsf_usbd_drv_ep_clear_stall(__ep)
#   define vsf_usbd_drv_ep_read_buffer(__ep, __buf, __size)
#   define vsf_usbd_drv_ep_write_buffer(__ep, __buf, __size)
#   define vsf_usbd_drv_ep_get_data_size(__ep)
#   define vsf_usbd_drv_ep_set_data_size(__ep, __size)
#   define vsf_usbd_drv_ep_enable_out(__ep)

#else
#   define VSF_USBD_CFG_DRV_INTERFACE
// interface-based API used as usbd driver
#   undef VSF_USBD_DRV_PREPARE
#   define VSF_USBD_DRV_PREPARE(__dev)                                          \
         const i_usb_dc_t *__drv = (__dev)->drv;

#   define vsf_usbd_drv_init(__cfg)                     __drv->Init((__cfg))
#   define vsf_usbd_drv_fini()                          __drv->Fini()
#   define vsf_usbd_drv_reset()                         __drv->Reset()
#   define vsf_usbd_drv_connect()                       __drv->Connect()
#   define vsf_usbd_drv_disconnect()                    __drv->Disconnect()
#   define vsf_usbd_drv_wakeup()                        __drv->Wakeup()
#   define vsf_usbd_drv_set_address(__addr)             __drv->SetAddress((__addr))
#   define vsf_usbd_drv_get_setup(__request)            __drv->GetSetup((uint8_t *)(__request))
#   define vsf_usbd_drv_status_stage(__is_in)           __drv->StatusStage((__is_in))
#   define vsf_usbd_drv_ep_add(__addr, __attr, __size)  __drv->Ep.Add((__addr), (__attr), (__size))

#   define vsf_usbd_drv_ep_is_dma                       __drv->Ep.IsDMA
#   define vsf_usbd_drv_ep_number                       __drv->Ep.Number
#   define vsf_usbd_drv_ep_get_size(__ep)               __drv->Ep.GetSize((__ep))
#   define vsf_usbd_drv_ep_is_stalled(__ep)             __drv->Ep.IsStalled((__ep))
#   define vsf_usbd_drv_ep_set_stall(__ep)              __drv->Ep.SetStall((__ep))
#   define vsf_usbd_drv_ep_clear_stall(__ep)            __drv->Ep.ClearStall((__ep))
#   define vsf_usbd_drv_ep_read_buffer(__ep, __buf, __size)     __drv->Ep.ReadBuffer((__ep), (__buf), (__size))
#   define vsf_usbd_drv_ep_write_buffer(__ep, __buf, __size)    __drv->Ep.WriteBuffer((__ep), (__buf), (__size))
#   define vsf_usbd_drv_ep_get_data_size(__ep)          __drv->Ep.GetDataSize((__ep))
#   define vsf_usbd_drv_ep_set_data_size(__ep, __size)  __drv->Ep.SetDataSize((__ep), (__size))
#   define vsf_usbd_drv_ep_enable_out(__ep)             __drv->Ep.EnableOUT((__ep))

#endif

#if defined(VSF_USBD_IMPLEMENT) || defined(VSF_USBD_INHERIT)
#   if VSF_USBD_CFG_USE_EDA == ENABLED
#       define vsf_usbd_class_priority(__dev)           vsf_priority_inherit
#   else
#       define vsf_usbd_class_priority(__dev)           (__dev)->priority_eda
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef union vsf_usbd_ep_cfg_t vsf_usbd_ep_cfg_t;
union vsf_usbd_ep_cfg_t {
    struct {
        uint8_t notify;
        uint8_t out;
        uint8_t in;
    };
    uint32_t ep_cfg;
};

declare_simple_class(vsf_usbd_dev_t)
declare_simple_class(vsf_usbd_cfg_t)
declare_simple_class(vsf_usbd_ifs_t)
declare_simple_class(vsf_usbd_trans_t)

enum vsf_usbd_evt_t {
    USB_ON_INIT =   USB_USR_EVT + 0,
    USB_ON_FINI =   USB_USR_EVT + 1
};
typedef enum vsf_usbd_evt_t vsf_usbd_evt_t;

struct vsf_usbd_desc_t {
    uint8_t type;
    uint8_t index;
    uint16_t lanid;
    uint16_t size;
    uint8_t *buffer;
};
typedef struct vsf_usbd_desc_t vsf_usbd_desc_t;

struct vsf_usbd_class_op_t {
    vsf_usbd_desc_t * (*get_desc)(vsf_usbd_dev_t *dev, uint_fast8_t type,
                uint_fast8_t index, uint_fast16_t lanid);

    vsf_err_t (*request_prepare)(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs);
    vsf_err_t (*request_process)(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs);

    vsf_err_t (*init)(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs);
    vsf_err_t (*fini)(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs);
};
typedef struct vsf_usbd_class_op_t vsf_usbd_class_op_t;

def_simple_class(vsf_usbd_trans_t) {
    public_member(
        uint8_t ep;
        bool zlp;
        bool notify_eda;
        implement(vsf_mem_t)
        union {
            struct {
                void (*on_finish)(void *param);
                void *param;
            };
            struct {
                vsf_eda_t *eda;
            };
        };
    )

	private_member(
        vsf_slist_node_t node;
        uint8_t *cur;
    )
};

struct vsf_usbd_ctrl_handler_t {
    struct usb_ctrlrequest_t request;
    vsf_usbd_trans_t trans;
    uint8_t reply_buffer[4];
};
typedef struct vsf_usbd_ctrl_handler_t vsf_usbd_ctrl_handler_t;

def_simple_class(vsf_usbd_ifs_t) {

    public_member(
        const vsf_usbd_class_op_t *class_op;
        void *class_param;
    )

    private_member(
        uint8_t alternate_setting;
        bool is_inited;
    )
};

def_simple_class(vsf_usbd_cfg_t) {

    public_member(
        vsf_err_t (*init)(vsf_usbd_dev_t *dev);
        vsf_err_t (*fini)(vsf_usbd_dev_t *dev);

        uint8_t num_of_ifs;
        vsf_usbd_ifs_t *ifs;
    )

    private_member(
        uint8_t configuration_value;
        int8_t ep_ifs_map[32];
    )
};

def_simple_class(vsf_usbd_dev_t) {
    public_member(
        uint8_t num_of_config;
        uint8_t num_of_desc;
        uint8_t device_class_ifs;

        usb_dc_speed_t speed;
        vsf_arch_priority_t priority_int;
#if VSF_USBD_CFG_USE_EDA == ENABLED
        vsf_priority_t priority_eda;
#endif
        vsf_usbd_cfg_t *config;
        vsf_usbd_desc_t *desc;

#ifdef VSF_USBD_CFG_DRV_INTERFACE
        const i_usb_dc_t *drv;
#endif
    )

    protected_member(
        vsf_usbd_ctrl_handler_t ctrl_handler;
    )

    private_member(
        uint8_t address;
        uint8_t configuration;
        uint8_t feature;
        bool configured;
        vsf_slist_t trans_list;
#if VSF_USBD_CFG_USE_EDA == ENABLED
        vsf_eda_t eda;
#endif
    )
};

#if VSF_USBD_CFG_STREAM_EN == ENABLED
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
declare_simple_class(vsf_usbd_ep_stream_t)
def_simple_class(vsf_usbd_ep_stream_t) {

    public_member(
        vsf_stream_t *stream;
    )

    protected_member(
        implement(vsf_usbd_trans_t)
        vsf_usbd_dev_t *dev;                //!< todo: rx and tx can share the same dev
        uint32_t cur_size;
    )
};

#elif VSF_USE_SERVICE_STREAM == ENABLED

declare_class(vsf_usbd_ep_stream_t)


typedef struct {
    uint8_t rx_ep;
    uint8_t tx_ep;
    //vsf_usbd_dev_t *dev;
}vsf_usbd_ep_stream_cfg_t;

def_class(vsf_usbd_ep_stream_t,
    which(
        implement(vsf_stream_src_t)
        implement(vsf_stream_usr_t)
    ),
    private_member(
        vsf_usbd_trans_t tx_trans;
        vsf_usbd_trans_t rx_trans;
        vsf_pbuf_t     *tx_current;
        vsf_pbuf_t     *rx_current;
        vsf_usbd_dev_t *dev;
    )
)
end_def_class(vsf_usbd_ep_stream_src_t)

#endif
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_usbd_init(vsf_usbd_dev_t *dev);
extern void vsf_usbd_fini(vsf_usbd_dev_t *dev);
extern void vsf_usbd_connect(vsf_usbd_dev_t *dev);
extern void vsf_usbd_disconnect(vsf_usbd_dev_t *dev);
extern void vsf_usbd_wakeup(vsf_usbd_dev_t *dev);

#if defined(VSF_USBD_IMPLEMENT) || defined(VSF_USBD_INHERIT)
extern vsf_usbd_desc_t * vsf_usbd_get_descriptor(vsf_usbd_desc_t *desc,
        uint_fast8_t desc_num, uint_fast8_t type,
        uint_fast8_t index, uint_fast16_t lanid);
extern vsf_usbd_cfg_t * vsf_usbd_get_cur_cfg(vsf_usbd_dev_t *dev);
extern vsf_usbd_ifs_t * vsf_usbd_get_ifs(vsf_usbd_dev_t *dev, uint_fast8_t ifs_no);

extern vsf_err_t vsf_usbd_ep_recv(vsf_usbd_dev_t *dev, vsf_usbd_trans_t *trans);
extern vsf_err_t vsf_usbd_ep_send(vsf_usbd_dev_t *dev, vsf_usbd_trans_t *trans);


#if VSF_USBD_CFG_STREAM_EN == ENABLED
extern vsf_err_t vsf_usbd_ep_recv_stream(vsf_usbd_ep_stream_t *);
extern vsf_err_t vsf_usbd_ep_send_stream(vsf_usbd_ep_stream_t *);

#if VSF_USE_SERVICE_STREAM == ENABLED

extern 
void vsf_usbd_ep_stream_init(   vsf_usbd_ep_stream_t *obj, 
                                vsf_usbd_ep_stream_cfg_t *cfg);

extern 
void vsf_usbd_ep_stream_connect_dev(vsf_usbd_ep_stream_t *obj, 
                                    vsf_usbd_dev_t *dev);
#endif      // VSF_USE_SERVICE_STREAM
#endif      // VSF_USBD_CFG_STREAM_EN

#endif      // VSF_USBD_IMPLEMENT || VSF_USBD_INHERIT

#undef VSF_USBD_IMPLEMENT
#undef VSF_USBD_INHERIT

#endif    // VSF_USE_USB_DEVICE
#endif    // __VSF_USBD_H__
