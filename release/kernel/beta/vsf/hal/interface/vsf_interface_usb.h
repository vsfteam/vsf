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
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __USB_HC_FUNC_DEF(__N, __VALUE)                                         \
static vsf_err_t    usb_hc##__N##_init(usb_hc_cfg_t *cfg);                      \
static void *       usb_hc##__N##_get_regbase(void);                            \
static void         usb_hc##__N##_irq(void);

#define __USB_HC_INTERFACE_FUNC_DEF(__N, __VALUE)                               \
            {                                                                   \
                .Init           = &usb_hc##__N##_init,                          \
                .GetRegBase     = &usb_hc##__N##_get_regbase,                   \
                .Irq            = &usb_hc##__N##_irq,                           \
            },

#define __USB_HC_BODY(__N, __VALUE)                                             \
static vsf_err_t usb_hc##__N##_init(usb_hc_cfg_t *cfg)                          \
{ return vsf_usb_hc_init((vsf_usb_hc_t *)&USB_HC##__N, cfg); }                  \
static void * usb_hc##__N##_get_regbase(void)                                   \
{ return vsf_usb_hc_get_regbase((vsf_usb_hc_t *)&USB_HC##__N); }                \
static void usb_hc##__N##_irq(void)                                             \
{ vsf_usb_hc_irq((vsf_usb_hc_t *)&USB_HC##__N); }

#define __USB_DC_FUNC_DEF(__N, __VALUE)                                         \
static vsf_err_t    usb_dc##__N##_init(usb_dc_cfg_t *cfg);                      \
static void         usb_dc##__N##_fini();                                       \
static void         usb_dc##__N##_reset();                                      \
                                                                                \
static void         usb_dc##__N##_connect();                                    \
static void         usb_dc##__N##_disconnect();                                 \
static void         usb_dc##__N##_wakeup();                                     \
                                                                                \
static void         usb_dc##__N##_set_address(uint_fast8_t addr);               \
static uint_fast8_t usb_dc##__N##_get_address();                                \
                                                                                \
static uint_fast16_t    usb_dc##__N##_get_frame_number();                       \
static uint_fast8_t usb_dc##__N##_get_mframe_number();                          \
                                                                                \
static void         usb_dc##__N##_get_setup(uint8_t *buffer);                   \
                                                                                \
static vsf_err_t    usb_dc##__N##_ep_add(uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);\
static uint_fast16_t    usb_dc##__N##_ep_get_size(uint_fast8_t ep);             \
                                                                                \
static vsf_err_t    usb_dc##__N##_ep_set_stall(uint_fast8_t ep);                \
static bool         usb_dc##__N##_ep_is_stalled(uint_fast8_t ep);               \
static vsf_err_t    usb_dc##__N##_ep_clear_stall(uint_fast8_t ep);              \
                                                                                \
static uint_fast16_t    usb_dc##__N##_ep_get_data_size(uint_fast8_t ep);        \
static vsf_err_t    usb_dc##__N##_ep_read_buffer(uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);\
static vsf_err_t    usb_dc##__N##_ep_enable_OUT(uint_fast8_t ep);               \
                                                                                \
static vsf_err_t    usb_dc##__N##_ep_set_data_size(uint_fast8_t ep, uint_fast16_t size);\
static vsf_err_t    usb_dc##__N##_ep_write_buffer(uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);\
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
                                                                                \
                .Ep.Number      = USB_DC##__N##_EP_NUMBER,                      \
                .Ep.IsDMA       = USB_DC##__N##_EP_IS_DMA,                      \
                .Ep.Add         = &usb_dc##__N##_ep_add,                        \
                .Ep.GetSize     = &usb_dc##__N##_ep_get_size,                   \
                .Ep.SetStall    = &usb_dc##__N##_ep_set_stall,                  \
                .Ep.IsStalled   = &usb_dc##__N##_ep_is_stalled,                 \
                .Ep.ClearStall  = &usb_dc##__N##_ep_clear_stall,                \
                .Ep.GetDataSize = &usb_dc##__N##_ep_get_data_size,              \
                .Ep.ReadBuffer  = &usb_dc##__N##_ep_read_buffer,                \
                .Ep.EnableOUT   = &usb_dc##__N##_ep_enable_OUT,                 \
                .Ep.SetDataSize = &usb_dc##__N##_ep_set_data_size,              \
                .Ep.WriteBuffer = &usb_dc##__N##_ep_write_buffer,               \
                                                                                \
                .Irq            = &usb_dc##__N##_irq,                           \
            }

#define __USB_DC_BODY(__N, __VALUE)                                             \
static vsf_err_t usb_dc##__N##_init(usb_dc_cfg_t *cfg)                          \
{ return vsf_usb_dc_init((vsf_usb_dc_t *)&USB_DC##__N, cfg); }                  \
static void usb_dc##__N##_fini(void)                                            \
{ vsf_usb_dc_fini((vsf_usb_dc_t *)&USB_DC##__N); }                              \
static void usb_dc##__N##_reset(void)                                           \
{ vsf_usb_dc_reset((vsf_usb_dc_t *)&USB_DC##__N); }                             \
static void usb_dc##__N##_connect(void)                                         \
{ vsf_usb_dc_connect((vsf_usb_dc_t *)&USB_DC##__N); }                           \
static void usb_dc##__N##_disconnect(void)                                      \
{ vsf_usb_dc_disconnect((vsf_usb_dc_t *)&USB_DC##__N); }                        \
static void usb_dc##__N##_wakeup(void)                                          \
{ vsf_usb_dc_wakeup((vsf_usb_dc_t *)&USB_DC##__N); }                            \
static void usb_dc##__N##_set_address(uint_fast8_t addr)                        \
{ vsf_usb_dc_set_address((vsf_usb_dc_t *)&USB_DC##__N, addr); }                 \
static uint_fast8_t usb_dc##__N##_get_address(void)                             \
{ return vsf_usb_dc_get_address((vsf_usb_dc_t *)&USB_DC##__N); }                \
static uint_fast16_t usb_dc##__N##_get_frame_number(void)                       \
{ return vsf_usb_dc_get_frame_number((vsf_usb_dc_t *)&USB_DC##__N); }           \
static uint_fast8_t usb_dc##__N##_get_mframe_number(void)                       \
{ return vsf_usb_dc_get_mframe_number((vsf_usb_dc_t *)&USB_DC##__N);  }         \
static void usb_dc##__N##_get_setup(uint8_t *buffer)                            \
{ vsf_usb_dc_get_setup((vsf_usb_dc_t *)&USB_DC##__N, buffer); }                 \
static vsf_err_t usb_dc##__N##_ep_add(uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size)\
{ return vsf_usb_dc_ep_add((vsf_usb_dc_t *)&USB_DC##__N, ep, type, size); }     \
static uint_fast16_t usb_dc##__N##_ep_get_size(uint_fast8_t ep)                 \
{ return vsf_usb_dc_ep_get_size((vsf_usb_dc_t *)&USB_DC##__N, ep); }            \
static vsf_err_t usb_dc##__N##_ep_set_stall(uint_fast8_t ep)                    \
{ return vsf_usb_dc_ep_set_stall((vsf_usb_dc_t *)&USB_DC##__N, ep); }           \
static bool usb_dc##__N##_ep_is_stalled(uint_fast8_t ep)                        \
{ return vsf_usb_dc_ep_is_stalled((vsf_usb_dc_t *)&USB_DC##__N, ep); }          \
static vsf_err_t usb_dc##__N##_ep_clear_stall(uint_fast8_t ep)                  \
{ return vsf_usb_dc_ep_clear_stall((vsf_usb_dc_t *)&USB_DC##__N, ep); }         \
static uint_fast16_t usb_dc##__N##_ep_get_data_size(uint_fast8_t ep)            \
{ return vsf_usb_dc_ep_get_data_size((vsf_usb_dc_t *)&USB_DC##__N, ep); }       \
static vsf_err_t usb_dc##__N##_ep_read_buffer(uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)\
{ return vsf_usb_dc_ep_read_buffer((vsf_usb_dc_t *)&USB_DC##__N, ep, buffer, size); }\
static vsf_err_t usb_dc##__N##_ep_enable_OUT(uint_fast8_t ep)                   \
{ return vsf_usb_dc_ep_enable_OUT((vsf_usb_dc_t *)&USB_DC##__N, ep); }          \
static vsf_err_t usb_dc##__N##_ep_set_data_size(uint_fast8_t ep, uint_fast16_t size)\
{ return vsf_usb_dc_ep_set_data_size((vsf_usb_dc_t *)&USB_DC##__N, ep, size); } \
static vsf_err_t usb_dc##__N##_ep_write_buffer(uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)\
{ return vsf_usb_dc_ep_write_buffer((vsf_usb_dc_t *)&USB_DC##__N, ep, buffer, size); }\
static void usb_dc##__N##_irq(void)                                             \
{ vsf_usb_dc_irq((vsf_usb_dc_t *)&USB_DC##__N); }

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

typedef struct vsf_usb_dc_t vsf_usb_dc_t;
typedef void (*usb_dc_irq_handler_t)(void *param);
typedef void (*usb_dc_evt_handler_t)(void *param, usb_evt_t evt, uint_fast8_t value);

//! \name usb_dc configuration structure
//! @{
struct usb_dc_cfg_t{
    vsf_arch_priority_t         priority;       //!< interrupt priority
    usb_dc_speed_t              speed;          //!< speed
    union {
        usb_dc_irq_handler_t    irq_handler;    //!< irq_handler function
        usb_dc_evt_handler_t    evt_handler;    //!< evt_handler function
    };
    void *                      param;          //!< dcd related parameters
};
typedef struct usb_dc_cfg_t usb_dc_cfg_t;
//! @}

//! \name usb_dc control interface
//! @{
def_interface(i_usb_dc_ip_t)

    //! initialize
    void            (*Init)             (usb_dc_cfg_t *cfg);

    //! get register base
    void *          (*GetRegBase)       (void);

end_def_interface(i_usb_dc_ip_t)
//! @}

//! \name usb_dc and ep control interface
//! @{

def_interface(i_usb_dc_t)

    vsf_err_t       (*Init)             (usb_dc_cfg_t *cfg);
    void            (*Fini)             (void);
    void            (*Reset)            (void);

    void            (*Connect)          (void);
    void            (*Disconnect)       (void);
    void            (*Wakeup)           (void);

    void            (*SetAddress)       (uint_fast8_t addr);
    uint_fast8_t    (*GetAddress)       (void);

    uint_fast16_t   (*GetFrameNo)       (void);
    uint_fast8_t    (*GetMicroFrameNo)  (void);

    void            (*GetSetup)         (uint8_t *buffer);

    struct {
        //! Number of endpoint
        uint8_t         Number;

        //! is DMA supported
        //!     DMA:    ReadBuffer and WriteBuffer can accept larget buffer,
        //!             GetDataSize and SetDataSize are not used
        //!     NON-DMA:ReadBuffer and WriteBuffer can accept buffer smaller than ep size
        //!             GetDataSize and SetDataSize are used to get/set the size to transfer
        bool            IsDMA;

        vsf_err_t       (*Add)              (uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);
        uint_fast16_t   (*GetSize)          (uint_fast8_t ep);

        vsf_err_t       (*SetStall)         (uint_fast8_t ep);
        bool            (*IsStalled)        (uint_fast8_t ep);
        vsf_err_t       (*ClearStall)       (uint_fast8_t ep);

        uint_fast16_t   (*GetDataSize)      (uint_fast8_t ep);
        vsf_err_t       (*ReadBuffer)       (uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
        vsf_err_t       (*EnableOUT)        (uint_fast8_t ep);

        vsf_err_t       (*SetDataSize)      (uint_fast8_t ep, uint_fast16_t size);
        vsf_err_t       (*WriteBuffer)      (uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
    } Ep;

    //! irq handler
    void            (*Irq)              (void);

end_def_interface(i_usb_dc_hl_t)
//! @}


typedef struct vsf_usb_hc_t vsf_usb_hc_t;
typedef void (*usb_hc_irq_handler_t)(void *param);

//! \name usb_hc configuration structure
//! @{
struct usb_hc_cfg_t{
    vsf_arch_priority_t         priority;       //!< interrupt priority
    usb_hc_irq_handler_t        irq_handler;    //!< irq_handler function
    void *                      param;          //!< hcd related parameters
};
typedef struct usb_hc_cfg_t usb_hc_cfg_t;
//! @}

//! \name usb_hc control interface
//! @{
def_interface(i_usb_hc_t)

    //! initialize
    vsf_err_t       (*Init)             (usb_hc_cfg_t *cfg);

    //! get register base
    void *          (*GetRegBase)       (void);

    //! irq handler
    void            (*Irq)              (void);

end_def_interface(i_usb_hc_t)
//! @}

//! \name usb user interface
//! @{
#if USB_HC_COUNT > 0 || USB_DC_COUNT > 0
def_interface(i_usb_t)

#if USB_HC_COUNT > 0
    i_usb_hc_t      HC[USB_HC_COUNT];
#endif

#if USB_DC_COUNT > 0
    i_usb_dc_t      DC[USB_DC_COUNT];
#endif

end_def_interface(i_usb_t)
//! @}
#endif

/*============================ GLOBAL VARIABLES ==============================*/

extern const i_usb_t VSF_USB;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_usb_hc_init(vsf_usb_hc_t *hc, usb_hc_cfg_t *cfg);
extern void *vsf_usb_hc_get_regbase(vsf_usb_hc_t *hc);
extern void vsf_usb_hc_irq(vsf_usb_hc_t *hc);




extern vsf_err_t vsf_usb_dc_init(vsf_usb_dc_t *dc, usb_dc_cfg_t *cfg);
extern void vsf_usb_dc_fini(vsf_usb_dc_t *dc);
extern void vsf_usb_dc_reset(vsf_usb_dc_t *dc);

extern void vsf_usb_dc_connect(vsf_usb_dc_t *dc);
extern void vsf_usb_dc_disconnect(vsf_usb_dc_t *dc);
extern void vsf_usb_dc_wakeup(vsf_usb_dc_t *dc);

extern void vsf_usb_dc_set_address(vsf_usb_dc_t *dc, uint_fast8_t addr);
extern uint_fast8_t vsf_usb_dc_get_address(vsf_usb_dc_t *dc);

extern uint_fast16_t vsf_usb_dc_get_frame_number(vsf_usb_dc_t *dc);
extern uint_fast8_t vsf_usb_dc_get_mframe_number(vsf_usb_dc_t *dc);

extern void vsf_usb_dc_get_setup(vsf_usb_dc_t *dc, uint8_t *buffer);

extern vsf_err_t vsf_usb_dc_ep_add(vsf_usb_dc_t *dc, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);
extern uint_fast16_t vsf_usb_dc_ep_get_size(vsf_usb_dc_t *dc, uint_fast8_t ep);

extern vsf_err_t vsf_usb_dc_ep_set_stall(vsf_usb_dc_t *dc, uint_fast8_t ep);
extern bool vsf_usb_dc_ep_is_stalled(vsf_usb_dc_t *dc, uint_fast8_t ep);
extern vsf_err_t vsf_usb_dc_ep_clear_stall(vsf_usb_dc_t *dc, uint_fast8_t ep);

extern uint_fast16_t vsf_usb_dc_ep_get_data_size(vsf_usb_dc_t *dc, uint_fast8_t ep);
extern vsf_err_t vsf_usb_dc_ep_read_buffer(vsf_usb_dc_t *dc, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern vsf_err_t vsf_usb_dc_ep_enable_OUT(vsf_usb_dc_t *dc, uint_fast8_t ep);

extern vsf_err_t vsf_usb_dc_ep_set_data_size(vsf_usb_dc_t *dc, uint_fast8_t ep, uint_fast16_t size);
extern vsf_err_t vsf_usb_dc_ep_write_buffer(vsf_usb_dc_t *dc, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);

extern void vsf_usb_dc_irq(vsf_usb_dc_t *dc);

#endif
/* EOF */
