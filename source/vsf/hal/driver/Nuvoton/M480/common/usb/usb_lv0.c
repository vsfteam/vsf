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

#include "../common.h"
#include "./ohci/ohci.h"
#include "./dc/usbd_hs.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_USE_USB_HOST == ENABLED
#define __USB_HC_IMPLEMENT(__N, __TYPE)                                         \
static const m480_##__TYPE##_const_t __USB_HC##__N_const = { USB_HC##__N##_CONFIG };\
m480_##__TYPE##_t USB_HC##__N = { .index = __N, .param = &__USB_HC##__N_const };\
ROOT void USB_HC##__N##_IRQHandler(void)                                        \
{                                                                               \
    if (USB_HC##__N.callback.irq_handler != NULL) {                             \
        USB_HC##__N.callback.irq_handler(USB_HC##__N.callback.param);           \
    }                                                                           \
}
#else
#define __USB_HC_IMPLEMENT(__N, __TYPE)                                         \
static const m480_##__TYPE##_const_t __USB_HC##__N_const = { USB_HC##__N##_CONFIG };\
m480_##__TYPE##_t USB_HC##__N = { .index = __N, .param = &__USB_HC##__N_const };\
void USB_HC##__N##_IRQHandler(void)                                             \
{                                                                               \
    if (USB_HC##__N.callback.irq_handler != NULL) {                             \
        USB_HC##__N.callback.irq_handler(USB_HC##__N.callback.param);           \
    }                                                                           \
}
#endif

#define _USB_HC_IMPLEMENT(__N, __TYPE)      __USB_HC_IMPLEMENT(__N, __TYPE)
#define USB_HC_IMPLEMENT(__N, __VALUE)      _USB_HC_IMPLEMENT(__N, USB_HC##__N##_TYPE)

#define __USB_HC_INIT(__N, __TYPE)                                              \
    case __N: return m480_##__TYPE##_init((m480_##__TYPE##_t *)hc, cfg);
#define _USB_HC_INIT(__N, __TYPE)           __USB_HC_INIT(__N, __TYPE)
#define USB_HC_INIT(__N, __VALUE)           _USB_HC_INIT(__N, USB_HC##__N##_TYPE)

#define __USB_HC_GET_REGBASE(__N, __TYPE)                                       \
    case __N: return m480_##__TYPE##_get_regbase((m480_##__TYPE##_t *)hc);
#define _USB_HC_GET_REGBASE(__N, __TYPE)    __USB_HC_GET_REGBASE(__N, __TYPE)
#define USB_HC_GET_REGBASE(__N, __VALUE)    _USB_HC_GET_REGBASE(__N, USB_HC##__N##_TYPE)

#define USB_HC_IRQ(__N, __VALUE)                                                \
    case __N: USB_HC##__N##_IRQHandler(); break;



#if VSF_USE_USB_DEVICE == ENABLED
#define __USB_DC_IMPLEMENT(__N, __TYPE)                                         \
static const m480_##__TYPE##_const_t __USB_DC##__N_const = { USB_DC##__N##_CONFIG };\
m480_##__TYPE##_t USB_DC##__N = { .index = __N, .param = &__USB_DC##__N_const}; \
ROOT void USB_DC##__N##_IRQHandler(void)                                        \
{                                                                               \
    m480_##__TYPE##_irq(&USB_DC##__N);                                          \
}
#else
#define __USB_DC_IMPLEMENT(__N, __TYPE)                                         \
static const m480_##__TYPE##_const_t __USB_DC##__N_const = { USB_DC##__N##_CONFIG };\
m480_##__TYPE##_t USB_DC##__N = { .index = __N, .param = &__USB_DC##__N_const}; \
void USB_DC##__N##_IRQHandler(void)                                             \
{                                                                               \
    m480_##__TYPE##_irq(&USB_DC##__N);                                          \
}
#endif

#define _USB_DC_IMPLEMENT(__N, __TYPE)      __USB_DC_IMPLEMENT(__N, __TYPE)
#define USB_DC_IMPLEMENT(__N, __VALUE)      _USB_DC_IMPLEMENT(__N, USB_DC##__N##_TYPE)

