;******************** Copyright (c) 2014 Winner Micro Electronic Design Co., Ltd. ********************
;* File Name     : startup_venus.s
;* Author            : 
;* Version          : 
;* Date               : 
;* Description    : 
; <h> Stack Configuration
;   <o> Stack Size (in Bytes)
; </h>

Stack_Size      EQU     0x00000400

                AREA    |.bss|, BSS, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp

; <h> Heap Configuration
;   <o>  Heap Size (in Bytes):at least 80Kbyte
; </h>

Heap_Size       EQU     0x001A000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size



__Vectors       DCD     __initial_sp    ; Top of Stack
                DCD     Reset_Handler              ; Reset Handler
                DCD     NMI_Handler                ; NMI Handler
                DCD     HardFault_Handler          ; Hard Fault Handler
                DCD     MemManage_Handler          ; MPU Fault Handler
                DCD     BusFault_Handler           ; Bus Fault Handler
                DCD     UsageFault_Handler         ; Usage Fault Handler
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     SVC_Handler                ; SVCall Handler
                DCD     DebugMon_Handler           ; Debug Monitor Handler
                DCD     0                          ; Reserved
                DCD     PendSV_Handler             ; PendSV Handler
                DCD     SysTick_Handler            ; SysTick Handler

                ; External Interrupts
                DCD     SDIO_RX_IRQHandler         ;
                DCD     SDIO_TX_IRQHandler         ;
                DCD     SDIO_RX_CMD_IRQHandler     ;
                DCD     SDIO_TX_CMD_IRQHandler     ;
                DCD     tls_wl_mac_isr             ;
                DCD     0                          ; rf_cfg
                DCD     tls_wl_rx_isr              ;
                DCD     tls_wl_mgmt_tx_isr         ;
                DCD     tls_wl_data_tx_isr         ;
                DCD     PMU_TIMER1_IRQHandler      ;
                DCD     PMU_TIMER0_IRQHandler      ;
                DCD     PMU_GPIO_WAKE_IRQHandler   ;
                DCD     PMU_SDIO_WAKE_IRQHandler   ;
                DCD     DMA_Channel0_IRQHandler    ;
                DCD     DMA_Channel1_IRQHandler    ;
                DCD     DMA_Channel2_IRQHandler    ;
                DCD     DMA_Channel3_IRQHandler    ;
                DCD     DMA_Channel4_7_IRQHandler  ;
                DCD     DMA_BRUST_IRQHandler       ;
                DCD     I2C_IRQHandler             ;
                DCD     ADC_IRQHandler             ;
                DCD     SPI_LS_IRQHandler          ;
                DCD     SPI_HS_IRQHandler          ;
                DCD     UART0_IRQHandler           ;
                DCD     UART1_IRQHandler           ;
                DCD     GPIOA_IRQHandler           ;
                DCD     TIM0_IRQHandler            ;
                DCD     TIM1_IRQHandler            ;
                DCD     TIM2_IRQHandler            ;
                DCD     TIM3_IRQHandler            ;
                DCD     TIM4_IRQHandler            ;
                DCD     TIM5_IRQHandler            ;
                DCD     WDG_IRQHandler             ;
                DCD     PMU_IRQHandler             ;
                DCD     FLASH_IRQHandler           ;
                DCD     PWM_IRQHandler             ;
                DCD     I2S_IRQHandler             ;
                DCD     PMU_RTC_IRQHandler
                DCD     RSA_IRQHandler
                DCD     CRYPTION_IRQHandler        ;
                DCD     GPIOB_IRQHandler           ;
                DCD     UART2_IRQHandler           ;
                DCD     SWI0_IRQHandler            ; 43
                DCD     SWI1_IRQHandler
                DCD     SWI2_IRQHandler
                DCD     SWI3_IRQHandler
                DCD     SWI4_IRQHandler
                DCD     SWI5_IRQHandler
__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

