        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        PUBLIC  __vector_table

        DATA
__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler                  ; Reset Handler

        DCD     NMI_Handler                    ; NMI Handler
        DCD     HardFault_Handler              ; Hard Fault Handler
        DCD     MemManage_Handler              ; MPU Fault Handler//by denvice
        DCD     BusFault_Handler               ; Bus Fault Handler//by denvice
        DCD     UsageFault_Handler             ; Usage Fault Handler//by denvice
        DCD     0                              ; Reserved
        DCD     0                              ; Reserved
        DCD     0                              ; Reserved
        DCD     0                              ; Reserved
        DCD     SVC_Handler                    ; SVCall Handler
        DCD     DebugMon_Handler               ; Debug Monitor Handler//by denvice
        DCD     0                              ; Reserved
        DCD     PendSV_Handler                 ; PendSV Handler
        DCD     SysTick_Handler                ; SysTick Handler

        ; External Interrupts
        DCD     SDIO_RX_IRQHandler             ; 
        DCD     SDIO_TX_IRQHandler             ; 
        DCD     SDIO_RX_CMD_IRQHandler         ; 
        DCD     SDIO_TX_CMD_IRQHandler         ; 
        DCD     tls_wl_mac_isr                 ; 
        DCD     0                              ; rf_cfg
        DCD     tls_wl_rx_isr                  ; 
        DCD     tls_wl_mgmt_tx_isr             ; 
        DCD     tls_wl_data_tx_isr             ; 
        DCD     PMU_TIMER1_IRQHandler          ; 
        DCD     PMU_TIMER0_IRQHandler          ; 
        DCD     PMU_GPIO_WAKE_IRQHandler       ; 
        DCD     PMU_SDIO_WAKE_IRQHandler       ; 
        DCD     DMA_Channel0_IRQHandler        ; 
        DCD     DMA_Channel1_IRQHandler        ; 
        DCD     DMA_Channel2_IRQHandler        ; 
        DCD     DMA_Channel3_IRQHandler        ; 
        DCD     DMA_Channel4_IRQHandler        ; 
        DCD     DMA_BRUST_IRQHandler           ; 
        DCD     I2C_IRQHandler                 ; 
        DCD     ADC_IRQHandler                 ; 
        DCD     SPI_LS_IRQHandler              ; 
        DCD     SPI_HS_IRQHandler              ; 
        DCD     UART0_IRQHandler               ; 
        DCD     UART1_IRQHandler               ; 
        DCD     GPIOA_IRQHandler               ; 
        DCD     TIM0_IRQHandler                ; 
        DCD     TIM1_IRQHandler                ; 
        DCD     TIM2_IRQHandler                ; 
        DCD     TIM3_IRQHandler                ; 
        DCD     TIM4_IRQHandler                ; 
        DCD     TIM5_IRQHandler                ; 
        DCD     WDG_IRQHandler                 ; 
        DCD     PMU_IRQHandler                 ; 
        DCD     FLASH_IRQHandler               ; 
        DCD     PWM_IRQHandler                 ; 
        DCD     I2S_IRQHandler                 ; 
        DCD     PMU_RTC_IRQHandler
        DCD     RSA_IRQHandler
        DCD     CRYPTION_IRQHandler            ; 
        DCD     GPIOB_IRQHandler               ; 
        DCD     UART2_IRQHandler               ; 
        DCD     SWI0_IRQHandler                ; 43
        DCD     SWI1_IRQHandler
        DCD     SWI2_IRQHandler
        DCD     SWI3_IRQHandler
        DCD     SWI4_IRQHandler
        DCD     SWI5_IRQHandler
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:NOROOT:REORDER(2)
Reset_Handler
        LDR     R0, =__iar_program_start
        BX      R0
        
        PUBWEAK NMI_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
NMI_Handler
        B NMI_Handler
        
        PUBWEAK HardFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
HardFault_Handler
        B HardFault_Handler
       
        PUBWEAK MemManage_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
UsageFault_Handler
        B UsageFault_Handler
        
        PUBWEAK SVC_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
SVC_Handler
        B SVC_Handler
       
         PUBWEAK DebugMon_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
DebugMon_Handler
        B DebugMon_Handler
        
        PUBWEAK PendSV_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
PendSV_Handler
        B PendSV_Handler
        
        
        PUBWEAK SysTick_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