#define __USB_DC_INIT(__N, __TYPE)                                              \
    case __N: return m480_##__TYPE##_init((m480_##__TYPE##_t *)dc, cfg);
#define _USB_DC_INIT(__N, __TYPE)           __USB_DC_INIT(__N, __TYPE)
#define USB_DC_INIT(__N, __VALUE)           _USB_DC_INIT(__N, USB_DC##__N##_TYPE)

#define __USB_DC_FINI(__N, __TYPE)                                              \
    case __N: m480_##__TYPE##_fini((m480_##__TYPE##_t *)dc); break;
#define _USB_DC_FINI(__N, __TYPE)           __USB_DC_FINI(__N, __TYPE)
#define USB_DC_FINI(__N, __VALUE)           _USB_DC_FINI(__N, USB_DC##__N##_TYPE)

#define __USB_DC_RESET(__N, __TYPE)                                             \
    case __N: m480_##__TYPE##_reset((m480_##__TYPE##_t *)dc); break;
#define _USB_DC_RESET(__N, __TYPE)          __USB_DC_RESET(__N, __TYPE)
#define USB_DC_RESET(__N, __VALUE)          _USB_DC_RESET(__N, USB_DC##__N##_TYPE)

#define __USB_DC_CONNECT(__N, __TYPE)                                           \
    case __N: m480_##__TYPE##_connect((m480_##__TYPE##_t *)dc); break;
#define _USB_DC_CONNECT(__N, __TYPE)        __USB_DC_CONNECT(__N, __TYPE)
#define USB_DC_CONNECT(__N, __VALUE)        _USB_DC_CONNECT(__N, USB_DC##__N##_TYPE)

#define __USB_DC_DISCONNECT(__N, __TYPE)                                        \
    case __N: m480_##__TYPE##_disconnect((m480_##__TYPE##_t *)dc); break;
#define _USB_DC_DISCONNECT(__N, __TYPE)     __USB_DC_DISCONNECT(__N, __TYPE)
#define USB_DC_DISCONNECT(__N, __VALUE)     _USB_DC_DISCONNECT(__N, USB_DC##__N##_TYPE)

#define __USB_DC_WAKEUP(__N, __TYPE)                                            \
    case __N: m480_##__TYPE##_wakeup((m480_##__TYPE##_t *)dc); break;
#define _USB_DC_WAKEUP(__N, __TYPE)         __USB_DC_WAKEUP(__N, __TYPE)
#define USB_DC_WAKEUP(__N, __VALUE)         _USB_DC_WAKEUP(__N, USB_DC##__N##_TYPE)

#define __USB_DC_SET_ADDRESS(__N, __TYPE)                                       \
    case __N: m480_##__TYPE##_set_address((m480_##__TYPE##_t *)dc, addr); break;
#define _USB_DC_SET_ADDRESS(__N, __TYPE)    __USB_DC_SET_ADDRESS(__N, __TYPE)
#define USB_DC_SET_ADDRESS(__N, __VALUE)    _USB_DC_SET_ADDRESS(__N, USB_DC##__N##_TYPE)

#define __USB_DC_GET_ADDRESS(__N, __TYPE)                                       \
    case __N: return m480_##__TYPE##_get_address((m480_##__TYPE##_t *)dc);
#define _USB_DC_GET_ADDRESS(__N, __TYPE)    __USB_DC_GET_ADDRESS(__N, __TYPE)
#define USB_DC_GET_ADDRESS(__N, __VALUE)    _USB_DC_GET_ADDRESS(__N, USB_DC##__N##_TYPE)

#define __USB_DC_GET_FRAME_NO(__N, __TYPE)                                      \
    case __N: return m480_##__TYPE##_get_frame_number((m480_##__TYPE##_t *)dc);
#define _USB_DC_GET_FRAME_NO(__N, __TYPE)   __USB_DC_GET_FRAME_NO(__N, __TYPE)
#define USB_DC_GET_FRAME_NO(__N, __VALUE)   _USB_DC_GET_FRAME_NO(__N, USB_DC##__N##_TYPE)

