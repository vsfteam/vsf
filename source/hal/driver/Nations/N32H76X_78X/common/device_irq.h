
#define VSF_HW_INTERRUPTS_NUM           234

#ifdef VSF_HW_INTERRUPTS_EXPORT

#if     defined(CORE_CM4)
#   define WWDG2_IRQHandler             VSF_HW_IRQHandler0
#   define AHB_ICACHE_IRQHandler        VSF_HW_IRQHandler53
#   define AHB_DCACHE_IRQHandler        VSF_HW_IRQHandler54
#   define FPU_CPU2_IRQHandler          VSF_HW_IRQHandler55
#   define SWI0_IRQHandler              VSF_HW_IRQHandler134
#   define AHB_CACHE_PARMON_IRQHandler  VSF_HW_IRQHandler167
#   define SWI1_IRQHandler              VSF_HW_IRQHandler172
#   define SEMA4_INT2_IRQHandler        VSF_HW_IRQHandler173
#   define WWDG1_RST_IRQHandler         VSF_HW_IRQHandler174
#   define DCMUA_IRQHandler             VSF_HW_IRQHandler177
#   define SWI2_IRQHandler              VSF_HW_IRQHandler191
#else
#   define WWDG1_IRQHandler             VSF_HW_IRQHandler0
#   define SWI0_IRQHandler              VSF_HW_IRQHandler53
#   define SWI1_IRQHandler              VSF_HW_IRQHandler54
#   define FPU_CPU1_IRQHandler          VSF_HW_IRQHandler55
#   define SWI2_IRQHandler              VSF_HW_IRQHandler134
#   define SWI3_IRQHandler              VSF_HW_IRQHandler167
#   define SEMA4_INT1_IRQHandler        VSF_HW_IRQHandler172
#   define SWI4_IRQHandler              VSF_HW_IRQHandler173
#   define WWDG2_RST_IRQHandler         VSF_HW_IRQHandler174
#   define DCMUB_IRQHandler             VSF_HW_IRQHandler177
#   define SWI5_IRQHandler              VSF_HW_IRQHandler191
#endif

