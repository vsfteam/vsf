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

#ifndef __HAL_DRIVER_USB_INTERFACE_H__
#define __HAL_DRIVER_USB_INTERFACE_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "hal/arch/vsf_arch.h"
/*============================ MACROS ========================================*/

#define USB_DC_FEATURE_TRANSFER             (1 << 0)

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __USB_HC_IP_FUNC_DEF(__N, __VALUE)                                      \
static vsf_err_t    usb_hc##__N##_init(usb_hc_ip_cfg_t *cfg);                   \
static void         usb_hc##__N##_get_info(usb_hc_ip_info_t *info);             \
static void         usb_hc##__N##_irq(void);

#define __USB_HC_IP_INTERFACE_FUNC_DEF(__N, __VALUE)                            \
            {                                                                   \
                .Init           = &usb_hc##__N##_init,                          \
                .GetInfo        = &usb_hc##__N##_get_info,                      \
                .Irq            = &usb_hc##__N##_irq,                           \
            }

#define __USB_HC_IP_BODY_EX(__N, __OBJ, __HEADER)                               \
static vsf_err_t usb_hc##__N##_init(usb_hc_ip_cfg_t *cfg)                       \
{ return __HEADER##_init(&(__OBJ), cfg); }                                      \
static void usb_hc##__N##_get_info(usb_hc_ip_info_t *info)                      \
{ __HEADER##_get_info(&(__OBJ), info); }                                        \
static void usb_hc##__N##_irq(void)                                             \
{ __HEADER##_irq(&(__OBJ)); }

#define __USB_HC_IP_BODY(__N, __HEADER)                                         \
    __USB_HC_IP_BODY_EX(__N, USB_HC##__N##_IP, __HEADER)
#define __USB_OTG_HC_IP_BODY(__N, __HEADER)                                     \
    __USB_HC_IP_BODY_EX(__N, USB_OTG##__N##_IP, __HEADER)



#define __USB_DC_IP_FUNC_DEF(__N, __VALUE)                                      \
static vsf_err_t    usb_dc##__N##_init(usb_dc_ip_cfg_t *cfg);                   \
static void         usb_dc##__N##_fini(void);                                   \
static void         usb_dc##__N##_get_info(usb_dc_ip_info_t *info);             \
static void         usb_dc##__N##_connect(void);                                \
static void         usb_dc##__N##_disconnect(void);                             \
static void         usb_dc##__N##_irq(void);

#define __USB_DC_IP_INTERFACE_FUNC_DEF(__N, __VALUE)                            \
            {                                                                   \
                .Init           = &usb_dc##__N##_init,                          \
                .Fini           = &usb_dc##__N##_fini,                          \
                .GetInfo        = &usb_dc##__N##_get_info,                      \
                .Connect        = &usb_dc##__N##_connect,                       \
                .Disconnect     = &usb_dc##__N##_disconnect,                    \
                .Irq            = &usb_dc##__N##_irq,                           \
            }

#define __USB_DC_IP_BODY_EX(__N, __OBJ, __HEADER)                               \
static vsf_err_t usb_dc##__N##_init(usb_dc_ip_cfg_t *cfg)                       \
{ return __HEADER##_init(&(__OBJ), cfg); }                                      \
static void usb_dc##__N##_fini(void)                                            \
{ __HEADER##_fini(&(__OBJ)); }                                                  \
static void usb_dc##__N##_get_info(usb_dc_ip_info_t *info)                      \
{ __HEADER##_get_info(&(__OBJ), info); }                                        \
static void usb_dc##__N##_connect(void)                                         \
{ __HEADER##_connect(&(__OBJ)); }                                               \
static void usb_dc##__N##_disconnect(void)                                      \
{ __HEADER##_disconnect(&(__OBJ)); }                                            \
static void usb_dc##__N##_irq(void)                                             \
{ __HEADER##_irq(&(__OBJ)); }

#define __USB_DC_IP_BODY(__N, __HEADER)                                         \
    __USB_DC_IP_BODY_EX(__N, USB_DC##__N##_IP, __HEADER)
#define __USB_OTG_DC_IP_BODY(__N, __HEADER)                                     \
    __USB_DC_IP_BODY_EX(__N, USB_OTG##__N##_IP, __HEADER)

#define __USB_DC_FROM_IP(__N, __OBJ, __DRV_NAME, __HEADER)                      \
__USB_DC_FUNC_DEF(__N, NULL)                                                    \
static const i_usb_dc_t __DRV_NAME = __USB_DC_INTERFACE_FUNC_DEF(__N, NULL);    \
__USB_DC_BODY_EX(__N, __HEADER, __OBJ)


#define __USB_DC_FUNC_DEF(__N, __VALUE)                                         \
static vsf_err_t    usb_dc##__N##_init(usb_dc_cfg_t *cfg);                      \
static void         usb_dc##__N##_fini(void);                                   \
static void         usb_dc##__N##_reset(usb_dc_cfg_t *cfg);                     \
                                                                                \
static void         usb_dc##__N##_connect(void);                                \
static void         usb_dc##__N##_disconnect(void);                             \
static void         usb_dc##__N##_wakeup(void);                                 \
                                                                                \
static void         usb_dc##__N##_set_address(uint_fast8_t addr);               \
static uint_fast8_t usb_dc##__N##_get_address(void);                            \
                                                                                \
static uint_fast16_t    usb_dc##__N##_get_frame_number(void);                   \
static uint_fast8_t usb_dc##__N##_get_mframe_number(void);                      \
                                                                                \
static void         usb_dc##__N##_get_setup(uint8_t *buffer);                   \
static void         usb_dc##__N##_status_stage(bool is_in);                     \
                                                                                \
static uint_fast8_t usb_dc##__N##_ep_get_feature(uint_fast8_t ep);              \
static vsf_err_t    usb_dc##__N##_ep_add(uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);\
static uint_fast16_t    usb_dc##__N##_ep_get_size(uint_fast8_t ep);             \
                                                                                \
static vsf_err_t    usb_dc##__N##_ep_set_stall(uint_fast8_t ep);                \
static bool         usb_dc##__N##_ep_is_stalled(uint_fast8_t ep);               \
static vsf_err_t    usb_dc##__N##_ep_clear_stall(uint_fast8_t ep);              \
                                                                                \
static uint_fast32_t    usb_dc##__N##_ep_get_data_size(uint_fast8_t ep);        \
static vsf_err_t    usb_dc##__N##_ep_transaction_read_buffer(uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);\
static vsf_err_t    usb_dc##__N##_ep_transaction_enable_out(uint_fast8_t ep);   \
                                                                                \
static vsf_err_t    usb_dc##__N##_ep_transaction_set_data_size(uint_fast8_t ep, uint_fast16_t size);\
static vsf_err_t    usb_dc##__N##_ep_transaction_write_buffer(uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);\
                                                                                \
static vsf_err_t    usb_dc##__N##_ep_transfer_recv(uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size);\
static vsf_err_t    usb_dc##__N##_ep_transfer_send(uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp);\
                                                                                \
static void         usb_dc##__N##_irq(void);

#define __USB_DC_INTERFACE_FUNC_DEF(__N, __VALUE)                               \
            {                                                                   \
                .Init           = &usb_dc##__N##_init,                          \
                .Fini           = &usb_dc##__N##_fini,                          \
                .Reset          = &usb_dc##__N##_reset,                         \
                .Connect        = &usb_dc##__N##_connect,                       \
                .Disconnect     = &usb_dc##__N##_disconnect,                    \
                .Wakeup         = &usb_dc##__N##_wakeup,                        \
                .SetAddress     = &usb_dc##__N##_set_address,                   \
                .GetAddress     = &usb_dc##__N##_get_address,                   \
                .GetFrameNo     = &usb_dc##__N##_get_frame_number,              \
                .GetMicroFrameNo= &usb_dc##__N##_get_mframe_number,             \
                .GetSetup       = &usb_dc##__N##_get_setup,                     \
                .StatusStage    = &usb_dc##__N##_status_stage,                  \
                                                                                \
                .Ep.GetFeature  = &usb_dc##__N##_ep_get_feature,                \
                .Ep.Add         = &usb_dc##__N##_ep_add,                        \
                .Ep.GetSize     = &usb_dc##__N##_ep_get_size,                   \
                .Ep.SetStall    = &usb_dc##__N##_ep_set_stall,                  \
                .Ep.IsStalled   = &usb_dc##__N##_ep_is_stalled,                 \
                .Ep.ClearStall  = &usb_dc##__N##_ep_clear_stall,                \
                .Ep.GetDataSize = &usb_dc##__N##_ep_get_data_size,              \
                .Ep.Transaction.ReadBuffer  = &usb_dc##__N##_ep_transaction_read_buffer,    \
                .Ep.Transaction.EnableOut   = &usb_dc##__N##_ep_transaction_enable_out,     \
                .Ep.Transaction.SetDataSize = &usb_dc##__N##_ep_transaction_set_data_size,  \
                .Ep.Transaction.WriteBuffer = &usb_dc##__N##_ep_transaction_write_buffer,   \
                .Ep.Transfer.Recv           = &usb_dc##__N##_ep_transfer_recv,  \
                .Ep.Transfer.Send           = &usb_dc##__N##_ep_transfer_send,  \
                                                                                \
                .Irq            = &usb_dc##__N##_irq,                           \
            }

#define __USB_DC_BODY_EX(__N, __HEADER, __OBJ)                                  \
static vsf_err_t usb_dc##__N##_init(usb_dc_cfg_t *cfg)                          \
{ return __HEADER##_init(&(__OBJ), cfg); }                                      \
static void usb_dc##__N##_fini(void)                                            \
{ __HEADER##_fini(&(__OBJ)); }                                                  \
static void usb_dc##__N##_reset(usb_dc_cfg_t *cfg)                              \
{ __HEADER##_reset(&(__OBJ), cfg); }                                            \
static void usb_dc##__N##_connect(void)                                         \
{ __HEADER##_connect(&(__OBJ)); }                                               \
static void usb_dc##__N##_disconnect(void)                                      \
{ __HEADER##_disconnect(&(__OBJ)); }                                            \
static void usb_dc##__N##_wakeup(void)                                          \
{ __HEADER##_wakeup(&(__OBJ)); }                                                \
static void usb_dc##__N##_set_address(uint_fast8_t addr)                        \
{ __HEADER##_set_address(&(__OBJ), addr); }                                     \
static uint_fast8_t usb_dc##__N##_get_address(void)                             \
{ return __HEADER##_get_address(&(__OBJ)); }                                    \
static uint_fast16_t usb_dc##__N##_get_frame_number(void)                       \
{ return __HEADER##_get_frame_number(&(__OBJ)); }                               \
static uint_fast8_t usb_dc##__N##_get_mframe_number(void)                       \
{ return __HEADER##_get_mframe_number(&(__OBJ));  }                             \
static void usb_dc##__N##_get_setup(uint8_t *buffer)                            \
{ __HEADER##_get_setup(&(__OBJ), buffer); }                                     \
static void usb_dc##__N##_status_stage(bool is_in)                              \
{ __HEADER##_status_stage(&(__OBJ), is_in); }                                   \
static uint_fast8_t usb_dc##__N##_ep_get_feature(uint_fast8_t ep)               \
{ return __HEADER##_ep_get_feature(&(__OBJ), ep); }                             \
static vsf_err_t usb_dc##__N##_ep_add(uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size)\
{ return __HEADER##_ep_add(&(__OBJ), ep, type, size); }                         \
static uint_fast16_t usb_dc##__N##_ep_get_size(uint_fast8_t ep)                 \
{ return __HEADER##_ep_get_size(&(__OBJ), ep); }                                \
static vsf_err_t usb_dc##__N##_ep_set_stall(uint_fast8_t ep)                    \
{ return __HEADER##_ep_set_stall(&(__OBJ), ep); }                               \
static bool usb_dc##__N##_ep_is_stalled(uint_fast8_t ep)                        \
{ return __HEADER##_ep_is_stalled(&(__OBJ), ep); }                              \
static vsf_err_t usb_dc##__N##_ep_clear_stall(uint_fast8_t ep)                  \
{ return __HEADER##_ep_clear_stall(&(__OBJ), ep); }                             \
static uint_fast32_t usb_dc##__N##_ep_get_data_size(uint_fast8_t ep)            \
{ return __HEADER##_ep_get_data_size(&(__OBJ), ep); }                           \
static vsf_err_t usb_dc##__N##_ep_transaction_read_buffer(uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)\
{ return __HEADER##_ep_transaction_read_buffer(&(__OBJ), ep, buffer, size); }   \
static vsf_err_t usb_dc##__N##_ep_transaction_enable_out(uint_fast8_t ep)       \
{ return __HEADER##_ep_transaction_enable_out(&(__OBJ), ep); }                  \
static vsf_err_t usb_dc##__N##_ep_transaction_set_data_size(uint_fast8_t ep, uint_fast16_t size)\
{ return __HEADER##_ep_transaction_set_data_size(&(__OBJ), ep, size); }         \
static vsf_err_t usb_dc##__N##_ep_transaction_write_buffer(uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)\
{ return __HEADER##_ep_transaction_write_buffer(&(__OBJ), ep, buffer, size); }  \
static vsf_err_t usb_dc##__N##_ep_transfer_recv(uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size)\
{ return __HEADER##_ep_transfer_recv(&(__OBJ), ep, buffer, size); }             \
static vsf_err_t usb_dc##__N##_ep_transfer_send(uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp)\
{ return __HEADER##_ep_transfer_send(&(__OBJ), ep, buffer, size, zlp); }        \
static void usb_dc##__N##_irq(void)                                             \
{ __HEADER##_irq(&(__OBJ)); }

#define __USB_DC_BODY(__N, __HEADER)                                            \
        __USB_DC_BODY_EX(__N, __HEADER, USB_DC##__N)

/*============================ TYPES =========================================*/

enum usb_ep_type_t {
    USB_EP_TYPE_CONTROL     = 0,
    USB_EP_TYPE_INTERRUPT   = 3,
    USB_EP_TYPE_BULK        = 2,
    USB_EP_TYPE_ISO         = 1,
};
typedef enum usb_ep_type_t usb_ep_type_t;

enum usb_evt_t {
    USB_ON_ATTACH,
    USB_ON_DETACH,
    USB_ON_RESET,
    USB_ON_SETUP,
    USB_ON_ERROR,
    USB_ON_SUSPEND,
    USB_ON_RESUME,
    USB_ON_SOF,
    USB_ON_IN,
    USB_ON_NAK,
    USB_ON_OUT,
    USB_ON_STATUS,
    USB_ON_UNDERFLOW,
    USB_ON_OVERFLOW,
    USB_USR_EVT,
};
typedef enum usb_evt_t usb_evt_t;

enum usb_dc_err_t {
    USB_DC_ERR_ERROR,
    USB_DC_ERR_INVALID_CRC,
    USB_DC_ERR_SOF_TO,
};
typedef enum usb_dc_err_t usb_dc_err_t;

enum usb_dc_speed_t {
    USB_DC_SPEED_LOW,
    USB_DC_SPEED_FULL,
    USB_DC_SPEED_HIGH,
    USB_DC_SPEED_SUPER,
};
typedef enum usb_dc_speed_t usb_dc_speed_t;

typedef void (*usb_ip_irq_handler_t)(void *param);
typedef void (*usb_dc_evt_handler_t)(void *param, usb_evt_t evt, uint_fast8_t value);

//! \name usb_dc configuration structure
//! @{
struct usb_dc_cfg_t {
    vsf_arch_prio_t         priority;       //!< interrupt priority
    usb_dc_evt_handler_t    evt_handler;    //!< evt_handler function
    void                    *param;         //!< dcd related parameters

    usb_dc_speed_t          speed;          //!< speed
};
typedef struct usb_dc_cfg_t usb_dc_cfg_t;
//! @}

//! \name usb_dc and ep control interface
//! @{

def_interface(i_usb_dc_t)

    vsf_err_t       (*Init)             (usb_dc_cfg_t *cfg);
    void            (*Fini)             (void);
    void            (*Reset)            (usb_dc_cfg_t *cfg);

    void            (*Connect)          (void);
    void            (*Disconnect)       (void);
    void            (*Wakeup)           (void);

    void            (*SetAddress)       (uint_fast8_t addr);
    uint_fast8_t    (*GetAddress)       (void);

    uint_fast16_t   (*GetFrameNo)       (void);
    uint_fast8_t    (*GetMicroFrameNo)  (void);

    void            (*GetSetup)         (uint8_t *buffer);
    void            (*StatusStage)      (bool is_in);

    struct {
        uint_fast8_t    (*GetFeature)       (uint_fast8_t ep);

        vsf_err_t       (*Add)              (uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);
        uint_fast16_t   (*GetSize)          (uint_fast8_t ep);

        vsf_err_t       (*SetStall)         (uint_fast8_t ep);
        bool            (*IsStalled)        (uint_fast8_t ep);
        vsf_err_t       (*ClearStall)       (uint_fast8_t ep);

        //! get the data size in hw-buffer in transaction mode
        //! get the all transfered data size in transfer mode  
        uint_fast32_t   (*GetDataSize)      (uint_fast8_t ep);

        struct {
            vsf_err_t   (*ReadBuffer)       (uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
            vsf_err_t   (*EnableOut)        (uint_fast8_t ep);
            vsf_err_t   (*SetDataSize)      (uint_fast8_t ep, uint_fast16_t size);
            vsf_err_t   (*WriteBuffer)      (uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
        } Transaction;

        struct {
            vsf_err_t   (*Recv)             (uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size);
            vsf_err_t   (*Send)             (uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp);
        } Transfer;
    } Ep;

    //! irq handler
    void            (*Irq)              (void);

end_def_interface(i_usb_dc_t)
//! @}




//! \name usb_dc_ip configuration structure
//! @{
struct usb_dc_ip_cfg_t {
    vsf_arch_prio_t         priority;       //!< interrupt priority
    usb_ip_irq_handler_t    irq_handler;    //!< irq_handler function
    void                    *param;         //!< dcd related parameters
};
typedef struct usb_dc_ip_cfg_t usb_dc_ip_cfg_t;
//! @}

//! \name usb_dc_ip information structure
//! @{
struct usb_dc_ip_info_t {
    void                    *regbase;
    uint8_t                 ep_num;
    bool                    is_dma;
};
typedef struct usb_dc_ip_info_t usb_dc_ip_info_t;
//! @}

//! \name usb_dc_ip control interface
//! @{
def_interface(i_usb_dc_ip_t)

    //! initialize
    vsf_err_t       (*Init)             (usb_dc_ip_cfg_t *cfg);
    void            (*Fini)             (void);

    void            (*GetInfo)          (usb_dc_ip_info_t *info);

    void            (*Connect)          (void);
    void            (*Disconnect)       (void);

    //! irq handler
    void            (*Irq)              (void);

end_def_interface(i_usb_dc_ip_t)
//! @}






//! \name usb_hc_ip configuration structure
//! @{
struct usb_hc_ip_cfg_t {
    vsf_arch_prio_t         priority;       //!< interrupt priority
    usb_ip_irq_handler_t    irq_handler;    //!< irq_handler function
    void                    *param;         //!< hcd related parameters
};
typedef struct usb_hc_ip_cfg_t usb_hc_ip_cfg_t;
//! @}

//! \name usb_hc_ip information structure
//! @{
struct usb_hc_ip_info_t {
    void                    *regbase;
    uint8_t                 ep_num;
    bool                    is_dma;
};
typedef struct usb_hc_ip_info_t usb_hc_ip_info_t;
//! @}

//! \name usb_hc_ip control interface
//! @{
def_interface(i_usb_hc_ip_t)

    //! initialize
    vsf_err_t       (*Init)             (usb_hc_ip_cfg_t *cfg);

    //! get register base
    void            (*GetInfo)          (usb_hc_ip_info_t *info);

    //! irq handler
    void            (*Irq)              (void);

end_def_interface(i_usb_hc_ip_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