; Reset handler
Reset_Handler   PROC
                EXPORT  Reset_Handler              [WEAK]
                IMPORT  __main
                MOV     R0, #0
                MSR     PRIMASK, R0
                LDR     R0, =__main
                BX      R0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler                [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler          [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler          [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler           [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler                [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler           [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler             [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler            [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  tls_wl_mac_isr             [WEAK]
                EXPORT  tls_wl_rx_isr              [WEAK]
                EXPORT  tls_wl_mgmt_tx_isr         [WEAK]
                EXPORT  tls_wl_data_tx_isr         [WEAK]
                EXPORT  SDIO_RX_IRQHandler         [WEAK]
                EXPORT  SDIO_TX_IRQHandler         [WEAK]
                EXPORT  SDIO_RX_CMD_IRQHandler     [WEAK]
                EXPORT  SDIO_TX_CMD_IRQHandler     [WEAK]
                EXPORT  RSV_IRQHandler             [WEAK]
                EXPORT  PMU_RTC_IRQHandler         [WEAK]
                EXPORT  PMU_TIMER1_IRQHandler      [WEAK]
                EXPORT  PMU_TIMER0_IRQHandler      [WEAK]
                EXPORT  PMU_GPIO_WAKE_IRQHandler   [WEAK]
                EXPORT  PMU_SDIO_WAKE_IRQHandler   [WEAK]
                EXPORT  DMA_Channel0_IRQHandler    [WEAK]
                EXPORT  DMA_Channel1_IRQHandler    [WEAK]
                EXPORT  DMA_Channel2_IRQHandler    [WEAK]
                EXPORT  DMA_Channel3_IRQHandler    [WEAK]
                EXPORT  DMA_Channel4_7_IRQHandler  [WEAK]
                EXPORT  DMA_BRUST_IRQHandler       [WEAK]
                EXPORT  I2C_IRQHandler             [WEAK]
                EXPORT  ADC_IRQHandler             [WEAK]
                EXPORT  SPI_LS_IRQHandler          [WEAK]
                EXPORT  SPI_HS_IRQHandler          [WEAK]
                EXPORT  UART0_IRQHandler           [WEAK]
                EXPORT  UART1_IRQHandler           [WEAK]
                EXPORT  GPIOA_IRQHandler           [WEAK]
                EXPORT  TIM0_IRQHandler            [WEAK]
                EXPORT  TIM1_IRQHandler            [WEAK]
                EXPORT  TIM2_IRQHandler            [WEAK]
                EXPORT  TIM3_IRQHandler            [WEAK]
                EXPORT  TIM4_IRQHandler            [WEAK]
                EXPORT  TIM5_IRQHandler            [WEAK]
                EXPORT  WDG_IRQHandler             [WEAK]
                EXPORT  PMU_IRQHandler             [WEAK]
                EXPORT  FLASH_IRQHandler           [WEAK]
                EXPORT  PWM_IRQHandler             [WEAK]
                EXPORT  I2S_IRQHandler             [WEAK]
                EXPORT  PMU_6IRQHandler            [WEAK]
                EXPORT  RSA_IRQHandler             [WEAK]    
                EXPORT  CRYPTION_IRQHandler        [WEAK]
                EXPORT  GPIOB_IRQHandler           [WEAK]
                EXPORT  UART2_IRQHandler           [WEAK]
                EXPORT  SWI0_IRQHandler            [WEAK]
                EXPORT  SWI1_IRQHandler            [WEAK]
                EXPORT  SWI2_IRQHandler            [WEAK]
                EXPORT  SWI3_IRQHandler            [WEAK]
                EXPORT  SWI4_IRQHandler            [WEAK]
                EXPORT  SWI5_IRQHandler            [WEAK]
tls_wl_mac_isr
tls_wl_rx_isr
tls_wl_mgmt_tx_isr
tls_wl_data_tx_isr
SDIO_RX_IRQHandler
SDIO_TX_IRQHandler
SDIO_RX_CMD_IRQHandler
SDIO_TX_CMD_IRQHandler
RSV_IRQHandler
PMU_RTC_IRQHandler          
PMU_TIMER1_IRQHandler       
PMU_TIMER0_IRQHandler       
PMU_GPIO_WAKE_IRQHandler   
PMU_SDIO_WAKE_IRQHandler
DMA_Channel0_IRQHandler
DMA_Channel1_IRQHandler
DMA_Channel2_IRQHandler
DMA_Channel3_IRQHandler
DMA_Channel4_7_IRQHandler
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
PMU_6IRQHandler      
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
                B       .

                ENDP

                ALIGN

;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                 IF      :DEF:__MICROLIB

                 EXPORT  __initial_sp
                 EXPORT  __heap_base
                 EXPORT  __heap_limit

                 ELSE

                 IMPORT  __use_two_region_memory
                 EXPORT  __user_initial_stackheap

__user_initial_stackheap

                 LDR     R0, =  Heap_Mem
                 LDR     R1, =(Stack_Mem + Stack_Size)
                 LDR     R2, = (Heap_Mem +  Heap_Size)
                 LDR     R3, = Stack_Mem
                 BX      LR

                 ALIGN

                 ENDIF

                 END

;******************* Copyright (c) 2014 Winner Micro Electronic Design Co., Ltd. *****END OF FILE*****