#define PVD_IRQHandler                  VSF_HW_IRQHandler1
#define RTC_TAMPER_IRQHandler           VSF_HW_IRQHandler2
#define RTC_WKUP_IRQHandler             VSF_HW_IRQHandler3
#define RCC_IRQHandler                  VSF_HW_IRQHandler4
#define EXTI0_IRQHandler                VSF_HW_IRQHandler5
#define EXTI1_IRQHandler                VSF_HW_IRQHandler6
#define EXTI2_IRQHandler                VSF_HW_IRQHandler7
#define EXTI3_IRQHandler                VSF_HW_IRQHandler8
#define EXTI4_IRQHandler                VSF_HW_IRQHandler9
#define EXTI9_5_IRQHandler              VSF_HW_IRQHandler10
#define EXTI15_10_IRQHandler            VSF_HW_IRQHandler11
#define DMA1_Channel0_IRQHandler        VSF_HW_IRQHandler12
#define DMA1_Channel1_IRQHandler        VSF_HW_IRQHandler13
#define DMA1_Channel2_IRQHandler        VSF_HW_IRQHandler14
#define DMA1_Channel3_IRQHandler        VSF_HW_IRQHandler15
#define DMA1_Channel4_IRQHandler        VSF_HW_IRQHandler16
#define DMA1_Channel5_IRQHandler        VSF_HW_IRQHandler17
#define DMA1_Channel6_IRQHandler        VSF_HW_IRQHandler18
#define DMA1_Channel7_IRQHandler        VSF_HW_IRQHandler19
#define DMA2_Channel0_IRQHandler        VSF_HW_IRQHandler20
#define DMA2_Channel1_IRQHandler        VSF_HW_IRQHandler21
#define DMA2_Channel2_IRQHandler        VSF_HW_IRQHandler22
#define DMA2_Channel3_IRQHandler        VSF_HW_IRQHandler23
#define DMA2_Channel4_IRQHandler        VSF_HW_IRQHandler24
#define DMA2_Channel5_IRQHandler        VSF_HW_IRQHandler25
#define DMA2_Channel6_IRQHandler        VSF_HW_IRQHandler26
#define DMA2_Channel7_IRQHandler        VSF_HW_IRQHandler27
#define DMA3_Channel0_IRQHandler        VSF_HW_IRQHandler28
#define DMA3_Channel1_IRQHandler        VSF_HW_IRQHandler29
#define DMA3_Channel2_IRQHandler        VSF_HW_IRQHandler30
#define DMA3_Channel3_IRQHandler        VSF_HW_IRQHandler31
#define DMA3_Channel4_IRQHandler        VSF_HW_IRQHandler32
#define DMA3_Channel5_IRQHandler        VSF_HW_IRQHandler33
#define DMA3_Channel6_IRQHandler        VSF_HW_IRQHandler34
#define DMA3_Channel7_IRQHandler        VSF_HW_IRQHandler35
#define MDMA_Channel0_IRQHandler        VSF_HW_IRQHandler36
#define MDMA_Channel1_IRQHandler        VSF_HW_IRQHandler37
#define MDMA_Channel2_IRQHandler        VSF_HW_IRQHandler38
#define MDMA_Channel3_IRQHandler        VSF_HW_IRQHandler39
#define MDMA_Channel4_IRQHandler        VSF_HW_IRQHandler40
#define MDMA_Channel5_IRQHandler        VSF_HW_IRQHandler41
#define MDMA_Channel6_IRQHandler        VSF_HW_IRQHandler42
#define MDMA_Channel7_IRQHandler        VSF_HW_IRQHandler43
#define MDMA_Channel8_IRQHandler        VSF_HW_IRQHandler44
#define MDMA_Channel9_IRQHandler        VSF_HW_IRQHandler45
#define MDMA_Channel10_IRQHandler       VSF_HW_IRQHandler46
#define MDMA_Channel11_IRQHandler       VSF_HW_IRQHandler47
#define MDMA_Channel12_IRQHandler       VSF_HW_IRQHandler48
#define MDMA_Channel13_IRQHandler       VSF_HW_IRQHandler49
#define MDMA_Channel14_IRQHandler       VSF_HW_IRQHandler50
#define MDMA_Channel15_IRQHandler       VSF_HW_IRQHandler51
#define SDPU_IRQHandler                 VSF_HW_IRQHandler52
#define ECCMON_IRQHandler               VSF_HW_IRQHandler56
#define RTC_ALARM_IRQHandler            VSF_HW_IRQHandler57
#define I2C1_EV_IRQHandler              VSF_HW_IRQHandler58
#define I2C1_ER_IRQHandler              VSF_HW_IRQHandler59
#define I2C2_EV_IRQHandler              VSF_HW_IRQHandler60
#define I2C2_ER_IRQHandler              VSF_HW_IRQHandler61
#define I2C3_EV_IRQHandler              VSF_HW_IRQHandler62
#define I2C3_ER_IRQHandler              VSF_HW_IRQHandler63
#define I2C4_EV_IRQHandler              VSF_HW_IRQHandler64
#define I2C4_ER_IRQHandler              VSF_HW_IRQHandler65
#define I2C5_EV_IRQHandler              VSF_HW_IRQHandler66
#define I2C5_ER_IRQHandler              VSF_HW_IRQHandler67
#define I2C6_EV_IRQHandler              VSF_HW_IRQHandler68
#define I2C6_ER_IRQHandler              VSF_HW_IRQHandler69
#define I2C7_EV_IRQHandler              VSF_HW_IRQHandler70
#define I2C7_ER_IRQHandler              VSF_HW_IRQHandler71
#define I2C8_EV_IRQHandler              VSF_HW_IRQHandler72
#define I2C8_ER_IRQHandler              VSF_HW_IRQHandler73
#define I2C9_EV_IRQHandler              VSF_HW_IRQHandler74
#define I2C9_ER_IRQHandler              VSF_HW_IRQHandler75
#define I2C10_EV_IRQHandler             VSF_HW_IRQHandler76
#define I2C10_ER_IRQHandler             VSF_HW_IRQHandler77
#define I2S1_IRQHandler                 VSF_HW_IRQHandler78
#define I2S2_IRQHandler                 VSF_HW_IRQHandler79
#define I2S3_IRQHandler                 VSF_HW_IRQHandler80
#define I2S4_IRQHandler                 VSF_HW_IRQHandler81
#define xSPI1_IRQHandler                VSF_HW_IRQHandler82
#define xSPI2_IRQHandler                VSF_HW_IRQHandler83
#define SPI1_IRQHandler                 VSF_HW_IRQHandler84
#define SPI2_IRQHandler                 VSF_HW_IRQHandler85
#define SPI3_IRQHandler                 VSF_HW_IRQHandler86
#define SPI4_IRQHandler                 VSF_HW_IRQHandler87
#define SPI5_IRQHandler                 VSF_HW_IRQHandler88
#define SPI6_IRQHandler                 VSF_HW_IRQHandler89
#define SPI7_IRQHandler                 VSF_HW_IRQHandler90
#define LCD_EV_IRQHandler               VSF_HW_IRQHandler91
#define LCD_ER_IRQHandler               VSF_HW_IRQHandler92
#define DVP1_IRQHandler                 VSF_HW_IRQHandler93
#define DVP2_IRQHandler                 VSF_HW_IRQHandler94
#define DMAMUX2_IRQHandler              VSF_HW_IRQHandler95
#define USB1_HS_EPx_OUT_IRQHandler      VSF_HW_IRQHandler96
#define USB1_HS_EPx_IN_IRQHandler       VSF_HW_IRQHandler97
#define USB1_HS_WKUP_IRQHandler         VSF_HW_IRQHandler98
#define USB1_HS_IRQHandler              VSF_HW_IRQHandler99
#define USB2_HS_EPx_OUT_IRQHandler      VSF_HW_IRQHandler100
#define USB2_HS_EPx_IN_IRQHandler       VSF_HW_IRQHandler101
#define USB2_HS_WKUP_IRQHandler         VSF_HW_IRQHandler102
#define USB2_HS_IRQHandler              VSF_HW_IRQHandler103
#define ETH1_IRQHandler                 VSF_HW_IRQHandler104
#define ETH1_PMT_LPI_IRQHandler         VSF_HW_IRQHandler105
#define ETH2_IRQHandler                 VSF_HW_IRQHandler106
#define ETH2_PMT_LPI_IRQHandler         VSF_HW_IRQHandler107
#define FDCAN1_INT0_IRQHandler          VSF_HW_IRQHandler108
#define FDCAN2_INT0_IRQHandler          VSF_HW_IRQHandler109
#define FDCAN3_INT0_IRQHandler          VSF_HW_IRQHandler110
#define FDCAN4_INT0_IRQHandler          VSF_HW_IRQHandler111
#define FDCAN1_INT1_IRQHandler          VSF_HW_IRQHandler112
#define FDCAN2_INT1_IRQHandler          VSF_HW_IRQHandler113
#define FDCAN3_INT1_IRQHandler          VSF_HW_IRQHandler114
#define FDCAN4_INT1_IRQHandler          VSF_HW_IRQHandler115
#define USART1_IRQHandler               VSF_HW_IRQHandler116
#define USART2_IRQHandler               VSF_HW_IRQHandler117
#define USART3_IRQHandler               VSF_HW_IRQHandler118
#define USART4_IRQHandler               VSF_HW_IRQHandler119
#define USART5_IRQHandler               VSF_HW_IRQHandler120
#define USART6_IRQHandler               VSF_HW_IRQHandler121
#define USART7_IRQHandler               VSF_HW_IRQHandler122
#define USART8_IRQHandler               VSF_HW_IRQHandler123
#define UART9_IRQHandler                VSF_HW_IRQHandler124
#define UART10_IRQHandler               VSF_HW_IRQHandler125
#define UART11_IRQHandler               VSF_HW_IRQHandler126
#define UART12_IRQHandler               VSF_HW_IRQHandler127
#define UART13_IRQHandler               VSF_HW_IRQHandler128
#define UART14_IRQHandler               VSF_HW_IRQHandler129
#define UART15_IRQHandler               VSF_HW_IRQHandler130
#define LPUART1_IRQHandler              VSF_HW_IRQHandler131
#define LPUART2_IRQHandler              VSF_HW_IRQHandler132
#define GPU_IRQHandler                  VSF_HW_IRQHandler133
#define SDMMC1_IRQHandler               VSF_HW_IRQHandler135
#define SDMMC2_IRQHandler               VSF_HW_IRQHandler136
#define ADC1_IRQHandler                 VSF_HW_IRQHandler137
#define ADC2_IRQHandler                 VSF_HW_IRQHandler138
#define ADC3_IRQHandler                 VSF_HW_IRQHandler139
#define COMP1_2_IRQHandler              VSF_HW_IRQHandler140
#define COMP3_4_IRQHandler              VSF_HW_IRQHandler141
#define SHRTIM1_INT1_IRQHandler         VSF_HW_IRQHandler142
#define SHRTIM1_INT2_IRQHandler         VSF_HW_IRQHandler143
#define SHRTIM1_INT3_IRQHandler         VSF_HW_IRQHandler144
#define SHRTIM1_INT4_IRQHandler         VSF_HW_IRQHandler145
#define SHRTIM1_INT5_IRQHandler         VSF_HW_IRQHandler146
#define SHRTIM1_INT6_IRQHandler         VSF_HW_IRQHandler147
#define SHRTIM1_INT7_IRQHandler         VSF_HW_IRQHandler148
#define SHRTIM1_INT8_IRQHandler         VSF_HW_IRQHandler149
#define SHRTIM2_INT1_IRQHandler         VSF_HW_IRQHandler150
#define SHRTIM2_INT2_IRQHandler         VSF_HW_IRQHandler151
#define SHRTIM2_INT3_IRQHandler         VSF_HW_IRQHandler152
#define SHRTIM2_INT4_IRQHandler         VSF_HW_IRQHandler153
#define SHRTIM2_INT5_IRQHandler         VSF_HW_IRQHandler154
#define SHRTIM2_INT6_IRQHandler         VSF_HW_IRQHandler155
#define SHRTIM2_INT7_IRQHandler         VSF_HW_IRQHandler156
#define SHRTIM2_INT8_IRQHandler         VSF_HW_IRQHandler157
#define FDCAN5_INT0_IRQHandler          VSF_HW_IRQHandler158
#define FDCAN6_INT0_IRQHandler          VSF_HW_IRQHandler159
#define FDCAN7_INT0_IRQHandler          VSF_HW_IRQHandler160
#define FDCAN8_INT0_IRQHandler          VSF_HW_IRQHandler161
#define FDCAN5_INT1_IRQHandler          VSF_HW_IRQHandler162
#define FDCAN6_INT1_IRQHandler          VSF_HW_IRQHandler163
#define FDCAN7_INT1_IRQHandler          VSF_HW_IRQHandler164
#define FDCAN8_INT1_IRQHandler          VSF_HW_IRQHandler165
#define DSI_IRQHandler                  VSF_HW_IRQHandler166
#define LPTIM5_WKUP_IRQHandler          VSF_HW_IRQHandler168
#define JPEG_SGDMA_H2P_IRQHandler       VSF_HW_IRQHandler169
#define JPEG_SGDMA_P2H_IRQHandler       VSF_HW_IRQHandler170
#define WAKEUP_IO_IRQHandler            VSF_HW_IRQHandler171
#define OTPC_IRQHandler                 VSF_HW_IRQHandler175
#define FEMC_IRQHandler                 VSF_HW_IRQHandler176
#define DAC1_IRQHandler                 VSF_HW_IRQHandler178
#define DAC2_IRQHandler                 VSF_HW_IRQHandler179
#define MDMA_AHBS_ER_IRQHandler         VSF_HW_IRQHandler180
#define CM7_CATCH_READ_ER_IRQHandler    VSF_HW_IRQHandler181
#define DAC3_IRQHandler                 VSF_HW_IRQHandler182
#define DAC4_IRQHandler                 VSF_HW_IRQHandler183
#define EMC_IRQHandler                  VSF_HW_IRQHandler184
#define DAC5_IRQHandler                 VSF_HW_IRQHandler185
#define DAC6_IRQHandler                 VSF_HW_IRQHandler186
#define ESC_OPB_IRQHandler              VSF_HW_IRQHandler187
#define ESC_SYNC0_IRQHandler            VSF_HW_IRQHandler188
#define ESC_SYNC1_IRQHandler            VSF_HW_IRQHandler189
#define ESC_WRP_IRQHandler              VSF_HW_IRQHandler190
#define ATIM1_BRK_IRQHandler            VSF_HW_IRQHandler192
#define ATIM1_TRG_COM_IRQHandler        VSF_HW_IRQHandler193
#define ATIM1_CC_IRQHandler             VSF_HW_IRQHandler194
#define ATIM1_UP_IRQHandler             VSF_HW_IRQHandler195
#define ATIM2_BRK_IRQHandler            VSF_HW_IRQHandler196
#define ATIM2_TRG_COM_IRQHandler        VSF_HW_IRQHandler197
#define ATIM2_CC_IRQHandler             VSF_HW_IRQHandler198
#define ATIM2_UP_IRQHandler             VSF_HW_IRQHandler199
#define ATIM3_BRK_IRQHandler            VSF_HW_IRQHandler200
#define ATIM3_TRG_COM_IRQHandler        VSF_HW_IRQHandler201
#define ATIM3_CC_IRQHandler             VSF_HW_IRQHandler202
#define ATIM3_UP_IRQHandler             VSF_HW_IRQHandler203
#define ATIM4_BRK_IRQHandler            VSF_HW_IRQHandler204
#define ATIM4_TRG_COM_IRQHandler        VSF_HW_IRQHandler205
#define ATIM4_CC_IRQHandler             VSF_HW_IRQHandler206
#define ATIM4_UP_IRQHandler             VSF_HW_IRQHandler207
#define GTIMA1_IRQHandler               VSF_HW_IRQHandler208
#define GTIMA2_IRQHandler               VSF_HW_IRQHandler209
#define GTIMA3_IRQHandler               VSF_HW_IRQHandler210
#define GTIMA4_IRQHandler               VSF_HW_IRQHandler211
#define GTIMA5_IRQHandler               VSF_HW_IRQHandler212
#define GTIMA6_IRQHandler               VSF_HW_IRQHandler213
#define GTIMA7_IRQHandler               VSF_HW_IRQHandler214
#define GTIMB1_IRQHandler               VSF_HW_IRQHandler215
#define GTIMB2_IRQHandler               VSF_HW_IRQHandler216
#define GTIMB3_IRQHandler               VSF_HW_IRQHandler217
#define BTIM1_IRQHandler                VSF_HW_IRQHandler218
#define BTIM2_IRQHandler                VSF_HW_IRQHandler219
#define BTIM3_IRQHandler                VSF_HW_IRQHandler220
#define BTIM4_IRQHandler                VSF_HW_IRQHandler221
#define LPTIM1_WKUP_IRQHandler          VSF_HW_IRQHandler222
#define LPTIM2_WKUP_IRQHandler          VSF_HW_IRQHandler223
#define LPTIM3_WKUP_IRQHandler          VSF_HW_IRQHandler224
#define LPTIM4_WKUP_IRQHandler          VSF_HW_IRQHandler225
#define DSMU_FLT0_IRQHandler            VSF_HW_IRQHandler226
#define DSMU_FLT1_IRQHandler            VSF_HW_IRQHandler227
#define DSMU_FLT2_IRQHandler            VSF_HW_IRQHandler228
#define DSMU_FLT3_IRQHandler            VSF_HW_IRQHandler229
#define FMAC_IRQHandler                 VSF_HW_IRQHandler230
#define CORDIC_IRQHandler               VSF_HW_IRQHandler231
#define DMAMUX1_IRQHandler              VSF_HW_IRQHandler232
#define MMU_IRQHandler                  VSF_HW_IRQHandler233

