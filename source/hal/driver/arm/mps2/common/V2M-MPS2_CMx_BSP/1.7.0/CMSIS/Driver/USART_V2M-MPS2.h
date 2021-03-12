/* -----------------------------------------------------------------------------
 * Copyright (c) 2016 -2017 ARM Ltd.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * $Date:        16. February 2017
 * $Revision:    V1.0.2
 *
 * Project:      UART Driver Definitions for ARM CMSDK_CM device
 * -------------------------------------------------------------------------- */

#ifndef USART_V2M_MPS2_H_
#define USART_V2M_MPS2_H_


#include <stdint.h>
#include <string.h>
#include "Driver_USART.h"
#include "Device.h"
#include "RTE_Device.h"
#include "RTE_Components.h"

#if (defined(RTE_Drivers_USART) \
     && (RTE_USART0 == 0)       \
     && (RTE_USART1 == 0)       \
     && (RTE_USART2 == 0)       \
     && (RTE_USART3 == 0))
  #error "USART not configured in RTE_Device.h!"
#endif

// USART Flags
#define USART_FLAG_INITIALIZED      ((uint8_t)(1U))
#define USART_FLAG_POWERED          ((uint8_t)(1U << 1))
#define USART_FLAG_CONFIGURED       ((uint8_t)(1U << 2))
#define USART_FLAG_TX_ENABLED       ((uint8_t)(1U << 3))
#define USART_FLAG_RX_ENABLED       ((uint8_t)(1U << 4))

typedef struct _PIN
{
  CMSDK_GPIO_TypeDef *PinGpio;   // Pin GPIO
  uint8_t             PinNum;    // Pin Number
} PIN;

// USART Pin Configuration
typedef const struct _USART_PINS {
        PIN                       *pin_rx;            // RX  Pin identifier
        PIN                       *pin_tx;            // TX  Pin identifier
} USART_PINS;

// USART Transfer Information (Run-Time)
typedef struct _USART_TRANSFER_INFO {
  uint32_t              rx_num;         // Total number of receive data
  uint32_t              tx_num;         // Total number of transmit data
  uint8_t              *rx_buf;         // Pointer to in data buffer
  const uint8_t        *tx_buf;         // Pointer to out data buffer
  uint32_t              rx_cnt;         // Number of data received
  uint32_t              tx_cnt;         // Number of data sent
} USART_TRANSFER_INFO;

typedef struct _USART_STATUS {
  uint8_t tx_busy;                      // Transmitter busy flag
  uint8_t rx_busy;                      // Receiver busy flag
  uint8_t rx_overflow;                  // Receive data overflow detected (cleared on start of next receive operation)
} USART_STATUS;

// USART Information (Run-Time)
typedef struct _USART_INFO {
  ARM_USART_SignalEvent_t cb_event;            // Event callback
  USART_STATUS            status;              // Status flags
  uint8_t                 flags;               // Current USART flags
} USART_INFO;

// USART Resources Definitions
typedef const struct {
  ARM_USART_CAPABILITIES  capabilities;        // Capabilities
  USART_PINS               pins;               // USART pins configuration
  CMSDK_UART_TypeDef      *reg;                // USART peripheral pointer
  IRQn_Type                irq_num_rx;         // USART RX IRQ Number
  IRQn_Type                irq_num_tx;         // USART TX IRQ Number
  USART_INFO              *info;               // Run-Time Information
  USART_TRANSFER_INFO     *xfer;               // USART transfer information
} USART_RESOURCES;

#endif /* USART_V2M_MPS2_H_ */
