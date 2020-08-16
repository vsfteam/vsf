#ifndef __VSF_USART_H__
#define __VSF_USART_H__

#include "hal/vsf_hal_cfg.h"
#include "../../__device.h"
#include "hal/interface/vsf_interface_usart.h"

enum em_clk_reg_rw_t {
    USART1_CLK_EN   = 0x04000U,
    USART2_CLK_EN   = 0x20000U,
    USART3_CLK_EN   = 0x40000U,
    
    GPIOA_CLK_EN    = 0X00004U,
    GPIOB_CLK_EN    = 0X00008U,
};

enum em_gpio_reg_rw_t {
    USART1_GPIO_MODE_CLEAR  = 0XFFFFF00FU,
    USART1_GPIO_MODE        = 0x000008B0U,
    
    USART2_GPIO_MODE_CLEAR  = 0XFFFF00FFU,
    USART2_GPIO_MODE        = 0x00008B00U,
    
    USART3_GPIO_MODE_CLEAR  = 0XFFFF00FFU,
    USART3_GPIO_MODE        = 0x00008B00U,
};

enum em_usart_mode_t {
    
    USART_8_BIT_LENGTH      = 0x0000U,
    USART_9_BIT_LENGTH      = 0x1000U,
    
    USART_1_STOPBIT         = 0x0000U,
    USART_2_STOPBIT         = 0x2000U,
    
    USART_NO_PARITY         = 0x0000U,
    USART_EVEN_PARITY       = 0x0400U,
    USART_ODD_PARITY        = 0x0600U,

    USART_NO_HWCONTROL      = 0x0000U,
    USART_RTS_HWCONTROL     = 0x0100U,
    USART_CTS_HWCONTROL     = 0x0200U,
    USART_RTS_CTS_HWCONTROL = 0x0300U,
    
    USART_RX_MODE           = 0x0004U,
    USART_TX_MODE           = 0x0008U,
    USART_TX_RX_MODE        = 0x000CU
};

enum em_usart_reg_rw_t {
    USART_EN               = 0x2000U,
    USART_DISEN            = 0xDFFFU,

    USART_SR_IDLE_FREE     = 0x10U,
    USART_SR_RXNE_FULL     = 0X20U,
    USART_SR_TC_TRUE       = 0X40U,
    USART_SR_TXE_TRUE      = 0X80U,
    
    USART_CR1_TXEIE_EN     = 0X0080U,
    USART_CR1_TXEIE_DISEN  = 0XFF7FU,
    USART_CR1_RXNEIE_EN    = 0X0020U,
    USART_CR1_RXNEIE_DISEN = 0XFFDFU,
};

struct usart_status_t {
    union {
        inherit(peripheral_status_t)
        struct {
            uint32_t    bIsBusy     : 1;
            uint32_t    chEvtStatus : 8;
        };
    };
};

//struct usart_capability_t {
//    implement(peripheral_status_t)
//    struct {

//    };
//};

struct vsf_usart_t {
    USART_TypeDef      *pBase;
    uint8_t            *pchWriteBuffer;
    uint8_t            *pchReadBuffer;
    uint32_t           wWriteSize;
    uint32_t           wReadSize;
    uint32_t           wWriteSizeTimer;
    uint32_t           wReadSizeTimer;
    bool               bIsWriting;
    bool               bIsLoading;
    vsf_usart_evt_t    tEvtRx;
    vsf_usart_evt_t    tEvtTx;
    vsf_usart_evt_t    tEvtRcv;
    vsf_usart_evt_t    tEvtSnd;
    usart_status_t     tEvtStatus;
};

extern vsf_err_t vsf_usart_init(vsf_usart_t *pusart, usart_cfg_t *cfg_ptr);
extern fsm_rt_t vsf_usart_enable(vsf_usart_t *pUsart);
extern fsm_rt_t vsf_usart_disable(vsf_usart_t *pusart);
extern usart_status_t vsf_usart_status(vsf_usart_t *pUsart);
extern bool vsf_usart_read_byte(vsf_usart_t *pUsart, uint8_t *byte_ptr);
extern bool vsf_usart_write_byte(vsf_usart_t *pUsart, uint_fast8_t chByte);
extern fsm_rt_t vsf_usart_request_read(vsf_usart_t *pUsart, uint8_t *buffer_ptr, uint_fast32_t u32_size);
extern fsm_rt_t vsf_usart_request_write(vsf_usart_t *pUsart, uint8_t *buffer_ptr, uint_fast32_t u32_size);

extern void vsf_usart_evt_register(vsf_usart_evt_type_t tType, vsf_usart_evt_t tEvent);
extern usart_evt_status_t vsf_usart_evt_enable(usart_evt_status_t tEventMask);
extern usart_evt_status_t vsf_usart_evt_disable(usart_evt_status_t tEventMask);
extern void vsf_usart_evt_resume(usart_evt_status_t tEventStatus);

#endif