#endif

#if     defined(CORE_CM4)
#   define VSF_HW_SWI_NUM               3

#   define VSF_HW_INTERRUPT0            WWDG2_IRQHandler
#   define VSF_HW_INTERRUPT53           AHB_ICACHE_IRQHandler
#   define VSF_HW_INTERRUPT54           AHB_DCACHE_IRQHandler
#   define VSF_HW_INTERRUPT55           FPU_CPU2_IRQHandler
#   define VSF_HW_INTERRUPT134          SWI0_IRQHandler
#   define VSF_HW_INTERRUPT167          AHB_CACHE_PARMON_IRQHandler
#   define VSF_HW_INTERRUPT172          SWI1_IRQHandler
#   define VSF_HW_INTERRUPT173          SEMA4_INT2_IRQHandler
#   define VSF_HW_INTERRUPT174          WWDG1_RST_IRQHandler
#   define VSF_HW_INTERRUPT177          DCMUA_IRQHandler
#   define VSF_HW_INTERRUPT191          SWI2_IRQHandler

#   define SWI0_IRQn                    134
#   define SWI1_IRQn                    172
#   define SWI2_IRQn                    191
#else
#   define VSF_HW_SWI_NUM               6

#   define VSF_HW_INTERRUPT0            WWDG1_IRQHandler
#   define VSF_HW_INTERRUPT53           SWI0_IRQHandler
#   define VSF_HW_INTERRUPT54           SWI1_IRQHandler
#   define VSF_HW_INTERRUPT55           FPU_CPU1_IRQHandler
#   define VSF_HW_INTERRUPT134          SWI2_IRQHandler
#   define VSF_HW_INTERRUPT167          SWI3_IRQHandler
#   define VSF_HW_INTERRUPT172          SEMA4_INT1_IRQHandler
#   define VSF_HW_INTERRUPT173          SWI4_IRQHandler
#   define VSF_HW_INTERRUPT174          WWDG2_RST_IRQHandler
#   define VSF_HW_INTERRUPT177          DCMUB_IRQHandler
#   define VSF_HW_INTERRUPT191          SWI5_IRQHandler