SysTick_Handler
        B SysTick_Handler
        
        
        PUBWEAK SDIO_RX_IRQHandler
        PUBWEAK SDIO_TX_IRQHandler
        PUBWEAK SDIO_RX_CMD_IRQHandler
        PUBWEAK SDIO_TX_CMD_IRQHandler
        PUBWEAK tls_wl_mac_isr
        PUBWEAK tls_wl_rx_isr
        PUBWEAK tls_wl_mgmt_tx_isr
        PUBWEAK tls_wl_data_tx_isr
        PUBWEAK PMU_TIMER1_IRQHandler
        PUBWEAK PMU_TIMER0_IRQHandler
        PUBWEAK PMU_GPIO_WAKE_IRQHandler
        PUBWEAK PMU_SDIO_WAKE_IRQHandler
        PUBWEAK DMA_Channel0_IRQHandler
        PUBWEAK DMA_Channel1_IRQHandler
        PUBWEAK DMA_Channel2_IRQHandler
        PUBWEAK DMA_Channel3_IRQHandler
        PUBWEAK DMA_Channel4_IRQHandler
        PUBWEAK DMA_BRUST_IRQHandler
        PUBWEAK I2C_IRQHandler
        PUBWEAK ADC_IRQHandler
        PUBWEAK SPI_LS_IRQHandler
        PUBWEAK SPI_HS_IRQHandler
        PUBWEAK UART0_IRQHandler
        PUBWEAK UART1_IRQHandler
        PUBWEAK GPIOA_IRQHandler
        PUBWEAK TIM0_IRQHandler
        PUBWEAK TIM1_IRQHandler
        PUBWEAK TIM2_IRQHandler
        PUBWEAK TIM3_IRQHandler
        PUBWEAK TIM4_IRQHandler
        PUBWEAK TIM5_IRQHandler
        PUBWEAK WDG_IRQHandler
        PUBWEAK PMU_IRQHandler
        PUBWEAK FLASH_IRQHandler
        PUBWEAK PWM_IRQHandler
        PUBWEAK I2S_IRQHandler
        PUBWEAK PMU_RTC_IRQHandler
        PUBWEAK RSA_IRQHandler
        PUBWEAK CRYPTION_IRQHandler
        PUBWEAK GPIOB_IRQHandler
        PUBWEAK UART2_IRQHandler
        PUBWEAK SWI0_IRQHandler
        PUBWEAK SWI1_IRQHandler
        PUBWEAK SWI2_IRQHandler
        PUBWEAK SWI3_IRQHandler
        PUBWEAK SWI4_IRQHandler
        PUBWEAK SWI5_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SDIO_RX_IRQHandler
SDIO_TX_IRQHandler
SDIO_RX_CMD_IRQHandler
SDIO_TX_CMD_IRQHandler
tls_wl_mac_isr
tls_wl_rx_isr
tls_wl_mgmt_tx_isr
tls_wl_data_tx_isr
PMU_TIMER1_IRQHandler
PMU_TIMER0_IRQHandler
PMU_GPIO_WAKE_IRQHandler
PMU_SDIO_WAKE_IRQHandler
DMA_Channel0_IRQHandler
DMA_Channel1_IRQHandler
DMA_Channel2_IRQHandler
DMA_Channel3_IRQHandler
DMA_Channel4_IRQHandler
DMA_BRUST_IRQHandler
I2C_IRQHandler
ADC_IRQHandler
SPI_LS_IRQHandler
SPI_HS_IRQHandler
UART0_IRQHandler
UART1_IRQHandler
GPIOA_IRQHandler
TIM0_IRQHandler
TIM1_IRQHandler
TIM2_IRQHandler
TIM3_IRQHandler
TIM4_IRQHandler
TIM5_IRQHandler
WDG_IRQHandler
PMU_IRQHandler
FLASH_IRQHandler
PWM_IRQHandler
I2S_IRQHandler
PMU_RTC_IRQHandler
RSA_IRQHandler
CRYPTION_IRQHandler
GPIOB_IRQHandler
UART2_IRQHandler
SWI0_IRQHandler
SWI1_IRQHandler
SWI2_IRQHandler
SWI3_IRQHandler
SWI4_IRQHandler
SWI5_IRQHandler
        B .

        END

