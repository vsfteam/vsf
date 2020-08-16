/**
  ******************************************************************************
  * @file    usb_regs.h
  * @author  MIC Software Team
  * @version V1.0.1
  * @date    08-01-2018
  * @brief   Interface prototype functions to USB cell registers
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2018 MIC</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_REGS_H
#define __USB_REGS_H

#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
typedef enum _EP_DBUF_DIR
{
  /* double buffered endpoint direction */
  EP_DBUF_OUT,
  EP_DBUF_IN,
  EP_DBUF_ERR,
}EP_DBUF_DIR;

/* endpoint buffer number */
enum EP_BUF_NUM
{
  EP_NOBUF,
  EP_BUF0,
  EP_BUF1
};

/* Exported defines ----------------------------------------------------------*/
#define RegBase  (0x40060000UL)  /* USB_IP Peripheral Registers base address */
#define PMAAddr  (0x40006000L)  /* USB_IP Packet Memory Area base address   */

#define FIFO_SIZE_8     ((uint8_t)0x00)
#define FIFO_SIZE_16    ((uint8_t)0x20)
#define FIFO_SIZE_32    ((uint8_t)0x40)
#define FIFO_SIZE_64    ((uint8_t)0x60)
#define FIFO_SIZE_128   ((uint8_t)0x80)
#define FIFO_SIZE_256   ((uint8_t)0xA0)
#define FIFO_SIZE_512   ((uint8_t)0xC0)
#define FIFO_SIZE_1024  ((uint8_t)0xE0)

/******************************************************************************/
/*                         General registers                                  */
/******************************************************************************/
#define __IO                            volatile
#define USB_Common_Base                 RegBase
#define USB_Peripheral_EP0_Base         (USB_Common_Base + 0x10)
#define USB_Peripheral_EPN_Base         (USB_Common_Base + 0x10)
#define USB_FIFO_Base                   (USB_Common_Base + 0x20)

enum USB_EP_NUM
{
  USB_EP_0 = 0,
  USB_EP_1,
  USB_EP_2,
  USB_EP_3,
  USB_EP_4,
  USB_EP_5,
  USB_EP_6,
  USB_EP_7,
  USB_EP_8
};

typedef struct
{
    __IO uint8_t FAddr;
    __IO uint8_t Power;
    __IO uint8_t IntrTx;
    __IO uint8_t Reserved0;
    __IO uint8_t IntrRx;
    __IO uint8_t Reserved1;
    __IO uint8_t IntrUSB;
    __IO uint8_t IntrTxE;
    __IO uint8_t Reserved3;
    __IO uint8_t IntrRxE;
    __IO uint8_t Reserved4;
    __IO uint8_t IntrUSBE;
    __IO uint8_t Frame1;
    __IO uint8_t Frame2;
    __IO uint8_t Index;
    __IO uint8_t DevCtl;
}USB_Common_Typedef;

typedef struct
{
    __IO uint8_t Reserved0;
    __IO uint8_t CSR1;
    __IO uint8_t CSR2;
    __IO uint8_t Reserved1[3];
    __IO uint8_t Count;
}USB_Peripheral_EP0_Typedef;

typedef struct
{
    __IO uint8_t TxMAXP;
    __IO uint8_t TxCSR1;
    __IO uint8_t TxCSR2;
    __IO uint8_t RxMAXP;
    __IO uint8_t RxCSR1;
    __IO uint8_t RxCSR2;
    __IO uint8_t RxCount1;
    __IO uint8_t RxCount2;
    __IO uint8_t Reserved[4];
    __IO uint8_t TXFIFO1;
    __IO uint8_t TXFIFO2;
    __IO uint8_t RXFIFO1;
    __IO uint8_t RXFIFO2;
}USB_Peripheral_EPN_Typedef;

typedef struct
{
    __IO uint32_t FIFO0;
    __IO uint32_t FIFO1;
    __IO uint32_t FIFO2;
    __IO uint32_t FIFO3;
    __IO uint32_t FIFO4;
    __IO uint32_t FIFO5;
    __IO uint32_t FIFO6;
    __IO uint32_t FIFO7;
}USB_FIFO_Typedef;

#define USB_Common                      ((USB_Common_Typedef *)USB_Common_Base)
#define USB_EP0                         ((USB_Peripheral_EP0_Typedef *)USB_Peripheral_EP0_Base)
#define USB_EPN                         ((USB_Peripheral_EPN_Typedef *)USB_Peripheral_EPN_Base)
#define USB_FIFO                        ((USB_FIFO_Typedef *)USB_FIFO_Base)