#   define SWI0_IRQn                    53
#   define SWI1_IRQn                    54
#   define SWI2_IRQn                    134
#   define SWI3_IRQn                    167
#   define SWI4_IRQn                    173
#   define SWI5_IRQn                    191
#endif

#define VSF_HW_INTERRUPT1               PVD_IRQHandler
#define VSF_HW_INTERRUPT2               RTC_TAMPER_IRQHandler
#define VSF_HW_INTERRUPT3               RTC_WKUP_IRQHandler
#define VSF_HW_INTERRUPT4               RCC_IRQHandler
#define VSF_HW_INTERRUPT5               EXTI0_IRQHandler
#define VSF_HW_INTERRUPT6               EXTI1_IRQHandler
#define VSF_HW_INTERRUPT7               EXTI2_IRQHandler
#define VSF_HW_INTERRUPT8               EXTI3_IRQHandler
#define VSF_HW_INTERRUPT9               EXTI4_IRQHandler
#define VSF_HW_INTERRUPT10              EXTI9_5_IRQHandler
#define VSF_HW_INTERRUPT11              EXTI15_10_IRQHandler
#define VSF_HW_INTERRUPT12              DMA1_Channel0_IRQHandler
#define VSF_HW_INTERRUPT13              DMA1_Channel1_IRQHandler
#define VSF_HW_INTERRUPT14              DMA1_Channel2_IRQHandler
#define VSF_HW_INTERRUPT15              DMA1_Channel3_IRQHandler
#define VSF_HW_INTERRUPT16              DMA1_Channel4_IRQHandler
#define VSF_HW_INTERRUPT17              DMA1_Channel5_IRQHandler
#define VSF_HW_INTERRUPT18              DMA1_Channel6_IRQHandler
#define VSF_HW_INTERRUPT19              DMA1_Channel7_IRQHandler
#define VSF_HW_INTERRUPT20              DMA2_Channel0_IRQHandler
#define VSF_HW_INTERRUPT21              DMA2_Channel1_IRQHandler
#define VSF_HW_INTERRUPT22              DMA2_Channel2_IRQHandler
#define VSF_HW_INTERRUPT23              DMA2_Channel3_IRQHandler
#define VSF_HW_INTERRUPT24              DMA2_Channel4_IRQHandler
#define VSF_HW_INTERRUPT25              DMA2_Channel5_IRQHandler
#define VSF_HW_INTERRUPT26              DMA2_Channel6_IRQHandler
#define VSF_HW_INTERRUPT27              DMA2_Channel7_IRQHandler
#define VSF_HW_INTERRUPT28              DMA3_Channel0_IRQHandler
#define VSF_HW_INTERRUPT29              DMA3_Channel1_IRQHandler
#define VSF_HW_INTERRUPT30              DMA3_Channel2_IRQHandler
#define VSF_HW_INTERRUPT31              DMA3_Channel3_IRQHandler
#define VSF_HW_INTERRUPT32              DMA3_Channel4_IRQHandler
#define VSF_HW_INTERRUPT33              DMA3_Channel5_IRQHandler
#define VSF_HW_INTERRUPT34              DMA3_Channel6_IRQHandler
#define VSF_HW_INTERRUPT35              DMA3_Channel7_IRQHandler
#define VSF_HW_INTERRUPT36              MDMA_Channel0_IRQHandler
#define VSF_HW_INTERRUPT37              MDMA_Channel1_IRQHandler
#define VSF_HW_INTERRUPT38              MDMA_Channel2_IRQHandler
#define VSF_HW_INTERRUPT39              MDMA_Channel3_IRQHandler
#define VSF_HW_INTERRUPT40              MDMA_Channel4_IRQHandler
#define VSF_HW_INTERRUPT41              MDMA_Channel5_IRQHandler
#define VSF_HW_INTERRUPT42              MDMA_Channel6_IRQHandler
#define VSF_HW_INTERRUPT43              MDMA_Channel7_IRQHandler
#define VSF_HW_INTERRUPT44              MDMA_Channel8_IRQHandler
#define VSF_HW_INTERRUPT45              MDMA_Channel9_IRQHandler
#define VSF_HW_INTERRUPT46              MDMA_Channel10_IRQHandler
#define VSF_HW_INTERRUPT47              MDMA_Channel11_IRQHandler
#define VSF_HW_INTERRUPT48              MDMA_Channel12_IRQHandler
#define VSF_HW_INTERRUPT49              MDMA_Channel13_IRQHandler
#define VSF_HW_INTERRUPT50              MDMA_Channel14_IRQHandler
#define VSF_HW_INTERRUPT51              MDMA_Channel15_IRQHandler
#define VSF_HW_INTERRUPT52              SDPU_IRQHandler
#define VSF_HW_INTERRUPT56              ECCMON_IRQHandler
#define VSF_HW_INTERRUPT57              RTC_ALARM_IRQHandler
#define VSF_HW_INTERRUPT58              I2C1_EV_IRQHandler
#define VSF_HW_INTERRUPT59              I2C1_ER_IRQHandler
#define VSF_HW_INTERRUPT60              I2C2_EV_IRQHandler
#define VSF_HW_INTERRUPT61              I2C2_ER_IRQHandler
#define VSF_HW_INTERRUPT62              I2C3_EV_IRQHandler
#define VSF_HW_INTERRUPT63              I2C3_ER_IRQHandler
#define VSF_HW_INTERRUPT64              I2C4_EV_IRQHandler
#define VSF_HW_INTERRUPT65              I2C4_ER_IRQHandler
#define VSF_HW_INTERRUPT66              I2C5_EV_IRQHandler
#define VSF_HW_INTERRUPT67              I2C5_ER_IRQHandler
#define VSF_HW_INTERRUPT68              I2C6_EV_IRQHandler
#define VSF_HW_INTERRUPT69              I2C6_ER_IRQHandler
#define VSF_HW_INTERRUPT70              I2C7_EV_IRQHandler
#define VSF_HW_INTERRUPT71              I2C7_ER_IRQHandler
#define VSF_HW_INTERRUPT72              I2C8_EV_IRQHandler
#define VSF_HW_INTERRUPT73              I2C8_ER_IRQHandler
#define VSF_HW_INTERRUPT74              I2C9_EV_IRQHandler
#define VSF_HW_INTERRUPT75              I2C9_ER_IRQHandler
#define VSF_HW_INTERRUPT76              I2C10_EV_IRQHandler
#define VSF_HW_INTERRUPT77              I2C10_ER_IRQHandler
#define VSF_HW_INTERRUPT78              I2S1_IRQHandler
#define VSF_HW_INTERRUPT79              I2S2_IRQHandler
#define VSF_HW_INTERRUPT80              I2S3_IRQHandler
#define VSF_HW_INTERRUPT81              I2S4_IRQHandler
#define VSF_HW_INTERRUPT82              xSPI1_IRQHandler
#define VSF_HW_INTERRUPT83              xSPI2_IRQHandler
#define VSF_HW_INTERRUPT84              SPI1_IRQHandler
#define VSF_HW_INTERRUPT85              SPI2_IRQHandler
#define VSF_HW_INTERRUPT86              SPI3_IRQHandler
#define VSF_HW_INTERRUPT87              SPI4_IRQHandler
#define VSF_HW_INTERRUPT88              SPI5_IRQHandler
#define VSF_HW_INTERRUPT89              SPI6_IRQHandler
#define VSF_HW_INTERRUPT90              SPI7_IRQHandler
#define VSF_HW_INTERRUPT91              LCD_EV_IRQHandler
#define VSF_HW_INTERRUPT92              LCD_ER_IRQHandler
#define VSF_HW_INTERRUPT93              DVP1_IRQHandler
#define VSF_HW_INTERRUPT94              DVP2_IRQHandler
#define VSF_HW_INTERRUPT95              DMAMUX2_IRQHandler
#define VSF_HW_INTERRUPT96              USB1_HS_EPx_OUT_IRQHandler
#define VSF_HW_INTERRUPT97              USB1_HS_EPx_IN_IRQHandler
#define VSF_HW_INTERRUPT98              USB1_HS_WKUP_IRQHandler
#define VSF_HW_INTERRUPT99              USB1_HS_IRQHandler
#define VSF_HW_INTERRUPT100             USB2_HS_EPx_OUT_IRQHandler
#define VSF_HW_INTERRUPT101             USB2_HS_EPx_IN_IRQHandler
#define VSF_HW_INTERRUPT102             USB2_HS_WKUP_IRQHandler
#define VSF_HW_INTERRUPT103             USB2_HS_IRQHandler
#define VSF_HW_INTERRUPT104             ETH1_IRQHandler
#define VSF_HW_INTERRUPT105             ETH1_PMT_LPI_IRQHandler
#define VSF_HW_INTERRUPT106             ETH2_IRQHandler
#define VSF_HW_INTERRUPT107             ETH2_PMT_LPI_IRQHandler
#define VSF_HW_INTERRUPT108             FDCAN1_INT0_IRQHandler
#define VSF_HW_INTERRUPT109             FDCAN2_INT0_IRQHandler
#define VSF_HW_INTERRUPT110             FDCAN3_INT0_IRQHandler
#define VSF_HW_INTERRUPT111             FDCAN4_INT0_IRQHandler
#define VSF_HW_INTERRUPT112             FDCAN1_INT1_IRQHandler
#define VSF_HW_INTERRUPT113             FDCAN2_INT1_IRQHandler
#define VSF_HW_INTERRUPT114             FDCAN3_INT1_IRQHandler
#define VSF_HW_INTERRUPT115             FDCAN4_INT1_IRQHandler
#define VSF_HW_INTERRUPT116             USART1_IRQHandler
#define VSF_HW_INTERRUPT117             USART2_IRQHandler
#define VSF_HW_INTERRUPT118             USART3_IRQHandler
#define VSF_HW_INTERRUPT119             USART4_IRQHandler
#define VSF_HW_INTERRUPT120             USART5_IRQHandler
#define VSF_HW_INTERRUPT121             USART6_IRQHandler
#define VSF_HW_INTERRUPT122             USART7_IRQHandler
#define VSF_HW_INTERRUPT123             USART8_IRQHandler
#define VSF_HW_INTERRUPT124             UART9_IRQHandler
#define VSF_HW_INTERRUPT125             UART10_IRQHandler
#define VSF_HW_INTERRUPT126             UART11_IRQHandler
#define VSF_HW_INTERRUPT127             UART12_IRQHandler
#define VSF_HW_INTERRUPT128             UART13_IRQHandler
#define VSF_HW_INTERRUPT129             UART14_IRQHandler
#define VSF_HW_INTERRUPT130             UART15_IRQHandler
#define VSF_HW_INTERRUPT131             LPUART1_IRQHandler
#define VSF_HW_INTERRUPT132             LPUART2_IRQHandler
#define VSF_HW_INTERRUPT133             GPU_IRQHandler
#define VSF_HW_INTERRUPT135             SDMMC1_IRQHandler
#define VSF_HW_INTERRUPT136             SDMMC2_IRQHandler
#define VSF_HW_INTERRUPT137             ADC1_IRQHandler
#define VSF_HW_INTERRUPT138             ADC2_IRQHandler
#define VSF_HW_INTERRUPT139             ADC3_IRQHandler
#define VSF_HW_INTERRUPT140             COMP1_2_IRQHandler
#define VSF_HW_INTERRUPT141             COMP3_4_IRQHandler
#define VSF_HW_INTERRUPT142             SHRTIM1_INT1_IRQHandler
#define VSF_HW_INTERRUPT143             SHRTIM1_INT2_IRQHandler
#define VSF_HW_INTERRUPT144             SHRTIM1_INT3_IRQHandler
#define VSF_HW_INTERRUPT145             SHRTIM1_INT4_IRQHandler
#define VSF_HW_INTERRUPT146             SHRTIM1_INT5_IRQHandler
#define VSF_HW_INTERRUPT147             SHRTIM1_INT6_IRQHandler
#define VSF_HW_INTERRUPT148             SHRTIM1_INT7_IRQHandler
#define VSF_HW_INTERRUPT149             SHRTIM1_INT8_IRQHandler
#define VSF_HW_INTERRUPT150             SHRTIM2_INT1_IRQHandler
#define VSF_HW_INTERRUPT151             SHRTIM2_INT2_IRQHandler
#define VSF_HW_INTERRUPT152             SHRTIM2_INT3_IRQHandler
#define VSF_HW_INTERRUPT153             SHRTIM2_INT4_IRQHandler
#define VSF_HW_INTERRUPT154             SHRTIM2_INT5_IRQHandler
#define VSF_HW_INTERRUPT155             SHRTIM2_INT6_IRQHandler
#define VSF_HW_INTERRUPT156             SHRTIM2_INT7_IRQHandler
#define VSF_HW_INTERRUPT157             SHRTIM2_INT8_IRQHandler
#define VSF_HW_INTERRUPT158             FDCAN5_INT0_IRQHandler
#define VSF_HW_INTERRUPT159             FDCAN6_INT0_IRQHandler
#define VSF_HW_INTERRUPT160             FDCAN7_INT0_IRQHandler
#define VSF_HW_INTERRUPT161             FDCAN8_INT0_IRQHandler
#define VSF_HW_INTERRUPT162             FDCAN5_INT1_IRQHandler
#define VSF_HW_INTERRUPT163             FDCAN6_INT1_IRQHandler
#define VSF_HW_INTERRUPT164             FDCAN7_INT1_IRQHandler
#define VSF_HW_INTERRUPT165             FDCAN8_INT1_IRQHandler
#define VSF_HW_INTERRUPT166             DSI_IRQHandler
#define VSF_HW_INTERRUPT168             LPTIM5_WKUP_IRQHandler
#define VSF_HW_INTERRUPT169             JPEG_SGDMA_H2P_IRQHandler
#define VSF_HW_INTERRUPT170             JPEG_SGDMA_P2H_IRQHandler
#define VSF_HW_INTERRUPT171             WAKEUP_IO_IRQHandler
#define VSF_HW_INTERRUPT175             OTPC_IRQHandler
#define VSF_HW_INTERRUPT176             FEMC_IRQHandler
#define VSF_HW_INTERRUPT178             DAC1_IRQHandler
#define VSF_HW_INTERRUPT179             DAC2_IRQHandler
#define VSF_HW_INTERRUPT180             MDMA_AHBS_ER_IRQHandler
#define VSF_HW_INTERRUPT181             CM7_CATCH_READ_ER_IRQHandler
#define VSF_HW_INTERRUPT182             DAC3_IRQHandler
#define VSF_HW_INTERRUPT183             DAC4_IRQHandler
#define VSF_HW_INTERRUPT184             EMC_IRQHandler
#define VSF_HW_INTERRUPT185             DAC5_IRQHandler
#define VSF_HW_INTERRUPT186             DAC6_IRQHandler
#define VSF_HW_INTERRUPT187             ESC_OPB_IRQHandler
#define VSF_HW_INTERRUPT188             ESC_SYNC0_IRQHandler
#define VSF_HW_INTERRUPT189             ESC_SYNC1_IRQHandler
#define VSF_HW_INTERRUPT190             ESC_WRP_IRQHandler
#define VSF_HW_INTERRUPT192             ATIM1_BRK_IRQHandler
#define VSF_HW_INTERRUPT193             ATIM1_TRG_COM_IRQHandler
#define VSF_HW_INTERRUPT194             ATIM1_CC_IRQHandler
#define VSF_HW_INTERRUPT195             ATIM1_UP_IRQHandler
#define VSF_HW_INTERRUPT196             ATIM2_BRK_IRQHandler
#define VSF_HW_INTERRUPT197             ATIM2_TRG_COM_IRQHandler
#define VSF_HW_INTERRUPT198             ATIM2_CC_IRQHandler
#define VSF_HW_INTERRUPT199             ATIM2_UP_IRQHandler
#define VSF_HW_INTERRUPT200             ATIM3_BRK_IRQHandler
#define VSF_HW_INTERRUPT201             ATIM3_TRG_COM_IRQHandler
#define VSF_HW_INTERRUPT202             ATIM3_CC_IRQHandler
#define VSF_HW_INTERRUPT203             ATIM3_UP_IRQHandler
#define VSF_HW_INTERRUPT204             ATIM4_BRK_IRQHandler
#define VSF_HW_INTERRUPT205             ATIM4_TRG_COM_IRQHandler
#define VSF_HW_INTERRUPT206             ATIM4_CC_IRQHandler
#define VSF_HW_INTERRUPT207             ATIM4_UP_IRQHandler
#define VSF_HW_INTERRUPT208             GTIMA1_IRQHandler
#define VSF_HW_INTERRUPT209             GTIMA2_IRQHandler
#define VSF_HW_INTERRUPT210             GTIMA3_IRQHandler
#define VSF_HW_INTERRUPT211             GTIMA4_IRQHandler
#define VSF_HW_INTERRUPT212             GTIMA5_IRQHandler
#define VSF_HW_INTERRUPT213             GTIMA6_IRQHandler
#define VSF_HW_INTERRUPT214             GTIMA7_IRQHandler
#define VSF_HW_INTERRUPT215             GTIMB1_IRQHandler
#define VSF_HW_INTERRUPT216             GTIMB2_IRQHandler
#define VSF_HW_INTERRUPT217             GTIMB3_IRQHandler
#define VSF_HW_INTERRUPT218             BTIM1_IRQHandler
#define VSF_HW_INTERRUPT219             BTIM2_IRQHandler
#define VSF_HW_INTERRUPT220             BTIM3_IRQHandler
#define VSF_HW_INTERRUPT221             BTIM4_IRQHandler
#define VSF_HW_INTERRUPT222             LPTIM1_WKUP_IRQHandler
#define VSF_HW_INTERRUPT223             LPTIM2_WKUP_IRQHandler
#define VSF_HW_INTERRUPT224             LPTIM3_WKUP_IRQHandler
#define VSF_HW_INTERRUPT225             LPTIM4_WKUP_IRQHandler
#define VSF_HW_INTERRUPT226             DSMU_FLT0_IRQHandler
#define VSF_HW_INTERRUPT227             DSMU_FLT1_IRQHandler
#define VSF_HW_INTERRUPT228             DSMU_FLT2_IRQHandler
#define VSF_HW_INTERRUPT229             DSMU_FLT3_IRQHandler
#define VSF_HW_INTERRUPT230             FMAC_IRQHandler
#define VSF_HW_INTERRUPT231             CORDIC_IRQHandler
#define VSF_HW_INTERRUPT232             DMAMUX1_IRQHandler
#define VSF_HW_INTERRUPT233             MMU_IRQHandler