#define __USB_DC_GET_MFRAME_NO(__N, __TYPE)                                     \
    case __N: return m480_##__TYPE##_get_mframe_number((m480_##__TYPE##_t *)dc);
#define _USB_DC_GET_MFRAME_NO(__N, __TYPE)  __USB_DC_GET_MFRAME_NO(__N, __TYPE)
#define USB_DC_GET_MFRAME_NO(__N, __VALUE)  _USB_DC_GET_MFRAME_NO(__N, USB_DC##__N##_TYPE)

#define __USB_DC_GET_SETUP(__N, __TYPE)                                         \
    case __N: m480_##__TYPE##_get_setup((m480_##__TYPE##_t *)dc, buffer); break;
#define _USB_DC_GET_SERUP(__N, __TYPE)      __USB_DC_GET_SETUP(__N, __TYPE)
#define USB_DC_GET_SETUP(__N, __VALUE)      _USB_DC_GET_SERUP(__N, USB_DC##__N##_TYPE)

#define __USB_DC_STATUS_STAGE(__N, __TYPE)                                      \
    case __N: m480_##__TYPE##_status_stage((m480_##__TYPE##_t *)dc, is_in);
#define _USB_DC_STATUS_STAGE(__N, __TYPE)   __USB_DC_STATUS_STAGE(__N, __TYPE)
#define USB_DC_STATUS_STAGE(__N, __VALUE)   _USB_DC_STATUS_STAGE(__N, USB_DC##__N##_TYPE)

#define __USB_DC_EP_ADD(__N, __TYPE)                                            \
    case __N: return m480_##__TYPE##_ep_add((m480_##__TYPE##_t *)dc, ep, type, size);
#define _USB_DC_EP_ADD(__N, __TYPE)         __USB_DC_EP_ADD(__N, __TYPE)
#define USB_DC_EP_ADD(__N, __VALUE)         _USB_DC_EP_ADD(__N, USB_DC##__N##_TYPE)

#define __USB_DC_EP_GET_SIZE(__N, __TYPE)                                       \
    case __N: return m480_##__TYPE##_ep_get_size((m480_##__TYPE##_t *)dc, ep);
#define _USB_DC_EP_GET_SIZE(__N, __TYPE)         __USB_DC_EP_GET_SIZE(__N, __TYPE)
#define USB_DC_EP_GET_SIZE(__N, __VALUE)         _USB_DC_EP_GET_SIZE(__N, USB_DC##__N##_TYPE)

#define __USB_DC_EP_SET_STALL(__N, __TYPE)                                      \
    case __N: return m480_##__TYPE##_ep_set_stall((m480_##__TYPE##_t *)dc, ep);
#define _USB_DC_EP_SET_STALL(__N, __TYPE)       __USB_DC_EP_SET_STALL(__N, __TYPE)
#define USB_DC_EP_SET_STALL(__N, __VALUE)       _USB_DC_EP_SET_STALL(__N, USB_DC##__N##_TYPE)

#define __USB_DC_EP_IS_STALLED(__N, __TYPE)                                     \
    case __N: return m480_##__TYPE##_ep_is_stalled((m480_##__TYPE##_t *)dc, ep);
#define _USB_DC_EP_IS_STALLED(__N, __TYPE)      __USB_DC_EP_IS_STALLED(__N, __TYPE)
#define USB_DC_EP_IS_STALLED(__N, __VALUE)      _USB_DC_EP_IS_STALLED(__N, USB_DC##__N##_TYPE)

#define __USB_DC_EP_CLEAR_STALL(__N, __TYPE)                                    \
    case __N: return m480_##__TYPE##_ep_clear_stall((m480_##__TYPE##_t *)dc, ep);
#define _USB_DC_EP_CLEAR_STALL(__N, __TYPE)     __USB_DC_EP_CLEAR_STALL(__N, __TYPE)
#define USB_DC_EP_CLEAR_STALL(__N, __VALUE)     _USB_DC_EP_CLEAR_STALL(__N, USB_DC##__N##_TYPE)

#define __USB_DC_EP_GET_DATA_SIZE(__N, __TYPE)                                  \
    case __N: return m480_##__TYPE##_ep_get_data_size((m480_##__TYPE##_t *)dc, ep);