#define USB_INTR_EP0                    ((uint8_t)0x01)
#define USB_INTR_EP1                    ((uint8_t)0x02)
#define USB_INTR_EP2                    ((uint8_t)0x04)
#define USB_INTR_EP3                    ((uint8_t)0x08)
#define USB_INTR_EP4                    ((uint8_t)0x10)
#define USB_INTR_EP5                    ((uint8_t)0x20)
#define USB_INTR_EP6                    ((uint8_t)0x40)
#define USB_INTR_EP7                    ((uint8_t)0x80)

#define	USB_INTR_VBUS_ERROR			    (0x80)
#define	USB_INTR_SESSION_REQ		    (0x40)
#define	USB_INTR_DISCONNECT			    (0x20)
#define	USB_INTR_CONNECT			    (0x10)
#define	USB_INTR_SOF                    (0x08)
#define	USB_INTR_RESET	                (0x04)
#define	USB_INTR_BABBLE	                (0x04)
#define	USB_INTR_RESUME                 (0x02)
#define	USB_INTR_SUSPEND	            (0x01)

#define USB_CSR0_SERVICED_SETUP_END     ((uint8_t)0x80)
#define USB_CSR0_SERVICED_RX_PKG_RDY    ((uint8_t)0x40)
#define USB_CSR0_SEND_STALL             ((uint8_t)0x20)
#define USB_CSR0_SETUP_END              ((uint8_t)0x10)
#define USB_CSR0_DATA_END               ((uint8_t)0x08)
#define USB_CSR0_SENT_STALL             ((uint8_t)0x04)
#define USB_CSR0_TX_PKT_RDY             ((uint8_t)0x02)
#define USB_CSR0_RX_PKT_RDY             ((uint8_t)0x01)

/* TXCSR1 in Peripheral mode*/
#define USB_TXCSR1_CLR_DATA_TOG         ((uint8_t)0x40)
#define USB_TXCSR1_SENT_STALL           ((uint8_t)0x20)
#define USB_TXCSR1_SEND_STALL           ((uint8_t)0x10)
#define USB_TXCSR1_FLUSH_FIFO           ((uint8_t)0x08)
#define USB_TXCSR1_UNDER_RUN            ((uint8_t)0x04)
#define USB_TXCSR1_FIFO_NOT_EMPTY       ((uint8_t)0x02)
#define USB_TXCSR1_TX_PKT_RDY           ((uint8_t)0x01)

/* TXCSR2 */
#define USB_TXCSR2_AUTO_SET             ((uint8_t)0x80)
#define USB_TXCSR2_ISO                  ((uint8_t)0x40)
#define USB_TXCSR2_MODE                 ((uint8_t)0x20)
#define USB_TXCSR2_DMA_ENABLE           ((uint8_t)0x10)
#define USB_TXCSR2_FRC_DATA_TOG         ((uint8_t)0x08)
#define USB_TXCSR2_DMA_MODE             ((uint8_t)0x04)

/* RXCSR1 in Peripheral mode*/
#define USB_RXCSR1_CLR_DATA_TOG         ((uint8_t)0x80)
#define USB_RXCSR1_SENT_STALL           ((uint8_t)0x40)
#define USB_RXCSR1_SEND_STALL           ((uint8_t)0x20)
#define USB_RXCSR1_FLUSH_FIFO           ((uint8_t)0x10)
#define USB_RXCSR1_DATA_ERROR           ((uint8_t)0x08)
#define USB_RXCSR1_OVER_RUN             ((uint8_t)0x04)
#define USB_RXCSR1_FIFO_FULL            ((uint8_t)0x02)
#define USB_RXCSR1_RX_PKT_RDY           ((uint8_t)0x01)

/* RXCSR2 */
#define USB_RXCSR2_AUTO_CLEAR           ((uint8_t)0x80)
#define USB_RXCSR2_ISO                  ((uint8_t)0x40)
#define USB_RXCSR2_DMA_ENABLE           ((uint8_t)0x20)
#define USB_RXCSR2_DMA_MODE             ((uint8_t)0x10)

//#define EP_BULK        (0x00) /* EndPoint BULK */
//#define EP_CONTROL     (0x00) /* EndPoint CONTROL */
//#define EP_ISOCHRONOUS (0x40) /* EndPoint ISOCHRONOUS */
//#define EP_INTERRUPT   (0x00) /* EndPoint INTERRUPT */


#endif /* __USB_REGS_H */

/************************ (C) COPYRIGHT MIC *****END OF FILE****/
