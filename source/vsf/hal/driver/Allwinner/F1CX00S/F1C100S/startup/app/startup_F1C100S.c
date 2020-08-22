/*****************************************************************************
 *   Copyright(C)2009-2020 by VSF Team                                       *
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

#include "../../device.h"

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler Function Prototype
 *----------------------------------------------------------------------------*/
typedef void( *pFunc )( void );

/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
void __NO_RETURN Default_Handler(void);

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/

#define WEAK_ISR(__NAME, ...)                                                   \
    WEAK(__NAME)                                                                \
    void __NAME(void) { __VA_ARGS__ }

/* Exceptions */
WEAK_ISR( NMI_Handler)
WEAK_ISR( UART0_Handler)
WEAK_ISR( UART1_Handler)
WEAK_ISR( UART2_Handler)

WEAK_ISR( OWA_Handler)
WEAK_ISR( CIR_Handler)
WEAK_ISR( TWI0_Handler)
WEAK_ISR( TWI1_Handler)                               /*!< IRQ8 */
WEAK_ISR( TWI2_Handler)
WEAK_ISR( SPI0_Handler)
WEAK_ISR( SPI1_Handler)

WEAK_ISR( Timer0_Handler)
WEAK_ISR( Timer1_Handler)
WEAK_ISR( Timer2_Handler)
WEAK_ISR( WatchDog_Handler)                           /*!< IRQ16 */
WEAK_ISR( RSB_Handler)
WEAK_ISR( DMA_Handler)

WEAK_ISR( TouchPanel_Handler)                         /*!< IRQ20 */
WEAK_ISR( AudioCodec_Handler)
WEAK_ISR( KEYADC_Handler)
WEAK_ISR( SDC0_Handler)
WEAK_ISR( SDC1_Handler)                               /*!< IRQ24 */
                                        
WEAK_ISR( USBOTG_Handler)
WEAK_ISR( TVD_Handler)
WEAK_ISR( TVE_Handler)                                /*!< IRQ28 */
WEAK_ISR( TCON_Handler)
WEAK_ISR( DEFE_Handler)
WEAK_ISR( DEBE_Handler)
WEAK_ISR( CSI_Handler)                                /*!< IRQ32 */
WEAK_ISR( DEInterlacer_Handler)
WEAK_ISR( VE_Handler)
WEAK_ISR( DAUDIO_Handler)

WEAK_ISR( PIOD_Handler)
WEAK_ISR( PIOE_Handler)
WEAK_ISR( PIOF_Handler)                               /*!< IRQ40 */


#define __DECLARE_SWI_IRQ_HANDLER(__N, __NULL)                                  \
    WEAK_ISR(SWI##__N##_IRQHandler)                                             

REPEAT_MACRO(VSF_DEV_SWI_NUM, __DECLARE_SWI_IRQ_HANDLER, NULL)

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if __IS_COMPILER_GCC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

ALIGN(256)
const pFunc __VECTOR_TABLE[] __VECTOR_TABLE_ATTRIBUTE  = {
    NMI_Handler,                                /*!< FIQ0 */
    UART0_Handler,
    UART1_Handler,
    UART2_Handler,
    0,                                          /*!< IRQ4 */
    OWA_Handler,
    CIR_Handler,
    TWI0_Handler,
    TWI1_Handler,                               /*!< IRQ8 */
    TWI2_Handler,
    SPI0_Handler,
    SPI1_Handler,
    0,                                          /*!< IRQ12 */ 
    Timer0_Handler,
    Timer1_Handler,
    Timer2_Handler,
    WatchDog_Handler,                           /*!< IRQ16 */
    RSB_Handler,
    DMA_Handler,
    0,
    TouchPanel_Handler,                         /*!< IRQ20 */
    AudioCodec_Handler,
    KEYADC_Handler,
    SDC0_Handler,
    SDC1_Handler,                               /*!< IRQ24 */
    0,                                            
    USBOTG_Handler,
    TVD_Handler,
    TVE_Handler,                                /*!< IRQ28 */
    TCON_Handler,
    DEFE_Handler,
    DEBE_Handler,
    CSI_Handler,                                /*!< IRQ32 */
    DEInterlacer_Handler,
    VE_Handler,
    DAUDIO_Handler,
    0,                                          /*!< IRQ36 */
    0,
    PIOD_Handler,
    PIOE_Handler,
    PIOF_Handler,                               /*!< IRQ40 */
    0,
    0,
    0,
    0,                                          /*!< IRQ44 */
    0,
    0,
    0,
    0,                                          /*!< IRQ48 */
    0,
    0,
    0,
    0,                                          /*!< IRQ52 */
    0,
    0,
    0,
    0,                                          /*!< IRQ56 */
    0,
    0,
    0,
    SWI0_IRQHandler,                            /*!< IRQ60 */
    SWI1_IRQHandler,
    SWI2_IRQHandler,
    SWI3_IRQHandler,
};

#if __IS_COMPILER_GCC__
#pragma GCC diagnostic pop
#endif