#define _USB_DC_EP_GET_DATA_SIZE(__N, __TYPE)   __USB_DC_EP_GET_DATA_SIZE(__N, __TYPE)
#define USB_DC_EP_GET_DATA_SIZE(__N, __VALUE)   _USB_DC_EP_GET_DATA_SIZE(__N, USB_DC##__N##_TYPE)

#define __USB_DC_EP_READ_BUFFER(__N, __TYPE)                                    \
    case __N: return m480_##__TYPE##_ep_read_buffer((m480_##__TYPE##_t *)dc, ep, buffer, size);
#define _USB_DC_EP_READ_BUFFER(__N, __TYPE)     __USB_DC_EP_READ_BUFFER(__N, __TYPE)
#define USB_DC_EP_READ_BUFFER(__N, __VALUE)     _USB_DC_EP_READ_BUFFER(__N, USB_DC##__N##_TYPE)

#define __USB_DC_EP_ENABLE_OUT(__N, __TYPE)                                     \
    case __N: return m480_##__TYPE##_ep_enable_OUT((m480_##__TYPE##_t *)dc, ep);
#define _USB_DC_EP_ENABLE_OUT(__N, __TYPE)      __USB_DC_EP_ENABLE_OUT(__N, __TYPE)
#define USB_DC_EP_ENABLE_OUT(__N, __VALUE)      _USB_DC_EP_ENABLE_OUT(__N, USB_DC##__N##_TYPE)

#define __USB_DC_EP_SET_DATA_SIZE(__N, __TYPE)                                  \
    case __N: return m480_##__TYPE##_ep_set_data_size((m480_##__TYPE##_t *)dc, ep, size);
#define _USB_DC_EP_SET_DATA_SIZE(__N, __TYPE)   __USB_DC_EP_SET_DATA_SIZE(__N, __TYPE)
#define USB_DC_EP_SET_DATA_SIZE(__N, __VALUE)   _USB_DC_EP_SET_DATA_SIZE(__N, USB_DC##__N##_TYPE)

#define __USB_DC_EP_WRITE_BUFFER(__N, __TYPE)                                   \
    case __N: return m480_##__TYPE##_ep_write_buffer((m480_##__TYPE##_t *)dc, ep, buffer, size);
#define _USB_DC_EP_WRITE_BUFFER(__N, __TYPE)    __USB_DC_EP_WRITE_BUFFER(__N, __TYPE)
#define USB_DC_EP_WRITE_BUFFER(__N, __VALUE)    _USB_DC_EP_WRITE_BUFFER(__N, USB_DC##__N##_TYPE)

#define USB_DC_IRQ(__N, __VALUE)                                                \
    case __N: USB_DC##__N##_IRQHandler(); break;

/*============================ TYPES =========================================*/

struct vsf_usb_hc_t {
    uint8_t index;
};

struct vsf_usb_dc_t {
    uint8_t index;
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

MREPEAT(USB_HC_COUNT, USB_HC_IMPLEMENT, NULL)
MREPEAT(USB_DC_COUNT, USB_DC_IMPLEMENT, NULL)

vsf_err_t vsf_usb_hc_init(vsf_usb_hc_t *hc, usb_hc_cfg_t *cfg)
{
    switch (hc->index) {
        MREPEAT(USB_HC_COUNT, USB_HC_INIT, NULL)
    default:
        return VSF_ERR_NOT_SUPPORT;
    }
}

void *vsf_usb_hc_get_regbase(vsf_usb_hc_t *hc)
{
    switch (hc->index) {
        MREPEAT(USB_HC_COUNT, USB_HC_GET_REGBASE, NULL)
    default:
        return NULL;
    }
}

void vsf_usb_hc_irq(vsf_usb_hc_t *hc)
{
    switch (hc->index) {
        MREPEAT(USB_HC_COUNT, USB_HC_IRQ, NULL)
    }
}




vsf_err_t vsf_usb_dc_init(vsf_usb_dc_t *dc, usb_dc_cfg_t *cfg)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_INIT, NULL)
    default:
        return VSF_ERR_NOT_SUPPORT;
    }
}

void vsf_usb_dc_fini(vsf_usb_dc_t *dc)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_FINI, NULL)
    }
}

void vsf_usb_dc_reset(vsf_usb_dc_t *dc)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_RESET, NULL)
    }
}

void vsf_usb_dc_connect(vsf_usb_dc_t *dc)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_CONNECT, NULL)
    }
}

void vsf_usb_dc_disconnect(vsf_usb_dc_t *dc)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_DISCONNECT, NULL)
    }
}

void vsf_usb_dc_wakeup(vsf_usb_dc_t *dc)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_WAKEUP, NULL)
    }
}

void vsf_usb_dc_set_address(vsf_usb_dc_t *dc, uint_fast8_t addr)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_SET_ADDRESS, NULL)
    }
}

uint_fast8_t vsf_usb_dc_get_address(vsf_usb_dc_t *dc)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_GET_ADDRESS, NULL)
    default:
        return 0;
    }
}

uint_fast16_t vsf_usb_dc_get_frame_number(vsf_usb_dc_t *dc)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_GET_FRAME_NO, NULL)
    default:
        return 0;
    }
}

uint_fast8_t vsf_usb_dc_get_mframe_number(vsf_usb_dc_t *dc)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_GET_MFRAME_NO, NULL)
    default:
        return 0;
    }
}

void vsf_usb_dc_get_setup(vsf_usb_dc_t *dc, uint8_t *buffer)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_GET_SETUP, NULL)
    }
}

void vsf_usb_dc_status_stage(vsf_usb_dc_t *dc, bool is_in)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_STATUS_STAGE, NULL)
    }
}

vsf_err_t vsf_usb_dc_ep_add(vsf_usb_dc_t *dc, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_EP_ADD, NULL)
    default:
        return VSF_ERR_NOT_SUPPORT;
    }
}

uint_fast16_t vsf_usb_dc_ep_get_size(vsf_usb_dc_t *dc, uint_fast8_t ep)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_EP_GET_SIZE, NULL)
    default:
        return 0;
    }
}

vsf_err_t vsf_usb_dc_ep_set_stall(vsf_usb_dc_t *dc, uint_fast8_t ep)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_EP_SET_STALL, NULL)
    default:
        return VSF_ERR_NOT_SUPPORT;
    }
}

bool vsf_usb_dc_ep_is_stalled(vsf_usb_dc_t *dc, uint_fast8_t ep)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_EP_IS_STALLED, NULL)
    default:
        return true;
    }
}

vsf_err_t vsf_usb_dc_ep_clear_stall(vsf_usb_dc_t *dc, uint_fast8_t ep)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_EP_CLEAR_STALL, NULL)
    default:
        return VSF_ERR_NOT_SUPPORT;
    }
}

uint_fast16_t vsf_usb_dc_ep_get_data_size(vsf_usb_dc_t *dc, uint_fast8_t ep)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_EP_GET_DATA_SIZE, NULL)
    default:
        return 0;
    }
}

vsf_err_t vsf_usb_dc_ep_read_buffer(vsf_usb_dc_t *dc, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_EP_READ_BUFFER, NULL)
    default:
        return VSF_ERR_NOT_SUPPORT;
    }
}

vsf_err_t vsf_usb_dc_ep_enable_OUT(vsf_usb_dc_t *dc, uint_fast8_t ep)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_EP_ENABLE_OUT, NULL)
    default:
        return VSF_ERR_NOT_SUPPORT;
    }
}

vsf_err_t vsf_usb_dc_ep_set_data_size(vsf_usb_dc_t *dc, uint_fast8_t ep, uint_fast16_t size)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_EP_SET_DATA_SIZE, NULL)
    default:
        return VSF_ERR_NOT_SUPPORT;
    }
}

vsf_err_t vsf_usb_dc_ep_write_buffer(vsf_usb_dc_t *dc, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_EP_WRITE_BUFFER, NULL)
    default:
        return VSF_ERR_NOT_SUPPORT;
    }
}

void vsf_usb_dc_irq(vsf_usb_dc_t *dc)
{
    switch (dc->index) {
        MREPEAT(USB_DC_COUNT, USB_DC_IRQ, NULL)
    }
}
